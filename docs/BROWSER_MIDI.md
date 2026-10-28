# Browser MIDI — prototype 0.0.4

Version 0.0.4 connects a class-compliant USB MIDI keyboard to the browser prototype. It does not flash or communicate with an FM-1; it routes browser MIDI messages into the same compiled C engine used by the onscreen controls.

## Connect

1. Connect and power the MIDI controller before opening ECHOLOG.
2. Open `preview/echolog-midi-v0.0.4.html` in a current desktop Chrome browser.
3. Select **Enable sound**.
4. Select **Enable MIDI**. This user action asks Chrome for MIDI permission; connecting a device alone does not cause the page to request access.
5. Approve the browser prompt, then choose the controller under **MIDI input**.
6. Play a note. The status line should display the device name, note, and velocity.

The page requests ordinary MIDI access with `sysex: false`; it does not request System Exclusive access. If permission is denied, the page reports the error and leaves mouse/computer input available.

## Captured messages

- Note On on any of the 16 MIDI channels
- Note Off, including Note On with velocity zero
- Velocity 1–127
- CC 120 (All Sound Off) and CC 123 (All Notes Off)

Other controller messages are intentionally ignored in this milestone. The C engine is monophonic and uses the most recently pressed note.

## Safety behavior

Changing or disconnecting the selected device sends All Notes Off across all channels and detaches its message handler. Browser focus loss still invokes the existing Stop behavior for onscreen/computer input. Mouse and computer keys remain usable when no MIDI device is present.

## Validation boundary

Automated Chrome tests provide a simulated Web MIDI input and verify permission options, device selection, note/velocity status, audio, phrase capture, Replay, and disconnect panic. Only an owner test with the physical SMK Pro Elite can establish that Chrome recognizes that controller in the owner's environment.

Web MIDI controller success does not validate the M-VAVE FM-1 hardware target, firmware flashing, USB host operation on the FM-1, or SMK-specific non-note controls.
