# psychic-octo-spork

Development of an original musical instrument firmware for the M-VAVE FM-1, with AI-assisted coding and documented hardware testing.

**Status: Vapor Loop 0.10.0 is owner-validated as functional and musical, including split performance and Poly Keys. Version 0.10.1 is a usability-only refinement with clearer musical terminology, exact MIDI-number split labels, and live color-coded Bass/Lead/Poly Keys routing feedback. Full automated validation is pending. No hardware firmware build or flashing has occurred.**

psychic-octo-spork remains the repository code name. **Vapor Loop** is the instrument name. ECHOLOG is the original melodic-conversation experiment that established its capture, Reply, and candidate-memory core.

## Vision
Create something creative, unusual, and musically useful that FM-1 owners want to install and play. The aim goes beyond another Juno clone, conventional virtual analog synth, basic sampler, or ordinary groovebox.

Initial concepts included GENOME, ECHOLOG, and GRAVITY. ECHOLOG's musical-memory experiment evolved into Vapor Loop: a melodic looper whose generated ideas disappear unless the player captures them. Originality and feasibility remain engineering and community-research questions rather than marketing claims.

This project is separate from the completed Local LLM Infrastructure Lab.

## Performance Clarity — prototype 0.10.1

Version 0.10.1 preserves the validated 0.10.0 engine while renaming ambiguous controls, presenting capture correction as Free/Humanized/Grid, showing exact MIDI numbers at every split point, and displaying the live destination of every incoming note. This is a usability refinement, not a new synthesis or sequencing engine.

[Performance Clarity guide](docs/PERFORMANCE_CLARITY.md)

## Split Performance and Poly Keys — prototype 0.10.0

Vapor Loop now supports a configurable lower-bass/upper-lead split from one controller and a separate four-voice Poly Keys track for manual chords. MIDI channel 1 addresses Lead, channel 2 Bass, and channel 3 Poly Keys. Chord notes can be recorded as a quantized pass on the shared 96-PPQN arrangement. The electric-keys tone is a compact synthesis foundation; Chord Companion generation, sustain-pedal behavior, and a finished Rhodes model remain future work.

[Split Performance and Poly Keys guide](docs/SPLIT_POLY_KEYS.md)

## Track and Sequencer Foundation — prototype 0.9.0

Vapor Loop now uses a compact 96-PPQN timeline for the captured lead, a recordable manual bass pass, and manual or companion drums. Web MIDI can be routed to Lead or Bass; the bass pass records after the configured count-in while the original phrase plays. A one-bar 16-step kick/snare/hat grid repeats across the selected pattern length. Quarter-, eighth-, and sixteenth-note bass input resolutions establish the path to later triplets, ratchets, ties, probability, and 32nd-note editing.

[Track and Sequencer Foundation guide](docs/SEQUENCER_FOUNDATION.md)

## Rhythm Reference and Extended Capture — prototype 0.8.1

[Versioned downloadable preview — 0.8.1](preview/echolog-rhythm-reference-v0.8.1.html) — download and open in current Chrome or Edge.

Choose a one- or two-bar count-in and capture 1, 2, 4, 8, or 16 bars with a bounded 128-note event memory. A visible bar/beat counter and optional integer-synthesized kick, snare, and eighth-note hat provide metrical context during recording and playback without changing the captured phrase, Reply, Candidate Memory, or synchronized Follow bass.

[Rhythm Reference and Extended Capture guide](docs/RHYTHM_REFERENCE.md)

## Tempo-Aware Capture — prototype 0.8.0

[Versioned downloadable preview — 0.8.0](preview/echolog-tempo-capture-v0.8.0.html) — download and open in current Chrome or Edge.

Capture now shares Reply's musical clock. Choose 1, 2, or 4 bars; hear a one-bar count-in; enter on the following beat one; and let capture close automatically at the selected bar boundary. Off, Light, and Tight input correction adjust note onsets toward the sixteenth-note grid while preserving note lengths and opening rests. This gives the first generated Reply—and therefore Follow bass—a defined rhythmic origin instead of treating the first played note as beat one.

[Tempo-Aware Capture guide](docs/TEMPO_AWARE_CAPTURE.md)

## Reply–Bass Synchronization — prototype 0.7.2

[Versioned downloadable preview — 0.7.2](preview/echolog-reply-bass-sync-v0.7.2.html) — download and open in current Chrome or Edge. Follow derives its timing from the completed transformed Reply. [Previous 0.7.1 preview](preview/echolog-bass-groove-v0.7.1.html) remains available for A/B testing.

Phrase Architecture and Reply Groove now finish the upper response before Follow chooses shared rhythmic anchors from that exact result. Bass Groove can then add controlled rests, timing displacement, gate variation, and velocity accents around those anchors. Root, Manual, Straight, Pedal, octave, volume, MIDI channel 2, candidate context, and manual override remain available.

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
- [Future Chord Companion concept](docs/CHORD_COMPANION_CONCEPT.md)
- [Track and Sequencer Foundation](docs/SEQUENCER_FOUNDATION.md)
- [Split Performance and Poly Keys](docs/SPLIT_POLY_KEYS.md)
- [Performance Clarity](docs/PERFORMANCE_CLARITY.md)
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


