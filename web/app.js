// SPDX-License-Identifier: GPL-3.0-only
const $ = id => document.getElementById(id);
const bindings = ['a','w','s','e','d','f','t','g','y','h','u','j','k'];
let ctx, node, ready = false;
let midiAccess = null, midiInput = null;
const held = new Map();
const keyButtons = [];
const heldBass = new Map();
const bassButtons = [];
function send(m) { if (ready) node.port.postMessage(m); }
function sync() {
  let mask = 0;
  for (const index of held.values()) mask |= 1 << index;
  send({type:'keys',mask});
  keyButtons.forEach((b,i)=>b.classList.toggle('held', !!(mask & (1 << i))));
}
function releaseKeys() { held.clear(); sync(); }
function releaseBass() { for(const pitch of heldBass.values()) send({type:'bass-off',pitch}); heldBass.clear(); bassButtons.forEach(b=>b.classList.remove('held')); }
function pulse(id) { send({type:'buttons',mask:1<<id});send({type:'buttons',mask:0}); }
function releaseAll() { releaseKeys(); releaseBass(); pulse(12); }
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
function syncAdventure() {
  const value=Number($('adventure').value);
  const character=value===0?'Faithful':value<=30?'Gentle':value<=60?'Curious':value<=80?'Bold':'Wild';
  $('adventure-value').textContent=`${value} · ${character}`;
  send({type:'adventure',value});
}
$('adventure').oninput=syncAdventure;
const grooveNames=['Original','Pulse','Swing','Pocket','Half-time'];
function syncGroove() {
  let bpm=Math.max(40,Math.min(240,Number($('bpm').value)||120));
  $('bpm').value=String(bpm);
  const style=Number($('groove-style').value), amount=Number($('groove-amount').value);
  const character=amount===0?'Unshaped':amount<=30?'Light':amount<=60?'Medium':amount<=80?'Strong':'Maximum';
  $('groove-value').textContent=`${amount} · ${character}`;
  $('groove-status').textContent=`Reply groove · ${grooveNames[style]} at ${bpm} BPM`;
  send({type:'groove',bpm,style,amount});
}
$('bpm').onchange=syncGroove;
$('groove-style').onchange=syncGroove;
$('groove-amount').oninput=syncGroove;
const phraseNames=['Original','Trap Pocket','R&B Flow','Techno Motion'];
function syncPhrase() {
  const style=Number($('phrase-style').value), amount=Number($('phrase-amount').value);
  const character=amount===0?'Original':amount<=30?'Light':amount<=60?'Structured':amount<=80?'Active':'Maximum';
  $('phrase-value').textContent=`${amount} · ${character}`;
  $('architecture-status').textContent=`Reply architecture · ${phraseNames[style]}`;
  send({type:'phrase',style,amount});
}
$('phrase-style').onchange=syncPhrase;
$('phrase-amount').oninput=syncPhrase;
const bassModeNames=['Off','Manual','Root','Follow'];
const bassDensityNames=['Pedal','Half notes','Quarter notes'];
const bassGrooveNames=['Straight','Auto','Trap Pocket','R&B Flow','Techno Motion'];
function updateBassStatus() {
  const mode=Number($('bass-mode').value), density=Number($('bass-density').value);
  const octave=Number($('bass-octave').value), style=Number($('bass-groove-style').value);
  const amount=Number($('bass-groove-amount').value);
  $('bass-status').textContent=`Bass Companion · ${bassModeNames[mode]} · ${bassDensityNames[density]} · ${bassGrooveNames[style]} ${amount} · octave ${octave}`;
}
function syncBass() {
  const mode=Number($('bass-mode').value), density=Number($('bass-density').value);
  const octave=Number($('bass-octave').value), volume=Number($('bass-volume').value);
  $('bass-volume-value').textContent=String(volume);
  updateBassStatus();
  bassButtons.forEach(b=>b.disabled=!ready||mode===0);
  if(mode===0) releaseBass();
  send({type:'bass',mode,density,octave,volume});
}
for(const id of ['bass-mode','bass-density','bass-octave']) $(id).onchange=syncBass;
$('bass-volume').oninput=syncBass;
function syncBassGroove() {
  const style=Number($('bass-groove-style').value), amount=Number($('bass-groove-amount').value);
  const character=amount===0?'Straight':amount<=30?'Light':amount<=60?'Medium':amount<=80?'Strong':'Maximum';
  $('bass-groove-value').textContent=`${amount} · ${character}`;
  updateBassStatus();
  send({type:'bass-groove',style,amount});
}
$('bass-groove-style').onchange=syncBassGroove;
$('bass-groove-amount').oninput=syncBassGroove;
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
    const lane=(status&0x0f)===1?' · bass channel 2':'';
    $('midi-status').textContent=on?`${midiInput?.name||'MIDI'} · ${midiNoteName(note)} · velocity ${value}${lane}`:
      `${midiInput?.name||'MIDI'} · ${midiNoteName(note)} released${lane}`;
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
let lastPhraseState={candidateMask:0,lastReplyValid:0,candidateCounts:[0,0,0]};
function candidateSlot(){return Number($('candidate-slot').value);}
function syncCandidateUi(p=lastPhraseState) {
  lastPhraseState=p;
  const slot=candidateSlot(), filled=!!(p.candidateMask&(1<<slot));
  for(let i=0;i<3;i++) $('candidate-slot').options[i].textContent=`${'ABC'[i]} · ${(p.candidateMask&(1<<i))?`${p.candidateCounts?.[i]||0} notes`:'Empty'}`;
  $('keep').disabled=!ready || !p.lastReplyValid;
  $('play-candidate').disabled=!ready || !filled;
  $('clear-candidate').disabled=!ready || !filled;
  const used=[0,1,2].filter(i=>p.candidateMask&(1<<i)).map(i=>'ABC'[i]);
  $('candidate-status').textContent=used.length?`Kept candidates · ${used.join(', ')} · selected ${'ABC'[slot]}`:'Three session-memory slots are empty.';
}
$('candidate-slot').onchange=()=>syncCandidateUi();
$('keep').onclick=()=>send({type:'keep',slot:candidateSlot()});
$('play-candidate').onclick=()=>{releaseKeys();send({type:'candidate-play',slot:candidateSlot()});};
$('clear-candidate').onclick=()=>send({type:'candidate-clear',slot:candidateSlot()});
function phraseStatus(p) {
  syncCandidateUi(p);
  const capturing=p.mode===1 || p.mode===2;
  $('capture').disabled=!ready || capturing;
  $('capture').classList.toggle('recording',capturing);
  $('finish').disabled=!ready || !capturing;
  $('replay').disabled=!ready || !p.count || capturing;
  $('reply').disabled=!ready || !p.count || capturing;
  $('clear').disabled=!ready || (!p.count && !capturing);
  const shownLength=p.mode===2?p.position:p.mode===3?(p.playLength||p.length):p.length;
  const duration=(shownLength/44100).toFixed(2);
  const limit=p.limit===1?' · 32-note limit reached':p.limit===2?' · 8-second limit reached':'';
  $('phrase-status').textContent=p.mode===1?'Armed · play your first note':
    p.mode===2?`Recording · ${p.count} notes · ${duration}s`:
    p.mode===3?`${p.playback===2?`Replying · ${phraseNames[p.phraseStyle||0]} ${p.phraseAmount||0} · ${p.replyCount||p.count} notes · variation ${p.variation||1}`:p.playback===3?`Playing kept candidate ${'ABC'[p.candidateSlot||0]} · ${p.candidateCounts?.[p.candidateSlot||0]||0} notes`:'Replaying'} · ${p.count} source notes · ${duration}s`:
    p.count?`Phrase ready · ${p.count} notes · ${duration}s${limit}`:'No phrase yet · press Capture';
  $('phrase-progress').max=p.mode===2 || p.mode===1 ? 352800 : p.mode===3?Math.max(p.playLength||p.length,1):Math.max(p.length,1);
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
const bassNames=['C1','C♯1','D1','E♭1','E1','F1','F♯1','G1','A♭1','A1','B♭1','B1'];
bassNames.forEach((name,i)=>{
  const b=document.createElement('button');b.textContent=name;b.disabled=true;
  b.setAttribute('aria-label',`Play bass ${name}`);if(name.includes('♯')||name.includes('♭'))b.classList.add('black');
  b.onpointerdown=e=>{if(b.disabled)return;e.preventDefault();b.setPointerCapture(e.pointerId);const pitch=24+i;heldBass.set('p'+e.pointerId,pitch);b.classList.add('held');send({type:'bass-on',pitch,velocity:105});};
  const up=e=>{const key='p'+e.pointerId,pitch=heldBass.get(key);if(pitch!==undefined)send({type:'bass-off',pitch});heldBass.delete(key);b.classList.remove('held');};
  b.onpointerup=up;b.onpointercancel=up;b.onlostpointercapture=up;bassButtons.push(b);$('bass-keys').append(b);
});
const bassWhite={z:24,x:26,c:28,v:29,b:31,n:33,m:35};
addEventListener('keydown',e=>{if(!ready || e.repeat || e.target.matches('input,select') || e.ctrlKey || e.metaKey || e.altKey)return;const key=e.key.toLowerCase();if(bassWhite[key]!==undefined && Number($('bass-mode').value)!==0){e.preventDefault();const pitch=bassWhite[key];heldBass.set('k'+e.code,pitch);send({type:'bass-on',pitch,velocity:105});bassButtons[pitch-24].classList.add('held');return;}const i=bindings.indexOf(key);if(i>=0){e.preventDefault();held.set('k'+e.code,i);sync();}});
addEventListener('keyup',e=>{const bassPitch=heldBass.get('k'+e.code);if(bassPitch!==undefined){send({type:'bass-off',pitch:bassPitch});heldBass.delete('k'+e.code);bassButtons[bassPitch-24].classList.remove('held');}held.delete('k'+e.code);sync();});
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
      if(m.type==='ready'){ready=true;$('status').textContent='Ready · 44.1 kHz · two voices';$('start').textContent='Sound enabled';$('stop').disabled=false;keyButtons.forEach(b=>b.disabled=false);syncHarmony();syncAdventure();syncGroove();syncPhrase();syncBass();syncBassGroove();phraseStatus({mode:0,count:0,replyCount:0,length:0,playLength:0,position:0,limit:0,playback:0,root:Number($('root').value),scale:Number($('scale').value),adventure:Number($('adventure').value),variation:0,grooveStyle:Number($('groove-style').value),grooveAmount:Number($('groove-amount').value),phraseStyle:Number($('phrase-style').value),phraseAmount:Number($('phrase-amount').value),candidateMask:0,candidateSlot:0,lastReplyValid:0,candidateCounts:[0,0,0],bassMode:Number($('bass-mode').value),bassDensity:Number($('bass-density').value),bassOctave:Number($('bass-octave').value),bassVolume:Number($('bass-volume').value),bassGrooveStyle:Number($('bass-groove-style').value),bassGrooveAmount:Number($('bass-groove-amount').value),bassCount:0});}
      if(m.type==='frame'){if(m.fb)draw(m.fb);if(m.phrase)phraseStatus(m.phrase);}
      if(m.type==='crash'){ready=false;$('status').textContent='Audio stopped: '+m.message;node.disconnect();}
    };
    node.onprocessorerror=()=>{ready=false;$('status').textContent='Audio processor failed. Reload to retry.';};
    node.connect(ctx.destination);
    node.port.postMessage({type:'load',wasm,master:Number($('volume').value)},[wasm]);
  }catch(error){$('status').textContent=error.message;node?.disconnect();await ctx?.close();$('start').disabled=false;}
};
