# Phrase Architecture — prototype 0.4.0

Version 0.4.0 adds a portable structural stage between harmony and final groove timing. Unlike the 0.3.0 groove profiles, Phrase Architecture may change the number of Reply events. Exact Replay still reads the untouched capture.

## Signal path

1. Capture stores source notes, velocities, onsets, and durations.
2. Harmony and Adventure choose scale-safe Reply pitches.
3. Phrase Architecture chooses rests, repetitions, note count, accents, and a one-bar pattern at the selected BPM.
4. Groove applies final quantization, swing, pocket displacement, or half-time scaling.
5. The monophonic voice renders the result.

## Experimental personalities

- **Original** preserves the source event structure.
- **Trap Pocket** creates a sparse syncopated pattern, accented anchors, short gates, and intensity-dependent ratchets.
- **R&B Flow** creates wider gaps, mixed note lengths, softer intervening notes, and a call-and-response shape.
- **Techno Motion** cycles a four-note motif on an even-step framework and adds events as intensity rises.

Intensity zero always preserves the original structure. Above zero, the selected personality generates a one-bar response. Higher intensity currently adds events at fixed thresholds; it is not a continuous genre-authenticity control.

## Honest limitations

The names describe design targets, not certified genre emulations. The prototype has no bar-position detection, MIDI clock, chord progression, polyphony, drum context, automatic key detection, learned style model, or audio time-stretching. It uses manual BPM and deterministic integer rules suitable for later embedded work.

The critical architectural change is that Reply now owns an independent note count, pitch list, velocity list, onset list, duration list, and phrase length. This makes removal, repetition, rests, accents, and ratchets possible without changing the recorded phrase.
