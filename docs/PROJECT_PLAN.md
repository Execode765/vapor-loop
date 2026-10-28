# Project plan

Status: planning. Updated: 2026-10-28.

## Purpose
Develop an original, unusual, musically useful firmware for the M-VAVE FM-1. This project is separate from the completed Local LLM Infrastructure Lab.

## Creative direction
The final concept and product name are undecided. psychic-octo-spork is the working code name.

| Candidate | Core interaction | Question to resolve |
| --- | --- | --- |
| GENOME | Combine, mutate, and select musical phrases and sounds across generations | Can inheritance and player selection distinguish it from existing evolving sequencers? |
| ECHOLOG | Remember played phrases and respond with related variations | Can responses remain recognizable, useful, and responsive? |
| GRAVITY | Use movement, attraction, and collisions to generate music | Can it offer a distinct interaction from existing physics and orbital instruments? |

ECHOLOG is an assistant recommendation, not an approved final selection. Other ideas remain welcome. Do not claim worldwide originality based on a preliminary search.

## Milestones
1. Compare existing firmware and document concrete overlap.
2. Verify platform choices, licenses, toolchain, memory limits, and recovery behavior.
3. Choose a small musical interaction and write its acceptance criteria.
4. Build an audible host/browser prototype and evaluate it with the project owner.
5. Establish a measured target build and designate one development FM-1.
6. Validate installation, operation, persistence, and departure to another firmware on hardware.
7. Publish source, documentation, compiled firmware, and test evidence.
8. Submit the tested release to The Firmware Hub.

## First prototype proposal
If ECHOLOG is selected: capture a monophonic phrase of up to 32 notes; play a related response using one lightweight internal sound. Start with explicit capture/reply controls. Automatic phrase detection, multiple memories, and richer synthesis are later possibilities.

## Release goals
- Dedicated public GitHub source repository with appropriate licensing and upstream credits.
- Installation instructions, user guide, technical design, and transparent AI attribution.
- Versioned compiled firmware downloads and checksums.
- Compatible browser playback if validated.
- Documented hardware compatibility and tested recovery procedure.
- Community directory submission after hardware validation.

## Development method
Work in small, explainable steps. Each meaningful change includes relevant documentation and evidence. Distinguish proposed features, host tests, emulator tests, and physical-device results. Keep concept decisions reversible until musical testing supports them.
