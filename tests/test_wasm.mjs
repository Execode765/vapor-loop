// Test the actual compiled C + foma browser platform, not a JS oscillator.
import fs from 'node:fs';
import assert from 'node:assert/strict';
const bytes=fs.readFileSync(new URL('../build/web/firmware.wasm',import.meta.url));
const module=new WebAssembly.Module(bytes);
assert.equal(WebAssembly.Module.imports(module).length,0);
const e=new WebAssembly.Instance(module,{}).exports;
e.web_master(700); e.web_boot();
assert(e.web_fb_gen()>0);
function render(n=1024){e.web_render(n);return Array.from(new Float32Array(e.memory.buffer,e.web_out_l(),n));}
function flush(){for(let i=0;i<6;i++)render();}
assert(render().every(x=>x===0));
e.web_keys(1); flush();
const note=render();assert(note.some(x=>Math.abs(x)>.01));
assert(note.every(x=>Number.isFinite(x)&&Math.abs(x)<.25));
e.web_keys(0);flush();assert(render().every(x=>x===0));
e.web_midi_in(0x90,69,100);flush();assert(render().some(x=>x!==0));
e.web_midi_in(0x90,69,0);flush();assert(render().every(x=>x===0));
e.web_keys(2);flush();assert(render().some(x=>x!==0));
e.web_buttons(1<<12);render();e.web_buttons(0);flush();assert(render().every(x=>x===0));
e.web_keys(0);flush();e.web_midi_in(0x90,60,100);flush();e.web_midi_in(0xb0,123,0);flush();assert(render().every(x=>x===0));
e.web_midi_in(0x90,60,100);e.web_master(0);flush();assert(render().every(x=>x===0));
console.log('PASS WASM: framebuffer, panel input, audible signal, note release, velocity-zero, stop, MIDI panic, volume zero');
