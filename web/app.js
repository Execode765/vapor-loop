// SPDX-License-Identifier: GPL-3.0-only
const $ = id => document.getElementById(id);
const bindings = ['a','w','s','e','d','f','t','g','y','h','u','j','k'];
let ctx, node, ready = false;
let midiAccess = null, midiInput = null;
const held = new Map();
const keyButtons = [];
function send(m) { if (ready) node.port.postMessage(m); }
function sync() {
  let mask = 0;
  for (const index of held.values()) mask |= 1 << index;
  send({type:'keys',mask});
  keyButtons.forEach((b,i)=>b.classList.toggle('held', !!(mask & (1 << i))));
}
function releaseKeys() { held.clear(); sync(); }
function pulse(id) { send({type:'buttons',mask:1<<id});send({type:'buttons',mask:0}); }
function releaseAll() { releaseKeys(); pulse(12); }
function action(id) { releaseKeys(); pulse(id); }
const rootNames=['C','C♯','D','E♭','E','F','F♯','G','A♭','A','B♭','B'];
const scaleNames=['major','natural minor','major pentatonic','minor pentatonic'];
function syncHarmony() {
  const root=Number($('root').value), scale=Number($('scale').value);
  send({type:'harmony',root,scale});
  $('harmony-status').textContent=`Reply context · ${rootNames[root]} ${scaleNames[scale]}`;
}
$('root').onchange=syncHarmony;
$('scale').onchange=syncHarmony;
function midiNoteName(n) {
  const names=['C','C♯','D','E♭','E','F','F♯','G','A♭','A','B♭','B'];
  return `${names[n%12]}${Math.floor(n/12)-1}`;
}
function midiPanic() {
  if (!ready) return;
  for (let channel=0; channel<16; channel++) send({type:'midi',data:[0xb0|channel,123,0]});
}
function onMidiMessage(event) {
  const data=Array.from(event.data||[]), status=data[0]||0, kind=status&0xf0;
  const note=data[1]&0x7f, value=data[2]&0x7f;
  if (kind===0x80 || kind===0x90) {
    send({type:'midi',data:[status,note,value]});
    const on=kind===0x90 && value>0;
    $('midi-status').textContent=on?`${midiInput?.name||'MIDI'} · ${midiNoteName(note)} · velocity ${value}`:
      `${midiInput?.name||'MIDI'} · ${midiNoteName(note)} released`;
  } else if (kind===0xb0 && (note===120 || note===123)) {
    send({type:'midi',data:[status,note,value]});
    $('midi-status').textContent=`${midiInput?.name||'MIDI'} · all notes off`;
  }
}
function chooseMidiInput(id) {
  if (midiInput) midiInput.onmidimessage=null;
  midiPanic();
  midiInput=id && midiAccess ? midiAccess.inputs.get(id)||null : null;
  if (midiInput) {
    midiInput.onmidimessage=onMidiMessage;
    $('midi-status').textContent=`Connected · ${midiInput.name||'Unnamed MIDI input'}`;
  } else if (midiAccess) $('midi-status').textContent='No MIDI input selected.';
}
function refreshMidiInputs() {
  if (!midiAccess) return;
  const select=$('midi-input'), previous=midiInput?.id||select.value;
  const inputs=Array.from(midiAccess.inputs.values()).filter(input=>input.state!=='disconnected');
  select.replaceChildren();
  if (!inputs.length) {
    select.add(new Option('No MIDI inputs',''));
    select.disabled=true;
    chooseMidiInput('');
    $('midi-status').textContent='MIDI enabled · no connected input found.';
    return;
  }
  for (const input of inputs) select.add(new Option(input.name||`MIDI input ${input.id}`,input.id));
  select.disabled=false;
  select.value=inputs.some(input=>input.id===previous)?previous:inputs[0].id;
  chooseMidiInput(select.value);
}
$('midi-input').onchange=e=>chooseMidiInput(e.target.value);
$('midi-enable').onclick=async()=>{
  if (!navigator.requestMIDIAccess) {
    $('midi-status').textContent='Web MIDI is unavailable. Use a current Chrome or Edge browser.';
    return;
  }
  $('midi-enable').disabled=true;
  $('midi-status').textContent='Waiting for browser MIDI permission…';
  try {
    midiAccess=await navigator.requestMIDIAccess({sysex:false});
    midiAccess.onstatechange=refreshMidiInputs;
    $('midi-enable').textContent='MIDI enabled';
    refreshMidiInputs();
  } catch(error) {
    $('midi-enable').disabled=false;
    $('midi-status').textContent=`MIDI access failed: ${error.message||error}`;
  }
};
$('capture').onclick=()=>action(13);
$('finish').onclick=()=>action(3);
$('replay').onclick=()=>action(11);
$('reply').onclick=()=>action(2);
$('clear').onclick=()=>action(8);
function phraseStatus(p) {
  const capturing=p.mode===1 || p.mode===2;
  $('capture').disabled=!ready || capturing;
  $('capture').classList.toggle('recording',capturing);
  $('finish').disabled=!ready || !capturing;
  $('replay').disabled=!ready || !p.count || capturing;
  $('reply').disabled=!ready || !p.count || capturing;
  $('clear').disabled=!ready || (!p.count && !capturing);
  const duration=((p.mode===2?p.position:p.length)/44100).toFixed(2);
  const limit=p.limit===1?' · 32-note limit reached':p.limit===2?' · 8-second limit reached':'';
  $('phrase-status').textContent=p.mode===1?'Armed · play your first note':
    p.mode===2?`Recording · ${p.count} notes · ${duration}s`:
    p.mode===3?`${p.playback===2?'Replying':'Replaying'} · ${p.count} notes · ${duration}s`:
    p.count?`Phrase ready · ${p.count} notes · ${duration}s${limit}`:'No phrase yet · press Capture';
  $('phrase-progress').max=p.mode===2 || p.mode===1 ? 352800 : Math.max(p.length,1);
  $('phrase-progress').value=p.mode===2 || p.mode===3?p.position:0;
}
const names=['F3','F♯3','G3','G♯3','A3','A♯3','B3','C4','C♯4','D4','D♯4','E4','F4'];
names.forEach((name,i)=>{
  const b=document.createElement('button');
  b.textContent=name; b.setAttribute('aria-label',`Play ${name}`); b.disabled=true;
  if(name.includes('♯')) b.classList.add('black');
  b.onpointerdown=e=>{ if(!ready)return; e.preventDefault(); b.setPointerCapture(e.pointerId); held.set('p'+e.pointerId,i); sync(); };
  const up=e=>{held.delete('p'+e.pointerId);sync();};
  b.onpointerup=up; b.onpointercancel=up; b.onlostpointercapture=up;
  b.onkeydown=e=>{if(ready && !e.repeat && [' ','Enter'].includes(e.key)){e.preventDefault();held.set('button'+i,i);sync();}};
  b.onkeyup=e=>{if([' ','Enter'].includes(e.key)){e.preventDefault();held.delete('button'+i);sync();}};
  b.onblur=()=>{held.delete('button'+i);sync();};
  keyButtons.push(b);$('keys').append(b);
});
addEventListener('keydown',e=>{if(!ready || e.repeat || e.target.matches('input,select') || e.ctrlKey || e.metaKey || e.altKey)return;const i=bindings.indexOf(e.key.toLowerCase());if(i>=0){e.preventDefault();held.set('k'+e.code,i);sync();}});
addEventListener('keyup',e=>{held.delete('k'+e.code);sync();});
addEventListener('blur',releaseAll);
document.addEventListener('visibilitychange',()=>{if(document.hidden)releaseAll();});
$('stop').onclick=releaseAll;
$('volume').oninput=e=>send({type:'master',value:Number(e.target.value)});
function draw(fb){
  const c=$('screen').getContext('2d'), im=c.createImageData(240,240);
  for(let i=0;i<fb.length;i++){const v=((fb[i]&255)<<8)|(fb[i]>>>8);im.data[4*i]=((v>>>11)&31)*255/31;im.data[4*i+1]=((v>>>5)&63)*255/63;im.data[4*i+2]=(v&31)*255/31;im.data[4*i+3]=255;}
  c.putImageData(im,0,0);
}
$('start').onclick=async()=>{
  $('start').disabled=true;
  try{
    ctx=new AudioContext({sampleRate:44100});
    if(ctx.sampleRate!==44100)throw Error('This first build requires a 44.1 kHz audio context.');
    await ctx.resume();
    await ctx.audioWorklet.addModule('worklet.js');
    const response=await fetch('firmware.wasm');if(!response.ok)throw Error('Could not load the WebAssembly build.');
    const wasm=await response.arrayBuffer();
    node=new AudioWorkletNode(ctx,'fm1',{numberOfInputs:0,numberOfOutputs:1,outputChannelCount:[2]});
    node.port.onmessage=({data:m})=>{
      if(m.type==='ready'){ready=true;$('status').textContent='Ready · 44.1 kHz · one voice';$('start').textContent='Sound enabled';$('stop').disabled=false;keyButtons.forEach(b=>b.disabled=false);syncHarmony();phraseStatus({mode:0,count:0,length:0,position:0,limit:0,playback:0,root:Number($('root').value),scale:Number($('scale').value)});}
      if(m.type==='frame'){if(m.fb)draw(m.fb);if(m.phrase)phraseStatus(m.phrase);}
      if(m.type==='crash'){ready=false;$('status').textContent='Audio stopped: '+m.message;node.disconnect();}
    };
    node.onprocessorerror=()=>{ready=false;$('status').textContent='Audio processor failed. Reload to retry.';};
    node.connect(ctx.destination);
    node.port.postMessage({type:'load',wasm,master:Number($('volume').value)},[wasm]);
  }catch(error){$('status').textContent=error.message;node?.disconnect();await ctx?.close();$('start').disabled=false;}
};
