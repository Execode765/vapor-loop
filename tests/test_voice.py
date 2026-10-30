"""Native C signal checks: pitch, bounded output, note ownership, release and stop."""
import ctypes as C
import math
from pathlib import Path
import subprocess
import tempfile
import wave
import struct
root = Path(__file__).resolve().parents[1]
class Voice(C.Structure):
    _fields_ = [('phase', C.c_uint32), ('increment', C.c_uint32),
                ('level', C.c_int32), ('target', C.c_int32), ('note', C.c_int)]
class KeysVoice(C.Structure):
    _fields_ = [('phase', C.c_uint32), ('tine_phase', C.c_uint32),
                ('increment', C.c_uint32), ('age', C.c_uint32),
                ('level', C.c_int32), ('target', C.c_int32),
                ('note', C.c_int), ('brightness', C.c_int)]
class Keys(C.Structure):
    _fields_ = [('voices', KeysVoice * 4), ('age', C.c_uint32)]
with tempfile.TemporaryDirectory() as temp:
    libpath = str(Path(temp) / 'voice.so')
    subprocess.run(['gcc','-std=c11','-Wall','-Wextra','-Werror','-fsanitize=undefined',
        '-shared','-fPIC','-Isrc','src/voice.c','src/keys.c','-o',libpath],cwd=root,check=True)
    lib = C.CDLL(libpath)
    lib.echo_voice_init.argtypes = [C.POINTER(Voice)]
    lib.echo_voice_on.argtypes = [C.POINTER(Voice), C.c_int, C.c_int]
    lib.echo_voice_off.argtypes = [C.POINTER(Voice), C.c_int]
    lib.echo_voice_stop.argtypes = [C.POINTER(Voice)]
    lib.echo_voice_render.argtypes = [C.POINTER(Voice), C.POINTER(C.c_int32), C.c_uint32]
    lib.echo_keys_init.argtypes = [C.POINTER(Keys)]
    lib.echo_keys_on.argtypes = [C.POINTER(Keys), C.c_int, C.c_int]
    lib.echo_keys_off.argtypes = [C.POINTER(Keys), C.c_int]
    lib.echo_keys_active.argtypes = [C.POINTER(Keys)]
    lib.echo_keys_active.restype = C.c_int
    lib.echo_keys_render.argtypes = [C.POINTER(Keys), C.POINTER(C.c_int32), C.c_uint32]
    v=Voice(); lib.echo_voice_init(C.byref(v))
    def render(n):
        out=(C.c_int32 * (2*n))()
        lib.echo_voice_render(C.byref(v),out,n)
        assert list(out[::2]) == list(out[1::2])
        return list(out[::2])
    assert not any(render(256))
    lib.echo_voice_on(C.byref(v),69,127)
    signal=render(44100)
    rms=math.sqrt(sum(x*x for x in signal)/len(signal))
    crossings=sum(a<=0<b for a,b in zip(signal[4096:],signal[4097:]))
    hz=crossings*44100/(len(signal)-4096)
    assert abs(hz-440)<2, hz
    assert 5000<rms<6000 and max(map(abs,signal))<=8128, rms
    lib.echo_voice_on(C.byref(v),72,100)
    lib.echo_voice_off(C.byref(v),69)
    assert v.note==72 and v.target>0
    lib.echo_voice_on(C.byref(v),72,0)
    render(4096); assert v.level==0 and not any(render(256))
    for pitch in range(128):
        lib.echo_voice_on(C.byref(v),pitch,200)
        assert max(map(abs,render(256)))<=8128
    lib.echo_voice_stop(C.byref(v)); render(4096)
    assert not any(render(256))
    keys=Keys();lib.echo_keys_init(C.byref(keys))
    for note in (60,64,67,71):lib.echo_keys_on(C.byref(keys),note,100)
    assert lib.echo_keys_active(C.byref(keys))==4
    chord=(C.c_int32 * 2048)();lib.echo_keys_render(C.byref(keys),chord,1024)
    assert any(chord) and list(chord[::2])==list(chord[1::2])
    lib.echo_keys_on(C.byref(keys),74,100)
    assert lib.echo_keys_active(C.byref(keys))==4
    for note in (60,64,67,71,74):lib.echo_keys_off(C.byref(keys),note)
    tail=(C.c_int32 * 8192)();lib.echo_keys_render(C.byref(keys),tail,4096)
    assert lib.echo_keys_active(C.byref(keys))==0
    out=root/'build/first-sound.wav';out.parent.mkdir(exist_ok=True)
    with wave.open(str(out),'wb') as w:
        w.setparams((1,2,44100,0,'NONE','not compressed'))
        w.writeframes(struct.pack('<'+'h'*len(signal),*signal))
    print(f'PASS native C: A4 {hz:.2f} Hz, RMS {rms:.1f}; mono bounds plus four-voice keys')

