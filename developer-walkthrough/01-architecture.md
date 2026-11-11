# Vapor Loop Developer Walkthrough — Lesson 01: Architecture

> **Developer walkthrough · Lesson 01**  
> Version studied: **Vapor Loop standalone v0.12.4** (November 10, 2026 package)  
> Learning principle: **Build → Test → Understand → Document**

## Who this is for

This is a beginner-friendly, source-based walkthrough of the browser implementation of Vapor Loop, an experimental music application developed toward a possible custom-firmware project for the M-VAVE FM-1. It is written for musicians, technical hobbyists, beginning programmers, and developers who want to examine the design.

**Current scope:** the *browser-based implementation* has been developed and tested. This walkthrough does **not** claim that v0.12.4 is installed as custom firmware on the physical FM-1.

## What we inspected

The reference archive `vapor-loop-standalone-0.12.4(1).zip` contains one file, `vapor-loop-standalone-0.12.4.html` (211,755 bytes). That HTML file includes:

- The visible user interface (HTML and CSS)
- Browser-side JavaScript for controls, state updates, and messages
- An embedded Base64-encoded **AudioWorklet JavaScript** module
- Embedded compiled **WebAssembly (WASM)** bytes for the engine

For this lesson, the AudioWorklet JavaScript was decoded for *reading*, not modified. **The original human-readable source code used to compile the WASM engine is not included in this ZIP.** The statements below therefore distinguish observable JavaScript behavior from internal engine algorithms that cannot yet be confirmed.

> **Source excerpts:** Unless otherwise marked, code blocks below come from the v0.12.4 HTML or its decoded embedded AudioWorklet. Some excerpts are formatted with extra whitespace or reduced to the relevant lines for readability; they are not proposed patches.

## 1. The architecture at a glance

```text
MPC / other MIDI controller       User changes a browser control
               |                              |
               v                              v
          Web MIDI API                 Browser JavaScript
               |                              |
               +-------------> messages <-----+
                                    |
                                    v
                           AudioWorklet (FM1)
                           receives commands
                                    |
                                    v
                          WebAssembly exports
                    (engine logic + audio rendering)
                                    |
                                    v
                      WASM left/right audio buffers
                                    |
                                    v
                      AudioWorklet output channels
                                    |
                                    v
                     Browser-only gain boost stage
                                    |
                                    v
                       Dynamics compressor/limiter
                                    |
                                    v
                           Audio device / speakers

Separate return path: AudioWorklet -> status/frame and
outgoing MIDI messages -> browser JavaScript.
```

**Key idea:** The *control path* delivers instructions; the *audio path* supplies samples to the speakers. They are related, but not the same thing.

### Terminology

| Term | Plain-English meaning |
| --- | --- |
| HTML | Describes the controls and content visible in the webpage |
| JavaScript | Handles browser interaction and coordinates messages |
| Event handler | A function that runs when something happens, such as moving a control |
| AudioContext | The browser's audio environment |
| AudioWorklet | A browser facility for processing blocks of audio off the main interface thread |
| WebAssembly (WASM) | A compiled binary program that the browser can execute |
| Export | A function or memory region made accessible by WASM to JavaScript |
| Audio frame | One instant of audio containing one sample per channel |
| Sample rate | The number of audio frames per second |
| `postMessage()` | Sends data between the main browser code and the AudioWorklet |
| `ready` | An application-level signal that engine initialization completed |

## 2. Sending controls: `syncGroove()`

The browser uses a small helper to send commands **only after initialization**:

```javascript
function send(m) { if (ready) node.port.postMessage(m); }
```

Here is the actual groove-control function:

```javascript
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
```

**Read it in plain English:**

1. Read the BPM control and use 120 if its numeric value is zero or invalid; clamp the result to **40–240 BPM**.
2. Read the selected groove style and timing amount.
3. Update the on-screen description.
4. Send a message containing `type`, `bpm`, `style`, and `amount`.
5. Run the function again when the relevant controls change.

Example of the resulting message when selecting 120 BPM, Swing, amount 70:

```javascript
{ type: 'groove', bpm: 120, style: 2, amount: 70 }
```

**Important:** This JavaScript does *not* directly shift MIDI events or generate swing audio. It sends configuration data to the processor.

### Following the message into the AudioWorklet

The worklet's constructor installs a message receiver:

```javascript
class FM1 extends AudioWorkletProcessor {
  constructor() {
    super()
    this.ex = null
    this.port.onmessage = (e) => this.onMessage(e.data)
  }
  // Other class methods omitted here.
}
```

Its message handler includes this condition, shown with readable formatting:

```javascript
else if (m.type === 'groove' && ex.echo_set_groove)
  ex.echo_set_groove(m.bpm | 0, m.style | 0, m.amount | 0)
```

- Check that the message is labeled `groove`.
- Check that the WASM module exposes `echo_set_groove`.
- Convert the three values to 32-bit integers using `| 0`.
- Call the compiled engine's exported groove-setting function.

The existence of that export **does not reveal the exact internal swing/pocket algorithm**. That requires the original engine source and/or focused behavioral testing.

## 3. Starting the engine: the readiness handshake

When **Enable Sound** is clicked, the browser requests an audio context at **44,100 frames per second**:

```javascript
$('start').onclick=async()=>{
  $('start').disabled=true;
  try{
    ctx=new AudioContext({sampleRate:44100});
    if(ctx.sampleRate!==44100)
      throw Error('This first build requires a 44.1 kHz audio context.');
    await ctx.resume();
    // AudioWorklet registration and other startup code follow.
```

The application then loads its embedded AudioWorklet module and creates the processor:

```javascript
node=new AudioWorkletNode(ctx,'fm1',{
  numberOfInputs:0,
  numberOfOutputs:1,
  outputChannelCount:[2]
});
```

This configures **one stereo audio output**, with no incoming audio channels. MIDI messages are separate from audio input channels.

The browser sends the compiled WASM bytes and initial master-volume setting:

```javascript
node.port.postMessage(
  {type:'load',wasm,master:Number($('volume').value)},
  [wasm]
);
```

In the AudioWorklet, the load branch includes:

```javascript
const { instance } = await WebAssembly.instantiate(m.wasm, {})
const ex = instance.exports
this.mem = ex.memory
// Optional saved-store restoration (only when m.store is supplied).
ex.web_master(m.master)
this.sentFb = -1
this.sentStore = ex.web_store_gen()
this.last = 0
this.midi = []
ex.web_boot()
this.ex = ex
this.port.postMessage({ type: 'ready' })
```

**Sequence:** instantiate WASM → access its exports and memory → configure and initialize → call `web_boot()` → make exports available → report `ready`.

Upon receiving `ready`, the main page sets `ready=true`, enables relevant controls, and invokes its synchronization functions:

```javascript
syncLeadEngine();syncHarmonyMode();syncAdventure();syncGroove();
syncCaptureClock();syncPhrase();syncBass();syncBassGroove();
syncKeys();syncSequencer();syncDrums();
```

Those calls send the **currently displayed settings** into the newly initialized processor. `syncGroove()` therefore serves both initial setup and subsequent user edits.

**Why wait?** Until `ready` arrives, the normal `send()` helper ignores control messages. The page does not assume that merely displaying a control means the engine can already accept commands.

### Failure handling

The worklet can report a caught error through:

```javascript
crash(err) {
  this.ex = null
  this.port.postMessage({ type: 'crash', message: String((err && err.message) || err) })
}
```

The browser handles a `crash` message by setting `ready=false`, displaying `Audio stopped: ...`, and disconnecting the node. It also installs a separate `onprocessorerror` handler. These mechanisms cover documented failure paths, **not necessarily every possible failure**. In particular, `ready` means startup finished; it is not proof that sound is audible or all functions have passed tests.

## 4. Rendering audio continuously: `process()`

This is the actual processing method from the decoded v0.12.4 AudioWorklet:

```javascript
process(inputs, outputs) {
  const ex = this.ex
  if (!ex) return true
  const out = outputs[0]
  const n = out[0].length
  try {
    ex.web_render(n)
    for (let m; (m = ex.web_midi_out()) >= 0;) this.midi.push(m)
  } catch (err) {
    this.crash(err)
    return true
  }
  out[0].set(new Float32Array(this.mem.buffer, ex.web_out_l(), n))
  if (out[1]) out[1].set(new Float32Array(this.mem.buffer, ex.web_out_r(), n))
  if (currentTime - this.last >= 1 / 30) {
    this.last = currentTime
    this.publish()
  }
  return true
}
```

**Step-by-step:**

1. Check whether the engine is loaded (`this.ex`).
2. Find how many audio frames the browser requested (`n`).
3. Ask WASM to render those frames: `ex.web_render(n)`.
4. Collect any outgoing MIDI data available through `ex.web_midi_out()`.
5. Copy left and right samples from exported WASM memory into the browser's output buffers.
6. Periodically call `publish()` to report state to the main page.
7. Return `true` to indicate the processor intends to continue operating.

**Why `Float32Array`?** The sample data are read as 32-bit floating-point values. `new Float32Array(...)` creates a *view* into WASM memory; `.set(...)` copies those values into the destination output buffer.

### Audio blocks versus screen updates

A **128-frame** audio block at 44.1 kHz lasts approximately **2.90 milliseconds**, or about **344.5 blocks per second**. This is an illustrative block size consistent with the embedded source comments, not a guarantee that all browser environments always use 128 frames. The actual code uses `out[0].length` rather than hard-coding 128.

Status messages are handled differently:

```javascript
if (currentTime - this.last >= 1 / 30) {
  this.last = currentTime
  this.publish()
}
```

This schedules publication at approximately **30 times per second**, subject to render-block scheduling. The display doesn't need updating at audio-block frequency.

**Engineering tradeoff:** Outgoing MIDI is collected during audio rendering and forwarded with published status data. This architecture introduces a possible delivery delay before the main browser thread handles outgoing MIDI; **no timing measurement is established by this source review**.

### Where v0.12.4's gain boost fits

The browser creates a gain stage and a dynamics compressor *after* the AudioWorklet, leaving the WASM/FM-1 gain model unchanged:

```javascript
browserBoost=ctx.createGain();
browserLimiter=ctx.createDynamicsCompressor();
browserLimiter.threshold.value=-3;
browserLimiter.knee.value=0;
browserLimiter.ratio.value=20;
browserLimiter.attack.value=0.003;
browserLimiter.release.value=0.12;
node.connect(browserBoost);
browserBoost.connect(browserLimiter);
browserLimiter.connect(ctx.destination);
```

The browser UI defaults to **+12 dB boost**. The compressor has a **−3 dB threshold** and high ratio, but is not a mathematically guaranteed brick-wall peak limiter. Earlier listening tests found the browser output louder through the MPC; low screen-recording volume was **not conclusively diagnosed**. The gain-staging implementation remains frozen for this educational review.

## 5. What we can confirm, and what we cannot yet confirm

| Source-supported observation | Still requires further evidence |
| --- | --- |
| Browser JavaScript sends groove settings | The exact WASM swing/pocket timing formulas |
| AudioWorklet calls `echo_set_groove` if exported | How every groove style transforms individual note events |
| `web_render(n)` is called per processing block | The complete internal synthesis and scheduler design |
| Audio samples are copied from WASM memory | Runtime CPU usage, underruns, or latency on particular systems |
| The browser uses a post-WASM gain/compression stage | Guaranteed true-peak limiting or recording-path equivalence |
| The worklet reports `ready` after boot | Guaranteed audible output on every browser and audio device |

To dig into these unknowns, future lessons should examine the original engine source (if available) and controlled tests. We should not reverse-engineer undocumented internal behavior by assumption.

## 6. Engineering decisions and current limitations

| Decision | Reason / benefit | Limitation or question |
| --- | --- | --- |
| Separate browser controls from audio rendering | Keeps user interaction distinct from repeated audio work | Messages cross a thread boundary |
| Use AudioWorklet + WASM | Uses a compiled engine within browser audio processing | Internal algorithms aren't readable from this ZIP |
| Guard normal messages with `ready` | Avoids sending controls before successful engine setup | Startup and recovery paths still need testing |
| Require 44.1 kHz in this build | Matches the build's explicit audio requirements | Other sample rates are rejected |
| Publish state ~30 times/s | Avoids sending visual updates for each audio block | Can add delay to browser-handled outgoing MIDI |
| Apply browser-only gain and compression | Makes output louder without changing the compiled engine | Compressor isn't a guaranteed peak limiter |

### Questions to revisit later (not changes approved now)

- Can we obtain the original C/engine source used for this binary to document musical algorithms precisely?
- What is measured MIDI-output timing through the `publish()` path?
- How does the application respond to device changes, underruns, or WASM memory problems?
- Is its existing error handling sufficient for recovery without page reload?
- How do the current browser features map to the eventual **physical FM-1 firmware** scope?

## 7. Knowledge check — our Lesson 01 answers

1. **B:** JavaScript reads BPM and sends a message.
2. **B:** Changing groove amount updates the displayed value and sends settings; the UI function does not shift notes itself.
3. **B:** Groove settings travel browser JavaScript → AudioWorklet → exported WASM function.
4. **B:** Normal control messages wait for successful initialization and a `ready` signal.
5. **B:** Audio is processed in frequent small blocks; visual/status updates are less frequent.

### Practice explaining it to someone else

> "Vapor Loop currently runs as a browser-based musical application. JavaScript handles the interface and MIDI-related control messages. An AudioWorklet receives those messages, calls functions exposed by a compiled WebAssembly engine, and copies generated audio into the browser's stereo audio buffers. The page applies its own gain/compression after the engine. We are documenting and testing this browser version before attempting a physical FM-1 firmware port."

## 8. Transparency about AI-assisted development

Vapor Loop is an **AI-assisted development and human-directed experimentation** project. AI has assisted with generating and explaining implementation code and documentation. The project lead defines musical goals, evaluates behavior using actual MIDI hardware, reports bugs, requests changes, selects development priorities, and is learning to inspect and explain the resulting code.

The goal is **not** to imply the project lead hand-authored every source line or to treat AI-generated code as automatically correct. Source inspection, real-device tests, documented limitations, and public learning are core parts of the process. Readers are welcome to learn from the material and suggest improvements; any future contributions and upstream licensing obligations must be evaluated on their own terms.

## 9. Source and licensing note

This lesson is based specifically on the supplied v0.12.4 standalone ZIP and its embedded AudioWorklet. The project repository was checked to select the documentation path and review existing attribution, but its contents were not used to infer undocumented engine behavior. The standalone HTML contains upstream notices, including GPL-3.0-related text. **Do not assume the complete project's distribution rights or licensing terms from this walkthrough alone**; verify provenance, applicable license notices, attribution, and obligations before distributing source or derivative firmware.

## Next step

Future lessons will examine specific MIDI processing and timing behavior using available source code and controlled tests. This lesson does not introduce any application or firmware changes.