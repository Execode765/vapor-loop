// SPDX-License-Identifier: GPL-3.0-only
const $ = id => document.getElementById(id);
const bindings = ['a','w','s','e','d','f','t','g','y','h','u','j','k'];
let ctx, node, ready = false;
let midiAccess = null, midiInput = null;
const held = new Map();
const keyButtons = [];
const heldBass = new Map();
const bassButtons = [];
const drumButtons = [];
function send(m) { if (ready) node.port.postMessage(m); }
const leadEngineNames=['Sine Diagnostic','FM Glass','Virtual Analog Warm','Karplus–Strong String'];
function syncLeadEngine() {
  const engine=Number($('lead-engine').value);
  $('engine-status').textContent=ready?`Selected · ${leadEngineNames[engine]} · verifying audio core…`:`Selected · ${leadEngineNames[engine]} · enable sound to verify audio core`;
  send({type:'lead-engine',engine});
}
$('lead-engine').onchange=syncLeadEngine;
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
const quantizeNames=['Free','Humanized','Grid'];
function syncCaptureClock() {
  const bars=Number($('capture-bars').value), quantize=Number($('capture-quantize').value);
  const countInBars=Number($('count-in-bars').value), metronome=Number($('capture-metronome').value);
  $('capture-clock-status').textContent=`Capture clock · ${bars} ${bars===1?'bar':'bars'} · ${countInBars}-bar count-in · ${quantizeNames[quantize]} sixteenth correction · metronome ${metronome?'on':'off'}`;
  send({type:'capture-clock',bars,countInBars,quantize,metronome});
}
for(const id of ['capture-bars','count-in-bars','capture-quantize','capture-metronome']) $(id).onchange=syncCaptureClock;
const resolutionNames={4:'quarter-note',8:'eighth-note',16:'sixteenth-note'};
const inputTrackNames={0:'Lead',1:'Bass',3:'Poly Keys',4:'Split'};
function syncSequencer() {
  const track=Number($('input-track').value), resolution=Number($('sequence-resolution').value);
  const splitPoint=Number($('split-point').value);
  send({type:'input-track',track}); send({type:'split-point',note:splitPoint});
  send({type:'sequence-resolution',resolution});
  $('split-point').disabled=track!==4;
  const split=track===4?` · split at ${midiNoteName(splitPoint)} (MIDI ${splitPoint})`:'';
  $('sequencer-status').textContent=`${inputTrackNames[track]} input${split} · ${resolutionNames[resolution]} grid · bass ${lastPhraseState.manualBassCount||0} · keys ${lastPhraseState.manualKeysCount||0}`;
  updateRecordButton(lastPhraseState);
}
$('input-track').onchange=syncSequencer;
$('split-point').onchange=syncSequencer;
$('sequence-resolution').onchange=syncSequencer;
const drumModeNames=['Off','Manual','Companion'];
function syncDrums() {
  const mode=Number($('drum-mode').value), volume=Number($('drum-volume').value);
  $('drum-volume-value').textContent=String(volume);
  $('drum-status').textContent=mode===2?`Companion drums · synchronized reference groove · level ${volume}`:mode===1?`Manual drums · one-bar 16-step pattern repeats · level ${volume}`:'Drum track off';
  drumButtons.forEach(b=>b.disabled=!ready||mode!==1);
  send({type:'drum-mode',mode,volume});
}
$('drum-mode').onchange=syncDrums;
$('drum-volume').oninput=syncDrums;
for(const [voice,label] of ['Kick','Snare','Hat'].entries()) {
  const name=document.createElement('span');name.className='drum-label';name.textContent=label;$('drum-grid').append(name);
  for(let step=0;step<16;step++) {
    const b=document.createElement('button');b.type='button';b.textContent=String(step+1);b.disabled=true;
    if(step%4===0)b.classList.add('downbeat');
    b.setAttribute('aria-label',`${label} step ${step+1}`);
    b.onclick=()=>{b.classList.toggle('active');send({type:'drum-toggle',step,voice});};
    drumButtons.push(b);$('drum-grid').append(b);
  }
}
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
  updateRecordButton(lastPhraseState);
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
function syncKeys() {
  const mode=Number($('keys-mode').value), volume=Number($('keys-volume').value);
  $('keys-volume-value').textContent=String(volume);
  const names=['Off · live free','Manual · recordable','Root Follow','Phrase Follow'];
  $('keys-status').textContent=`Chord Companion · ${names[mode]} · ${lastPhraseState.keysActive||0}/4 voices active · ${lastPhraseState.manualKeysCount||0} recorded notes`;
  send({type:'keys-mode',mode,volume});
  updateRecordButton(lastPhraseState);
}
$('keys-mode').onchange=syncKeys;
$('keys-volume').oninput=syncKeys;
function midiNoteName(n) {
  const names=['C','C♯','D','E♭','E','F','F♯','G','A♭','A','B♭','B'];
  return `${names[n%12]}${Math.floor(n/12)-1}`;
}
function midiPanic() {
  if (!ready) return;
  for (let channel=0; channel<16; channel++) send({type:'midi',data:[0xb0|channel,123,0]});
}
function setMidiStatus(text, route='idle') {
  const status=$('midi-status');
  status.textContent=text;
  status.className=`route-${route}`;
}
function onMidiMessage(event) {
  const data=Array.from(event.data||[]), status=data[0]||0, kind=status&0xf0;
  const note=data[1]&0x7f, value=data[2]&0x7f;
  if (kind===0x80 || kind===0x90) {
    const inputTrack=Number($('input-track').value);
    const splitPoint=Number($('split-point').value);
    const routedTrack=inputTrack===4?(note<splitPoint?1:0):inputTrack;
    const routedChannel=routedTrack===1?1:routedTrack===3?2:0;
    const routedStatus=kind|routedChannel;
    send({type:'midi',data:[routedStatus,note,value]});
    const on=kind===0x90 && value>0;
    const route=inputTrackNames[routedTrack];
    const routeClass=routedTrack===1?'bass':routedTrack===3?'keys':'lead';
    setMidiStatus(on?`MIDI ${note} · ${midiNoteName(note)} → ${route.toUpperCase()} · velocity ${value}`:
      `MIDI ${note} · ${midiNoteName(note)} released → ${route.toUpperCase()}`,routeClass);
  } else if (kind===0xb0 && (note===120 || note===123)) {
    send({type:'midi',data:[status,note,value]});
    setMidiStatus(`${midiInput?.name||'MIDI'} · all notes off`);
  }
}
function chooseMidiInput(id) {
  if (midiInput) midiInput.onmidimessage=null;
  midiPanic();
  midiInput=id && midiAccess ? midiAccess.inputs.get(id)||null : null;
  if (midiInput) {
    midiInput.onmidimessage=onMidiMessage;
    setMidiStatus(`Connected · ${midiInput.name||'Unnamed MIDI input'}`);
  } else if (midiAccess) setMidiStatus('No MIDI input selected.');
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
    setMidiStatus('MIDI enabled · no connected input found.');
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
    setMidiStatus('Web MIDI is unavailable. Use a current Chrome or Edge browser.');
    return;
  }
  $('midi-enable').disabled=true;
  setMidiStatus('Waiting for browser MIDI permission…');
  try {
    midiAccess=await navigator.requestMIDIAccess({sysex:false});
    midiAccess.onstatechange=refreshMidiInputs;
    $('midi-enable').textContent='MIDI enabled';
    refreshMidiInputs();
  } catch(error) {
    $('midi-enable').disabled=false;
    setMidiStatus(`MIDI access failed: ${error.message||error}`);
  }
};
$('capture').onclick=()=>action(13);
$('finish').onclick=()=>action(3);
$('replay').onclick=()=>action(11);
$('reply').onclick=()=>action(2);
$('clear').onclick=()=>action(8);
let lastPhraseState={candidateMask:0,lastReplyValid:0,candidateCounts:[0,0,0],manualBassCount:0,manualKeysCount:0,keysActive:0};
function updateRecordButton(p=lastPhraseState) {
  const track=Number($('input-track').value);
  const valid=track===1?Number($('bass-mode').value)===1:track===3?Number($('keys-mode').value)===1:false;
  const label=track===1?'Record bass pass':track===3?'Record keys pass':'Select Bass or Poly Keys';
  $('record-track').textContent=label;
  const capturing=p.mode===1||p.mode===2||p.mode===4;
  $('record-track').disabled=!ready||!p.count||capturing||!valid;
}
$('record-track').onclick=()=>{
  releaseBass();
  const track=Number($('input-track').value);
  if(track===1||track===3)send({type:'track-record',track});
};
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
  const activeEngine=Math.max(0,Math.min(leadEngineNames.length-1,Number(p.leadEngine)||0));
  $('engine-status').textContent=`Active in audio core · ${leadEngineNames[activeEngine]} · fixed musical preset`;
  $('lead-engine').classList.toggle('engine-mismatch',activeEngine!==Number($('lead-engine').value));
  const capturing=p.mode===1 || p.mode===2 || p.mode===4;
  $('capture').disabled=!ready || capturing;
  $('capture').classList.toggle('recording',capturing);
  $('finish').disabled=!ready || !capturing;
  $('replay').disabled=!ready || !p.count || capturing;
  $('reply').disabled=!ready || !p.count || capturing;
  $('clear').disabled=!ready || (!p.count && !capturing);
  updateRecordButton(p);
  const track=Number($('input-track').value), splitPoint=Number($('split-point').value);
  const split=track===4?` · split at ${midiNoteName(splitPoint)} (MIDI ${splitPoint})`:'';
  const recordingName=p.recordingTrack===3?'Poly Keys':'Bass';
  const trackArmed=p.mode===4 && (p.recordingTrack===1||p.recordingTrack===3);
  $('sequencer-status').textContent=trackArmed?`ARMED · ${p.recordingTrack===3?'Poly Keys':'Bass'} pass begins after count-in · enter slightly early or on beat one`:
    p.trackRecording?`RECORDING ${recordingName} · bass ${p.manualBassCount||0} · keys ${p.manualKeysCount||0} · ${resolutionNames[p.sequenceResolution||16]} grid`:
    `${inputTrackNames[track]} input${split} · ${resolutionNames[p.sequenceResolution||16]} grid · bass ${p.manualBassCount||0} · keys ${p.manualKeysCount||0}`;
  const keyModeNames=['Off · live free','Manual · recordable','Root Follow','Phrase Follow'];
  $('keys-status').textContent=`Chord Companion · ${keyModeNames[Number($('keys-mode').value)]} · ${p.keysActive||0}/4 voices active · ${p.manualKeysCount||0} recorded notes`;
  const shownLength=p.mode===2||p.mode===4?p.position:p.mode===3?(p.playLength||p.length):p.length;
  const duration=(shownLength/44100).toFixed(2);
  const limit=p.limit===1?' · 128-note limit reached':p.limit===2?' · 8-second free-time limit reached':p.limit===3?` · ${p.captureBars||2} bars complete`:'';
  const beat=Math.max(p.tempoBeat||1,1), barFrames=beat*4;
  const countBeat=Math.min(4,Math.floor((p.position||0)/beat)+1);
  const countBar=Math.min(p.countInBars||1,Math.floor((p.position||0)/barFrames)+1);
  const recordBar=Math.min(p.captureBars||1,Math.floor((p.position||0)/barFrames)+1);
  $('clock-position').textContent=p.clockBar&&p.clockBeat?`Bar ${p.clockBar} · Beat ${p.clockBeat}`:'Clock standing by';
  $('phrase-status').textContent=p.mode===4?`Count-in · bar ${countBar} of ${p.countInBars||1} · beat ${countBeat} of 4 · enter after the final bar`:
    p.mode===1?'Armed · play your first note':
    p.mode===2?`Recording · bar ${recordBar} of ${p.captureBars||1} · ${p.count} notes · ${duration}s`:
    p.mode===3?`${p.playback===2?`Replying · ${phraseNames[p.phraseStyle||0]} ${p.phraseAmount||0} · ${p.replyCount||p.count} notes · variation ${p.variation||1}`:p.playback===3?`Playing kept candidate ${'ABC'[p.candidateSlot||0]} · ${p.candidateCounts?.[p.candidateSlot||0]||0} notes`:'Replaying'} · ${p.count} source notes · ${duration}s`:
    p.count?`Phrase ready · ${p.count} notes · ${duration}s${limit}`:'No phrase yet · press Capture';
  $('phrase-progress').max=p.mode===4?Math.max(beat*4*(p.countInBars||1),1):p.mode===2?Math.max(p.captureLength||352800,1):p.mode===1?352800:p.mode===3?Math.max(p.playLength||p.length,1):Math.max(p.length,1);
  $('phrase-progress').value=p.mode===2 || p.mode===3 || p.mode===4?p.position:0;
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
      if(m.type==='ready'){ready=true;$('status').textContent='Ready · 44.1 kHz · four-track performance clock';$('start').textContent='Sound enabled';$('stop').disabled=false;keyButtons.forEach(b=>b.disabled=false);syncLeadEngine();syncHarmony();syncAdventure();syncGroove();syncCaptureClock();syncPhrase();syncBass();syncBassGroove();syncKeys();syncSequencer();syncDrums();phraseStatus({mode:0,count:0,replyCount:0,length:0,playLength:0,position:0,limit:0,playback:0,root:Number($('root').value),scale:Number($('scale').value),adventure:Number($('adventure').value),variation:0,grooveStyle:Number($('groove-style').value),grooveAmount:Number($('groove-amount').value),phraseStyle:Number($('phrase-style').value),phraseAmount:Number($('phrase-amount').value),candidateMask:0,candidateSlot:0,lastReplyValid:0,candidateCounts:[0,0,0],bassMode:Number($('bass-mode').value),bassDensity:Number($('bass-density').value),bassOctave:Number($('bass-octave').value),bassVolume:Number($('bass-volume').value),bassGrooveStyle:Number($('bass-groove-style').value),bassGrooveAmount:Number($('bass-groove-amount').value),bassCount:0,manualBassCount:0,manualKeysCount:0,keysMode:Number($('keys-mode').value),keysVolume:Number($('keys-volume').value),keysActive:0,splitPoint:Number($('split-point').value),leadEngine:Number($('lead-engine').value),recordingTrack:-1,trackRecording:0,inputTrack:Number($('input-track').value),sequenceResolution:Number($('sequence-resolution').value),drumMode:Number($('drum-mode').value),captureBars:Number($('capture-bars').value),countInBars:Number($('count-in-bars').value),captureQuantize:Number($('capture-quantize').value),metronomeEnabled:Number($('capture-metronome').value),rhythmEnabled:Number($('drum-mode').value)>0?1:0,rhythmVolume:Number($('drum-volume').value),captureLength:0,tempoBeat:0,clockBar:0,clockBeat:0});}
      if(m.type==='frame'){if(m.fb)draw(m.fb);if(m.phrase)phraseStatus(m.phrase);}
      if(m.type==='crash'){ready=false;$('status').textContent='Audio stopped: '+m.message;node.disconnect();}
    };
    node.onprocessorerror=()=>{ready=false;$('status').textContent='Audio processor failed. Reload to retry.';};
    node.connect(ctx.destination);
    node.port.postMessage({type:'load',wasm,master:Number($('volume').value)},[wasm]);
  }catch(error){$('status').textContent=error.message;node?.disconnect();await ctx?.close();$('start').disabled=false;}
};




