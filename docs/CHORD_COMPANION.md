# Chord Companion — prototype 0.12.0

Chord Companion extends Vapor Loop's existing four-voice Poly Keys lane. It does not add a second unbounded synth or claim finished harmonic intelligence; it adds a compact, deterministic harmony layer suitable for browser prototyping and later FM-1 feasibility work.

## Modes

| Mode | Behavior |
| --- | --- |
| Off · Live free | No generated harmony. Incoming Poly Keys notes remain playable but are not recorded. |
| Manual · Recordable | Incoming Poly Keys notes are playable and can be captured as a quantized chord pass. |
| Root Follow | Holds a three-note, scale-aware triad anchored to the selected key center. |
| Phrase Follow | Generates a three-note, scale-aware triad from each Lead playback onset. |

Root and Phrase Follow accept live Poly Keys input. The generated triad stops while the player holds manual notes, then automatic operation can resume. Chord volume remains independent of Lead and Bass.

## Musical rules

The current builder stacks scale steps into a deterministic root-position triad and keeps notes in a bounded MIDI register. Major, natural minor, Dorian, Mixolydian, and the two pentatonic scales use the same scale-membership guarantee.

This release does **not** implement smooth inversions or sophisticated voice-leading. Those require a deliberate musical pass so the harmony moves economically rather than merely remaining legal.

## MIDI

The existing routing remains unchanged:

- MIDI channel 1: Lead
- MIDI channel 2: Bass
- MIDI channel 3: Poly Keys / Chord Companion manual override
- Selected input routing and Split mode continue to work as before.

## Test focus

1. Capture a short phrase and Replay it.
2. Select Root Follow; confirm a held three-note harmony.
3. Select Phrase Follow; confirm the harmony retriggers with Lead events.
4. While either follow mode plays, send channel-3 notes; confirm live playing temporarily replaces the generated chord.
5. Return to Off; confirm channel-3 notes remain freely playable.
6. Return to Manual; record and replay a chord pass to confirm the 0.10 behavior remains intact.

## Deferred

Smooth inversions, richer chord vocabulary, chord rhythm/stab patterns, sustain pedal handling, chord sound presets, effects, plugin builds, and hardware flashing are separate milestones.
