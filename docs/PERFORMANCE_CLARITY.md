# Performance Clarity — 0.10.1

Version 0.10.1 is a usability-only refinement of the owner-validated 0.10.0 engine. It does not change note generation, split routing, polyphony, sequencing, capture timing, Reply, Bass Companion, Candidate Memory, or drum behavior.

## Renamed controls

| Earlier label | 0.10.1 label | Musical meaning |
| --- | --- | --- |
| Adventure | Melodic variation | How far Reply may move from the captured pitch choices |
| Architecture | Phrase style | The event pattern: note count, rests, motifs, accents, and ratchets |
| Intensity | Phrase amount | How strongly the selected phrase style restructures Reply |
| Shape | Timing amount | How strongly the selected groove changes final timing |
| Off / Light / Tight | Free / Humanized / Grid | No onset correction, gentle correction, or firm sixteenth-grid correction |

The internal parameter names remain stable to avoid unnecessary engine risk.

## Exact split behavior

Every split option now includes its MIDI note number. Notes below the selected number route to **Bass**. The selected note and all higher notes route to **Lead**.

For example, with **G3 · MIDI 55** selected:

- F♯3 · MIDI 54 routes to Bass.
- G3 · MIDI 55 routes to Lead.
- A♭3 · MIDI 56 routes to Lead.

Some controllers label middle C and every octave one number differently. MIDI 55 is still MIDI 55 regardless of whether a screen calls it G2 or G3.

## Live routing readout

Incoming notes display:

- exact MIDI number;
- Vapor Loop note name;
- destination track;
- velocity while held.

Bass, Lead, and Poly Keys use distinct colors. This readout is diagnostic UI; it does not alter the MIDI message sent to the audio engine.

## Validation boundary

0.10.0 has passed owner hands-on musical testing. The 0.10.1 release must still pass the existing native C, WebAssembly, AudioWorklet, served Chrome, direct-file Chrome, Web MIDI, and responsive-layout regression pipeline. No FM-1 firmware image or hardware flash is part of this milestone.
