// Exercise the real worklet message bridge with the actual WASM in Node.
// This is not a browser or a listening test.
import vm from 'node:vm';
import fs from 'node:fs';
import assert from 'node:assert/strict';
const messages=[];
let Processor;
const context=vm.createContext({WebAssembly,Uint8Array,Uint16Array,Float32Array,Proxy,String,currentTime:0,
  AudioWorkletProcessor:class {constructor(){this.port={postMessage:m=>messages.push(m)};}},
  registerProcessor:(name,ctor)=>{assert.equal(name,'fm1');Processor=ctor;}});
vm.runInContext(fs.readFileSync(new URL('../web/worklet.js',import.meta.url),'utf8'),context);
const p=new Processor();
await p.onMessage({type:'load',wasm:fs.readFileSync(new URL('../build/web/firmware.wasm',import.meta.url)),master:700});
assert(messages.some(m=>m.type==='ready'));
await p.onMessage({type:'keys',mask:1});
let peak=0;
for(let i=0;i<100;i++){
  context.currentTime=i*128/44100;
  const out=[[new Float32Array(128),new Float32Array(128)]];
  assert.equal(p.process([],out),true);
  for(const sample of out[0][0])peak=Math.max(peak,Math.abs(sample));
}
assert(peak>.01);
assert(messages.some(m=>m.type==='frame'&&m.fb?.length===240*240));
await p.onMessage({type:'keys',mask:0});
let out;
for(let i=0;i<100;i++){out=[[new Float32Array(128),new Float32Array(128)]];p.process([],out);}
assert(out[0][0].every(x=>x===0));
assert(!messages.some(m=>m.type==='crash'));
console.log('PASS worklet bridge: WASM load, ready, rendered signal, framebuffer messages, note release');
