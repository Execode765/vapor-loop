# Lead Engine Lab — 0.11.0

Version 0.11.0 begins Vapor Loop's portable sound-design layer without changing the validated capture, Reply, Candidate Memory, split, Bass Companion, Poly Keys, drum, MIDI, or sequencing behavior.

## Engines

| Engine | Method | First preset goal |
| --- | --- | --- |
| Sine Diagnostic | Original wavetable sine | Neutral regression reference |
| FM Glass | Two-operator phase modulation at a 2:1 ratio | Bright, glassy digital lead |
| Virtual Analog Warm | Saw oscillator through a one-pole integer low-pass stage | Rounded subtractive lead |
| Karplus–Strong String | Noise-excited, damped delay-line feedback | Plucked physical-model voice |

## Engineering constraints

The synthesis core uses integer arithmetic, fixed storage, no heap allocation, and the existing 44.1 kHz render path. Karplus–Strong uses a bounded 2,048-sample signed 16-bit delay line. Very low notes that require a longer period are deliberately clamped in this first experiment.

Engine changes reset model-specific phase/filter state but preserve the instrument's phrase memory and control settings. The Lead engine applies to live Lead input, Replay, Reply, and kept candidates. Bass and Poly Keys retain their validated voices in this release.

## Why fixed presets first

Operator ratio/index, oscillator shape, filter cutoff/resonance, envelopes, damping, brightness, and modulation are intentionally not exposed yet. First, owner listening must establish that each model is audibly distinct, stable, useful, and directionally suitable for FM-1 resources.

## Next gates

1. Full native, WebAssembly, AudioWorklet, served Chrome, and standalone Chrome regression pass.
2. Owner comparison of all four engines using the same phrase.
3. Refine levels and tone based on that recording.
4. Expose a small shared macro set.
5. Begin the synthesized drum-engine milestone with machine-inspired, original models.
