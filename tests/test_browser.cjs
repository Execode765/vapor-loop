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
    await page.getByRole('button',{name:'Enable MIDI',exact:true}).click();
    await page.waitForFunction(()=>document.getElementById('midi-status').textContent==='Connected · SMK Pro Elite');
    assert.equal(await page.locator('#midi-input').inputValue(),'smk-pro-elite');
    assert.deepEqual(await page.evaluate(()=>window.__fakeMidi.requested),{sysex:false});
    await page.evaluate(()=>window.__fakeMidi.emit([0x90,60,37]));
    await page.waitForFunction(()=>document.getElementById('midi-status').textContent.includes('C4 · velocity 37'));
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
    // Empty take is safe, then record two notes with a rest between them.
    await page.getByRole('button',{name:'Capture',exact:true}).click();
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('Armed'));
    await page.getByRole('button',{name:'Finish',exact:true}).click();
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('No phrase'));
    assert(await page.getByRole('button',{name:'Replay',exact:true}).isDisabled());
    await page.getByRole('button',{name:'Capture',exact:true}).click();
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('Armed'));
    await page.keyboard.down('d'); await page.waitForTimeout(180); await page.keyboard.up('d');
    await page.waitForTimeout(100);
    const c4=page.getByRole('button',{name:'Play C4',exact:true});
    await c4.hover(); await page.mouse.down(); await page.waitForTimeout(220); await page.mouse.up();
    await page.waitForTimeout(80); await page.keyboard.down('j'); await page.waitForTimeout(160); await page.keyboard.up('j');
    await page.getByRole('button',{name:'Finish',exact:true}).click();
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('Phrase ready · 3 notes'));
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
    await page.getByRole('button',{name:'Reply',exact:true}).click();
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('Replying · Trap Pocket 100 · 10 notes · variation 1'));
    await page.waitForFunction(()=>{const a=window.__audioProbe,x=new Float32Array(a.fftSize);a.getFloatTimeDomainData(x);return x.some(v=>Math.abs(v)>.01);});
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('Phrase ready · 3 notes'));
    await page.getByRole('button',{name:'Reply',exact:true}).click();
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('Replying · Trap Pocket 100 · 10 notes · variation 2'));
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('Phrase ready · 3 notes'));
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
    await page.evaluate(()=>window.__fakeMidi.emit([0x91,48,42])); await page.waitForTimeout(120);
    await page.evaluate(()=>window.__fakeMidi.emit([0x81,48,0])); await page.waitForTimeout(80);
    await page.evaluate(()=>window.__fakeMidi.emit([0x91,76,119])); await page.waitForTimeout(160);
    await page.evaluate(()=>window.__fakeMidi.emit([0x91,76,0]));
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
    console.log('PASS Chromium: phrase architecture changes reply count, Adventure, groove, Web MIDI, exact replay, mobile width, no JS errors');
  } finally {await browser.close();}
})().catch(e=>{console.error(e);process.exit(1);});
