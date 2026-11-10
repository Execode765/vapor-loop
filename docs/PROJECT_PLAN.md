# Project plan

Status: Vapor Loop 0.12.4 protected browser gain staging implemented for validation. Updated: 2026-11-10.

Current implementation milestone: 0.12.4 adds a browser-only +12 dB default output stage, optional +18 dB recording boost, and −3 dBFS safety limiter while retaining the 0.12.3 engine-state repair and all prior musical behavior.

Owner testing confirmed the 0.12.3 FM Glass repair through Capture, Replay, Reply, Keep, and kept-candidate playback. The test video again captured much quieter audio than the live MPC monitoring path, so 0.12.4 repairs browser gain staging without increasing synthesis amplitude or changing the future hardware path. The present scrolling interface has still reached its usability limit.

Immediate next milestones:

1. **0.12.3 Engine-state repair — owner validated** — preserve the selected synthesis engine through all transport and memory operations and add native, WASM, and browser regression coverage.
2. **0.12.4 Gain-staging repair — implemented for validation** — raise browser listening/recording level behind a protected output stage while leaving the portable C core unchanged.
3. **0.13.0 Vapor Loop Workbench** — reorganize the browser app into Play, Shape, Companions, and Setup pages with persistent transport/status and a visual multi-lane timeline.
4. Use the Workbench timeline, playhead, and lane mute/solo controls to investigate the owner's unresolved timing observation.
5. Build a standalone desktop wrapper around the shared C core only after the browser instrument is stable; follow with VST3 and DAW integration.

The detailed evidence, cautions, acceptance criteria, and restart procedure are recorded in [V0.12.2_TEST_CHECKPOINT.md](V0.12.2_TEST_CHECKPOINT.md).

Name decision: **Vapor Loop** is both the instrument name and the public repository identity. ECHOLOG identifies the initial conversation prototype.

## Purpose
Develop an original, unusual, musically useful firmware for the M-VAVE FM-1. This project is separate from the completed Local LLM Infrastructure Lab.

## Creative direction
The initial ECHOLOG direction—melodic conversation—became Vapor Loop: musical ideas appear, transform, and disappear unless captured. The repository was renamed to `vapor-loop` and made public on 2026-11-09.

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
5. Adapt the shared instrument core into a standalone desktop application and validate audio, MIDI, state, and transport behavior.
6. Add a VST3 wrapper for macOS and Windows and validate it in actual DAWs on both platforms.
7. After browser and desktop/plugin validation, establish a measured hardware target build and designate one development FM-1.
8. Only then perform hardware flashing and validate installation, operation, persistence, recovery, and departure to another firmware.
9. Publish source, documentation, compiled firmware, and test evidence.
10. Submit the tested release to The Firmware Hub.

## Execution order agreed 2026-10-28
Browser simulation first, desktop standalone second, VST3 plugin third, hardware testing last. No hardware flashing is authorized at this stage. Keep the musical core portable so browser, plugin, and firmware can share behavior. Browser playback alone does not establish hardware timing or compatibility.

A local Codex installation is optional for the browser phase. It becomes useful when working with compilers, plugin builds, DAWs, and logs on the owner's computer. Native macOS and Windows plugin testing requires access to those environments; installing Codex does not provide their toolchains or substitute for testing.

## First prototype proposal
Approved direction: capture a monophonic phrase of up to 32 notes; play a related response using one lightweight internal sound. Start with explicit capture/reply controls. Automatic phrase detection, multiple memories, and richer synthesis are later possibilities.

## Recorded future concept — Chord Companion

The owner proposed a third musical role that can automatically create scale-aware chords around the lead, with automatic accompaniment disabled when the player wants direct keyboard performance. Automatic generation remains deferred, but 0.10.0 establishes the required four-voice manual chord track and input-role design. The next Chord Companion milestone must build on owner-validated polyphony rather than replacing it. See [CHORD_COMPANION_CONCEPT.md](CHORD_COMPANION_CONCEPT.md).

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

