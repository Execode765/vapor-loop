# Development log

Record decisions and evidence rather than treating chat history as the only project record.

## 2026-10-28 — Project setup
- Established a dedicated repository under Execode765.
- Adopted psychic-octo-spork as a temporary working code name; final firmware name undecided.
- Defined GENOME, ECHOLOG, and GRAVITY as candidates.
- Identified ECHOLOG as the assistant's initial recommendation; no final concept selected.
- Owner reports eight FM-1 units and one SMK Pro Elite, with custom firmware installed on the FM-1s.
- Resolved GitHub access by reconnecting the intended account.
- Added initial planning, hardware testing, and AI attribution documentation.
- No project firmware has been written, built, flashed, or hardware-tested.

## 2026-10-28 — First prototype direction selected
- Owner approved ECHOLOG's melodic conversation interaction as the first experiment.
- Added [prototype specification](PROTOTYPE_SPEC.md): explicit capture/reply, one short monophonic phrase, one answer, Adventure control, session-only Keep, Stop and Clear.
- Proposed note/time limits and response rules are design defaults pending implementation and musical evaluation.
- Specification introduced in commit 3f0d4632d4c260df71cb4c54c2d0b441c76b5c9b.
- Validation: documentation only; no code, browser listening test, or hardware test yet.
- Next: inspect development foundations and establish the portable phrase engine/build workflow.

## Entry format
For future work, record:
- Date and objective.
- Change or decision, with rationale.
- Commit/build identifier.
- Validation environment and actual result.
- Known limitations and next step.

Never record a planned test as a passed test.

## 2026-10-28 — First browser sound-check implementation
- Selected a pinned foma browser platform subset; retained its GPL and font notices.
- Added a portable integer sine voice, foma adapter, onscreen keys/display, computer-keyboard input, volume, and Stop.
- Added a browser-only compiler script and GitHub Actions workflow; no hardware image or installer.
- Local checks passed: native C oscillator pitch (439.85 Hz measured for A4), amplitude bounds and note lifecycle; compiled WebAssembly input/display/audio; worklet message bridge.
- Local Chromium download returned an invalid archive, preventing a real-browser test here. Added a CI browser test using the runner's Chrome; result pending at this entry.
- This milestone implements sound only. Capture, Reply, Adventure, and Keep are not implemented.
- Build details and validation limits: BROWSER_BUILD.md and BROWSER_VALIDATION.md.


### Browser verification follow-up
- GitHub Actions run 40736104361 passed the real Chrome audio/control test at source commit e110254f51922b5d287e045a70cca8c45899fe8f.
- Inspected the desktop/mobile screenshots. Browser audio samples were checked automatically; human listening remains pending.
- Added a reproducible single-file downloadable preview and local-file browser check.

### Downloadable preview verified
- Chrome passed the complete audio/control check for both the served browser build and the standalone HTML opened directly from disk in workflow 40737010064.
- Fixed local-file worklet loading by embedding its module as a data URL.
- Updated the browser test dependency; the final CI install reported zero audited vulnerabilities.
- Human listening and hardware compatibility remain untested.

## 2026-10-28 — First owner listening test passed
- Owner confirmed that the browser sound check produces sound and responds to both computer-keyboard and mouse input.
- This is owner-reported hands-on evidence, separate from automated Chrome signal/control checks.
- Exact browser/OS and individual Stop/volume checks were not recorded.
- Next musical milestone: implement capture and replay of one short monophonic phrase, then add a recognizable melodic response. No hardware flashing.

## 2026-10-28 — First phrase memory, 0.0.2
- Implemented a portable C capture/replay state machine with relative audio-frame timestamps, durations, velocities, fixed capacity, and no heap allocation.
- Added Capture, Finish, Replay, Clear, status/progress, and a visibly bounded take. Stop retains partial capture; live playing interrupts replay.
- Local address/undefined-behavior sanitizer checks passed (leak detection disabled locally because this runtime restricts process inspection; the memory engine does not allocate). Existing voice, WASM, and bridge checks still pass, plus the new WASM memory integration test.
- Native tests confirm identical replay samples across single-frame and larger render chunks, both capacity limits, overlap/retrigger behavior, and Stop/Clear handling.
- Browser and owner listening results for this new milestone are pending at implementation time. Prior owner approval was for 0.0.1 only.
- Rebuilt the single-file preview. No hardware image produced or flashed.

### 0.0.2 browser checks completed
- Workflow 40738989216 passed native voice/memory sanitizer tests, WASM integration, worklet regression checks, and real Chrome recording/replay tests.
- Both the served page and downloaded HTML recorded keyboard/mouse input and replayed a two-note phrase with nonzero audio; Stop preserved it and Clear erased it.
- Inspected desktop/mobile screenshots. No user listening test for 0.0.2 is claimed.

## 2026-10-28 — Owner validation of first phrase memory
- Owner tested the downloaded 0.0.2 standalone browser build.
- Owner confirmed that Capture, Finish, Replay, and Clear all work.
- This establishes hands-on validation of the complete first-memory interaction, separate from automated Chrome checks.
- Browser capture/replay is now the validated foundation for the next musical milestone: generating a transformed reply. No hardware flashing occurred.

## 2026-10-28 — First transformed Reply, 0.0.3
- Added a distinct Reply command while retaining exact Replay for A/B comparison.
- Reply preserves recorded onset, duration, rest, and velocity data, starts a perfect fifth above the source, and mirrors subsequent melodic intervals around that new anchor.
- The rule is deterministic and integer-only: `reply = 2 × first pitch + 7 − captured pitch`, clamped to MIDI 0–127.
- Native sanitizer tests verify the expected transformed pitches and preserved velocities. Compiled WASM, worklet, and existing regression checks pass locally.
- Rebuilt the standalone browser preview. Real-Chrome CI and owner musical evaluation are pending. No plugin or hardware image was built or flashed.

### 0.0.3 browser checks completed
- Workflow 40753333474 passed native sanitizer, compiled WASM, worklet, served-Chrome, and direct-file Chrome checks for source commit `17352a2c28203ac4bd246ac9e69d8bb7ed37a78c`.
- Both browser forms produced audio from Reply and returned to phrase-ready state without JavaScript errors.
- Owner musical evaluation of the reply remains pending.

## 2026-10-28 — Owner audiovisual validation of first Reply
- Owner supplied a screen recording of the downloaded 0.0.3 standalone build.
- Captured phrase: C4–D4–E4–D4–C4. Reply: G4–F4–E♭4–F4–G4, exactly matching the fixed-fifth contour inversion.
- The take reached the eight-second limit and remained valid; Replay and Reply both completed with five notes and preserved timing.
- This is owner-supplied audiovisual evidence for browser behavior. It is not plugin or physical FM-1 validation.

### Owner musical assessment of 0.0.3
- Functional behavior passed: capture, exact Replay, transformed Reply, timing preservation, and the eight-second boundary all worked.
- Musical behavior failed: the owner judged the Reply to be a technical inversion, not a musical answer.
- Retain the deterministic mirror as a diagnostic baseline only; do not describe it as musically successful.
- Defer musical refinement until scale, key, and chord constraints can be designed and evaluated deliberately.

## 2026-10-28 — Browser MIDI connection, 0.0.4
- Added a user-initiated Enable MIDI permission flow and connected-input selector.
- Routes Note On/Off from all 16 channels, including velocity-zero Note Off, plus CC 120/123 panic messages, into the existing compiled C MIDI path.
- Displays controller name, incoming note, and velocity. Mouse/computer-key input remains available without MIDI.
- Device change/disconnect detaches the old handler and sends All Notes Off across all channels to prevent stuck notes.
- Added browser automation with a simulated SMK Pro Elite covering permission options, device selection, live audio, velocity status, phrase capture, Replay, and disconnect safety.
- Native/WASM/worklet regressions pass locally. Real-Chrome CI and physical owner testing are pending. No hardware firmware image was produced or flashed.

### 0.0.4 browser checks completed
- Workflow 40757919690 passed the full simulated-controller test in both served Chrome and the downloaded local HTML.
- Permission request options, selection, note/channel/velocity routing, audio, phrase capture, Replay, disconnect panic, and absence of JavaScript errors were verified.
- Physical SMK Pro Elite validation remains pending.

## 2026-10-28 — Owner physical MIDI validation
- Owner confirmed that the SMK Pro Elite running custom Vector firmware is detected and works; Chrome displays the firmware-provided name “Vector.”
- Owner supplied video evidence using MPC Live III USB-C In 1: velocity status, seven-note Capture/Finish, exact Replay, and diagnostic Reply all worked.
- Recorded source/replay pitches were MIDI 60–61–62–63–62–61–60; Reply was 67–66–65–64–65–66–67, exactly matching the current rule.
- Browser MIDI 0.0.4 is owner-validated with physical controllers. This does not validate an FM-1 hardware build or improve the already-failed musical assessment of the Reply algorithm.

## 2026-10-28 — Scale-aware musical Reply, 0.1.0
- Replaced the chromatic diagnostic transform with a separate portable harmony strategy module.
- Added manual selection of all 12 roots with major, natural minor, major pentatonic, and minor pentatonic scales.
- Reply maps source pitches to nearest scale degrees, creates equal contrary diatonic motion from an answer near a fifth above, stays within MIDI 36–96, and moves the final note to the nearest tonic-triad tone.
- Preserves the existing phrase timing, rests, note lengths, velocities, count, limits, Replay, MIDI, and control behavior.
- Native sanitizer tests cover all 48 key/scale contexts plus a known C-major phrase. Compiled WASM and worklet tests pass locally.
- Real-Chrome CI and owner musical evaluation remain pending. No plugin or FM-1 image was built or flashed.

### 0.1.0 browser checks completed
- Workflow 40761057791 passed native harmony/memory sanitizer checks, compiled WASM, worklet, and real Chrome for both served and downloaded builds.
- Chrome changed the Reply context to D natural minor, produced audible Reply, completed playback, and retained all MIDI/UI protections without JavaScript errors.
- Owner musical evaluation remains pending and is the next gate.

## 2026-10-28 — Owner audiovisual test of scale-aware Reply
- Owner supplied a C-major/MPC screen recording with C5–D5–E5–D5–C5 repeated twice in an eight-second, ten-note take.
- Exact Replay reproduced the source. Reply produced G5–F5–E5–F5–G5 twice, confirming in-scale contrary motion and a tonic-triad ending.
- The prior chromatic E-flat problem is absent; E natural is retained in C major.
- Functional audiovisual validation passed. The owner's subjective judgment of musical usefulness remains intentionally unrecorded until stated.

### Owner musical assessment of 0.1.0
- Owner reported that the scale-aware Reply sounded “a lot more musical” than the prior chromatic inversion.
- Functional behavior and the musical direction both pass for this baseline.
- Retain scale-degree contrary motion and tonic-triad resolution as a starting strategy, while continuing to label automatic harmony, varied rhythm, Adventure, and broader key/scale evaluation as unfinished.

## 2026-10-28 — Adventure controlled variation, 0.2.0
- Added a 0–100 Adventure control. Zero reproduces the approved 0.1.0 Reply; higher values increase the probability of interior scale-step substitutions, with two-step changes available at high settings.
- Each Reply press advances a deterministic variation sequence seeded by phrase pitches, harmony context, and generation number. Changing Adventure or starting a new Capture resets the sequence.
- Protected source rhythm, rests, durations, velocities, count and length; selected scale; MIDI 36–96 register; opening anchor; and tonic-triad cadence.
- Added native tests for baseline compatibility, distinct high-Adventure generations, all 48 contexts, and protected constraints; compiled WASM tests cover the control and counter.
- Local native/WASM/worklet regressions pass. Real-Chrome CI and owner musical evaluation are pending. Keep, rhythm mutation, plugin, and FM-1 firmware remain unimplemented.
