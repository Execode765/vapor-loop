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

## 2026-10-28 — First phrase memory, 0.0.2
- Implemented a portable C capture/replay state machine with relative audio-frame timestamps, durations, velocities, fixed capacity, and no heap allocation.
- Added Capture, Finish, Replay, Clear, status/progress, and a visibly bounded take. Stop retains partial capture; live playing interrupts replay.
- Local address/undefined-behavior sanitizer checks passed (leak detection disabled locally because this runtime restricts process inspection; the memory engine does not allocate). Existing voice, WASM, and bridge checks still pass, plus the new WASM memory integration test.
- Native tests confirm identical replay samples across single-frame and larger render chunks, both capacity limits, overlap/retrigger behavior, and Stop/Clear handling.
- Browser and owner listening results for this new milestone are pending at implementation time. Prior owner approval was for 0.0.1 only.
- Rebuilt the single-file preview. No hardware image produced or flashed.

### 0.0.2 browser checks completed
- Workflow 40738989216 passed native voice/memory sanitizer tests, WASM integration, worklet regression checks, and real Chrome recording/replay tests.
- Both the served page and downloaded HTML recorded keyboard/mouse input and replayed a two-note phrase with nonzero audio; Stop preserved it and Clear erased it.
- Inspected desktop/mobile screenshots. No user listening test for 0.0.2 is claimed.

## 2026-10-28 — Owner validation of first phrase memory
- Owner tested the downloaded 0.0.2 standalone browser build.
- Owner confirmed that Capture, Finish, Replay, and Clear all work.
- This establishes hands-on validation of the complete first-memory interaction, separate from automated Chrome checks.
- Browser capture/replay is now the validated foundation for the next musical milestone: generating a transformed reply. No hardware flashing occurred.

## 2026-10-28 — First transformed Reply, 0.0.3
- Added a distinct Reply command while retaining exact Replay for A/B comparison.
- Reply preserves recorded onset, duration, rest, and velocity data, starts a perfect fifth above the source, and mirrors subsequent melodic intervals around that new anchor.
- The rule is deterministic and integer-only: `reply = 2 × first pitch + 7 − captured pitch`, clamped to MIDI 0–127.
- Native sanitizer tests verify the expected transformed pitches and preserved velocities. Compiled WASM, worklet, and existing regression checks pass locally.
- Rebuilt the standalone browser preview. Real-Chrome CI and owner musical evaluation are pending. No plugin or hardware image was built or flashed.

### 0.0.3 browser checks completed
- Workflow 40753333474 passed native sanitizer, compiled WASM, worklet, served-Chrome, and direct-file Chrome checks for source commit `17352a2c28203ac4bd246ac9e69d8bb7ed37a78c`.
- Both browser forms produced audio from Reply and returned to phrase-ready state without JavaScript errors.
- Owner musical evaluation of the reply remains pending.

## 2026-10-28 — Owner audiovisual validation of first Reply
- Owner supplied a screen recording of the downloaded 0.0.3 standalone build.
- Captured phrase: C4–D4–E4–D4–C4. Reply: G4–F4–E♭4–F4–G4, exactly matching the fixed-fifth contour inversion.
- The take reached the eight-second limit and remained valid; Replay and Reply both completed with five notes and preserved timing.
- This is owner-supplied audiovisual evidence for browser behavior. It is not plugin or physical FM-1 validation.

### Owner musical assessment of 0.0.3
- Functional behavior passed: capture, exact Replay, transformed Reply, timing preservation, and the eight-second boundary all worked.
- Musical behavior failed: the owner judged the Reply to be a technical inversion, not a musical answer.
- Retain the deterministic mirror as a diagnostic baseline only; do not describe it as musically successful.
- Defer musical refinement until scale, key, and chord constraints can be designed and evaluated deliberately.

## 2026-10-28 — Browser MIDI connection, 0.0.4
- Added a user-initiated Enable MIDI permission flow and connected-input selector.
- Routes Note On/Off from all 16 channels, including velocity-zero Note Off, plus CC 120/123 panic messages, into the existing compiled C MIDI path.
- Displays controller name, incoming note, and velocity. Mouse/computer-key input remains available without MIDI.
- Device change/disconnect detaches the old handler and sends All Notes Off across all channels to prevent stuck notes.
- Added browser automation with a simulated SMK Pro Elite covering permission options, device selection, live audio, velocity status, phrase capture, Replay, and disconnect safety.
- Native/WASM/worklet regressions pass locally. Real-Chrome CI and physical owner testing are pending. No hardware firmware image was produced or flashed.

### 0.0.4 browser checks completed
- Workflow 40757919690 passed the full simulated-controller test in both served Chrome and the downloaded local HTML.
- Permission request options, selection, note/channel/velocity routing, audio, phrase capture, Replay, disconnect panic, and absence of JavaScript errors were verified.
- Physical SMK Pro Elite validation remains pending.

## 2026-10-28 — Owner physical MIDI validation
- Owner confirmed that the SMK Pro Elite running custom Vector firmware is detected and works; Chrome displays the firmware-provided name “Vector.”
- Owner supplied video evidence using MPC Live III USB-C In 1: velocity status, seven-note Capture/Finish, exact Replay, and diagnostic Reply all worked.
- Recorded source/replay pitches were MIDI 60–61–62–63–62–61–60; Reply was 67–66–65–64–65–66–67, exactly matching the current rule.
- Browser MIDI 0.0.4 is owner-validated with physical controllers. This does not validate an FM-1 hardware build or improve the already-failed musical assessment of the Reply algorithm.

## 2026-10-28 — Scale-aware musical Reply, 0.1.0
- Replaced the chromatic diagnostic transform with a separate portable harmony strategy module.
- Added manual selection of all 12 roots with major, natural minor, major pentatonic, and minor pentatonic scales.
- Reply maps source pitches to nearest scale degrees, creates equal contrary diatonic motion from an answer near a fifth above, stays within MIDI 36–96, and moves the final note to the nearest tonic-triad tone.
- Preserves the existing phrase timing, rests, note lengths, velocities, count, limits, Replay, MIDI, and control behavior.
- Native sanitizer tests cover all 48 key/scale contexts plus a known C-major phrase. Compiled WASM and worklet tests pass locally.
- Real-Chrome CI and owner musical evaluation remain pending. No plugin or FM-1 image was built or flashed.

### 0.1.0 browser checks completed
- Workflow 40761057791 passed native harmony/memory sanitizer checks, compiled WASM, worklet, and real Chrome for both served and downloaded builds.
- Chrome changed the Reply context to D natural minor, produced audible Reply, completed playback, and retained all MIDI/UI protections without JavaScript errors.
- Owner musical evaluation remains pending and is the next gate.

## 2026-10-28 — Owner audiovisual test of scale-aware Reply
- Owner supplied a C-major/MPC screen recording with C5–D5–E5–D5–C5 repeated twice in an eight-second, ten-note take.
- Exact Replay reproduced the source. Reply produced G5–F5–E5–F5–G5 twice, confirming in-scale contrary motion and a tonic-triad ending.
- The prior chromatic E-flat problem is absent; E natural is retained in C major.
- Functional audiovisual validation passed. The owner's subjective judgment of musical usefulness remains intentionally unrecorded until stated.

### Owner musical assessment of 0.1.0
- Owner reported that the scale-aware Reply sounded “a lot more musical” than the prior chromatic inversion.
- Functional behavior and the musical direction both pass for this baseline.
- Retain scale-degree contrary motion and tonic-triad resolution as a starting strategy, while continuing to label automatic harmony, varied rhythm, Adventure, and broader key/scale evaluation as unfinished.

## 2026-10-28 — Adventure controlled variation, 0.2.0
- Added a 0–100 Adventure control. Zero reproduces the approved 0.1.0 Reply; higher values increase the probability of interior scale-step substitutions, with two-step changes available at high settings.
- Each Reply press advances a deterministic variation sequence seeded by phrase pitches, harmony context, and generation number. Changing Adventure or starting a new Capture resets the sequence.
- Protected source rhythm, rests, durations, velocities, count and length; selected scale; MIDI 36–96 register; opening anchor; and tonic-triad cadence.
- Added native tests for baseline compatibility, distinct high-Adventure generations, all 48 contexts, and protected constraints; compiled WASM tests cover the control and counter.
- Local native/WASM/worklet regressions pass. Real-Chrome CI and owner musical evaluation are pending. Keep, rhythm mutation, plugin, and FM-1 firmware remain unimplemented.

### 0.2.0 browser checks completed
- Workflow 40773797192 passed native Adventure/harmony sanitizer tests, compiled WASM, worklet, and real Chrome in served and downloaded forms.
- Chrome set Adventure 80, completed successive variations 1 and 2, and retained Replay, Web MIDI, disconnect, mobile layout, and no-error behavior.
- Owner musical evaluation of the Adventure curve remains pending.

## 2026-10-28 — Owner audiovisual test of Adventure
- Owner supplied an MPC/C-major recording of a 15-note, eight-second phrase, exact Replay, Adventure 0 baseline, and raised settings through 80.
- Raised settings produced audibly and analytically distinct C-major contours. The G opening anchor, MIDI register bound, and tonic-triad ending held; high Adventure traveled farther into the lower register.
- Physical functional behavior passed. The owner's subjective judgment of the Adventure probability curve remains intentionally pending until stated.

### Owner same-setting Adventure and style assessment
- Follow-up MPC video held C major / Adventure 10 and repeatedly pressed Reply. Variation numbers advanced through multiple generations, and answers changed without moving the slider.
- Same-setting generation behavior passes on the owner's physical setup.
- Owner judged the output musically acceptable but closer to a nursery rhyme than modern trap, R&B, melodic techno, or contemporary electronic phrasing.
- Treat 0.2.0 as a coherent variation foundation, not a genre-ready generator. The style gap points to rhythm, chord progression/context, phrase-shape rules, and sound design rather than simply increasing pitch randomness.

## 2026-10-28 — Groove and phrase-shaping engine, 0.3.0
- Separated Reply timing into portable `src/groove.c`, independent from phrase memory and harmony/Adventure pitch generation.
- Added manual BPM 40–240, Shape 0–100, and Original, Pulse, Swing, Pocket, and Half-time profiles.
- Original preserves timing; Pulse quantizes/gates; Swing delays odd 16ths; Pocket adds alternating laid-back placement/articulation; Half-time scales onset/duration/length up to 2×.
- Exact Replay remains untouched. Event order, positive durations, note count, velocity, and a 16-second reply bound are enforced.
- Native sanitizer tests cover profile math and all amount values. Compiled WASM/worklet and previous regression suites pass locally.
- Real-Chrome CI and owner musical evaluation are pending. The profiles are timing foundations, not claims of finished genre modeling.

### 0.3.0 browser checks completed
- Workflow 40778697465 passed native groove/harmony/memory sanitizer tests, compiled WASM, worklet, and real Chrome in served and downloaded forms.
- Chrome used 128 BPM / Swing / Shape 80, completed successive Adventure replies, and retained exact Replay, Web MIDI, disconnect, mobile layout, and no-error behavior.
- Owner musical/style evaluation remains pending.


### Owner audiovisual validation and musical verdict for 0.3.0
- Owner video confirms physical MPC Web MIDI, a 26-note capture, exact Replay, repeated Reply, BPM/Shape changes, and operation of Original, Pulse, Swing, Pocket, and Half-time.
- Functional behavior passes: controls respond, replies finish, Half-time extends the phrase, and no crash or stuck-note failure was observed.
- Musical groove fails: the owner reported that the profiles did not materially change the groove or create a convincing pocket.
- The pronounced tempo/Half-time effect changes event timing and duration, not oscillator pitch. Any pitch-shifter impression is perceptual; the current engine retriggers the same MIDI pitches rather than resampling audio.
- Retain the separated groove stage as infrastructure. Next, add a phrase-architecture layer capable of changing rests, density, accents, motifs, syncopation, pickups, ratchets, and note-length hierarchy instead of relying only on onset offsets and quantization.


## 2026-10-28 — Phrase Architecture engine, 0.4.0
- Owner validation of 0.3.0 passed functionally but failed musically: timing offsets and tempo changes did not create a convincing groove.
- Added portable `src/phrase.c` between harmony and final groove timing.
- Reply now owns an independent event count, velocities, onsets, durations, and length; exact Replay remains untouched.
- Added experimental Original, Trap Pocket, R&B Flow, and Techno Motion structural personalities. They can create rests, remove/repeat notes, add intensity-dependent ratchets, establish motif repetition, vary note lengths, and apply accent hierarchy.
- Native address/undefined-behavior sanitizer tests pass across all styles and intensity values, including event order, nonzero durations/velocities, capacity, and reply-length bounds.
- Compiled WASM, real-Chrome CI, and owner musical evaluation are pending. The genre labels are design targets, not authenticity claims. No plugin or hardware image was built or flashed.


### 0.4.0 browser checks completed
- Workflow 40786013924 passed native sanitizer, compiled WASM, worklet, served-Chrome, and direct-file Chrome checks for source commit `d7329c22e2e54a8798f90324e3ca1cac0af945e8`.
- Chrome generated a ten-event Trap Pocket reply from a three-note source, completed successive variations, and then replayed the untouched three-note performance.
- Packaged a versioned standalone preview for owner listening. Musical feel remains intentionally pending until the owner reports it.


### Owner audiovisual validation and musical verdict for 0.4.0
- Owner video confirms a 24-note/eight-second C-major capture, exact Replay, and independent eight- or ten-event generated responses.
- Trap Pocket 100, R&B Flow 100, and Techno Motion 100 were all exercised across repeated generations and multiple BPM values.
- The owner also combined Phrase Architecture with the separate Pulse, Swing, and Pocket groove profiles, confirming that the two transformation stages compose successfully.
- Functional behavior, melodic coherence, and audible groove all pass. The owner stated: “It's definitely grooving now. It grooves and it's melodic. We are definitely on track.”
- This is the first positive owner groove verdict. Retain independent event structure as the foundation; do not inflate this result into a claim of universal trap, R&B, or techno authenticity.
- Remaining major browser-stage gaps include chord progression/context, MIDI clock and bar synchronization, Keep/selection workflow, richer synthesis, and broader musical stress testing.


## 2026-10-28 — Keep and Candidate Memory, 0.5.0
- Added three portable session slots, A/B/C, for deep-copy preservation of the latest generated Reply.
- Each candidate stores pitch, velocity, onset, duration, count, length, generation, harmony, Adventure, BPM, Phrase Architecture, and Groove metadata.
- Later Reply generations and control changes cannot mutate kept candidates. Slots support independent audition, replacement, and forgetting.
- Source Clear/new Capture does not erase kept work; closing/reloading the browser still does. Persistent serialization is intentionally deferred.
- Native address/undefined-behavior sanitizer tests pass for slot bounds, deep-copy isolation, playback, clearing, and persistence.
- Recorded **Bass Companion** as a later second-voice/sub-oscillator milestone and **Vapor Loop** as the owner's leading name candidate. Neither is implemented or final.
- Compiled WASM, real-Chrome CI, and owner validation are pending. No plugin or hardware firmware image was built or flashed.


### 0.5.0 browser checks completed
- Workflow 40789153651 passed native sanitizer, compiled WASM, worklet, served-Chrome, and direct-file Chrome checks for source commit `ffab5d25077c2d544224bb710c4015c47eb3832e`.
- Chrome kept variation 1 in A, generated variation 2, auditioned the unchanged A, kept the newer response in B, forgot only B, retained A, and exact-Replayed the source.
- Packaged a versioned standalone preview. Owner workflow/usefulness validation remains pending.


### Owner audiovisual validation of Candidate Memory
- Owner video uses a 32-note/7.75-second MPC source at the note limit.
- Slot A first stores a ten-note Trap Pocket response and remains available through later generations.
- Re-keeping into A deliberately replaces it with an eight-note R&B Flow candidate; kept playback recalls that candidate.
- Exact 32-note Replay works without erasing A.
- A is later deliberately replaced with a ten-note Techno Motion/Pulse candidate and recalled successfully.
- Owner-observed functionality passes for save, deep-copy isolation across later generation/control changes, replacement, recall, and Replay independence.
- Slots B/C, Forget, and persistence across source Clear/new Capture were not owner-tested in this video; they remain automation-tested. No separate subjective usefulness verdict was stated.


### Musical foundation gate passed; Bass Companion approved
- Owner judged the current system melodic, rhythmic, tune-capable, and musically correct in feel.
- Treat browser milestones through 0.5.0 as a successful bare-bones musical architecture. Effects and deeper synthesis remain enhancement layers rather than prerequisites for musical validity.
- Owner approved Bass Companion as the next experiment and requested both direct player control and automatic following.
- Recommended design boundary: a genuinely independent second voice with Off, Manual, Root, and Follow behaviors. Follow should use a reduced-density low-register interpretation of the generated response rather than mirror every upper note.


## 2026-10-28 — Bass Companion, 0.6.0
- Added an independent second integer-voice path and bounded two-voice mixer.
- Added Off, Manual, Root, and Follow modes; Pedal, Half-note, and Quarter-note movement; octave 0–3; and bass volume.
- Manual bass accepts dedicated browser keys, Z/X/C/V/B/N/M white-note keys, and MIDI channel 2. It does not interrupt upper Reply/candidate playback.
- Manual notes temporarily override Root/Follow; automatic bass rejoins at the next future boundary after release.
- Root anchors the selected key. Follow samples the upper phrase at bass boundaries and maps movement to low root/third/fifth tones using the Reply or candidate's stored harmony/BPM context.
- Live mode changes join at the next boundary rather than rewinding playback. Stop and panic silence both voices.
- Native sanitizer tests pass for simultaneous voices, Root/Follow generation, manual override, candidate context, bounds, and lifecycle. JavaScript syntax checks pass.
- Compiled WASM, real-Chrome CI, and owner musical evaluation remain pending. No plugin or hardware image was built or flashed.


### 0.6.0 browser checks completed
- Workflow 40794256419 passed native sanitizer, compiled WASM, worklet, served-Chrome, direct-file Chrome, and mobile-layout checks for source commit `94107e307f8b16183edae1c4bd09888feabe15e6`.
- The final regression suite confirms MIDI channel 2 is dedicated to manual bass while channel 1 and channels 3–16 drive the upper voice.
- Packaged the versioned standalone preview `preview/echolog-bass-v0.6.0.html` from the exact successful CI artifact.
- Owner musical and playability evaluation is the next gate. No plugin or FM-1 firmware image was built or flashed.


## 2026-10-28 — Owner validation of Bass Companion 0.6.0
- Owner video exercised Follow, Root, Manual, Pedal, Half-note, and Quarter-note behavior with Reply and a kept candidate.
- Octave 3 and bass volume 65 made the deep sine voice audible on the available speakers; the owner explicitly chose to retain the deep range.
- Functional design passed. The owner found automatic bass harmonically useful but rhythmically too even.
- Approved next boundary: preserve the two-voice architecture and add a separate, modest genre-aware bass-groove layer.

## 2026-10-28 — Bass Groove Engine, 0.7.0
- Added a portable bass-groove stage after Root/Follow pitch selection and Movement grid creation.
- Straight and amount zero exactly bypass shaping. Auto maps the active or kept candidate's Phrase Architecture to Trap Pocket, R&B Flow, or Techno Motion.
- Profiles add bounded rests, timing displacement, gate variation, and velocity accents without changing the manual bass path, octave range, upper phrase, candidate memory, or MIDI channel ownership.
- Default Auto 55 is intentionally moderate. Pedal remains an unshaped sustained foundation.
- Native sanitizer tests cover every style/amount, exact bypass, event ordering and bounds, candidate context, two-voice mixing, and manual override.
- Workflow 40811250692 passed compiled WASM, worklet, served/direct-file Chrome, MIDI, and mobile layout for source commit `29694a5826ae0a7c92c0e09c04959bee50095ff2`.
- Owner musical evaluation is the next gate. No plugin or FM-1 image was built or flashed.


## 2026-10-28 — Bass Groove short-phrase refinement, 0.7.1
- Owner video initially suggested a one-Reply lag. Spectral and timeline analysis showed both Replies contained bass: G3 (~196.0 Hz) on the first and C3 (~130.8 Hz) on the second.
- The actual weakness was a two-opportunity Half-note phrase combined with Trap 55: the second opportunity was removed, leaving one higher note that blended into the upper sine voice.
- Follow now anchors its first event on the selected root. Every two-opportunity phrase retains both events while the second still receives profile timing, gate, and velocity shaping.
- Longer phrases retain strategic rests. Straight/amount-zero bypass, Manual, Root, candidate context, MIDI channel 2, and the upper phrase engine are unchanged.
- Workflow 40814437014 passed the full native/WASM/worklet/Chrome/MIDI/mobile suite for source commit `137fdfec9ec330596e08d08430703d14f74c5837`.
- Packaged a uniquely named standalone preview. Owner musical retest is pending; no plugin or hardware image was built or flashed.


## 2026-10-29 — Reply–Bass Synchronization, 0.7.2
- Owner video showed that a transformed first Reply could sound disconnected from Follow bass while a later variation happened to align.
- Source inspection confirmed that the controls were current; the upper Reply and bass were using separate rhythmic grids.
- Follow now chooses anchors from the completed Reply after Phrase Architecture and Reply Groove establish final onsets. Bass Groove then adds pocket around those shared anchors.
- Native tests verify both the first and second variations against their own Reply onsets. Workflow 40818538942 passed native sanitizer, compiled WASM, worklet, served/direct-file Chrome, MIDI, and mobile-layout checks for source commit 40b49480d9f23d5db9b3409fb066c6dbf4e15b34.
- Chord Companion is recorded as a future polyphonic feature; it was not implemented here. No plugin or hardware image was built or flashed.

## 2026-10-29 — Tempo-Aware Capture, 0.8.0

- Added a shared 4/4 capture clock using the selected Reply BPM, with a one-bar audible count-in and automatic 1-, 2-, or 4-bar recording boundaries.
- Recording time now begins at the downbeat rather than at the first note, preserving intentional opening rests and giving the first Reply a defined beat-one origin.
- Added Off, Light, and Tight sixteenth-note onset correction. Light moves onsets halfway toward the grid; Tight places them on the grid; note lengths remain performance-derived.
- The metronome distinguishes beat one from beats two through four and may be disabled without disabling the clock.
- Existing exact Replay, Reply variation, Candidate Memory, Bass Companion modes, Reply-derived Follow anchors, and MIDI routing remain intact.
- Native sanitizer tests verify click/count-in behavior, exact bar length, initial-rest preservation, quantized onsets, invalid-input normalization, and existing memory/bass regressions.
- GitHub Actions workflow 40826957158 passed compiled WebAssembly, worklet, served-Chrome, direct-file standalone, MIDI, audio, and mobile-layout checks at commit 8fcc88d33f8d133d2015baf62c6f6e50be6e7138.
- Owner musical validation is pending. This is a browser prototype, not an external MIDI-clock implementation, plugin, or FM-1 firmware image.

## 2026-10-29 — Rhythm Reference and Extended Capture, 0.8.1

- Owner audiovisual review established that v0.8.0 first-Reply bass synchronization passes: measured Follow-bass attacks were a subset of upper-Reply attacks across the recording. The remaining ambiguity was metrical context, not drift.
- Added selectable one- or two-bar count-in; the browser default is two bars.
- Extended tempo-aware capture to 1, 2, 4, 8, or 16 bars and increased fixed event capacity from 32 to 128 notes.
- Added a visible one-based bar/beat position to the browser and simulated FM-1 display.
- Added an optional deterministic integer-synthesized reference kit: kick on beats 1/3, snare on 2/4, and closed hat on eighth notes, with independent level/mute.
- Reference drums accompany recording and playback but are not recorded, copied into candidates, or used to alter Reply/Bass timing.
- Expanded the bounded Reply timeline so Original replies can retain a complete 16-bar capture even at 40 BPM; compact Phrase Architecture personalities remain deliberate one-bar answers.
- Native sanitizer validation passed. Initial compiled-WASM testing sampled after the short kick transient had decayed; the assertion was corrected to sample the exact downbeat without changing the engine.
- Workflow 40984346867 passed compiled WebAssembly, worklet, served-Chrome, direct-file standalone, MIDI, audio, and mobile-layout checks at commit 9fc7e68a722b7163f039a8176d02546c51ba8aa6.
- Owner musical validation is pending. This milestone is not a full Drum Companion, sample engine, plugin, external MIDI-clock implementation, or FM-1 firmware image.

## 2026-10-29 — Vapor Loop identity and Track/Sequencer Foundation, 0.9.0

- Owner audiovisual testing passed 0.8.1 end to end: selectable count-in, two-bar capture, exact Replay, first-Reply bass synchronization, repeated Reply generation, candidate keep/recall, rhythm reference, and multiple Phrase/Bass Groove combinations remained musical and synchronized.
- The project name is now **Vapor Loop**. ECHOLOG remains the originating melodic-conversation experiment; psychic-octo-spork remains the repository code name.
- Added a bounded 96-PPQN sequencer representation with 1/2/4/8/16-bar lengths, 128 events, and quarter/eighth/sixteenth manual-input resolution.
- Added Lead/Bass Web-MIDI input routing and a count-in/manual bass recording pass. The original lead and drum track play once while the bass is recorded; subsequent Replay, Reply, and candidate playback reuse the stored bass events.
- Added Off, Manual, and Companion drum-track modes. Manual mode exposes a repeating one-bar 16-step kick/snare/hat lane using the existing diagnostic integer voices.
- Native sanitizer tests pass for frame/tick conversion, recording lifecycle, quantized bass events, manual drum masks, synchronized playback, bounds, and all 0.8.1 regressions. Workflow 41013008484 also passes compiled WASM, worklet, served/direct-file Chrome, Web MIDI, audio, and mobile-layout validation at commit cdf1cbde9ab03efe454339be822fb5adb9a93fb1.
- Repository visibility remains private. Recommended public-alpha gate: successful 0.9.0 CI, owner validation, refreshed README, and an explicit prototype/no-hardware-flashing notice.




## 2026-10-30 — Split Performance and Poly Keys 0.10.0

- Owner validation closed the 0.9.0 gate: manual bass, shared sequencing, drums, capture, Reply, and musical behavior passed hands-on testing with MPC input.
- Added a configurable lower-bass/upper-lead split while preserving direct channel roles: MIDI channel 1 Lead, channel 2 Bass, and channel 3 Poly Keys.
- Added a bounded four-voice integer Poly Keys engine, simultaneous chord input, deterministic voice stealing, independent Off/Manual and level controls, and quantized chord-pass recording on the shared 96-PPQN timeline.
- The sequencer now tracks open notes per pitch so simultaneous chord notes retain independent durations.
- Local native address/undefined-behavior sanitizer, voice, polyphony, chord-recording, split-bound, and all earlier memory regressions pass.
- GitHub Actions run 41139551280 passed compiled WebAssembly, AudioWorklet, served/direct-file Chrome, split/MIDI routing, chord recording/playback, screenshots, and mobile-layout checks at source commit `f650731c433a0996471422f808c2d5cd8a97cd11`. Owner musical validation of 0.10.0 remains pending.
- Chord Companion generation, sustain pedal support, finished electric-piano modeling, plugin builds, and FM-1 firmware remain deliberately out of scope for this milestone. No hardware image was built or flashed.


## 2026-10-31 — Owner validation of 0.10.0 and Performance Clarity 0.10.1

- Owner confirmed that 0.10.0 polyphony, manual bass, Poly Keys, MIDI input, and the broader musical workflow function correctly and sound musical.
- Video analysis confirmed the split is exact: with G3 / MIDI 55 selected, F♯3 / MIDI 54 routes to Bass while G3 / MIDI 55 and A♭3 / MIDI 56 route to Lead.
- The connected MPC displays octave names one number lower than Vapor Loop for the same MIDI note; this is a naming convention difference, not a routing error.
- Version 0.10.1 therefore preserves all engine behavior and changes only interface terminology, split-point labels, route visibility, tests, and documentation.
- Automated workflow result is pending at this entry. No plugin build, FM-1 firmware image, or hardware flashing occurred.


## 2026-10-30 — Owner validation of 0.10.1 and Lead Engine Lab 0.11.0

- Owner video confirms the G3 / MIDI 55 split boundary: MIDI 54 routes to Bass while MIDI 55 and 56 route to Lead.
- The exact-number and color-coded routing interface resolves the controller octave-label ambiguity; 0.10.1 is owner-validated.
- Version 0.11.0 begins sound design with four selectable Lead engines: the retained diagnostic sine, two-operator FM, filtered saw virtual analog, and bounded Karplus–Strong string synthesis.
- Engine implementations are integer-only, fixed-allocation, and preset-based for the first listening comparison.
- Native C engine checks pass locally. Full workflow and owner musical evaluation are pending. No FM-1 firmware image or flashing occurred.


## 2026-10-30 — Owner Engine Lab validation and 0.11.1 verification refinement

- Owner initially heard the older sine engine because the browser tab had not been refreshed; refreshing loaded the new WebAssembly/AudioWorklet build.
- Owner video then confirmed successful selection, retriggering, release, and distinct behavior for Sine Diagnostic, FM Glass, Virtual Analog Warm, and Karplus–Strong String.
- The video capture embedded a quiet signal, but the owner confirmed the live Chrome → MPC → soundbar path is loud enough to require reducing the MPC level. No synthesis gain increase is justified.
- Preserve mix headroom for future Lead, Bass, Poly Keys, Chord Companion, and drum summing. Harder drums should use transient design, saturation, harmonics, and controlled limiting rather than uncontrolled master gain.
- Version 0.11.1 adds audio-core engine readback, visible mismatch state, build/reload guidance, and browser assertions for every selected-to-active transition.
- Full workflow result is pending at this entry. No FM-1 image or flashing occurred.


## 2026-10-31 — Chord Companion foundation 0.12.0

- Extended the existing four-voice Poly Keys lane with Off/live-free, Manual/recordable, Root Follow, and Phrase Follow modes.
- Root Follow holds a scale-aware key-center triad. Phrase Follow derives a new scale-aware triad from each Lead playback onset.
- Live Poly Keys input temporarily replaces generated harmony; releasing the manual notes permits automatic harmony to resume.
- The first implementation uses deterministic root-position triads and fixed memory. Smooth inversions, advanced voice-leading, chord rhythms, sustain, richer chord timbres, and MIDI-output harmony are deferred.
- Native and browser regression coverage now exercises scale membership, register bounds, three-voice generation, manual takeover, Off-mode live playing, and all existing Lead/Bass/Drum behavior.
- Automated workflow and owner musical validation are pending. No plugin build, FM-1 firmware image, or hardware flashing occurred.

## 2026-10-31 — Owner validation of Chord Companion 0.12.0

- The first test exposed a routing/workflow mismatch rather than a recorder defect: main Capture records Lead, while a Poly Keys pass requires an existing Lead timeline and the separate Record keys pass action.
- The owner repeated the test with MPC USB MIDI Port 1, channel 1 for the Lead capture and channel 3 for Poly Keys.
- Video evidence confirms a 13-note, two-bar Lead phrase; a synchronized six-note manual chord pass; preserved Replay; Root Follow; and Phrase Follow with three active generated chord voices.
- The recorded-note count remains visible when switching follow modes, confirming the manual pass remains stored while automatic harmony is auditioned.
- Owner reports the clean workflow worked perfectly. No plugin build, FM-1 firmware image, or hardware flashing occurred.

## 2026-10-31 — Chord handoff and beat-one capture 0.12.1

- Follow-up owner video exposed two boundary defects: a chord held slightly before the end of count-in could sound live but miss the recorded pass, and generated chord release tails could overlap a manual takeover in the shared four-voice pool.
- Manual Poly Keys input now tracks exact held pitches and velocities. Duplicate note-ons and stray note-offs no longer corrupt override ownership.
- A first manual note in Root or Phrase Follow immediately clears generated voices before taking ownership, preventing transient overlap and voice stealing.
- Manual chord notes still held when count-in ends are injected into the sequencer at tick zero, preserving natural slightly-early entrances on beat one.
- The browser now distinguishes an ARMED track pass from active RECORDING and explicitly tells the player that a slightly early entry is accepted.
- GitHub Actions workflow 41222801370 passed native sanitizer, compiled WebAssembly, worklet, served and standalone Chromium audio/control, MIDI, and layout checks at commit `f0e45e0f0503e0825543e5d8db8216f77ac3994a`.
- Owner musical validation of the packaged 0.12.1 build remains pending. No plugin build, FM-1 firmware image, or hardware flashing occurred.



# 2026-11-09 — 0.12.2 Shared Harmonic Context

- Added automatic post-capture root/scale detection using bounded integer scoring.
- Unified generated Reply, Bass, and Chord companions under the active playback context.
- Preserved exact Replay and exact manual bass/chord MIDI passes.
- Added explicit Auto/Manual selection and visible mismatch warnings for recorded passes.
- Confirmed kept candidates store and restore their own key/scale context.
- Clean GitHub Actions run 42684662979 passed native, sanitizer, WASM, AudioWorklet, Chromium, Web MIDI, served-page, and standalone-file validation before the owner-test build was packaged.

# 2026-11-09 — 0.12.3 Engine-state preservation

- Reproduced the owner-reported defect in the compiled 0.12.2 core: starting Capture reinitialized the Lead voice and changed FM Glass back to Sine Diagnostic.
- Added one bounded Lead-voice reset path that snapshots the selected engine, initializes voice state, and restores that engine immediately.
- Applied the preserving reset to Capture, Replay, Reply, kept-candidate playback, and manual-track recording. Finish, Stop, Keep, Clear, and candidate management retain the same engine by construction.
- Added native regression coverage for FM Glass, Virtual Analog Warm, and Karplus–Strong String across every transport/memory operation, plus compiled-WASM and browser assertions for FM Glass.
- Updated the standalone build identity and visible diagnostic copy to 0.12.3. Selected and active engine readbacks remain independent so later regressions remain observable.
- The unresolved timing observation remains deferred to the 0.13.0 Workbench timeline. No FM-1 firmware image was built and no hardware was accessed or flashed.

# 2026-11-10 — Owner validation and 0.12.4 browser gain staging

- Owner video confirms FM Glass remains selected and active through Capture, Replay, Reply, Keep, and kept-candidate playback; the 0.12.3 reset regression is closed.
- The recording again contains a much quieter signal than the owner's live MPC monitoring path. This repeated capture problem justified a browser-output repair even though the live amplified path remained audible.
- Added a browser-only output stage after the compiled AudioWorklet: +12 dB recommended by default, 0/+6/+12/+18 dB choices, and a fast 20:1 limiter at −3 dBFS.
- Kept the FM-1 master emulation, portable C synthesis core, track balance, velocity behavior, and future hardware path unchanged.
- Added Chromium assertions for default gain, selectable recording boost, limiter configuration, and a measured post-core level increase.
- No plugin build, FM-1 firmware image, hardware access, or flashing occurred.

