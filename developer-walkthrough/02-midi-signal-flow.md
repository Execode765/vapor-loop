# Vapor Loop Developer Walkthrough — Lesson 02: MIDI Signal Flow

> **Developer walkthrough · Lesson 02**  
> Version studied: **Vapor Loop standalone v0.12.4** (HTML packaged November 10, 2026)  
> Method: **Build → Test → Understand → Document → Verify**  
> Status: **Published educational walkthrough**

## Who this lesson is for

This lesson is for readers who know how to play a MIDI controller but may not yet know how a browser application handles MIDI. It follows the actual **v0.12.4 standalone browser build** of Vapor Loop, a music application developed toward a possible custom-firmware project for the M-VAVE FM-1.

The goal is to understand the system without suggesting that the browser prototype is already running as firmware on a physical FM-1. Code excerpts are copied from the provided standalone HTML or its embedded AudioWorklet, except where explicitly labeled *reformatted*, *shortened*, or *illustrative*.

**Scope:** MIDI reception, channel routing, Note On/Off, velocity, panic commands, the browser-to-WASM boundary, collection of MIDI output events, and timing implications. No code was modified or executed as part of writing this lesson.

## 1. MIDI is a message, not an audio recording

In the standard MIDI 1.0 channel-message format, a common Note On message has three bytes:

| Byte | Example | Meaning |
| --- | --- | --- |
| Status | `0x90` (144) | Note On, channel 1 |
| Data 1 | `48` | C3 (using the octave naming convention in this app) |
| Data 2 | `100` | Note-on velocity |

A typical Note Off on channel 1 is `[128, 48, 0]` (`0x80`, note 48, release velocity 0). Another valid note-release convention is Note On with **velocity zero**. Velocity is information; it does **not** automatically mean an equal percentage of speaker volume. How the sound reacts depends on the instrument engine.

**Mental model:** MIDI is like a set of instructions sent to a performer. Audio is the sound produced when those instructions are interpreted.

## 2. The verified MIDI path in v0.12.4

```text
MPC / MIDI controller
       |  incoming MIDI bytes
       v
Browser Web MIDI API
       |  onMidiMessage(event)
       v
JavaScript track + split routing
       |  send({ type: 'midi', data: [...] })
       v
AudioWorklet message port
       |  onMessage(m)
       v
WebAssembly export: web_midi_in(...)
       |
       v
Compiled musical engine
       |                              |
       | audio-rendering path         | outgoing MIDI-event path
       v                              v
web_render(n)                  web_midi_out() values
       |                              |
WASM sample buffers            worklet's this.midi queue
       |                              |
Browser audio graph            publish() frame message
(gain / limiter / output)              |
       |                       main browser thread
       v                       (no external MIDI send
Speakers / audio device         implementation found)
```

This diagram follows the application's observable interfaces. The musical algorithms *inside* the compiled engine are not established by these browser/AudioWorklet excerpts alone.

## 3. Asking the browser for MIDI access

From the standalone HTML:

```javascript
midiAccess=await navigator.requestMIDIAccess({sysex:false});
midiAccess.onstatechange=refreshMidiInputs;
$('midi-enable').textContent='MIDI enabled';
refreshMidiInputs();
```

**In plain English:**

- `requestMIDIAccess()` asks the browser to provide MIDI-device access (subject to browser support and permission).
- `sysex:false` means this request does not seek System Exclusive permission.
- `onstatechange` calls a refresh function when device connection state changes.
- `refreshMidiInputs()` populates and chooses from available inputs.

The code also checks whether `navigator.requestMIDIAccess` exists and displays an error if permission or access fails. **Discovering an input is not the same as sending MIDI to an output.**

### Attaching a listener to one selected input

```javascript
function chooseMidiInput(id) {
  if (midiInput) midiInput.onmidimessage=null;
  midiPanic();
  midiInput=id && midiAccess ? midiAccess.inputs.get(id)||null : null;
  if (midiInput) {
    midiInput.onmidimessage=onMidiMessage;
    setMidiStatus(`Connected · ${midiInput.name||'Unnamed MIDI input'}`);
  } else if (midiAccess) setMidiStatus('No MIDI input selected.');
}
```

**Translation:** stop listening to the previous input, attempt an All Notes Off safety step, select the desired input, and attach the new `onMidiMessage` handler. This is an *event-driven* design: JavaScript responds when a MIDI message arrives.

## 4. Reading and routing Note On / Note Off

Here is the **complete relevant conditional block** from `onMidiMessage(event)` in the provided standalone HTML:

```javascript
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
```

**Key pieces:**

- `status & 0xf0` extracts the message category independently of the incoming channel.
- `& 0x7f` keeps a MIDI data value in the 0–127 range.
- `0x90` is Note On; `0x80` is Note Off.
- `kind === 0x90 && value > 0` is the browser **display's** test for a pressed note; it treats Note On with velocity 0 as a release **in the status display**. The original message category and value still go to the engine.
- `inputTrack===4` enables the split comparison: below the split point means Bass (`1`); at or above it means Lead (`0`).
- `routedChannel` is zero-based: channel index `0` = MIDI channel 1 (Lead), `1` = channel 2 (Bass), `2` = channel 3 (Poly Keys). This app maps the Poly Keys input-track value `3` to channel index `2`.
- Other ordinary incoming control changes, pitch bend, aftertouch, and program changes are not generally forwarded by this handler. The specific exception here is controller 120 or 123.

### Worked MPC example: C3 in Split mode

Suppose the split point is C4 / MIDI note 60, and the MPC sends a Note On for note 48 at velocity 100 on channel 1.

```text
Incoming Note On:  [144, 48, 100]
48 < 60:           yes — choose Bass
Routed Note On:    [145, 48, 100]
Worklet invocation: web_midi_in(145, 48, 100)
```

**Why 145?** The message remains Note On, but the status byte changes from `0x90` (channel 1) to `0x91` (channel 2). Note and velocity remain `48` and `100`. The note is **routed**, not discarded.

## 5. Why there is an All Notes Off safety function

If the controller disconnects after a Note On but before its corresponding Note Off, an instrument may keep a voice active. That is one way a *hanging note* can occur.

Actual v0.12.4 source:

```javascript
function midiPanic() {
  if (!ready) return;
  for (let channel=0; channel<16; channel++) send({type:'midi',data:[0xb0|channel,123,0]});
}
```

This sends **Control Change 123: All Notes Off** on all 16 MIDI channels, if the processor is ready. It is a precaution, **not a universal guarantee** that every instrument voice or sound will stop immediately.

Also note the additional conditional in `onMidiMessage()` above: incoming **CC120 (All Sound Off)** and **CC123 (All Notes Off)** are forwarded to the engine using their incoming status/channel. The browser currently labels both in its display as “all notes off”; the MIDI controller numbers have distinct meanings.

### Possible edge case, not a confirmed defect

The browser calculates the destination channel independently for each Note On or Note Off. If a performer changes Split mode or the split point *while holding a note*, the release message could be routed differently from the original press. This is a source-based **test hypothesis**; no failure has been reproduced in this review.

## 6. Sending the message into the worklet and WASM

The main-thread helper from the HTML is:

```javascript
function send(m) { if (ready) node.port.postMessage(m); }
```

The receiving AudioWorklet contains:

```javascript
this.port.onmessage = (e) => this.onMessage(e.data)
```

And, in its message handler:

```javascript
else if (m.type === 'midi') ex.web_midi_in(m.data[0], m.data[1] || 0, m.data[2] || 0)
```

**Translation:** when ready, the browser sends a small object across a message port; the worklet recognizes a MIDI command and passes its three numeric values to `web_midi_in()` in the initialized WebAssembly engine.

`web_midi_in()` is a verified *interface*. This excerpt does not prove exactly how an incoming note affects internal sequencing, accompaniment, envelopes, or voice allocation. Those details need engine source review and/or experiments.

## 7. What the engine does with outgoing MIDI values

The worklet's recurring audio process includes:

```javascript
ex.web_render(n)
for (let m; (m = ex.web_midi_out()) >= 0;) this.midi.push(m)
```

`web_render(n)` advances the engine and renders an audio block. The loop then retrieves nonnegative values from the engine's MIDI-output interface until it returns a negative sentinel value, accumulating them in `this.midi`.

Later in the worklet's `publish()` function:

```javascript
if (this.midi.length) {
  msg.midi = this.midi
  this.midi = []
}
this.port.postMessage(msg, transfer)
```

This puts accumulated values into a frame/status message and sends it to the main browser thread. It **does not itself transmit bytes to an external MIDI device**.

The main browser frame handler in the standalone build handles display and phrase updates, but we did not find a corresponding `m.midi` output handler or selection of a `midiAccess.outputs` port followed by a hardware MIDI `output.send(...)` call.

**Therefore:** the browser build implements internal collection and publication of outgoing engine MIDI events, **but external Web MIDI output transmission is not implemented in the inspected standalone application**. This does not negate its internal audio playback. It also does not establish the exact encoding of the values returned by `web_midi_out()`; decode rules would need separate source analysis.

## 8. Timing: what the source does and does not prove

The worklet's `process()` routine renders blocks often and publishes status less often:

```javascript
const out = outputs[0]
const n = out[0].length
// ...
ex.web_render(n)
// ...
if (currentTime - this.last >= 1 / 30) {
  this.last = currentTime
  this.publish()
}
```

This excerpt is **shortened** from the actual function. At the requested sample rate of 44,100 Hz, a **128-frame example block** lasts about **2.90 ms**. The `1/30` threshold is about **33.33 ms**; publication is checked at rendering boundaries, so it is not a promise of exactly 30 deliveries per second.

Outgoing MIDI values can remain queued until the next `publish()`. That establishes a **possible batching/delivery delay** for the outgoing-event reporting path, not measured keypress-to-speaker latency. Audio samples use a distinct path through `web_render()` and the audio graph. The build has not been benchmarked here for MIDI or audio latency, nor for jitter.

### A no-code test plan for later (not performed)

1. Use the actual v0.12.4 build with the MPC and note the tested hardware/browser/audio settings.
2. Set a known BPM and an audible metronome; choose a fixed capture length and a count-in.
3. Record an intentionally simple pattern using direct computer audio capture, if practical.
4. Repeat with Free, Humanized, and Grid capture correction; keep other controls constant.
5. Compare alignment and consistency in an editor **while acknowledging that human playing and recording buffers introduce uncertainty**.
6. Report observations separately from explanations. A repeatable timestamped MIDI source and synchronized measurement setup would be needed for stronger end-to-end latency claims.

A previous observation that a musical phrase *sounded synchronized* is useful real-world feedback. It is **not** an instrument-grade measurement, and outgoing MIDI batching does not automatically describe internal audio scheduling.

## 9. What we can say confidently

| Statement | Classification |
| --- | --- |
| The HTML requests MIDI input access and attaches `onMidiMessage()` | **Source-confirmed** |
| Notes are routed to Lead, Bass, or Poly Keys using browser-side track/split logic | **Source-confirmed** |
| The browser forwards routed Note On/Off and selected panic commands to WASM | **Source-confirmed** |
| `midiPanic()` issues CC123 across all 16 channels when ready | **Source-confirmed** |
| The worklet pulls outgoing MIDI values and includes them in periodic frame messages | **Source-confirmed** |
| The standalone build sends those events to an external synth through Web MIDI | **Not implemented in inspected source** |
| Changing the split while holding notes causes hanging notes | **Possible edge case; not tested** |
| The outgoing publication interval proves exactly 33 ms total latency | **Not supported** |
| The exact internal musical note and timing algorithms are fully explained here | **Not established by this layer of source** |
| Browser MIDI + audio work proves this code is installed as hardware FM-1 firmware | **Not established** |

## 10. Practice explaining it to another developer

> Vapor Loop uses the browser's Web MIDI API to receive controller messages. JavaScript identifies note events and routes them to the selected internal track, including Lead/Bass keyboard splitting. The browser forwards the routed MIDI bytes to an AudioWorklet, which invokes the compiled WebAssembly engine's `web_midi_in()` interface. The engine renders audio and exposes outgoing MIDI values; the worklet collects and periodically publishes those values. In this standalone build, audio reaches the browser output, while an external Web MIDI output connection is not implemented. We document source-confirmed behavior separately from unmeasured latency and engine internals.

### Quick learning review (answers)

1. **Split point 60, play note 48:** Bass receives the routed MIDI message (not ignored). **B**.
2. **Why `midiPanic()` when switching inputs?** It sends All Notes Off across 16 channels to reduce hanging-note risk. **B**.
3. **Does `web_midi_out()` alone prove hardware output?** No; the final browser-to-device send path is still needed. **B**.
4. **Is a 30 Hz publication interval exactly 33 ms of MIDI latency?** No; that is a code timing condition, not an end-to-end benchmark. **B**.
5. **Who chooses Lead or Bass in Split mode?** Browser JavaScript in `onMidiMessage()`. **A**.

Learning notes are not a formal skills certification; these are comprehension checks from this walkthrough.

## 11. Mini glossary

| Term | Plain-English meaning |
| --- | --- |
| MIDI | Messages describing musical actions and control changes; not recorded audio |
| Note On / Note Off | Instructions to begin a note and release it |
| Velocity | A 0–127 value representing playing intensity or release characteristics, interpreted by the receiver |
| Status byte | MIDI byte identifying the type of message and channel |
| MIDI channel | One of 16 logical channels in a standard MIDI port |
| Event handler | A function called when an event, such as incoming MIDI, occurs |
| Web MIDI API | Browser interface for supported MIDI input/output devices |
| AudioWorklet | Browser environment for processing audio blocks |
| WebAssembly (WASM) | Compiled code that runs within a browser-hosted environment |
| Export | A WASM function or object made callable from JavaScript |
| Buffer | A place where samples or events are temporarily stored |
| Latency | Delay between an input event and an observed result |
| Jitter | Variation in delay between repeated events |
| All Notes Off | MIDI Control Change 123, requesting release of active notes |

## 12. Engineering record and next questions

**Why these design choices matter:** Browser-level routing makes the destination choice visible and easy to explain; a worklet/WASM boundary separates UI events from audio rendering; a panic command reduces one common class of MIDI failure; queued publication reduces how often status data is sent to the main thread. These are **observed structural properties**, not claims that the current implementation is optimal.

**Candidate follow-up investigations:**

- Check the WASM/C source of `web_midi_in()` and `web_midi_out()` and their event formats.
- Test held notes while changing the split point or selected input track.
- Measure event and audio timing with a reproducible external reference instead of estimating from code.
- Determine what future external MIDI-out support would require, including port selection, event decoding, scheduling, and disconnect handling.

These are **future questions**, not feature changes authorized by this lesson.

## 13. AI assistance, authorship, and verification

Vapor Loop is an **AI-assisted, human-directed development and testing project**. AI has helped produce and explain implementation code and educational documentation. The project lead directs the musical objectives, tests the browser prototype with MIDI equipment, reports observed behavior, chooses priorities, and reviews the material to deepen technical understanding.

AI assistance is **not** evidence by itself that code is correct, hardware firmware has been deployed, timing is precise, or every musical feature has been verified. This educational explanation is grounded in the supplied standalone v0.12.4 HTML and its embedded AudioWorklet; unknowns are labeled. Upstream credits, licenses, and authorship remain separate from AI-attribution statements. See the repository's `AI_ATTRIBUTION.md` and `LICENSE` for overarching notices.

## 14. Source provenance and reproduction

- **Primary artifact inspected:** `vapor-loop-standalone-0.12.4.html`, the sole file within `vapor-loop-standalone-0.12.4(1).zip` supplied for the walkthrough.
- **Readable browser layer:** JavaScript in the standalone HTML, particularly `send()`, `midiPanic()`, `onMidiMessage()`, `chooseMidiInput()`, and MIDI enabling/refresh functions.
- **Readable worklet layer:** AudioWorklet JavaScript embedded in the HTML as encoded text, decoded for inspection; important functions include `onMessage()`, `process()`, and `publish()`.
- **Limits:** This lesson does not reverse engineer or establish the detailed internals of the compiled WASM; it does not include a hardware FM-1 firmware test or a new timing benchmark.
- **Version note:** Assertions refer to this standalone artifact; later branches, build outputs, or firmware ports may differ.

**Publication:** This educational document is version-controlled at `developer-walkthrough/02-midi-signal-flow.md` in `Execode765/vapor-loop`. The documentation commit does not change application source code.