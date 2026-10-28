# AI assistance and authorship

This is an independent project led by Execode765. ChatGPT/Codex assists with research, concept evaluation, explanations, code generation and review, debugging, documentation, and development tooling.

The project owner directs the musical design, evaluates playability, and performs or arranges physical hardware testing. AI assistance does not establish correctness, originality, safety, or compatibility.

## Reporting practice
- Identify upstream code, licenses, assets, and contributors separately from AI assistance.
- Describe features as implemented only when supported by the repository.
- Distinguish host/emulator validation from real-device testing.
- Record significant decisions and test evidence in the development log.
- Review generated code and documentation against the chosen platform and observed behavior.

## Current status
The initial project documentation was drafted with ChatGPT/Codex assistance from the owner's project handoff and discussion on 2026-10-28. A browser-only sound-check implementation has now been added with ChatGPT/Codex assistance. Local native C, WASM, and worklet bridge tests pass; no physical-device testing has occurred.

The browser implementation adopts a pinned GPL-3.0-only foma platform subset and retains upstream credits and the font license. Plugin and hardware integration remain future work. AI attribution does not replace applicable upstream license requirements.

