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
    });
    await page.goto(process.env.TEST_URL || 'http://127.0.0.1:8000');
    await page.getByRole('button',{name:'Enable sound',exact:true}).click();
    await page.waitForFunction(()=>{const t=document.getElementById('status').textContent;return t.startsWith('Ready') || t!=='Sound is off.';});
    assert((await page.locator('#status').textContent()).startsWith('Ready'),await page.locator('#status').textContent());
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
    await page.getByRole('button',{name:'Finish',exact:true}).click();
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('Phrase ready · 2 notes'));
    await page.getByRole('button',{name:'Replay',exact:true}).click();
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('Replaying'));
    await page.waitForFunction(()=>{const a=window.__audioProbe,x=new Float32Array(a.fftSize);a.getFloatTimeDomainData(x);return x.some(v=>Math.abs(v)>.01);});
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('Phrase ready · 2 notes'));
    await page.getByRole('button',{name:'Replay',exact:true}).click();
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('Replaying'));
    await page.getByRole('button',{name:'Stop all notes'}).click();
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('Phrase ready · 2 notes'));
    await page.waitForFunction(()=>{const a=window.__audioProbe,x=new Float32Array(a.fftSize);a.getFloatTimeDomainData(x);return x.every(v=>Math.abs(v)<.00001);});
    await page.screenshot({path:'build/browser-desktop.png',fullPage:true});
    await page.setViewportSize({width:390,height:844});
    await page.screenshot({path:'build/browser-mobile.png',fullPage:true});
    assert(await page.evaluate(()=>document.documentElement.scrollWidth<=innerWidth));
    await page.getByRole('button',{name:'Clear',exact:true}).click();
    await page.waitForFunction(()=>document.getElementById('phrase-status').textContent.startsWith('No phrase'));
    assert(await page.getByRole('button',{name:'Replay',exact:true}).isDisabled());
    assert.deepEqual(errors,[]);
    console.log('PASS Chromium: live audio, keyboard/mouse capture, empty take, 2-note replay, stop preserves phrase, clear, mobile width, no JS errors');
  } finally {await browser.close();}
})().catch(e=>{console.error(e);process.exit(1);});
