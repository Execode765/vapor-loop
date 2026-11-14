# Vapor Loop — Developer Walkthrough

> **Educational reference:** Vapor Loop browser prototype v0.12.4  
> **Current engineering priority:** original firmware development for the physical M-VAVE FM-1  
> **Method:** Build → Test → Understand → Document → Verify

## Why this walkthrough exists

The browser prototype is an important part of Vapor Loop's engineering history. It provided a way to explore musical ideas, inspect MIDI and audio paths, test behavior with real controllers, and learn from changes before attempting a physical-firmware implementation.

These lessons explain **what the browser prototype actually does, how the readable code is organized, why selected engineering decisions were made, and what remains unverified**. They are useful to beginners, musicians, software developers, and anyone learning to work with AI-assisted code.

**This is a historical, version-specific learning resource—not an assertion that the browser application has been ported to, flashed onto, or validated as original firmware on the FM-1.**

## Two complementary development tracks

| Track | Purpose | Priority |
| --- | --- | --- |
| **Browser prototype walkthrough** | Explain and preserve the architecture, MIDI handling, timing, phrase/groove behavior, audio pipeline, tests, and design decisions of the v0.12.4 prototype | Educational; continue selectively without delaying hardware work |
| **Physical FM-1 firmware development** | Research the hardware, establish recovery and backup procedures, determine firmware feasibility, implement carefully, and validate behavior on the device | **Primary engineering work** |

The browser prototype may suggest desirable *musical behavior* for the physical instrument. It does **not** establish that its Web MIDI, Web Audio, AudioWorklet, JavaScript, or WebAssembly implementation can be copied directly onto the hardware. The device's actual interfaces, timing facilities, memory, processing limits, boot and recovery paths, and firmware architecture must be established independently.

For current engineering status, see the [repository README](../README.md) and the [FM-1 recovery preparation record](../docs/FM1_RECOVERY_PREPARATION.md). Do not interpret a documented prototype feature as a completed hardware feature.

## Published lessons

1. [Lesson 01 — Architecture](01-architecture.md): browser UI, JavaScript, AudioWorklet, WASM interface, initialization, continuous rendering, and limitations.
2. [Lesson 02 — MIDI Signal Flow](02-midi-signal-flow.md): device input, Note On/Off, routing, stuck-note precautions, WASM handoff, outgoing-event collection, timing considerations, and limitations.

These lessons are preserved as records of **v0.12.4**. New findings should be added as clearly dated follow-up notes or new lessons; do not silently rewrite historical observations to match later firmware work.

## Planned educational subjects (not scheduled firmware features)

Further walkthroughs may cover the actual JavaScript functions, MIDI timing and capture, groove and phrase architecture, bass and chord accompaniment, audio rendering and gain staging, and engineering decision records. The order is flexible; firmware research and validation take precedence.

A planned lesson is **not** a promise of a new implementation or confirmation of a capability. Do not assume that any prototype processing technique will be reused in original firmware.

## Rules for future walkthroughs

Each new lesson should, where applicable:

1. **Identify its source and version.** Name the exact artifact, source file, revision, or test record used. Recheck the current source rather than relying on memory.
2. **Teach from real code.** Quote concise, accurate excerpts and explain variables, functions, events, and behavior in approachable language. Mark any simplified, illustrative, or reformatted code clearly.
3. **Separate layers.** Distinguish browser controls, Web MIDI messages, worklet communication, compiled engine behavior, audio samples, and physical-device behavior.
4. **Label evidence.** Separate *source-confirmed*, *test-observed*, *hypothesized / possible*, *not measured*, and *not yet implemented*. A source-code timing interval is not a measured end-to-end latency value.
5. **Explain engineering tradeoffs.** Describe what problems a design addressed, its limits, possible failure modes, and what would require a controlled test.
6. **Include “What this means for the physical FM-1.”** Identify which musical concepts might inform firmware and which browser-specific mechanisms cannot be assumed to transfer. Call out unknown hardware constraints and tests needed before making claims.
7. **Preserve truthful attribution.** Disclose AI assistance, human project direction, observed hardware testing, upstream dependencies, and licensing separately. See [AI attribution](../AI_ATTRIBUTION.md) and [license](../LICENSE).
8. **Avoid scope drift.** A walkthrough is educational documentation—not authorization to change the browser prototype, firmware, recovery procedure, or hardware. Propose changes separately and validate before claiming success.
9. **Verify publication.** Review the Markdown, obtain approval for repository changes, and confirm the committed content and changed-file list.

## Example: documenting MIDI timing responsibly

The browser prototype's worklet renders audio in blocks and periodically publishes status and collected outgoing MIDI events. That implementation can teach us about **latency, jitter, publication cadence, and the difference between the internal audio path and external MIDI delivery**.

It does **not** establish measured end-to-end MIDI latency, prove that outgoing MIDI reaches external hardware, or dictate how the physical FM-1 should schedule events. Those questions require relevant source inspection and controlled tests on the appropriate system.

A future timing lesson should retain the prototype explanation, add an evidence table, and close with a **“What this means for the physical FM-1”** section describing which timing requirements need independent hardware verification.

## Reading this series

Start with Lesson 01, then Lesson 02. Follow later lessons as they are published. The goal is technical understanding that survives scrutiny—not to imply that the project lead manually wrote every code line or that AI-generated code is correct merely because it runs.

**Build → Test → Understand → Document → Verify.**