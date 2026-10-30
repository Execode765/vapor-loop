# Split Performance and Poly Keys — 0.10.0

Version 0.10.0 adds a playable performance layer without changing Vapor Loop's existing Reply, Candidate Memory, Bass Companion, drum, or capture behavior.

## What changed

- **Split input:** notes below the selected split point play the manual bass voice; notes at or above it play the lead voice.
- **Independent MIDI roles:** channel 1 plays lead, channel 2 plays bass, and channel 3 plays Poly Keys.
- **Four-voice Poly Keys:** a compact integer electric-keys foundation supports triads and seventh chords, with deterministic voice stealing when a fifth note arrives.
- **Manual chord recording:** Poly Keys can record a quantized pass onto the same 96-PPQN arrangement used by lead, bass, and drums.
- **Independent controls:** Poly Keys can be Off or Manual and have their own level control.

## Browser test procedure

1. Enable sound and MIDI.
2. Set **Bass Companion** to Manual.
3. Choose **Split: Bass + Lead**, then select a split point.
4. Play below and above the split from one keyboard. Confirm that low notes use the bass voice while high notes use the lead voice.
5. Set **Poly Keys** to Manual and route a controller to MIDI channel 3, or choose Poly Keys as the browser input track.
6. Hold a three- or four-note chord. Confirm simultaneous notes and independent note releases.
7. Capture a lead phrase, choose Poly Keys, and select **Record Poly Keys pass**. After the count-in, record a chord part while the phrase plays.
8. Finish and Replay. Confirm that lead, recorded chords, bass/drums if present, and the common clock remain synchronized.
9. Press **Stop all notes** and confirm no voice remains sounding.

## Deliberate limits

- The Poly Keys tone is a small synthesis foundation, not a finished Rhodes emulation.
- Poly Keys has four voices and deterministic voice stealing; sustain pedal behavior is not implemented.
- The new track is manual only. Scale-aware Chord Companion generation is reserved for a later milestone after this performance foundation passes owner testing.
- Session data is not yet saved across page reloads.
- This is a browser/WebAssembly prototype. No FM-1 firmware image was produced or flashed.

## Architecture note

All four musical roles share one bounded, fixed-memory 96-PPQN timeline:

| Track | Role | Current input |
| --- | --- | --- |
| Lead | Captured source, Replay, and generated Reply | Browser keys, MIDI channel 1, or upper split |
| Bass | Manual pass or Bass Companion | MIDI channel 2 or lower split |
| Drums | Manual steps or companion reference | Browser step grid |
| Poly Keys | Four-voice chord pass | MIDI channel 3 or Poly Keys input |

The portable C core remains heap-free so this work can be measured against FM-1 limits before any hardware port is attempted.
