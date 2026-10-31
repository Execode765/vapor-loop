// Requires the built site served at localhost:8000. Tests real browser audio samples.
const {chromium}=require('playwright');
const assert=require('node:assert/strict');
(async()=>{
  const browser=await chromium.launch({headless:true,
    executablePath:process.env.PLAYWRIGHT_CHROMIUM_EXECUTABLE_PATH||undefined,
    args:['--no-sandbox','--autoplay-policy=no-user-gesture-required']});
  try {
    const page=await browser.newPage({viewport:{width:1100,height:1000}});
    const errors=[];page.on('pageerror',e=>{errors.push(e.message);console.error('PAGE ERROR',e.message);});
    page.on('console',m=>{if(m.type()==='error')console.error('CONSOLE',m.text());});
    page.setDefaultTimeout(15000);
    await page.addInitScript(()=>{
      const Original=window.AudioWorkletNode;
      window.AudioWorkletNode=class extends Original {
        constructor(ctx,...args){super(ctx,...args);window.__audioProbe=ctx.createAnalyser();this.connect(window.__audioProbe);}
      };
      const input={id:'smk-pro-elite',name:'SMK Pro Elite',manufacturer:'M-VAVE',state:'connected',onmidimessage:null};
      const access={inputs:new Map([[input.id,input]]),onstatechange:null};
      window.__fakeMidi={
        requested:null,
        emit(data){input.onmidimessage?.({data:new Uint8Array(data),target:input});},
        disconnect(){input.state='disconnected';access.inputs.delete(input.id);access.onstatechange?.({port:input});}
      };
      Object.defineProperty(navigator,'requestMIDIAccess',{configurable:true,value:async options=>{
        window.__fakeMidi.requested=options;return access;
      }});
    });
    await page.goto(process.env.TEST_URL || 'http://127.0.0.1:8000');
    await page.getByRole('button',{name:'Enable sound',exact:true}).click();
    await page.waitForFunction(()=>{const t=document.getElementById('status').textContent;return t.startsWith('Ready') || t!=='Sound is off.';});
    assert((await page.locator('#status').textContent()).startsWith('Ready'),await page.locator('#status').textContent());
    assert.equal(await page.locator('#build-status').textContent(),'Build 0.12.0 · if the selected and active engines ever disagree, reload this page.');
    assert.equal(await page.locator('#lead-engine').inputValue(),'1');
    for (const [value,label] of [['0','Sine Diagnostic'],['1','FM Glass'],['2','Virtual Analog Warm'],['3','Karplus–Strong String']]) {
      await page.locator('#lead-engine').selectOption(value);
      await page.waitForFunction(expected=>document.getElementById('engine-status').textContent===`Active in audio core · ${expected} · fixed musical preset`,label);
      assert(!(await page.locator('#lead-engine').evaluate(e=>e.classList.contains('engine-mismatch'))));
      await page.keyboard.down('a');
      await page.waitForFunction(()=>{const a=window.__audioProbe,x=new Float32Array(a.fftSize);a.getFloatTimeDomainData(x);return x.some(v=>Math.abs(v)>.005);});
      await page.keyboard.up('a');
      await page.waitForFunction(()=>{const a=window.__audioProbe,x=new Float32Array(a.fftSize);a.getFloatTimeDomainData(x);return x.every(v=>Math.abs(v)<.00001);});
    }
    await page.locator('#lead-engine').selectOption('1');
    await page.getByRole('button',{name:'Enable MIDI',exact:true}).click();
    await page.waitForFunction(()=>document.getElementById('midi-status').textContent==='Connected · SMK Pro Elite');
    assert.equal(await page.locator('#midi-input').inputValue(),'smk-pro-elite');
    assert.deepEqual(await page.evaluate(()=>window.__fakeMidi.requested),{sysex:false});
    await page.evaluate(()=>window.__fakeMidi.emit([0x90,60,37]));
    await page.waitForFunction(()=>document.getElementById('midi-status').textContent.includes('MIDI 60 · C4 → LEAD · velocity 37'));
    await page.waitForFunction(()=>{const a=window.__audioProbe,x=new Float32Array(a.fftSize);a.getFloatTimeDomainData(x);return x.some(v=>Math.abs(v)>.01);});
    await page.evaluate(()=>window.__fakeMidi.emit([0x80,60,0]));
    await page.waitForFunction(()=>{const a=window.__audioProbe,x=new Float32Array(a.fftSize);a.getFloatTimeDomainData(x);return x.every(v=>Math.abs(v)<.00001);});
    await page.keyboard.down('a');
    await page.waitForFunction(()=>{
      const a=window.__audioProbe,x=new Float32Array(a.fftSize);a.getFloatTimeDomainData(x);return x.some(v=>Math.abs(v)>.01);
    });
    await page.screenshot({path:'build/browser-desktop.png',fullPage:true});
    await page.keyboard.up('a');
    await page.waitForFunction(()=>{const a=window.__audioProbe,x=new Float32Array(a.fftSize);a.getFloatTimeDomainData(x);return x.every(v=>Math.abs(v)<.00001);});
    const key=page.getByRole('button',{name:'Play C4',exact:true});
    await key.hover();await page.mouse.down();
    await page.waitForFunction(()=>document.querySelectorAll('#keys .held').length===1);
    await page.mouse.up();
    await page.keyboard.down('s');
    await page.getByRole('button',{name:'Stop all notes'}).click();
    await page.waitForFunction(()=>{const a=window.__audioProbe,x=new Float32Array(a.fftSize);a.getFloatTimeDomainData(x);return x.every(v=>Math.abs(v)<.00001);});
    await page.keyboard.up('s');
    await page.locator('#capture-bars').selectOption('1');
    await page.locator('#count-in-bars').selectOption('1');
    await page.locator('#capture-quantize').selectOption('2');
    await page.locator('#bpm').fill('240');
    await page.waitForFunction(()=>document.getElementById('capture-clock-status').textContent.includes('1-bar count-in'));
    assert((await page.locator('#capture-clock-status').textContent()).includes('Grid sixteenth correction'));
    await page.locator('#drum-mode').selectOption('2');
    await page.locator('#drum-volume').fill('60');
    await page.waitForFunction(()=>document.getElementById('drum-status').textContent.includes('level 60'));
    // Empty take is safe, then record two notes with a rest between them.
    await page.getByRole('button',{name:'Capture',exact:true}).click();
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('Count-in'));
    await page.waitForFunction(()=>{const a=window.__audioProbe,x=new Float32Array(a.fftSize);a.getFloatTimeDomainData(x);return x.some(v=>Math.abs(v)>.001);});
    await page.getByRole('button',{name:'Finish',exact:true}).click();
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('No phrase'));
    assert(await page.getByRole('button',{name:'Replay',exact:true}).isDisabled());
    await page.getByRole('button',{name:'Capture',exact:true}).click();
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('Count-in'));
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('Recording'));
    assert((await page.locator('#clock-position').textContent()).startsWith('Bar 1 · Beat'));
    await page.keyboard.down('d'); await page.waitForTimeout(180); await page.keyboard.up('d');
    await page.waitForTimeout(100);
    const c4=page.getByRole('button',{name:'Play C4',exact:true});
    await c4.hover(); await page.mouse.down(); await page.waitForTimeout(220); await page.mouse.up();
    await page.waitForTimeout(80); await page.keyboard.down('j'); await page.waitForTimeout(160); await page.keyboard.up('j');
    await page.getByRole('button',{name:'Finish',exact:true}).click();
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('Phrase ready · 3 notes'));
    // Chord Companion: Root holds the key-center triad; Phrase follows Lead events.
    await page.locator('#keys-mode').selectOption('2');
    await page.getByRole('button',{name:'Replay',exact:true}).click();
    await page.waitForFunction(()=>document.getElementById('keys-status').textContent.includes('Root Follow · 3/4 voices active'));
    await page.getByRole('button',{name:'Stop all notes'}).click();
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('Phrase ready'));
    await page.locator('#keys-mode').selectOption('3');
    await page.getByRole('button',{name:'Replay',exact:true}).click();
    await page.waitForFunction(()=>document.getElementById('keys-status').textContent.includes('Phrase Follow · 3/4 voices active'));
    await page.locator('#input-track').selectOption('3');
    await page.evaluate(()=>window.__fakeMidi.emit([0x90,72,100]));
    await page.waitForFunction(()=>document.getElementById('keys-status').textContent.includes('1/4 voices active'));
    await page.evaluate(()=>window.__fakeMidi.emit([0x80,72,0]));
    await page.locator('#input-track').selectOption('0');
    await page.getByRole('button',{name:'Stop all notes'}).click();
    await page.locator('#keys-mode').selectOption('0');
    // Manual drum editing and a quantized manual bass pass use the same transport.
    await page.locator('#drum-mode').selectOption('1');
    await page.getByRole('button',{name:'Kick step 1',exact:true}).click();
    await page.getByRole('button',{name:'Snare step 5',exact:true}).click();
    await page.getByRole('button',{name:'Hat step 3',exact:true}).click();
    assert(await page.getByRole('button',{name:'Kick step 1',exact:true}).evaluate(b=>b.classList.contains('active')));
    await page.locator('#bass-mode').selectOption('1');
    await page.locator('#input-track').selectOption('1');
    await page.getByRole('button',{name:'Record bass pass',exact:true}).click();
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('Count-in'));
    await page.waitForFunction(()=>document.getElementById('sequencer-status').textContent.startsWith('Recording Bass'));
    await page.evaluate(()=>window.__fakeMidi.emit([0x90,36,108]));await page.waitForTimeout(180);
    await page.evaluate(()=>window.__fakeMidi.emit([0x80,36,0]));
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('Phrase ready'));
    await page.waitForFunction(()=>document.getElementById('sequencer-status').textContent.includes('bass 1'));
    // Dual-style split routes low notes to Bass and high notes to Lead from one controller.
    await page.locator('#input-track').selectOption('4');
    await page.locator('#split-point').selectOption('60');
    assert.equal(await page.locator('#split-point option:checked').textContent(),'C4 · MIDI 60');
    await page.waitForFunction(()=>document.getElementById('sequencer-status').textContent.includes('split at C4 (MIDI 60)'));
    await page.evaluate(()=>window.__fakeMidi.emit([0x90,48,100]));
    await page.waitForFunction(()=>document.getElementById('midi-status').textContent.includes('MIDI 48 · C3 → BASS'));
    assert(await page.locator('#midi-status').evaluate(e=>e.classList.contains('route-bass')));
    await page.evaluate(()=>window.__fakeMidi.emit([0x80,48,0]));
    await page.evaluate(()=>window.__fakeMidi.emit([0x90,72,100]));
    await page.waitForFunction(()=>document.getElementById('midi-status').textContent.includes('MIDI 72 · C5 → LEAD'));
    assert(await page.locator('#midi-status').evaluate(e=>e.classList.contains('route-lead')));
    await page.evaluate(()=>window.__fakeMidi.emit([0x80,72,0]));
    // Four-voice Poly Keys record simultaneous chord notes on the same 96-PPQN pass.
    await page.locator('#keys-mode').selectOption('1');
    await page.locator('#input-track').selectOption('3');
    await page.getByRole('button',{name:'Record keys pass',exact:true}).click();
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('Count-in'));
    await page.waitForFunction(()=>document.getElementById('sequencer-status').textContent.startsWith('Recording Poly Keys'));
    await page.evaluate(()=>{window.__fakeMidi.emit([0x90,60,100]);window.__fakeMidi.emit([0x90,64,96]);window.__fakeMidi.emit([0x90,67,92]);});
    await page.waitForFunction(()=>document.getElementById('keys-status').textContent.includes('3/4 voices active'));
    await page.waitForFunction(()=>document.getElementById('midi-status').textContent.includes('MIDI 67 · G4 → POLY KEYS'));
    assert(await page.locator('#midi-status').evaluate(e=>e.classList.contains('route-keys')));
    await page.waitForTimeout(180);
    await page.evaluate(()=>{window.__fakeMidi.emit([0x80,60,0]);window.__fakeMidi.emit([0x80,64,0]);window.__fakeMidi.emit([0x80,67,0]);});
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('Phrase ready'));
    await page.waitForFunction(()=>document.getElementById('sequencer-status').textContent.includes('keys 3'));
    await page.locator('#input-track').selectOption('0');
    await page.locator('#drum-mode').selectOption('2');
    await page.locator('#root').selectOption('2');
    await page.locator('#scale').selectOption('1');
    await page.waitForFunction(()=>document.getElementById('harmony-status').textContent==='Reply context · D natural minor');
    await page.locator('#adventure').fill('80');
    await page.waitForFunction(()=>document.getElementById('adventure-value').textContent==='80 · Bold');
    await page.locator('#bpm').fill('128');
    await page.locator('#groove-style').selectOption('2');
    await page.locator('#groove-amount').fill('80');
    await page.waitForFunction(()=>document.getElementById('groove-status').textContent==='Reply groove · Swing at 128 BPM');
    await page.locator('#phrase-style').selectOption('1');
    await page.locator('#phrase-amount').fill('100');
    await page.waitForFunction(()=>document.getElementById('architecture-status').textContent==='Reply architecture · Trap Pocket');
    await page.locator('#bass-mode').selectOption('2');
    await page.locator('#bass-density').selectOption('2');
    await page.locator('#bass-octave').selectOption('1');
    await page.locator('#bass-volume').fill('70');
    await page.locator('#bass-groove-style').selectOption('1');
    await page.locator('#bass-groove-amount').fill('55');
    await page.waitForFunction(()=>document.getElementById('bass-status').textContent==='Bass Companion · Root · Quarter notes · Auto 55 · octave 1');
    await page.getByRole('button',{name:'Reply',exact:true}).click();
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('Replying · Trap Pocket 100 · 10 notes · variation 1'));
    await page.waitForFunction(()=>{const a=window.__audioProbe,x=new Float32Array(a.fftSize);a.getFloatTimeDomainData(x);return x.some(v=>Math.abs(v)>.01);});
    await page.locator('#input-track').selectOption('1');
    await page.evaluate(()=>window.__fakeMidi.emit([0x90,28,110]));
    await page.waitForFunction(()=>document.getElementById('midi-status').textContent.includes('MIDI 28 · E1 → BASS'));
    assert((await page.locator('#phrase-status').textContent()).startsWith('Replying'));
    await page.evaluate(()=>window.__fakeMidi.emit([0x80,28,0]));
    await page.locator('#input-track').selectOption('0');
    await page.getByRole('button',{name:'Keep latest Reply',exact:true}).click();
    await page.waitForFunction(()=>document.getElementById('candidate-status').textContent.includes('Kept candidates · A'));
    assert.equal(await page.locator('#candidate-slot').locator('option').first().textContent(),'A · 10 notes');
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('Phrase ready · 3 notes'));
    await page.getByRole('button',{name:'Reply',exact:true}).click();
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('Replying · Trap Pocket 100 · 10 notes · variation 2'));
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('Phrase ready · 3 notes'));
    await page.getByRole('button',{name:'Play kept',exact:true}).click();
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('Playing kept candidate A · 10 notes'));
    await page.waitForFunction(()=>{const a=window.__audioProbe,x=new Float32Array(a.fftSize);a.getFloatTimeDomainData(x);return x.some(v=>Math.abs(v)>.01);});
    await page.getByRole('button',{name:'Stop all notes'}).click();
    await page.locator('#bass-mode').selectOption('3');
    await page.waitForFunction(()=>document.getElementById('bass-status').textContent.startsWith('Bass Companion · Follow'));
    await page.locator('#candidate-slot').selectOption('1');
    await page.getByRole('button',{name:'Keep latest Reply',exact:true}).click();
    await page.waitForFunction(()=>document.getElementById('candidate-status').textContent.includes('A, B'));
    await page.getByRole('button',{name:'Forget slot',exact:true}).click();
    await page.waitForFunction(()=>document.getElementById('candidate-status').textContent.includes('Kept candidates · A'));
    await page.locator('#candidate-slot').selectOption('0');
    await page.getByRole('button',{name:'Replay',exact:true}).click();
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('Replaying'));
    await page.waitForFunction(()=>{const a=window.__audioProbe,x=new Float32Array(a.fftSize);a.getFloatTimeDomainData(x);return x.some(v=>Math.abs(v)>.01);});
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('Phrase ready · 3 notes'));
    await page.getByRole('button',{name:'Replay',exact:true}).click();
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('Replaying'));
    await page.getByRole('button',{name:'Stop all notes'}).click();
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('Phrase ready · 3 notes'));
    await page.waitForFunction(()=>{const a=window.__audioProbe,x=new Float32Array(a.fftSize);a.getFloatTimeDomainData(x);return x.every(v=>Math.abs(v)<.00001);});
    await page.screenshot({path:'build/browser-desktop.png',fullPage:true});
    await page.setViewportSize({width:390,height:844});
    await page.screenshot({path:'build/browser-mobile.png',fullPage:true});
    assert(await page.evaluate(()=>document.documentElement.scrollWidth<=innerWidth));
    await page.getByRole('button',{name:'Clear',exact:true}).click();
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('No phrase'));
    assert(await page.getByRole('button',{name:'Replay',exact:true}).isDisabled());
    // Capture real note range and velocity through the browser Web MIDI connection.
    await page.getByRole('button',{name:'Capture',exact:true}).click();
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('Recording'));
    await page.evaluate(()=>window.__fakeMidi.emit([0x92,48,42])); await page.waitForTimeout(120);
    await page.evaluate(()=>window.__fakeMidi.emit([0x82,48,0])); await page.waitForTimeout(80);
    await page.evaluate(()=>window.__fakeMidi.emit([0x92,76,119])); await page.waitForTimeout(160);
    await page.evaluate(()=>window.__fakeMidi.emit([0x92,76,0]));
    await page.getByRole('button',{name:'Finish',exact:true}).click();
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('Phrase ready · 2 notes'));
    await page.getByRole('button',{name:'Replay',exact:true}).click();
    await page.waitForFunction(()=>{const a=window.__audioProbe,x=new Float32Array(a.fftSize);a.getFloatTimeDomainData(x);return x.some(v=>Math.abs(v)>.01);});
    await page.getByRole('button',{name:'Stop all notes'}).click();
    await page.evaluate(()=>{window.__fakeMidi.emit([0x90,60,100]);window.__fakeMidi.disconnect();});
    await page.waitForFunction(()=>document.getElementById('midi-status').textContent.includes('no connected input'));
    assert(await page.locator('#midi-input').isDisabled());
    await page.waitForFunction(()=>{const a=window.__audioProbe,x=new Float32Array(a.fftSize);a.getFloatTimeDomainData(x);return x.every(v=>Math.abs(v)<.00001);});
    assert.deepEqual(errors,[]);
    console.log('PASS Chromium: 0.12.0 Chord Companion plus 0.11.1 engine and 0.10.1 performance regressions');
  } finally {await browser.close();}
})().catch(e=>{console.error(e);process.exit(1);});



