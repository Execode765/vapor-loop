# Track and Sequencer Foundation — 0.9.0

Vapor Loop 0.9.0 gives the lead, bass, and drums one bounded musical timeline. It is the first architectural step from the original ECHOLOG conversation experiment toward a playable melodic-loop workstation.

## Clock

- 96 pulses per quarter note (PPQN)
- 1, 2, 4, 8, or 16 bars
- Quarter-, eighth-, or sixteenth-note manual-bass input resolution
- Integer frame/tick conversion at 44.1 kHz
- Maximum 128 portable sequence events

The resolution setting controls input correction for the manual bass pass. The 96-PPQN representation deliberately leaves room for later triplets, dotted divisions, ratchets, swing, and 32nd-note editing without replacing the stored event model.

## Track behavior

| Track | Manual behavior | Companion behavior |
| --- | --- | --- |
| Lead | Existing capture and exact Replay | Reply produces related variations |
| Bass | Live performance or one recorded pass | Root or Follow accompaniment |
| Drums | One-bar 16-step kick/snare/hat pattern | Synchronized diagnostic reference groove |
| Chords | Reserved | Reserved |

The browser MIDI Input Track control routes incoming note messages to Lead or Bass. Hardware-oriented MIDI channel 2 bass routing remains in the C adapter.

## Recording a bass pass

1. Capture a lead phrase with the tempo-aware transport.
2. Set Bass mode to **Manual**.
3. Choose a sequence resolution.
4. Select **Record bass pass**.
5. Wait through the configured count-in.
6. Play the bass keys, `Z X C V B N M`, or route Web MIDI to Bass.
7. The original lead and active drum track play once while the bass is recorded.

The completed bass sequence is retained in session memory and accompanies Replay, Reply, and kept candidates until it is replaced or a new source phrase is captured.

## Manual drums

Set Drum Track to **Manual** and toggle the Kick, Snare, and Hat cells. The one-bar 16-step pattern repeats over the selected phrase length. This uses the existing compact integer diagnostic voices; it is not yet the planned full Drum Companion or classic-machine-inspired sound set.

## Deliberate limits

This release does not add polyphonic synthesis, chords, sample loading, persistence, MIDI clock, advanced drum voices, automation, probability, ties, triplets, ratchets, effects, plugin formats, or hardware flashing. Those features build on this clock only after browser validation.

