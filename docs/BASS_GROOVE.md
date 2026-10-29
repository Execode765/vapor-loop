# Bass Groove Engine — prototypes 0.7.0–0.7.1

Version 0.7.0 preserves the independent Bass Companion voice and adds a separate rhythmic phrasing stage. Root and Follow still decide **which pitch** to play; Movement still sets the maximum activity; Bass Groove decides **which pulses breathe, where they sit, how long they last, and how strongly they speak**.

## Styles

- **Straight** is the exact 0.6.0 pulse behavior. It is the permanent A/B reference and safety bypass.
- **Auto** reads the active upper Phrase Architecture. Trap Pocket, R&B Flow, and Techno Motion select the matching bass profile. A kept candidate uses its own stored architecture context.
- **Trap Pocket** creates deliberate gaps, late answers, shorter punctuation, and stronger primary accents.
- **R&B Flow** uses a restrained ahead/behind pocket, varied gates, softer connecting notes, and an optional breath.
- **Techno Motion** retains a steady drive while alternating gates, accents, and a small off-beat drag.

## Groove amount

Amount zero is an exact bypass even when a non-Straight style is selected. Higher settings interpolate gate length, timing displacement, and velocity toward the chosen profile. Rests enter only after defined thresholds so low settings remain subtle.

The default is **Auto 55**, intended as an audible but controlled starting point. The goal is pocket, not maximum complexity.

## 0.7.1 short-phrase refinement

Owner video showed that a short two-opportunity Trap phrase could leave only one automatic bass event. Follow correctly chose G3 for the first variation and C3 for the second, but the higher single note blended into the upper sine voice and sounded like a one-Reply lag.

Version 0.7.1 makes the musical intention explicit:

- The first Follow event always anchors the selected root.
- A phrase with only two bass opportunities retains both events.
- The second event can still receive profile timing, gate, and velocity shaping.
- Longer phrases retain their strategic-rest behavior.
- Straight and amount zero remain exact bypasses.

## Preserved behavior

- Off, Manual, Root, and Follow modes are unchanged.
- Manual notes and MIDI channel 2 still override automatic bass without interrupting the upper phrase.
- Pedal remains a continuous foundation and intentionally bypasses rhythmic shaping.
- Straight preserves the previous even pulse.
- Stop and panic still silence both voices.
- Bass octave, volume, candidate memory, Replay, and the upper phrase engine remain independent.

## Honest limitations

These are deterministic pattern personalities, not learned genre emulations or claims of authentic session-bassist performance. The engine does not yet detect chords, record manual bass, follow MIDI clock, glide between notes, or use a dedicated bass waveform/filter. Musical usefulness requires owner listening tests.
