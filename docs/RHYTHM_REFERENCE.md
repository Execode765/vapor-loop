# Rhythm Reference and Extended Capture — prototype 0.8.1

## Purpose

Version 0.8.1 makes the shared musical clock easier to hear and use without committing the project to a full drum machine. It preserves the v0.8.0 Reply/Bass synchronization and adds an optional reference rhythm during recording and playback.

## Capture clock

- Capture length: 1, 2, 4, 8, or 16 bars in 4/4.
- Count-in: one or two complete bars; two bars is the browser default.
- Input correction: Off, Light, or Tight sixteenth-note onset correction.
- Metronome: independent on/off control for count-in and recording.
- Capacity: 128 monophonic note events. Reaching capacity stops storing new notes but preserves the selected bar boundary.
- Display: current one-based bar and beat are visible in the browser and simulated FM-1 display.

At the slowest supported tempo, 40 BPM, 16 bars lasts 96 seconds. The fixed event arrays and 128-note ceiling intentionally bound memory for an eventual FM-1 port. This milestone does not claim 32/64-bar hardware support.

## Reference kit

The optional kit is synthesized at 44.1 kHz with deterministic integer processing:

- Kick on beats 1 and 3.
- Snare on beats 2 and 4.
- Closed hat on eighth notes.
- Independent level and mute.

The drums begin at recording/playback beat one and follow the selected or saved phrase BPM. They are monitoring context only: they are not stored as captured notes, do not consume Candidate Memory, and do not change Reply or Bass Companion timing.

The implementation uses a swept sine kick and deterministic noise-derived snare/hat. It bundles no drum samples or proprietary ROM data. Classic-machine-inspired kits, pattern editing, per-voice tuning, fills, user samples, and a full Drum Companion are later milestones.

## Longer-phrase behavior

Exact Replay and Original Reply can retain the complete captured timeline. The current Trap Pocket, R&B Flow, and Techno Motion Phrase Architecture profiles deliberately create compact one-bar answers from the longer source; they are not yet arrangement generators.

## Honest limits

This remains a source-compiled browser simulation, not physical FM-1 validation. There is no external MIDI clock, song/project persistence, audio sampling, drum-pattern memory, or hardware firmware image. Human testing must determine whether the reference beat makes current Follow bass musically legible.
