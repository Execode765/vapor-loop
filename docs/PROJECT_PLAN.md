# Project plan

Status: first deterministic musical Reply implemented in the browser prototype. Updated: 2026-10-28.

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

## Scale-aware response milestone — 0.1.0
The first musical strategy is isolated in `src/harmony.c` so response rules can evolve without coupling them to phrase storage or synthesis. It supports 48 manual key/scale contexts, contrary diatonic motion, register bounds, and a tonic-triad cadence while retaining recorded timing and dynamics. Automated theory constraints are necessary but not sufficient: owner listening determines whether this becomes a baseline or is replaced. See [MUSICAL_REPLY.md](MUSICAL_REPLY.md).

## Controlled variation milestone — 0.2.0
Adventure makes successive Replies vary while retaining the approved scale-aware baseline at zero. It changes interior scale degrees probabilistically and protects timing, expression, scale, register, opening anchor, and tonic-triad cadence. The sequence is deterministic for testing. Musical evaluation must determine whether the probability curve is useful before adding Keep or rhythm mutation. See [ADVENTURE.md](ADVENTURE.md).

## Groove/phrase-shaping milestone — 0.3.0
Reply timing is now a separate portable stage with manual BPM, blend amount, and five profiles: Original, Pulse, Swing, Pocket, and Half-time. Exact Replay remains source-faithful. The engine transforms existing event timing only; MIDI clock, bar awareness, ratchets, dropped/repeated notes, chord/style grammar, and richer sound remain future work. See [GROOVE.md](GROOVE.md).
