// Exercise capture/replay commands through the real foma WASM interface.
import fs from 'node:fs';
import assert from 'node:assert/strict';
const e=new WebAssembly.Instance(new WebAssembly.Module(fs.readFileSync(new URL('../build/web/firmware.wasm',import.meta.url))),{}).exports;
e.web_master(700);e.web_boot();
function render(n=1024){e.web_render(n);return Array.from(new Float32Array(e.memory.buffer,e.web_out_l(),n));}
function advance(n){for(let i=0;i<n;i++)render();}
function command(b){e.web_buttons(1<<b);render();e.web_buttons(0);render();}
command(11);assert.equal(e.echo_mode(),0);assert.equal(e.echo_count(),0);
command(13);assert.equal(e.echo_mode(),1);
e.web_keys(1);advance(5);e.web_keys(0);advance(2);
e.web_keys(1<<7);advance(6);e.web_keys(0);advance(2);
command(3);assert.equal(e.echo_count(),2);assert.equal(e.echo_mode(),0);
assert(e.echo_length()>10000);
advance(6);assert(render().every(x=>x===0));
command(11);assert.equal(e.echo_mode(),3);assert(render().some(x=>Math.abs(x)>.01));
advance(30);assert.equal(e.echo_mode(),0);assert.equal(e.echo_count(),2);
command(11);advance(2);command(12);advance(6);assert.equal(e.echo_mode(),0);assert(render().every(x=>x===0));
command(2);assert.equal(e.echo_mode(),3);assert.equal(e.echo_playback(),2);assert(render().some(x=>Math.abs(x)>.01));
command(12);assert.equal(e.echo_mode(),0);assert.equal(e.echo_playback(),0);
e.echo_set_harmony(2,1);assert.equal(e.echo_root(),2);assert.equal(e.echo_scale(),1);
e.echo_set_adventure(80);assert.equal(e.echo_adventure(),80);assert.equal(e.echo_variation(),0);
e.echo_set_groove(96,2,80);assert.equal(e.echo_bpm(),96);assert.equal(e.echo_groove_style(),2);assert.equal(e.echo_groove_amount(),80);
e.echo_set_phrase(1,100);assert.equal(e.echo_phrase_style(),1);assert.equal(e.echo_phrase_amount(),100);assert.equal(e.echo_variation(),0);
command(2);assert.equal(e.echo_mode(),3);assert.equal(e.echo_playback(),2);assert.equal(e.echo_variation(),1);command(12);
assert.equal(e.echo_reply_count(),10);
assert.equal(e.echo_keep(0),1);assert.equal(e.echo_candidate_mask(),1);assert.equal(e.echo_candidate_count(0),10);
command(2);assert.equal(e.echo_variation(),2);command(12);
assert.equal(e.echo_keep(1),1);assert.equal(e.echo_candidate_mask(),3);assert.equal(e.echo_candidate_count(1),10);
e.echo_set_bass(2,2,1,70);assert.equal(e.echo_bass_mode(),2);assert.equal(e.echo_bass_density(),2);assert.equal(e.echo_bass_octave(),1);assert.equal(e.echo_bass_volume(),70);
e.echo_set_bass_groove(0,100);assert.equal(e.echo_bass_groove_style(),0);assert.equal(e.echo_bass_groove_amount(),100);
assert.equal(e.echo_play_candidate(0),1);assert.equal(e.echo_playback(),3);assert(e.echo_bass_count()>0);assert(render().some(x=>Math.abs(x)>.01));command(12);
e.echo_set_bass(1,1,1,75);e.echo_bass_on(28,110);assert(render().some(x=>Math.abs(x)>.01));e.echo_bass_off(28);advance(6);
e.echo_set_bass_groove(1,60);e.echo_set_bass(3,2,1,80);assert.equal(e.echo_play_candidate(1),1);assert(e.echo_bass_count()>0);command(12);
e.echo_clear_candidate(0);assert.equal(e.echo_candidate_mask(),2);assert.equal(e.echo_play_candidate(0),0);
e.echo_set_groove(120,4,100);command(2);assert(e.echo_play_length()>e.echo_length());command(12);
command(8);assert.equal(e.echo_count(),0);assert.equal(e.echo_length(),0);
assert.equal(e.echo_candidate_mask(),2);e.echo_clear_candidate(1);assert.equal(e.echo_candidate_mask(),0);
command(13);e.web_keys(1);advance(360);assert.equal(e.echo_mode(),0);assert.equal(e.echo_limit(),2);assert.equal(e.echo_length(),352800);
e.web_keys(0);command(8);command(13);
for(let i=0;i<32;i++){e.web_keys(1);advance(1);e.web_keys(0);advance(1);}
assert.equal(e.echo_count(),32);assert.equal(e.echo_mode(),0);assert.equal(e.echo_limit(),1);
console.log('PASS WASM: independent bass groove/manual/root/follow voice, bypass, override, candidate context, Keep, replay and limits');
