# Browser sound-check validation

Date: 2026-10-28. Version: 0.0.1, sound-check milestone only.

## Local results

| Check | Result | Evidence |
| --- | --- | --- |
| Native integer C voice | PASS | `python3 tests/test_voice.py` using GCC 13, undefined-behavior sanitizer |
| A4 frequency | PASS | 439.85 Hz from positive-going crossings after attack; expected 440 Hz, tolerance 2 Hz |
| Native amplitude and release | PASS | RMS 5740.8; peak bounded at 8128; silence after release/Stop; all 128 MIDI pitches exercised |
| Browser WebAssembly build | PASS | Clang 18.1.3 / LLD, wasm32 target; no imports |
| Compiled platform integration | PASS | `node tests/test_wasm.mjs`: framebuffer, panel notes, MIDI, velocity-zero note-off, Stop, MIDI panic, volume zero |
| Worklet message bridge | PASS | `node tests/test_worklet.mjs`: real WASM through actual worklet code in a mocked Node worklet environment |
| Real-browser UI/audio | PASS | Chrome on GitHub Actions: worklet startup, nonzero samples, release, Stop, pointer input, no uncaught JS errors, mobile overflow check |
| Downloadable standalone preview | PASS | Real Chrome opened the generated HTML directly from disk and passed the same audio/control checks |
| Owner listening/input smoke test | PASS (owner reported) | Owner confirmed sound and working computer-keyboard and mouse playback; musical response features are not yet implemented |
| macOS/Windows DAW plugin | NOT IMPLEMENTED | Later milestone |
| FM-1 / SMK hardware | NOT TESTED | No hardware images built or flashed |

These checks verify signal behavior and integration, not musical response quality. The synthesized reference WAV is a native core render, not a recording from a browser or FM-1.

## Reproduce

See [BROWSER_BUILD.md](BROWSER_BUILD.md). For actual Chromium automation, install the pinned dev dependency using `npm ci`, make Chrome available, serve `build/web` on port 8000, and run:

```sh
PLAYWRIGHT_CHROMIUM_EXECUTABLE_PATH=/path/to/chrome node tests/test_browser.cjs
```

The browser check verifies worklet startup, nonzero audio samples after key input, silence on release/Stop, pointer input, no uncaught JS errors, and mobile-width overflow. It saves desktop/mobile screenshots. It does not claim human listening or broad browser compatibility.

## CI evidence

[Successful browser workflow](https://github.com/Execode765/psychic-octo-spork/actions/runs/40736104361), source commit `e110254f51922b5d287e045a70cca8c45899fe8f`. Native, WASM, bridge, and real Chrome tests passed. Desktop and mobile screenshots were inspected; controls and text fit both layouts. This was Linux headless Chrome, not a macOS/Windows or Safari compatibility test.

[Final successful workflow](https://github.com/Execode765/psychic-octo-spork/actions/runs/40737010064) validates source commit e8c56546d20e7f4a1009577ad1ddfd7d2b719388 with both HTTP-served and local-file previews. The standalone packages its worklet as a data URL because Chrome rejected a null-origin blob URL when opened from disk. No browser-security settings or flags disabling web security were used.

## Owner feedback — 2026-10-28

The owner tested the browser sound check and reported: “It works from the keyboard, it works from the mouse.” This confirms audible playback and both input methods in the owner's session. Browser version, OS, Stop/volume behavior, and extended-session results were not individually recorded; do not infer those details from the general positive report. No hardware test or phrase-response evaluation occurred.

## First-memory milestone — 0.0.2

| Check | Result | Evidence |
| --- | --- | --- |
| Native memory engine | PASS locally | Address/undefined-behavior sanitizer build; stored event timing, deterministic render across chunk sizes, empty takes, overlap, both limits, Stop/Clear |
| Actual compiled WASM memory path | PASS locally | Capture/replay via foma buttons/keys, nonzero replay signal, Stop/Clear, 32-note and eight-second caps |
| Existing voice/WASM/worklet regression checks | PASS locally | Existing automated checks still pass |
| Chrome served and standalone capture/replay | PASS | Keyboard/mouse recording, empty take, audible two-note replay, Stop preserving phrase, Clear, mobile width, no uncaught JS errors |
| Owner listening for capture/replay | PASS (owner reported) | Owner confirmed Capture, Finish, Replay, and Clear all work in the downloaded 0.0.2 build |

The new browser test is not yet a claim of cross-browser/macOS/Windows support. No hardware flashing or plugin implementation occurred.

[Successful 0.0.2 workflow](https://github.com/Execode765/psychic-octo-spork/actions/runs/40738989216) tested source commit `03ca3ec1dd695bf147095f89b8893dbce9b9cc51`. All native, sanitizer, WASM, worklet, and Chrome checks passed. Both served and direct-from-disk standalone previews passed. Desktop/mobile screenshots were inspected. Owner listening and control feedback for this capture/replay milestone subsequently passed; see the owner feedback entry below.

## Owner feedback — 0.0.2 — 2026-10-28

The owner tested the downloaded standalone build and confirmed that Capture, Finish, Replay, and Clear all work. This validates the complete first-memory interaction in the owner's browser session. The report does not establish broad cross-browser support, plugin behavior, or FM-1 hardware compatibility.

## First transformed reply — 0.0.3

| Check | Result | Evidence |
| --- | --- | --- |
| Native reply transformation | PASS locally | Exact expected pitches, preserved velocities/timing, and safe playback lifecycle under address/undefined-behavior sanitizers |
| Compiled WASM reply command | PASS locally | Reply command enters transformed playback and produces nonzero audio through the real foma interface |
| Existing voice/memory/worklet regressions | PASS locally | Native voice, WASM, worklet, capture/replay, limits, Stop, and Clear checks remain green |
| Real Chrome UI/audio | PASS | Served and direct-file builds completed Reply with nonzero audio and returned to phrase-ready state |
| Owner audiovisual test | PASS (owner supplied) | Recorded source C4–D4–E4–D4–C4; Reply produced G4–F4–E♭4–F4–G4 with preserved timing and completed playback |

The transformation begins seven semitones above the captured first note and mirrors each captured interval while retaining the recorded rhythm, durations, and velocities. This is a browser prototype result, not hardware or plugin validation.

[Successful 0.0.3 workflow](https://github.com/Execode765/psychic-octo-spork/actions/runs/40753333474) validates source commit `17352a2c28203ac4bd246ac9e69d8bb7ed37a78c`. Native sanitizer, compiled WASM, worklet, served Chrome, and direct-file Chrome checks all passed. Owner musical evaluation remains pending.

## Owner feedback — 0.0.3 — 2026-10-28

The owner supplied a screen recording of the standalone 0.0.3 build. The video shows a five-note phrase reaching the eight-second capture limit, exact Replay, and transformed Reply completing successfully. Audio analysis identifies the captured contour as MIDI `60, 62, 64, 62, 60` (C4–D4–E4–D4–C4) and the Reply as `67, 65, 63, 65, 67` (G4–F4–E♭4–F4–G4), matching the documented transformation exactly. This validates function and the audible transformation in the owner's Chrome session. The owner judged the result to be a technical inversion rather than a musical answer, so musical usefulness did not pass. Scale- and chord-aware pitch handling is deferred to a later refinement milestone.

### Owner musical assessment

**Functional result: PASS. Musical result: FAIL.** The transformation behaved exactly as specified, but the owner reported that it was “not musical at all” and clearly sounded like a technical inversion. Version 0.0.3 remains useful as a deterministic diagnostic baseline. It must not be presented as a finished musical-response algorithm. Future refinement should constrain or remap generated pitches using explicit scale, key, and chord context.

## Browser MIDI — 0.0.4

| Check | Result | Evidence |
| --- | --- | --- |
| Browser MIDI capability/permission flow | PASS locally (syntax/build) | Explicit user button requests `requestMIDIAccess({sysex:false})`; denial and unavailable-API states retain non-MIDI input |
| Device routing | PASS | Chrome selected the simulated controller and verified Note On/Off, channel handling, velocity status, audio, capture, and Replay |
| Disconnect protection | PASS | Chrome removed the simulated input, disabled selection, sent panic, and reached silence without JavaScript errors |
| Native/WASM regressions | PASS locally | Voice, memory, Reply, compiled WASM, and worklet tests remain green |
| Real Chrome with simulated MIDI input | PASS | Both served and direct-file builds passed the full browser MIDI test |
| Physical MIDI controllers | PASS (owner tested) | SMK Pro Elite running Vector firmware was detected as “Vector”; MPC Live III USB-C In 1 was captured on video with note/velocity input, seven-note capture, Replay, and Reply |

The browser must request MIDI access in response to **Enable MIDI**; plugging in a device alone does not trigger a permission prompt. This milestone requests no System Exclusive access and does not test firmware flashing or FM-1 hardware compatibility.

[Successful 0.0.4 workflow](https://github.com/Execode765/psychic-octo-spork/actions/runs/40757919690) validates source commit `b62f4e08cf2a7cd12b16823ad569c608f87c18a4`. Native sanitizer, compiled WASM, worklet, served Chrome, and direct-file Chrome checks passed. Physical SMK Pro Elite testing remains pending.

## Owner physical MIDI validation — 0.0.4 — 2026-10-28

The owner confirmed that Chrome and ECHOLOG recognize an SMK Pro Elite running custom Vector firmware. The port is correctly reported as **Vector**, because the installed firmware supplies the USB MIDI product/port descriptor; this is not a detection failure. The owner also connected an MPC successfully.

The supplied MPC screen recording shows **MPC Live III USB-C In 1** selected, live note status including velocity 117, a seven-note phrase captured and finished at 6.05 seconds, exact Replay, and transformed Reply. Audio analysis identifies the source as MIDI `60, 61, 62, 63, 62, 61, 60`, the exact Replay as the same sequence, and Reply as `67, 66, 65, 64, 65, 66, 67`, matching the diagnostic rule. This validates physical browser MIDI input and phrase operation, not FM-1 firmware compatibility or musical quality of Reply.

## Scale-aware musical Reply — 0.1.0

| Check | Result | Evidence |
| --- | --- | --- |
| Harmony strategy across 48 contexts | PASS locally | Address/undefined-behavior sanitizer test covers 12 roots × four scales; every output is in scale and MIDI 36–96 |
| Known C-major answer | PASS locally | C4–D4–E4–D4–C4 generates G4–F4–E4–F4–G4 |
| Cadence and preservation | PASS locally | Final note is a tonic-triad tone; onset, rests, duration, velocity, count, and total length remain stored by the memory engine |
| Compiled WASM harmony controls | PASS locally | Root/scale setters and getters reach the actual compiled engine; audible Reply completes |
| Previous voice/memory/MIDI regressions | PASS locally | Existing native, WASM, worklet, limits, Stop/Clear, and MIDI checks remain green |
| Real Chrome UI/audio | PASS | Served and direct-file Chrome selected D natural minor, produced audible Reply, completed playback, and passed MIDI/UI regressions |
| Owner audiovisual functional test | PASS (owner supplied) | C5–D5–E5–D5–C5 repeated twice; exact Replay; scale-aware Reply G5–F5–E5–F5–G5 repeated twice; owner reported it sounded “a lot more musical” than the chromatic version |

This strategy is deterministic and manually keyed. Passing automated music-theory constraints does not establish that the response feels musical. No plugin or physical FM-1 result is claimed.

[Successful 0.1.0 workflow](https://github.com/Execode765/psychic-octo-spork/actions/runs/40761057791) validates source commit `39202a2c9da499012002cdc71589a077496c60f2`. Native sanitizer, all 48 harmony contexts, compiled WASM, worklet, served Chrome, and direct-file Chrome checks passed. Owner musical evaluation remains pending.

## Owner audiovisual test — 0.1.0 — 2026-10-28

The owner supplied a screen recording using **C major** and **MPC Live III USB-C In 1**. The take contains ten notes over eight seconds: MIDI `72, 74, 76, 74, 72` repeated twice. Replay reproduces all ten notes exactly. Reply produces MIDI `79, 77, 76, 77, 79` repeated twice: G5–F5–E5–F5–G5. The generated E natural remains inside C major, and the final G is a tonic-triad tone. Capture, the time limit, Replay, Reply, Web MIDI, and repeated playback all functioned. This proves the implemented theory rule and audio path; The owner subsequently reported that it sounded “a lot more musical” than the chromatic version. This establishes the strategy as a useful musical baseline, not a finished response system.

### Owner musical assessment

**Functional result: PASS. Musical-direction result: PASS.** After comparing the scale-aware response with the failed chromatic inversion, the owner reported that version 0.1.0 sounded “a lot more musical.” This supports retaining scale-degree motion and tonic-triad resolution as the baseline. It does not yet validate every key/scale, longer-term playability, chord changes, rhythmic variation, or Adventure behavior.

## Adventure — 0.2.0

| Check | Result | Evidence |
| --- | --- | --- |
| Adventure 0 compatibility | PASS locally | Approved C-major 0.1.0 answer is unchanged across variation seeds |
| Distinct high-Adventure generations | PASS locally | Successive Adventure 100 generations alter interior notes and differ from each other |
| Musical safety constraints | PASS locally | Opening anchor fixed; every generated pitch remains in selected scale and MIDI 36–96; ending remains a tonic-triad tone |
| All 48 key/scale contexts | PASS locally | Address/undefined-behavior sanitizer test at Adventure 100 |
| Timing and expression preservation | PASS locally | Source onsets, rests, durations, velocities, length, and count remain owned by the unchanged memory scheduler |
| Compiled WASM control/counter | PASS locally | Adventure setter/getter and successive variation counter reach the actual compiled engine |
| Previous voice/memory/MIDI regressions | PASS locally | Existing native, WASM, worklet, limits, controls, and MIDI checks remain green |
| Real Chrome UI/audio | PASS | Served and direct-file Chrome set Adventure 80, completed variations 1 and 2, and passed Replay/MIDI/UI regressions |
| Owner audiovisual functional test | PASS (owner supplied) | Physical MPC/C-major test exercised Adventure 0 and raised settings through 80; exact Replay, distinct in-scale contours, protected anchor/register/cadence; subjective verdict pending |

Adventure changes interior scale degrees only. Passing scale/cadence constraints does not prove that the variations feel tasteful. Keep and rhythmic mutation are not implemented.

[Successful 0.2.0 workflow](https://github.com/Execode765/psychic-octo-spork/actions/runs/40773797192) validates source commit `a80f68f07abe283f40bb862dda0b12886c513dbf`. Native sanitizer, Adventure constraints, all 48 harmony contexts, compiled WASM, worklet, served Chrome, and direct-file Chrome checks passed. Owner musical evaluation remains pending.

## Owner audiovisual test — Adventure 0.2.0 — 2026-10-28

The owner supplied a physical-MIDI screen recording using **C major** and **MPC Live III USB-C In 1**. The captured 15-note phrase reaches the eight-second limit and consists of the C-major contour C–D–E–G–E–D–C repeated, with a final C at the boundary. Replay is exact. Adventure 0 produces the approved scale-aware baseline. The recording visibly exercises raised Adventure settings through 80; generated pitch sequences differ, remain in C major, retain the G answer anchor, stay inside the protected register, and end on C-major tonic-triad tones. Higher Adventure produces wider and lower-register movement as intended. The video validates physical operation and audible variation. A follow-up recording confirms successive variations at one unchanged Adventure setting. The owner judged the results musically acceptable but stylistically closer to a nursery rhyme than modern trap, R&B, melodic techno, or contemporary electronic phrasing.

### Same-setting variation and musical assessment

A follow-up physical-MIDI recording holds **C major / Adventure 10** while repeatedly pressing Reply. The interface visibly advances through successive variation numbers, including 1, 2, 3, 5, and 7, and the audible pitch sequences change without moving the slider. This completes owner validation of the generation counter and same-setting variation behavior.

**Functional result: PASS. Musical coherence: PASS, limited. Style target: FAIL/PENDING.** The owner described the output as “okay” and “more nursery rhyme” than melodic techno, modern trap, or R&B. This is consistent with the present design: copied source rhythm, static scale, no chord progression, simple contrary motion, monophony, and a sine voice. Further random pitch variation alone is unlikely to solve the style gap; rhythmic vocabulary, chord context, phrase-shape rules, and sound design are the relevant next layers.

## Groove and phrase shaping — 0.3.0

| Check | Result | Evidence |
| --- | --- | --- |
| Original profile compatibility | PASS locally | Reply onset/duration arrays exactly equal captured timing |
| Pulse | PASS locally | Maximum Shape quantizes to the 16th-note grid and targets 3/4-grid duration |
| Swing | PASS locally | Odd 16ths receive the expected one-third-grid delay at maximum Shape |
| Pocket | PASS locally | Alternating laid-back placement/duration retains strict event order |
| Half-time | PASS locally | Onsets, durations, and phrase length reach 2× at maximum; reply remains bounded to 16 seconds |
| All profile/amount combinations | PASS locally | Address/undefined-behavior sanitizer checks nonzero duration, strict order, and length bounds |
| Compiled WASM controls | PASS locally | BPM/profile/Shape setters and getters reach the actual engine; Half-time play length exceeds source |
| Previous harmony/Adventure/MIDI regressions | PASS locally | Existing native, WASM, worklet, controls, limits, and MIDI checks remain green |
| Real Chrome UI/audio | PASS | Served and direct-file Chrome used 128 BPM / Swing / Shape 80, completed successive replies, and passed Replay/MIDI/UI regressions |
| Owner musical evaluation | NOT TESTED | Groove character and style usefulness require listening |

These profiles establish timing transformation, not finished genre emulation. BPM is manual; no MIDI clock, bar model, event addition/removal, or sound-design improvement is claimed.

[Successful 0.3.0 workflow](https://github.com/Execode765/psychic-octo-spork/actions/runs/40778697465) validates source commit `f51bd64f273a42ad47c2539b39601fafa5f0b488`. Native sanitizer, five groove profiles, compiled WASM, worklet, served Chrome, and direct-file Chrome checks passed. Owner musical/style evaluation remains pending.


## Owner audiovisual test — Groove and phrase shaping 0.3.0 — 2026-10-28

The owner supplied a physical-MIDI screen recording using **MPC Live III USB-C In 1**. The video shows a 26-note phrase captured successfully, exact Replay, repeated Reply generation, manual BPM changes, multiple Shape amounts, and all five profiles: Original, Pulse, Swing, Pocket, and Half-time. Half-time visibly and audibly extends the reply; the other profiles complete without crashes, stuck notes, or broken controls.

### Owner musical assessment

**Functional result: PASS. Groove result: FAIL. Directional value: PARTIAL PASS.** The owner reported that the grooves “didn't really change a whole lot” and did not meaningfully groove. Tempo/Half-time changes were clearly perceptible, but Pulse, Swing, and Pocket did not create convincing contemporary rhythmic feel. Version 0.3.0 therefore validates the separated timing architecture and controls, not musical pocket or genre authenticity.

The next refinement should not merely add more timing offsets. It should reshape phrase structure using rests, density, motif repetition, syncopation, pickups, ratchets, accents, and strategically lengthened notes. Tempo scaling must remain independent from pitch; any pitch-shift impression is perceptual because the synthesized notes themselves are not resampled.


## Phrase Architecture — 0.4.0

| Check | Result | Evidence |
| --- | --- | --- |
| Independent Reply event count | PASS | A three-note captured source generates a ten-event Trap Pocket reply while exact Replay remains three notes |
| Original compatibility | PASS | Original or Intensity 0 preserves source count, pitches, velocities, onsets, durations, and length |
| Trap Pocket structure | PASS | Sparse one-bar pattern, explicit rests, accent hierarchy, and threshold-added ratchets |
| R&B Flow structure | PASS | Mixed note lengths, response gaps, and softer intervening velocities |
| Techno Motion structure | PASS | Repeating four-note motif, cyclical accents, and intensity-added events |
| Bounds and lifecycle | PASS | Native sanitizers cover all styles/intensities, strict event order, positive duration/velocity, capacity, and 16-second reply bound |
| Compiled WASM controls | PASS | Architecture style/intensity exports and independent reply count reach the actual engine |
| Real Chrome UI/audio | PASS | Served and direct-file Chrome selected Trap Pocket 100, produced a ten-event reply from a three-note source, completed two variations, then replayed the untouched source |
| Owner musical evaluation | NOT TESTED | Contemporary feel and usefulness require listening |

[Successful 0.4.0 workflow](https://github.com/Execode765/psychic-octo-spork/actions/runs/40786013924) validates source commit `d7329c22e2e54a8798f90324e3ca1cac0af945e8`. The names Trap Pocket, R&B Flow, and Techno Motion describe experimental design targets, not verified genre authenticity. No plugin or hardware firmware result is claimed.


## Owner audiovisual validation — Phrase Architecture 0.4.0 — 2026-10-28

The owner supplied a 4:32 screen recording of the standalone browser build. The recording shows a **24-note C-major phrase reaching the eight-second capture limit**, followed by exact Replay and repeated generated replies.

Visible tests include:

- **Original** architecture as the source-preserving comparison.
- **Trap Pocket 100** at 120 BPM, producing independent ten-event responses of approximately two seconds across successive variations.
- **R&B Flow 100** at approximately 70 BPM, producing independent eight-event responses of approximately 3.43 seconds.
- **Techno Motion 100**, tested around 70–85 BPM, producing independent ten-event motif responses around 2.8 seconds.
- Additional combinations of Techno Motion and Trap Pocket with the separate Groove layer, including Pulse, Swing, and Pocket/Shape settings.

The interface consistently retains 24 source notes while showing generated reply counts of eight or ten, directly validating independent Reply event structure. Responses complete normally across repeated generations and profile changes; no crash, stuck note, or source-memory corruption is visible.

### Owner musical assessment

**Functional result: PASS. Melodic coherence: PASS. Audible groove: PASS. Direction: STRONGLY VALIDATED.** The owner reported: “It's definitely grooving now. It grooves and it's melodic. We are definitely on track.”

This is the first owner validation that ECHOLOG creates a convincing audible groove rather than merely applying technically correct timing changes. It validates the phrase-architecture direction, not universal genre authenticity. The video does not establish plugin behavior, MIDI-clock synchronization, polyphony, sound-design quality, or FM-1 hardware compatibility.
