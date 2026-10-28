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
| Owner musical evaluation | NOT TESTED | Probability curve and high-setting usefulness require listening |

Adventure changes interior scale degrees only. Passing scale/cadence constraints does not prove that the variations feel tasteful. Keep and rhythmic mutation are not implemented.

[Successful 0.2.0 workflow](https://github.com/Execode765/psychic-octo-spork/actions/runs/40773797192) validates source commit `a80f68f07abe283f40bb862dda0b12886c513dbf`. Native sanitizer, Adventure constraints, all 48 harmony contexts, compiled WASM, worklet, served Chrome, and direct-file Chrome checks passed. Owner musical evaluation remains pending.
