# psychic-octo-spork

Development of an original musical instrument firmware for the M-VAVE FM-1, with AI-assisted coding and documented hardware testing.

**Status: Bass Groove Engine 0.7.0 is implemented in the browser instrument. Native C, compiled WASM, worklet, served-Chrome, direct-file, MIDI, and mobile-layout checks pass; owner musical validation is pending. No hardware firmware build or flashing has occurred.**

psychic-octo-spork is the working code name, not the final firmware name. The first prototype will explore ECHOLOG: melodic conversation.

## Vision
Create something creative, unusual, and musically useful that FM-1 owners want to install and play. The aim goes beyond another Juno clone, conventional virtual analog synth, basic sampler, or ordinary groovebox.

Initial candidates are GENOME (musical evolution), ECHOLOG (musical memory and response), and GRAVITY (physics-based composition). The owner selected ECHOLOG's melodic conversation interaction for the first prototype; the final product scope remains open. Originality and feasibility require further investigation.

This project is separate from the completed Local LLM Infrastructure Lab.

## Bass Groove Engine — prototype 0.7.0

[Versioned downloadable preview](preview/echolog-bass-groove-v0.7.0.html) — download and open in current Chrome or Edge. Auto follows the selected upper Phrase Architecture; Straight and amount zero preserve the original Bass Companion pulse.

Bass Groove adds controlled rests, timing displacement, gate variation, and velocity accents through Auto, Trap Pocket, R&B Flow, and Techno Motion profiles. It preserves Off/Manual/Root/Follow, octave, volume, MIDI channel 2, candidate context, and manual override.

[Bass Groove guide](docs/BASS_GROOVE.md)

## Bass Companion — prototype 0.6.0

[Versioned downloadable preview](preview/echolog-bass-v0.6.0.html) — download and open in current Chrome or Edge. MIDI channel 2 is reserved for manual bass; use channel 1 or channels 3–16 for the upper voice.

An independent second voice can be played manually, hold the selected root, or generate reduced-density accompaniment from the current Reply or kept candidate. MIDI channel 2 and dedicated bass keys provide manual performance and temporary override without interrupting the upper phrase.

[Bass Companion guide](docs/BASS_COMPANION.md)

## Keep and Candidate Memory — prototype 0.5.0

[Versioned downloadable preview](preview/echolog-keep-v0.5.0.html) — download and open in current Chrome or Edge, generate a Reply, then keep and compare candidates A, B, and C.

Keep the exact latest generated Reply in session slots A, B, or C, continue exploring, then audition, replace, or forget each candidate independently. The saved phrase is a deep copy, so later variations and control changes cannot mutate it. Clearing the source does not erase kept candidates.

[Candidate Memory guide](docs/CANDIDATE_MEMORY.md)

## Phrase Architecture — prototype 0.4.0

[Versioned downloadable preview](preview/echolog-phrase-v0.4.0.html) — download and open in current Chrome or Edge, enable sound, capture a phrase, then compare Original, Trap Pocket, R&B Flow, and Techno Motion.

Version 0.4.0 gives Reply its own event structure. Experimental Trap Pocket, R&B Flow, and Techno Motion personalities can remove notes, create rests, repeat motifs, change accents and note lengths, and add ratchets. This directly addresses the owner's finding that 0.3.0 timing offsets worked technically but did not create a convincing groove.

These names are design targets, not claims of authentic genre emulation. See the [Phrase Architecture guide](docs/PHRASE_ARCHITECTURE.md).

## Groove and phrase shaping — prototype 0.3.0

[Versioned downloadable preview](preview/echolog-groove-v0.3.0.html) — open the saved HTML in Chrome, select Enable sound, choose musical and groove settings, and optionally enable a physical MIDI controller.

Version 0.3.0 separates pitch generation from rhythm shaping. Original preserves captured timing; Pulse quantizes and tightens; Swing delays off-sixteenths; Pocket alternates laid-back placement and articulation; Half-time stretches toward twice the duration. BPM and Shape amount are manual. Replay always remains exact.

[Bass Companion guide](docs/BASS_COMPANION.md) · [Candidate Memory guide](docs/CANDIDATE_MEMORY.md) · [Phrase Architecture guide](docs/PHRASE_ARCHITECTURE.md) · [Groove guide](docs/GROOVE.md) · [Adventure guide](docs/ADVENTURE.md) · [Musical Reply guide](docs/MUSICAL_REPLY.md) · [Browser MIDI guide](docs/BROWSER_MIDI.md) · [build instructions](docs/BROWSER_BUILD.md) · [validation record](docs/BROWSER_VALIDATION.md).

## Documentation
- [First prototype specification](docs/PROTOTYPE_SPEC.md)
- [Project plan and milestones](docs/PROJECT_PLAN.md)
- [Development log](docs/DEVELOPMENT_LOG.md)
- [Hardware testing and recovery record](docs/HARDWARE_TESTING.md)
- [AI assistance and attribution](AI_ATTRIBUTION.md)

## Intended release
Public source, an appropriate license and upstream credits, a user guide, installation and verified recovery instructions, compiled firmware downloads, and browser playback if technically validated. Submission to The Firmware Hub follows real-device validation.

## Hardware
The owner reports eight FM-1 units and one SMK Pro Elite. One FM-1 will be designated for development. SMK compatibility is a separate, unverified target.

## Learning and development
Proceed one small feature at a time, with explanations, musical evaluation, troubleshooting, and recorded evidence. Keep documentation aligned with the code and distinguish emulator results from real hardware tests.

There is currently no installable project firmware or verified project-specific recovery procedure.

## License and credits
The browser simulation uses a pinned foma platform subset, with Felucca / SLOOP / Dinghy lineage, under GPL-3.0-only. See LICENSE, vendor/foma/LICENSING.md, and the browser build guide. Hardware and plugin adapters have not yet been validated.

ChatGPT/Codex assists development and documentation; see [AI attribution](AI_ATTRIBUTION.md).

