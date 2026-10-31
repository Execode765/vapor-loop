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

## Future direction: Manual Chord Designer

After the 0.12.x Companion foundation is validated, the non-generative side may evolve from a plain keyboard lane into an intentional chord-performance instrument.

Proposed behavior:

- **Companion modes:** Root Follow and Phrase Follow continue generating harmony from Vapor Loop's musical context.
- **Manual Designer mode:** one input chooses a root while chord controls choose quality and extensions.
- **Chord vocabulary:** major, minor, diminished, suspended, sixth, minor seventh, major seventh, dominant seventh, ninth, and a deliberately curated set of modern voicings.
- **Voicing control:** inversions, spread, register, drop voicings, and smooth movement between consecutive chords.
- **Performance shaping:** strum up/down, two-octave strum, humanized/slop timing, tempo-synced arpeggiation, rhythmic patterns, and harp-like cascades.
- **Shared treatment:** a selected chord can feed the existing looper, candidate memory, bass relationship, future effects, and external MIDI without duplicating separate musical state.
- **Hardware discipline:** the eventual FM-1 mapping must remain playable from its limited keys, buttons, knobs, screen, memory, and CPU budget.

Interaction references include Telepathic Instruments Orchid and the open-source ChoralRoot FM-1 project. Vapor Loop should learn from their immediacy while retaining its own identity: captured phrases, disappearing variations, generated replies, candidate memory, and switchable player/companion relationships.

This is a later refinement, not part of the 0.12.0 validation build. Before implementation, define an original control map and review applicable licenses and attribution requirements. Do not copy names, visual identity, presets, proprietary assets, or undocumented behavior.

