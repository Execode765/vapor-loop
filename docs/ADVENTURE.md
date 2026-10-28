# Adventure — prototype 0.2.0

Adventure turns one deterministic scale-aware answer into a family of related answers. It is intentionally constrained: variation should increase without discarding the source phrase's identity or leaving its musical context.

## Use

1. Open `preview/echolog-adventure-v0.2.0.html` in desktop Chrome and enable sound.
2. Choose the key center and scale.
3. Capture and Finish a phrase with at least four or five notes. Two-note phrases have no interior notes to vary.
4. Set **Adventure** and press **Reply** repeatedly.
5. Use **Replay** at any time to compare against the unchanged source.

Changing the Adventure slider resets the variation sequence. Starting a new Capture also resets it. At Adventure 0, every Reply is the approved 0.1.0 baseline regardless of variation number.

## Levels

The UI describes the continuous 0–100 range as:

- **0 · Faithful:** no substitutions; identical to 0.1.0.
- **10–30 · Gentle:** occasional one-scale-step substitutions.
- **40–60 · Curious:** more frequent one-step substitutions.
- **70–80 · Bold:** frequent changes, with some two-step movement.
- **90–100 · Wild:** most interior notes are candidates; two-step changes become more likely.

The labels communicate intent rather than distinct algorithms. Probability is based on the exact slider value.

## Protected properties

- Selected root and scale
- First answer note/anchor
- Register range, MIDI 36–96
- Final tonic-triad function, though Adventure may choose a different root/third/fifth than the baseline
- Original onset timing, rests, durations, velocities, phrase length, and note count
- Exact Replay of the source

Only interior answer pitches vary. A deterministic integer pseudo-random sequence is seeded by the source pitches, key, scale, and variation number. There is no heap allocation, external randomness, JavaScript timing dependency, or model inference.

## Current limitations

- Adventure modifies pitch only; rhythm remains unchanged.
- Each variation is temporary. Keep/save is not implemented yet.
- The generator does not know a chord progression.
- High settings can still sound busy or arbitrary despite remaining in scale.
- Very short phrases have few or no eligible interior notes.
- The forced cadence can still create a noticeable final movement.

Owner listening at several settings determines whether the probability curve and step sizes feel useful before Keep or rhythmic variation is added.

## Validation

Native sanitizer tests require Adventure 0 to reproduce the approved baseline, require multiple Adventure 100 generations to differ, and verify protected anchors, tonic-triad cadence, register limits, and scale membership across all 48 key/scale contexts. WebAssembly tests verify the control and variation counter. Real-Chrome tests exercise an Adventure 80 three-note phrase and two successive replies in both served and downloaded builds.

This milestone does not validate VST3 behavior or physical FM-1 firmware.
