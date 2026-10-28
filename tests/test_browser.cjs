// Requires the built site served at localhost:8000. Tests real browser audio samples.
const {chromium}=require('playwright');
const assert=require('node:assert/strict');
(async()=>{
  const browser=await chromium.launch({headless:true,
    executablePath:process.env.PLAYWRIGHT_CHROMIUM_EXECUTABLE_PATH||undefined,
    args:['--no-sandbox','--autoplay-policy=no-user-gesture-required']});
  try {
    const page=await browser.newPage({viewport:{width:1100,height:1000}});
    const errors=[];page.on('pageerror',e=>errors.push(e.message));
    await page.addInitScript(()=>{
      const Original=window.AudioWorkletNode;
      window.AudioWorkletNode=class extends Original {
        constructor(ctx,...args){super(ctx,...args);window.__audioProbe=ctx.createAnalyser();this.connect(window.__audioProbe);}
      };
    });
    await page.goto('http://127.0.0.1:8000');
    await page.getByRole('button',{name:'Enable sound',exact:true}).click();
    await page.waitForFunction(()=>document.getElementById('status').textContent.startsWith('Ready'));
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
    await page.setViewportSize({width:390,height:844});
    await page.screenshot({path:'build/browser-mobile.png',fullPage:true});
    assert(await page.evaluate(()=>document.documentElement.scrollWidth<=innerWidth));
    assert.deepEqual(errors,[]);
    console.log('PASS Chromium: page load, worklet startup, nonzero audio samples, release, pointer key, stop, mobile width, no JS errors');
  } finally {await browser.close();}
})().catch(e=>{console.error(e);process.exit(1);});
