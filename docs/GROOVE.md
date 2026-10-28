# Groove and phrase shaping — prototype 0.3.0

Version 0.3.0 separates rhythmic response from pitch response. `src/groove.c` transforms Reply onset times and note lengths after harmony/Adventure chooses pitches. Exact Replay continues to use the untouched captured events.

## Controls

- **BPM:** 40–240. Defines the sixteenth-note grid used by Pulse, Swing, and Pocket.
- **Groove:** Original, Pulse, Swing, Pocket, or Half-time.
- **Shape:** 0–100. Blends from the captured rhythm toward the selected profile.

Changing a groove setting resets the Reply variation sequence so comparisons are reproducible.

## Profiles

### Original

Preserves captured onsets and durations. Shape has no timing effect. This is the 0.2.0 behavior.

### Pulse

Moves onsets toward the nearest sixteenth-note grid and moves durations toward three quarters of a sixteenth. Higher Shape values create tighter, more staccato answers suitable for testing clocked electronic phrasing.

### Swing

Moves onsets toward the sixteenth grid, then delays odd sixteenths. At maximum Shape, the delay is one third of a sixteenth, producing the long-short placement associated with 66% swing. Durations become moderately gated.

### Pocket

Uses lighter quantization, delays alternating notes by up to one fifth of a sixteenth, and alternates shorter and longer target durations. This is a general laid-back timing experiment, not a claim of authentic R&B performance modeling.

### Half-time

Scales onsets and durations continuously from 1× to 2×. An eight-second source can therefore produce a reply up to sixteen seconds. Pitch, count, and velocity do not change.

## Safety and invariants

- Event order is always strictly increasing, even if quantization would collapse two notes onto one boundary.
- Note durations remain at least one audio frame.
- Reply playback is bounded to sixteen seconds.
- Note count, velocity, harmony, Adventure pitch result, and exact Replay are unchanged.
- All timing uses 44.1 kHz integer audio frames; JavaScript timers do not schedule notes.

## Known limitations

- BPM is manual; MIDI clock is not connected.
- Profiles transform existing notes but do not add ratchets, drop notes, repeat motifs, or insert new rests.
- There is no bar/beat detection or time-signature model.
- Pocket is a timing heuristic, not learned human groove.
- A monophonic sine voice still limits style perception.
- Groove settings are session-only, and Keep is not implemented.

This milestone establishes the timing architecture. Modern trap, R&B, techno, and other style targets will require chord context, phrase grammar, event addition/removal, articulation, and sound design beyond these foundational profiles.

## Validation

Native sanitizer tests verify exact Original timing, Pulse quantization, Swing displacement, Pocket ordering, Half-time doubling, all profiles across Shape values, BPM bounds, duration safety, and maximum reply length. WebAssembly tests verify control export and Half-time playback length. Chrome automation selects 128 BPM / Swing / Shape 80 and completes successive Adventure replies in served and direct-file builds.
