# Bass Companion — prototypes 0.6.0–0.7.0

Version 0.6.0 adds a genuinely independent second synthesis voice. Bass can sound simultaneously with live playing, exact Replay, generated Reply, or a kept candidate without stealing the upper voice.

Owner testing passed the complete 0.6.0 functional design: Off/Manual/Root/Follow, movement choices, octave range, candidate playback, and the independent upper/bass relationship all worked. The owner retained the deep range after confirming that octave 3 makes it audible on smaller speakers. The remaining musical finding was that automatic bass followed correctly but pulsed too evenly.

Version 0.7.0 answers that finding with a separate [Bass Groove Engine](BASS_GROOVE.md). Straight remains the exact original behavior; Auto and explicit Trap, R&B, and Techno profiles add controlled space, timing, gate, and accent variation.

## Modes

- **Off** leaves the previous instrument behavior unchanged.
- **Manual** accepts bass notes from the dedicated browser keys, computer keys Z/X/C/V/B/N/M, or MIDI channel 2.
- **Root** plays the selected key center in the chosen low octave.
- **Follow** samples the upper phrase at bass-rhythm boundaries, reduces its density, and chooses the closest low root, third, or fifth. It responds to the variation without copying every upper event.

## Movement

- **Pedal** sustains one bass decision through the phrase.
- **Half notes** creates one event every two beats.
- **Quarter notes** creates one event each beat.

Timing uses the phrase or kept candidate BPM. Bass octave ranges from 0 through 3. Bass volume scales the second voice before a bounded two-voice mix.

## Manual override

Manual notes work in Manual, Root, and Follow modes. A manual note takes control of the bass voice without interrupting upper-phrase playback. Releasing it lets automatic accompaniment rejoin at the next scheduled boundary. Stop silences both voices.

## MIDI

MIDI channel 1 and all channels other than channel 2 continue to feed the upper instrument. MIDI channel 2 feeds Bass Companion. CC 120/123 remains a global panic/Stop command.

## Candidate behavior

Follow uses the kept candidate's stored pitch, timing, root, scale, BPM, and Phrase Architecture context. Bass mode, movement, octave, volume, groove style, and groove amount remain live controls, allowing the same kept upper phrase to be auditioned with different foundations.

## Honest limitations

This is a two-voice browser prototype, not chord recognition or a finished bass synthesizer. Manual bass is not recorded or kept. There is no MIDI clock, configurable channel/split, glide, filtering, effects, waveform selection, chord progression, compression, or hardware resource measurement yet.
