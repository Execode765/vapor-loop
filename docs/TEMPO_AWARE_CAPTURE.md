# Tempo-Aware Capture — prototype 0.8.0

## Why it exists

Earlier capture began its timeline on the first played note. That made a pickup or syncopated entrance indistinguishable from beat one. Phrase Architecture could therefore rotate a musically valid performance around the wrong origin even when Reply and bass were technically synchronized.

Tempo-Aware Capture establishes the clock before recording:

1. Press **Capture**.
2. Hear one count-in bar at the selected BPM. Beat one is accented.
3. Recording begins on the following beat one and continues for the selected 1, 2, or 4 bars.
4. The metronome can remain audible during capture.
5. Recording ends automatically at the bar boundary.

The note limit remains 32. Reaching it stops storing additional notes but does not move the bar boundary; this keeps the recorded phrase length deterministic.

## Input correction

- **Off** preserves captured note onsets.
- **Light** moves each onset halfway toward the nearest sixteenth-note boundary.
- **Tight** places each onset on the nearest sixteenth-note boundary.

Correction changes note onsets, not pitches or velocities. Note lengths remain performance-derived and are only shortened when necessary to prevent one monophonic event from crossing the next corrected onset.

## Scope and limitations

This is an internal browser clock, not MIDI clock synchronization. It uses 4/4 and a one-bar count-in. It does not yet record notes played before the recording downbeat as negative-time pickups; play an intended pickup inside the selected capture window. Phrase Architecture still supplies its designed structural personalities after capture. Human listening remains required to judge how much correction suits different performances.

No plugin or FM-1 hardware timing claim is made from this browser implementation.

## Extended in 0.8.1

[Rhythm Reference and Extended Capture](RHYTHM_REFERENCE.md) adds a selectable one/two-bar count-in, 1/2/4/8/16-bar capture, 128-note capacity, visible bar/beat position, and an optional synthesized reference kit. The v0.8.0 rules above describe the original milestone; the 0.8.1 guide is authoritative for the current controls.
