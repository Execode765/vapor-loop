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
| Device routing | PASS by automated test design; CI pending | Device selection, Note On/Off, channel preservation, velocity status, and C-engine audio/capture path |
| Disconnect protection | PASS by automated test design; CI pending | Handler detaches and All Notes Off is sent across 16 channels when the selected input disappears |
| Native/WASM regressions | PASS locally | Voice, memory, Reply, compiled WASM, and worklet tests remain green |
| Real Chrome with simulated MIDI input | PENDING | GitHub Actions will exercise served and direct-file builds |
| Physical SMK Pro Elite | NOT TESTED | Requires owner permission/device test in desktop Chrome |

The browser must request MIDI access in response to **Enable MIDI**; plugging in a device alone does not trigger a permission prompt. This milestone requests no System Exclusive access and does not test firmware flashing or FM-1 hardware compatibility.
