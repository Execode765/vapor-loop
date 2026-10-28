# First transformed Reply — browser prototype 0.0.3

This milestone turns ECHOLOG from a phrase recorder into the beginning of a musical conversation. Exact Replay remains available for comparison; Reply transforms the melody while preserving its rhythmic identity.

## Try it

1. Open `preview/echolog-soundcheck.html` in Chrome and select **Enable sound**.
2. Select **Capture**, play a short melody, and select **Finish**.
3. Select **Replay** to confirm the original phrase.
4. Select **Reply** to hear ECHOLOG's transformed answer.
5. Try a phrase with a clearly rising or falling shape. A rising source should produce a falling answer, and vice versa.

Stop interrupts either playback without erasing the phrase. Clear erases it. Capture replaces it.

## Transformation rule

The first answer note is one perfect fifth—seven semitones—above the captured first note. Every source interval is then inverted around that reply anchor:

```text
reply pitch = (2 × first captured pitch) + 7 − current captured pitch
```

For example, a source phrase `C4, E4, G4` (60, 64, 67) becomes `G4, E♭4, C4` (67, 63, 60). The source rises; the answer falls. Repeated notes remain repeated. Results outside MIDI 0–127 are clamped to the nearest valid note.

## What stays unchanged

- Relative note onsets and rests
- Note durations
- Captured velocities
- Phrase length and note count

This makes the relationship audible without merely transposing or replaying the phrase. The rule uses fixed integer arithmetic, no random generator, heap allocation, model, or JavaScript timing.

## Scope and next decision

This is Reply One: a testable baseline, not the final ECHOLOG variation engine. Owner listening will determine whether the contour mirror feels conversational and whether the fixed fifth is musically useful. Later work may add selectable response character, scale-aware pitch handling, register management, rhythmic mutation, and controlled unpredictability—but only after this behavior is evaluated.

No VST3 plugin or FM-1 hardware image is included in this milestone.
