# Project plan

Status: first deterministic musical Reply implemented in the browser prototype. Updated: 2026-10-28.

Current browser milestone: 0.4.0 Phrase Architecture gives Reply an independent event count and adds experimental structural personalities with rests, repetition, accents, note-length hierarchy, and ratchets. Browser CI and owner listening remain gates; this is not a plugin or hardware firmware build.

Next implemented browser milestone: 0.5.0 Keep and Candidate Memory provides three deep-copy session slots for generated responses. Compiled-browser and owner validation remain gates.

Current implementation milestone: 0.7.2 **Reply–Bass Synchronization** derives Follow timing anchors from the completed transformed Reply instead of an independent fixed grid. The upper phrase is fully shaped by Phrase Architecture and Reply Groove first; bass pitch, density, and pocket are then built against that exact result. Native, compiled-WASM, worklet, served/direct-file Chrome, MIDI, and mobile-layout checks pass. Owner musical validation remains the next gate.

Name exploration: **Vapor Loop** is the owner's current leading candidate for the finished instrument name. ECHOLOG and psychic-octo-spork remain the prototype/interface and repository names until a final naming decision is made.

## Purpose
Develop an original, unusual, musically useful firmware for the M-VAVE FM-1. This project is separate from the completed Local LLM Infrastructure Lab.

## Creative direction
The first prototype direction is ECHOLOG: melodic conversation. The final product scope and name remain open. psychic-octo-spork is the working code name.

| Candidate | Core interaction | Question to resolve |
| --- | --- | --- |
| GENOME | Combine, mutate, and select musical phrases and sounds across generations | Can inheritance and player selection distinguish it from existing evolving sequencers? |
| ECHOLOG | Remember played phrases and respond with related variations | Can responses remain recognizable, useful, and responsive? |
| GRAVITY | Use movement, attraction, and collisions to generate music | Can it offer a distinct interaction from existing physics and orbital instruments? |

On 2026-10-28, the owner approved melodic conversation as the first ECHOLOG experiment. See [the prototype specification](PROTOTYPE_SPEC.md). Other ideas remain available for later evaluation. Do not claim worldwide originality based on a preliminary search.

## Milestones
1. Compare existing firmware and document concrete overlap.
2. Verify platform choices, licenses, toolchain, memory limits, and recovery behavior.
3. Choose a small musical interaction and write its acceptance criteria.
4. Build an audible host/browser prototype and evaluate it with the project owner.
5. Adapt the shared instrument core into a VST3 plugin for macOS and Windows; validate it in actual DAWs on both platforms.
6. After browser and desktop validation, establish a measured hardware target build and designate one development FM-1.
7. Only then perform hardware flashing and validate installation, operation, persistence, recovery, and departure to another firmware.
8. Publish source, documentation, compiled firmware, and test evidence.
9. Submit the tested release to The Firmware Hub.

## Execution order agreed 2026-10-28
Browser/emulator first, desktop plugin second, hardware testing last. No hardware flashing is authorized at this stage. Keep the musical core portable so browser, plugin, and firmware can share behavior. Browser playback alone does not establish hardware timing or compatibility.

A local Codex installation is optional for the browser phase. It becomes useful when working with compilers, plugin builds, DAWs, and logs on the owner's computer. Native macOS and Windows plugin testing requires access to those environments; installing Codex does not provide their toolchains or substitute for testing.

## First prototype proposal
Approved direction: capture a monophonic phrase of up to 32 notes; play a related response using one lightweight internal sound. Start with explicit capture/reply controls. Automatic phrase detection, multiple memories, and richer synthesis are later possibilities.

## Recorded future concept — Chord Companion

The owner proposed a third musical role that can automatically create scale-aware chords around the lead, with automatic accompaniment disabled when the player wants direct keyboard performance. This is intentionally deferred until Reply–Bass synchronization is validated. Honest implementation requires polyphony and input-role design because the current upper engine is monophonic. See [CHORD_COMPANION_CONCEPT.md](CHORD_COMPANION_CONCEPT.md).

## Release goals
- Dedicated public GitHub source repository with appropriate licensing and upstream credits.
- Installation instructions, user guide, technical design, and transparent AI attribution.
- Versioned compiled firmware downloads and checksums.
- Compatible browser playback if validated.
- Documented hardware compatibility and tested recovery procedure.
- Community directory submission after hardware validation.

## Development method
Work in small, explainable steps. Each meaningful change includes relevant documentation and evidence. Distinguish proposed features, host tests, emulator tests, and physical-device results. Keep concept decisions reversible until musical testing supports them.

## Browser foundation selected
Use the pinned foma browser subset described in [BROWSER_BUILD.md](BROWSER_BUILD.md). A portable integer C voice and phrase engine build to WebAssembly and pass local signal/integration checks. No installer is bundled. See [validation](BROWSER_VALIDATION.md) for browser versus host evidence.


## Capture/replay milestone — 0.0.2
The first memory engine captures and replays one monophonic phrase with original pitches and recorded event timing. It enforces 32-note/eight-second limits, retains a partial take on Stop, and provides Clear. See [FIRST_MEMORY.md](FIRST_MEMORY.md). The owner validated all four memory controls in the downloaded 0.0.2 build.

## First transformed reply — 0.0.3
The first Reply preserves the captured onset times, durations, and velocities. It places the first answer note a perfect fifth above the captured first note, then mirrors every subsequent interval: `reply = 2 × first + 7 − captured`. MIDI-range results are clamped safely. This deterministic rule is a musical baseline for owner evaluation, not the final variation system. See [FIRST_REPLY.md](FIRST_REPLY.md).
