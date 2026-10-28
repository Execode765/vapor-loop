# Scale-aware musical Reply — prototype 0.1.0

Version 0.1.0 replaces the failed chromatic inversion with the first explicitly musical response strategy. It is intentionally deterministic so every result can be explained, reproduced, and evaluated.

## Try it

1. Open `preview/echolog-scale-reply-v0.1.0.html` in desktop Chrome and enable sound.
2. Choose a **Key center** and **Scale** before selecting Reply.
3. Capture and Finish a short melody. Input may come from Web MIDI, computer keys, mouse, or touch.
4. Select **Replay** to hear the source exactly.
5. Select **Reply** to hear the scale-aware answer.

Start with **C major** and play C4–D4–E4–D4–C4. The deterministic baseline answer is G4–F4–E4–F4–G4. Unlike the 0.0.3 diagnostic answer, E4 remains inside C major rather than becoming E-flat.

## Available contexts

- Twelve chromatic key centers
- Major
- Natural minor
- Major pentatonic
- Minor pentatonic

The selection affects Reply only. Replay always preserves the captured pitches exactly, even when the source contains notes outside the chosen scale.

## Strategy

The portable `src/harmony.c` module:

1. Maps each captured pitch to the nearest degree of the selected scale. Exact halfway ties choose the lower scale tone.
2. Starts the answer near a perfect fifth above the source's first note, snapped into the scale.
3. Converts each movement between source scale degrees into equal contrary movement in the answer.
4. Moves octave equivalents when necessary to keep output between MIDI notes 36 and 96.
5. Moves the final answer note to the nearest tonic-triad tone: root, scale-appropriate third, or perfect fifth.

Rhythm, rests, note durations, velocities, phrase length, and note count remain unchanged. The engine uses fixed arrays and integer operations, with no heap allocation or model dependency.

## Known musical limitations

- The player must provide the key and scale; there is no key detection.
- There is one static harmonic context, not a chord progression.
- Nearest-scale snapping can reinterpret chromatic approach notes unexpectedly.
- Contrary motion is always used and may become predictable.
- The forced final cadence can create a noticeable last-note change.
- Rhythm is copied exactly; there is no call-and-response rhythmic development.

These constraints are deliberate. Owner listening determines whether this strategy is a useful baseline before adding Adventure, chord awareness, or probabilistic variation.

## Tests

Native sanitizer tests cover every combination of 12 roots and four scales, ensuring all generated notes belong to the selected scale and stay within the reply register. Tests also verify a known C-major phrase, cadence behavior, preserved timing/velocity, WebAssembly harmony setters, and previous memory/audio behavior. Real-Chrome automation changes root/scale and exercises Reply in both the served and direct-file builds.

This milestone does not validate VST3 behavior, FM-1 hardware timing, control mapping, or flash safety.
