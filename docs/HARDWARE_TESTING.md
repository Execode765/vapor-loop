# Hardware testing and recovery record

Status: no project firmware hardware tests performed.

## Owner-reported inventory
| Device | Quantity | Current firmware | Project validation |
| --- | --- | --- | --- |
| M-VAVE FM-1 | 8 | Custom firmware; exact names and versions not recorded | Not tested |
| SMK Pro Elite (owner's designation) | 1 | Not recorded | Not tested; exact model designation to confirm |

Designate one FM-1 as DEV-01 before flashing project builds. Record hardware revision if identifiable, starting firmware/version, and backup filenames. Do not infer SMK compatibility from FM-1 results.

## Before first flash
- Identify the selected platform and its exact revision.
- Back up supported projects, patches, and other user data.
- Record the official restore image's source, version, and checksum.
- Verify the applicable updater, package layout, and rescue instructions.
- Confirm stable power and a working USB data connection.
- Document the expected route back from the proposed firmware.

The owner reports being able to revert to factory firmware. No project-specific restore or emergency recovery procedure has yet been verified. “Latest factory firmware” is not a reproducible version identifier.

## Test record template
| Field | Value |
| --- | --- |
| Date and tester | Pending |
| Device ID and hardware revision | Pending |
| Starting firmware | Pending |
| Project tag/commit and firmware checksum | Pending |
| Installation and reboot | Not tested |
| Keys, buttons, encoders, display | Not tested |
| Internal audio and MIDI | Not tested |
| Timing, note-off handling, stuck notes | Not tested |
| Save/load and power cycle | Not tested |
| Sustained operation and CPU/memory observations | Not tested |
| Update from project firmware to another firmware | Not tested |
| Rescue/restore route and result | Not tested |

## Evidence standards
Record host, browser, and hardware tests separately. Emulator success does not establish hardware compatibility, update safety, or worst-case audio performance. A normal restore does not prove recovery from every failure mode.

## Release gate
Document installation, musical operation, identity/update behavior, and successful departure to another firmware on a real FM-1 before recommending installation. Publish only recovery steps supported by the selected build and actual evidence.
