# psychic-octo-spork

Development of an original musical instrument firmware for the M-VAVE FM-1, with AI-assisted coding and documented hardware testing.

**Status: first browser sound-check implementation. Native C, WebAssembly, and worklet bridge signal checks pass locally. Real Chrome audio/control tests also pass in GitHub Actions. No hardware firmware build or flashing has occurred.**

psychic-octo-spork is the working code name, not the final firmware name. The first prototype will explore ECHOLOG: melodic conversation.

## Vision
Create something creative, unusual, and musically useful that FM-1 owners want to install and play. The aim goes beyond another Juno clone, conventional virtual analog synth, basic sampler, or ordinary groovebox.

Initial candidates are GENOME (musical evolution), ECHOLOG (musical memory and response), and GRAVITY (physics-based composition). The owner selected ECHOLOG's melodic conversation interaction for the first prototype; the final product scope remains open. Originality and feasibility require further investigation.

This project is separate from the completed Local LLM Infrastructure Lab.

## First sound check

[Downloadable browser sound check](preview/echolog-soundcheck.html) — use GitHub’s Download raw file control, open the saved HTML in a modern desktop browser, and select Enable sound. Browser validation status is recorded below.

The browser prototype plays a monophonic sine voice with keys, volume, and Stop. Phrase capture and replies are not implemented yet. [Build and play instructions](docs/BROWSER_BUILD.md) · [validation record](docs/BROWSER_VALIDATION.md).

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

