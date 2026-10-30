"""Build only the foma browser simulation; never produces or flashes firmware."""
import os
from pathlib import Path
import shutil
import subprocess
root = Path(__file__).resolve().parents[1]
os.chdir(root)
out = root / 'build/web'
out.mkdir(parents=True, exist_ok=True)
compiler = os.environ.get('CLANG', 'clang')
subprocess.run([compiler, '--target=wasm32', '-O2', '-nostdlib', '-fno-builtin',
    '-Wall', '-Wextra', '-Wno-unused-function',
    '-Wno-incompatible-library-redeclaration',
    '-Ivendor/foma/platform', '-Ivendor/foma/platform/src', '-Ivendor/foma/platform/gen', '-Isrc',
    '-DFOMA_USB_AUDIO=0', '-DFOMA_TRS_MIDI=0', '-DFOMA_MIDI_CLOCK=0', '-DFOMA_BLUETOOTH=0',
    '-Wl,--no-entry', '-Wl,-z,stack-size=65536',
    'vendor/foma/platform/web.c', 'src/main.c', 'src/voice.c', 'src/drums.c', 'src/sequencer.c', 'src/harmony.c', 'src/phrase.c', 'src/groove.c', 'src/bass_groove.c', 'src/memory.c',
    '-o', str(out / 'firmware.wasm')], check=True)
for file in (root / 'web').iterdir():
    if file.is_file(): shutil.copy2(file, out / file.name)
print('Built browser simulation:', out)

