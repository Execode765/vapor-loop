# Chord Companion — recorded future concept

Status: approved direction for later design; not implemented in 0.7.2.

## Musical intention

Chord Companion would add a harmonic partner to the existing lead/Reply and Bass Companion roles.

- **Companion enabled:** the instrument derives scale-aware chord voicings from the active lead or generated Reply and places changes at meaningful phrase boundaries.
- **Companion disabled:** automatic chords stop and the keyboard remains available for direct performance, including lead playing and, after polyphony is implemented, manually played chords.
- Automatic chords should support the lead rather than duplicate every note. Voicing, inversion, register, rhythm, density, and tension need intentional limits.

## Required foundation

The current upper synthesizer voice is monophonic. A genuine Chord Companion therefore requires:

1. A bounded polyphonic voice allocator suitable for the browser core and eventual FM-1 limits.
2. Clear routing between lead input, manual chord input, automatic chords, and the existing bass lane.
3. Scale/chord selection rules that remain deterministic and explainable.
4. Shared timing anchors so lead, chords, and bass use the same completed phrase structure.
5. Tests for stuck notes, voice stealing, MIDI channel behavior, Stop/panic, and CPU/headroom limits.

The concept will be designed after the synchronized Bass Companion is musically validated. No hardware or plugin claim is attached to this note.
