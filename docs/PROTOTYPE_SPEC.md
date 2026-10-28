# ECHOLOG: melodic conversation prototype

Decision date: 2026-10-28.
Status: design only; no implementation or test results.
Repository code name: psychic-octo-spork. Final product name remains open.

## Approved direction
The owner selected melodic conversation as the first experiment. Prove that a short played phrase can produce a recognizable, musically useful answer. This is a prototype direction, not a commitment to every eventual feature.

## First interaction
1. Press Capture. The first note starts the phrase timeline.
2. Play a short monophonic phrase.
3. Press Finish to end capture.
4. Press Reply to hear an answer.
5. Adjust Adventure and press Reply to explore another answer.
6. Press Keep to hold the current answer for repeat playback.
7. Press Clear to erase the phrase and answer; Stop silences playback immediately.

Names are logical actions, not final hardware button assignments. Capture and reply are separate so listening, timing, and transformation are easier to debug.

## Proposed small scope
- One input phrase, at most 32 note events and 8 seconds from the first note.
- One answer and one simple internal synthesis voice.
- Store note pitch, relative onset, duration, and velocity where supplied.
- Panel/onscreen keys use a fixed velocity when no velocity value is available.
- Begin with explicit capture; automatic turn-taking is deferred.
- Keep means a session-only hold, not a flash save.
- No audio recording, neural model, multiple tracks, or persistent phrase library in this milestone.

These are engineering defaults for evaluation, not measured hardware limits.

## Response rule
Preserve rhythm and phrase length initially. Alter pitch only, using three Adventure levels:
- Familiar: repeat most of the phrase and change its ending.
- Curious: alter a few pitches while retaining much of the original contour.
- Bold: alter more pitches, constrained to the selected scale and a bounded register.

Start with an explicit root/scale selection rather than guessing harmony from a very short phrase. C major is the initial default. Preserve the captured input for reference; scale constraints apply to generated pitches. Repeatable random seeds make failures reproducible.

The first implementation must define exact pitch rules and bounds in code and tests before claiming these musical behaviors work.

## Input and playback boundaries
- Empty input: show a prompt and produce no answer.
- Capture ends at the note/time limit with a visible indication; close any held note.
- If notes overlap, use the latest note, close the prior event, and ignore its later release. Retriggering the same pitch closes and restarts it.
- Stop closes active capture notes, cancels playback, and silences the voice without erasing completed phrases.
- Clear stops playback and resets memory.
- Keep holds an answer unchanged until released or cleared; changing Adventure must not silently overwrite it.
- Playback and response generation must not block audio or control processing.

## Acceptance checks
Software checks, not yet run:
- All captured events have valid pitch, nonnegative timing, and positive duration.
- Capacity and time limits are enforced without out-of-bounds writes.
- The same phrase/settings/seed produce identical output.
- Generated output preserves event count, onset times, and durations for this milestone.
- Scale/register constraints, empty capture, overlap, stop, and clear behave as specified.

Listening checks with the owner, not yet run:
- The answer is audibly related to the source phrase.
- Changing Adventure produces an understandable difference.
- Repeating a kept answer is reliable.
- At least one variation is worth keeping.
- The capture/reply interaction feels clear and invites another exchange.

## Implementation approach to evaluate next
Inspect foma and Dinghy source, licensing, toolchain, and browser build support. Prefer a portable C phrase engine separated from audio and panel code so its rules can be explained and tested. Select the platform only after inspection; no flashing is required for the first listening experiment.

## Deferred
Automatic phrase boundaries, MIDI clock, multiple memory generations, file export, flash persistence, richer synthesis, effects, and SMK compatibility.
