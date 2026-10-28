# First browser sound check — 0.0.1

This milestone plays one sine voice. Phrase capture, replies, Adventure and Keep are not implemented yet.

## Foundation decision

Use a pinned subset of [foma](https://github.com/konsumer/foma) for the browser platform. foma provides the C API, simulated input, framebuffer, timing, and WebAssembly/audio-worklet bridge. Dinghy is a useful hardware foundation but its own host harness is oriented around rendered audio/images; foma supplies the live browser path we need now.

Pinned foma revision: `58e1e25df47cc251e43395b5657c89bdb123d55d`. See `vendor/foma/LICENSING.md` for original attribution. The source was inspected through its README, licensing document, browser implementation, shared helpers, API header, and build script. No M-VAVE binary SDK or hardware installer is included.

This is a **source-compiled browser simulation**, not an emulation of the AC791N CPU. It uses the actual C sound engine, but does not prove firmware or hardware compatibility.

## Source map

| Path | Purpose |
| --- | --- |
| `src/voice.c`, `src/voice.h` | Portable monophonic integer oscillator and envelope |
| `src/voice_tables.h` | Generated constants; C audio processing needs no floating point |
| `src/main.c` | foma adapter: keys/MIDI, note display, Stop |
| `vendor/foma/platform/` | Pinned upstream browser platform and its dependencies |
| `web/` | Browser controls, canvas, audio worklet |
| `tools/build_web.py` | Browser-only build; no `.fwsc` generation or flashing |
| `tests/` | Native voice, WASM integration, worklet bridge checks |

The voice renders at 44.1 kHz. Its state is accessed serially inside the browser worklet. A future threaded plugin or interrupt-driven firmware adapter must serialize commands safely; the browser adapter is not yet hardware-ready.

## Downloadable single-file preview

`preview/echolog-soundcheck.html` contains the same compiled WebAssembly and browser code, bundled into one file. Download it using GitHub's Download raw file control, then open the downloaded file in a modern desktop browser and select Enable sound. This local-file route is subject to browser security policy; if it fails, serve the file from localhost with a local HTTP server instead. Real-browser results are recorded in BROWSER_VALIDATION.md.

Rebuild it after `tools/build_web.py` with `python3 tools/build_standalone.py`. The committed preview is a generated artifact; edit `src/` and `web/`, then rebuild rather than editing the preview directly.

## Build locally

Required: Python 3, Clang with its WebAssembly backend, LLD, Node.js 20 or later for browser automation, and GCC for the native check. Codex installation is optional. On Debian/Ubuntu, the compiler packages are `clang`, `lld`, and `gcc`.

From the repository root:

```sh
python3 tests/test_voice.py
python3 tools/build_web.py
node tests/test_wasm.mjs
node tests/test_worklet.mjs
python3 -m http.server 8000 --directory build/web
```

Open `http://localhost:8000`. Set `CLANG` to an alternative compiler path if needed. The build script never downloads dependencies or contacts a device.

For macOS and Windows, a native setup or Linux build environment still needs validation. The portable browser artifact can be served on either OS without compiling. Detailed native setup instructions will follow tested setups rather than guesses.

## GitHub build artifact

The Browser sound check workflow builds/tests on pushes to main and uploads `echolog-browser-soundcheck`. Download and extract it from the successful workflow run, serve the extracted folder with a local HTTP server, then open it in your browser. The artifact contains `index.html`, JavaScript, CSS, and `firmware.wasm`; no hardware image.

No public website deployment is configured. Keeping the repository private does not require changing its visibility to run the local preview.

## Play

1. Select **Enable sound** (browsers require a user gesture).
2. Click and hold a key, or play `A W S E D F T G Y H U J K` on the computer keyboard.
3. Release it to hear the short release envelope.
4. Adjust Volume or select Stop all notes.

The screen shows MIDI note numbers. Only one note sounds at a time; releasing an older note does not stop a newer note. The first UI exposes F3–F4. It has no MIDI-device picker yet, although the compiled platform MIDI path is covered by software checks.

The browser must support WebAssembly, Web Audio, AudioWorklet and a 44.1 kHz AudioContext. HTTPS or localhost is required. Browser compatibility and real listening feedback must be recorded separately from signal checks.

## Licensing

The browser implementation includes GPL-3.0-only foma platform code; the combined implementation is distributed under GPL-3.0-only in `LICENSE`. The bundled Terminus font retains its SIL OFL notice. Original upstream licensing/credits remain in `vendor/foma/LICENSING.md`; that file describes upstream components, including ones this project does not bundle. Future plugin dependencies must be evaluated before adoption.
