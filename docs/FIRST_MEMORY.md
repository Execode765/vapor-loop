# First memory — browser prototype 0.0.2

This milestone captures and replays one short monophonic phrase. It does not generate musical replies yet.

## Try it

Download `preview/echolog-soundcheck.html` and open it in Chrome, then select Enable sound. The filename stays the same so existing repository links remain useful; the page identifies this build as 0.0.2.

1. Select **Capture**. This replaces the previous take. The instrument waits for your first note.
2. Play a short melody using the computer keyboard, mouse, or touch keys. The first note starts the recording clock; subsequent rests are preserved.
3. Select **Finish**. Any held note is closed, and the phrase becomes ready.
4. Select **Replay** to hear the stored pitches, velocities, durations, and rests.
5. **Stop all notes** interrupts playback without erasing the phrase. During capture it finishes and retains the partial take.
6. **Clear** stops playback and erases the phrase. A new Capture also replaces the previous take.

Memory is session-only: closing/reloading the page loses the phrase. There is no save-to-disk feature or overwrite undo yet.

## Limits and behavior

- At most 32 notes and eight seconds from the first note. An empty armed take does not time out.
- The 32nd note is allowed to finish; its release ends capture automatically. An attempted 33rd note closes the 32nd and finishes the take without storing the new note.
- The eight-second limit closes a held note automatically. The page shows which limit ended the take.
- Most recently pressed note wins. Overlap closes the previous event; releasing an older key does not silence a newer one. Same-pitch retriggers form separate notes when rendered time has elapsed.
- Gestures that begin and end without any C audio frames rendered between them are discarded rather than stored with zero duration. The browser platform samples input around audio blocks, approximately 5.8 ms at 44.1 kHz. This is not a claim of sample-accurate timestamping of physical key input.
- Replay schedules stored boundaries in audio frames, independent of JavaScript timers and render chunk size. The sine voice retains a short release envelope, so sound decays briefly after the stored note ends.
- Live playing interrupts replay, preserving the phrase. Replay ignores stale live key-release messages.
- Moving focus away or hiding the page stops playback/finishes capture to prevent stuck notes.
- Captured input is not scale-quantized or otherwise transformed in this milestone.

## Technical design

`src/memory.c` is a portable C state machine with idle, armed, recording, and replaying states. Each event stores pitch, velocity, relative onset, and duration. Time is stored in 44.1 kHz frames. Fixed arrays bound the memory use; no heap allocation, external model, or recorded audio is involved.

The renderer splits audio at event boundaries and the recording cap. The C memory engine owns the voice, so note durations do not depend on browser animation or UI refresh. `src/main.c` connects this engine to the foma API; the browser worklet publishes status to the page.

All state is currently serialized in the browser's audio worklet. Plugin and hardware adapters will need a safe command queue when UI and audio execute on different threads or interrupts. This is still a browser prototype, with no `.fwsc` or hardware flashing.

## Tests

- `tests/test_memory.c`: event values, rests, deterministic replay across different render block sizes, empty/zero-length takes, retrigger/overlap, held-note finish, both limits, live interruption, Stop and Clear. Run with address and undefined-behavior sanitizers.
- `tests/test_memory_wasm.mjs`: actual compiled foma input/command path, capture, nonzero replay output, automatic completion, limits and Stop/Clear.
- `tests/test_browser.cjs`: Chrome capture with keyboard and mouse, empty capture, two-note audible replay, Stop preserving the phrase, Clear, and desktop/mobile layouts. Both served and standalone versions are exercised in CI.

Automated signal checks are separate from the owner's next listening test. Prior owner approval applies to the 0.0.1 sound check, not this new recording behavior.
