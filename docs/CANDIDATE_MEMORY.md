# Keep and Candidate Memory — prototype 0.5.0

Version 0.5.0 adds three session-memory slots: A, B, and C. A generated Reply can be copied into any slot, allowing continued exploration without losing a preferred result.

## What Keep stores

Each candidate is a deep copy containing:

- generated pitches and velocities;
- onset times, note durations, event count, and phrase length;
- variation number;
- key, scale, and Adventure setting;
- BPM, Phrase Architecture personality/intensity, and Groove profile/Shape.

Changing controls or generating another Reply does not alter a kept candidate. A candidate can be auditioned independently, replaced by keeping into the same slot, or removed with **Forget slot**. Clearing or replacing the captured source does not erase candidates.

## Browser operation

1. Capture a source phrase and press Reply.
2. Select candidate A, B, or C.
3. Press **Keep latest Reply**.
4. Continue pressing Reply or changing controls.
5. Press **Play kept** to audition the selected candidate.
6. Press **Forget slot** to empty only that slot.

Candidate memory is intentionally session-only in this milestone. Closing or reloading the page erases it. Persistent files, preset serialization, naming candidates, and hardware storage mapping are not implemented.

## Safety and scope

Keep does not alter exact Replay or the captured source. Candidate playback uses the existing monophonic voice. Simultaneous melody and bass accompaniment requires a later multi-voice architecture.
