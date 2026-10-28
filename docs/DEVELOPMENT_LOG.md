# Development log

Record decisions and evidence rather than treating chat history as the only project record.

## 2026-10-28 — Project setup
- Established a dedicated repository under Execode765.
- Adopted psychic-octo-spork as a temporary working code name; final firmware name undecided.
- Defined GENOME, ECHOLOG, and GRAVITY as candidates.
- Identified ECHOLOG as the assistant's initial recommendation; no final concept selected.
- Owner reports eight FM-1 units and one SMK Pro Elite, with custom firmware installed on the FM-1s.
- Resolved GitHub access by reconnecting the intended account.
- Added initial planning, hardware testing, and AI attribution documentation.
- No project firmware has been written, built, flashed, or hardware-tested.

## 2026-10-28 — First prototype direction selected
- Owner approved ECHOLOG's melodic conversation interaction as the first experiment.
- Added [prototype specification](PROTOTYPE_SPEC.md): explicit capture/reply, one short monophonic phrase, one answer, Adventure control, session-only Keep, Stop and Clear.
- Proposed note/time limits and response rules are design defaults pending implementation and musical evaluation.
- Specification introduced in commit 3f0d4632d4c260df71cb4c54c2d0b441c76b5c9b.
- Validation: documentation only; no code, browser listening test, or hardware test yet.
- Next: inspect development foundations and establish the portable phrase engine/build workflow.

## Entry format
For future work, record:
- Date and objective.
- Change or decision, with rationale.
- Commit/build identifier.
- Validation environment and actual result.
- Known limitations and next step.

Never record a planned test as a passed test.

## 2026-10-28 — First browser sound-check implementation
- Selected a pinned foma browser platform subset; retained its GPL and font notices.
- Added a portable integer sine voice, foma adapter, onscreen keys/display, computer-keyboard input, volume, and Stop.
- Added a browser-only compiler script and GitHub Actions workflow; no hardware image or installer.
- Local checks passed: native C oscillator pitch (439.85 Hz measured for A4), amplitude bounds and note lifecycle; compiled WebAssembly input/display/audio; worklet message bridge.
- Local Chromium download returned an invalid archive, preventing a real-browser test here. Added a CI browser test using the runner's Chrome; result pending at this entry.
- This milestone implements sound only. Capture, Reply, Adventure, and Keep are not implemented.
- Build details and validation limits: BROWSER_BUILD.md and BROWSER_VALIDATION.md.


### Browser verification follow-up
- GitHub Actions run 40736104361 passed the real Chrome audio/control test at source commit e110254f51922b5d287e045a70cca8c45899fe8f.
- Inspected the desktop/mobile screenshots. Browser audio samples were checked automatically; human listening remains pending.
- Added a reproducible single-file downloadable preview and local-file browser check.

### Downloadable preview verified
- Chrome passed the complete audio/control check for both the served browser build and the standalone HTML opened directly from disk in workflow 40737010064.
- Fixed local-file worklet loading by embedding its module as a data URL.
- Updated the browser test dependency; the final CI install reported zero audited vulnerabilities.
- Human listening and hardware compatibility remain untested.

## 2026-10-28 — First owner listening test passed
- Owner confirmed that the browser sound check produces sound and responds to both computer-keyboard and mouse input.
- This is owner-reported hands-on evidence, separate from automated Chrome signal/control checks.
- Exact browser/OS and individual Stop/volume checks were not recorded.
- Next musical milestone: implement capture and replay of one short monophonic phrase, then add a recognizable melodic response. No hardware flashing.
