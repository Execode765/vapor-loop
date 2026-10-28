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
| Downloadable standalone preview | PENDING | Embedded WASM matches tested build; real-browser local-file check added to next CI run |
| Human listening/playability | NOT TESTED | Owner feedback needed |
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
