# FM-1 Recovery Preparation — Diagnostics 01–34

**Project:** Vapor Loop  
**Repository:** [Execode765/vapor-loop](https://github.com/Execode765/vapor-loop)  
**Target hardware:** M-VAVE FM-1  
**Development unit:** DEV-01  
**Observed stock firmware:** V16  
**Status:** Recovery preparation documented; hardware testing paused  
**Checkpoint:** Diagnostic 34 completed  
**Custom firmware flashed:** No  
**Original full-flash backup completed:** No

## 1. Purpose

Vapor Loop is an original musical-instrument firmware project intended for the M-VAVE FM-1 MIDI controller.

The project began with a browser-based prototype, allowing musical ideas, MIDI behavior, timing, and synthesis features to be developed and tested before modifying physical hardware.

After reaching browser prototype v0.12.4, the project owner decided to prioritize actual firmware development rather than continue expanding browser-only functionality.

Before attempting firmware installation, however, a recovery strategy was needed.

This document records the preparation, software testing, findings, limitations, and hardware-identification work completed before entering the FM-1 ROM bootloader.

The guiding development principle is:

**Build → Test → Understand → Document → Verify**

A passing simulation is evidence about the tested software behavior. It is not proof that physical hardware will behave identically.

## 2. Safety boundaries

The development unit, DEV-01, is one of several available FM-1 controllers and has been designated for experimental firmware development.

The following restrictions apply:

- Do not flash custom firmware before preserving and verifying the original device contents.
- Do not assume that an official V15 firmware package is compatible with a V16 device.
- Do not treat a successful simulated restoration as proof of physical recovery.
- Do not send recovery-mode MIDI SysEx without explicit authorization.
- Do not run hardware-affecting recovery commands automatically.
- Preserve device-specific settings and data where supported.
- Review the recovery operation, risks, and expected outcome before executing it.

No physical flash-writing operation has been performed during Diagnostics 01–34.

## 3. Hardware and firmware baseline

### Normal operating mode

The FM-1 presents a composite USB device with the following identifiers:

| Property | Observed value |
|---|---|
| USB vendor ID | `4C4A` |
| USB product ID | `C755` |
| MIDI interface | FM-1 Midi |
| Audio interface | FM-1 Audio |
| Composite interface | USB Composite Device |
| Windows device status | OK |

The device was previously identified by the official M-VAVE Workbench as `FM-1_016`, corresponding to firmware V16.

The Workbench also presented an official open-firmware V14 option with a firmware-difference warning. That firmware was not installed.

### Expected ROM recovery mode

The recovery utility targets a separate bootloader identity:

| Property | Expected value |
|---|---|
| USB vendor ID | `4C4A` |
| USB product ID | `8057` |
| Recovery disk identity | `WL82 UBOOT1.00` |
| Bootloader family | JieLi WL82 / AC791N |

These identifiers describe the expected recovery interface. DEV-01 has not yet been observed operating in this mode.

## 4. Recovery software selection

The principal recovery utility selected for evaluation was:

[MvaveFM1Unbricker — Quixotic7](https://github.com/Quixotic7/MvaveFM1Unbricker)

An alternative recovery project was also considered:

[FM1recovery — SikkenWorks](https://github.com/SikkenWorks/FM1recovery)

The Quixotic7 utility was selected for further preparation because its source includes backup, verification, simulated-device testing, and full-image recovery workflows.

The utility is third-party software. Its inclusion does not constitute a guarantee of safety, compatibility, or successful recovery.

### Important command distinctions

| Command | Purpose | Safety consideration |
|---|---|---|
| `find` | Locate recovery-mode devices | Discovery operation |
| `softkey` | Request ROM-mode entry through MIDI | Changes device operating state |
| `info` | Identify recovery hardware | May upload a temporary loader into RAM |
| `backup DIR` | Read and preserve flash | Requires recovery communication and loader activity |
| `restore` | Restore a supported firmware region | Writes flash; V15-oriented limitations |
| `full-restore` | Restore a larger firmware package or image | High-risk flash-writing operation |
| `repair-settings` | Repair supported settings data | Changes flash contents |

The `info` command must not be described as purely passive because it can load code into device RAM.

A `--dry-run` or `--plan-only` option must be evaluated according to its actual implementation rather than assumed to be hardware-independent.

## 5. Windows recovery environment

The preparation environment used:

- Windows PowerShell 5.1
- Python 3.12.10
- pip 25.0.1
- An isolated Python virtual environment
- A downloaded source archive instead of Git, because Git was not installed

Workspace:

```text
Documents/
└── VaporLoop-FM1-Recovery/
    ├── .venv/
    └── Source/
        └── MvaveFM1Unbricker-main/
```

The recovery utility's setup process completed successfully using its pinned dependency archive fallback.

### Source archive fingerprint

The downloaded recovery source archive was recorded with SHA-256:

```text
64E454387FD7D2BA1DF9897EBAA334238AF5B7A20F47E950D738EB6362E52A4B
```

This is a local integrity fingerprint, not independent proof of upstream authenticity.

### Pinned dependency revision

The `jl-uboot-tool` dependency was installed at:

```text
adb3f18889e88ac512ce0a3c4d8cc3d3cb30696a
```

The local revision marker matched the expected pinned revision.

### Loader fingerprint

The loader file:

```text
data/loaderblobs/usb/wl82loader.bin
```

was verified against the recovery script's expected SHA-256 fingerprint:

```text
d41da6126760c9d66660bcc0cac8d27d221806c5e369a8036921efe68dca5376
```

This establishes consistency with the selected utility's expected loader, not independent hardware compatibility.

### Python dependencies

The isolated environment included:

| Package | Version |
|---|---|
| colorama | 0.4.6 |
| crcmod | 1.7 |
| pycryptodomex | 3.24.2 |
| python-rtmidi | 1.5.8 |
| PyYAML | 6.0.3 |
| tqdm | 4.70.1 |

The environment reported:

```text
No broken requirements found.
```

This was a successful dependency-consistency check.

## 6. Diagnostics 01–24 — Preparation and source inspection

The preliminary diagnostic phase established:

- Normal FM-1 USB identification
- Expected ROM bootloader identification
- Recovery utility selection
- Recovery command behavior and risk classification
- Local source archive acquisition
- Python virtual-environment setup
- Pinned dependency installation
- Loader fingerprint verification
- Python dependency consistency
- Preliminary test isolation and entry-point review

The source contained guarded Python entry points:

```python
if __name__ == "__main__":
    sys.exit(main())
```

and:

```python
if __name__ == "__main__":
    unittest.main()
```

Test fixtures included temporary working directories, mocked MIDI interfaces, simulated flash memory, and simulated USB/SCSI communication.

These observations supported proceeding to controlled offline tests. They did not establish that every test was isolated from hardware.

## 7. Diagnostics 25–33 — Automated recovery testing

The test suite was exercised in selected groups while DEV-01 was disconnected.

### Test execution summary

| Diagnostic | Test group | Passed | Skipped | Errors |
|---|---|---:|---:|---:|
| 25 | Synthetic firmware round-trip | 1 | 0 | 0 |
| 26 | Mock and flash safeguards | 3 | 0 | 0 |
| 27 | Backup simulation | 3 | 0 | 0 |
| 28 | Restore simulation | 14 | 1 | 0 |
| 29 | Full-restore simulation | 11 | 2 | 0 |
| 30 | Softkey, finder, protocol | 15 | 0 | 2 |
| 33 | Firmware parsing, extraction, settings, macOS USB/protocol | 22 | 2 | 0 |
| **Total** | **Test executions** | **69** | **5** | **2** |

These totals represent test executions, not unique test cases. The synthetic firmware round-trip test was executed twice.

No assertion failures were reported in these runs. Diagnostic 30 returned exit code `1` because of two environment errors.

### Firmware package parsing

The tested code successfully exercised synthetic firmware encoding and decoding, classification, and rejection of certain corrupted or truncated package inputs.

Tests requiring external oracle data were skipped when that data was unavailable.

### Flash safeguards

Three tests passed covering:

- Block and sector erase behavior
- NOR flash programming semantics
- A tested protection boundary below address `0x4000`

The boundary test does not establish that every available recovery command protects that address range.

### Backup simulation

Three tests passed covering:

- Simulated backup reads
- Double-read comparison
- Aborting when inconsistent or unreliable reads occur

This provides evidence that the tested backup logic detects certain read-integrity failures.

It does not establish that DEV-01's flash can currently be read successfully.

### Restoration simulation

Fourteen tests passed and one was skipped.

The passing tests included scenarios involving:

- Simulated end-to-end restoration
- Identical-content detection
- Damaged firmware rejection
- Incorrect flash identification
- Incorrect chip keys
- Read and write failures
- Retry behavior
- User confirmation requirements
- Post-write verification

The authentic V15 firmware test was skipped because the required package was unavailable.

### Full-flash restoration simulation

Eleven tests passed and two were skipped.

The tested scenarios included:

- Long firmware packages
- Boot-head validation
- Writing sequence
- Final readback
- Image rejection
- Device-specific data preservation
- Write retry and failure behavior
- Confirmation and plan-only handling

Tests requiring authentic V15 and V16 packages were skipped.

**A successful simulated full restore is not evidence that a physically bricked V16 device can be recovered.**

### MIDI and protocol simulation

The softkey tests passed using simulated MIDI interfaces.

The protocol tests passed using mocked device communication.

This verifies selected software behavior without establishing that a physical FM-1 will accept the recovery-mode request or communicate with the loader.

## 8. Windows symbolic-link limitation

Diagnostic 30 produced two errors:

```text
test_linux_sysfs
test_two_devices_refused
```

Both failed during creation of symbolic links for a simulated Linux device filesystem.

The Python error was:

```text
OSError: [WinError 1314]
A required privilege is not held by the client
```

Diagnostic 31 independently reproduced the restriction through PowerShell:

```text
SYMLINK TEST: BLOCKED
Administrator privilege required for this operation.
```

This supports classifying the two errors as Windows test-environment limitations.

The affected test behavior remains unverified because execution stopped during fixture setup.

No Windows privilege escalation, Developer Mode change, or security-policy modification was performed to work around the issue.

Diagnostic 32 confirmed cleanup of the temporary symbolic-link test directory:

```text
CLEANUP: PASSED
```

## 9. Diagnostic 34 — Physical USB identity verification

DEV-01 was connected in normal operating mode.

The following read-only PowerShell query was executed:

```powershell
Get-PnpDevice -PresentOnly |
    Where-Object {
        $_.InstanceId -match "VID_4C4A&PID_C755"
    } |
    Select-Object Status, Class, FriendlyName, InstanceId |
    Format-Table -AutoSize
```

Windows returned three matching interfaces:

```text
OK  MEDIA  FM-1 Midi
OK  USB    USB Composite Device
OK  MEDIA  FM-1 Audio
```

The ROM bootloader query was:

```powershell
Get-PnpDevice -PresentOnly |
    Where-Object {
        $_.InstanceId -match "VID_4C4A&PID_8057"
    } |
    Select-Object Status, Class, FriendlyName, InstanceId |
    Format-Table -AutoSize
```

No matching device was returned.

### Interpretation

DEV-01 was successfully enumerated by Windows in its expected normal USB configuration.

The expected ROM bootloader interface was not present.

This is consistent with normal operation and does not indicate a recovery failure.

The diagnostic did not send MIDI messages, load firmware, or request a mode change.

## 10. Current limitations and unresolved questions

The following items have not been verified:

1. Whether DEV-01 can successfully enter ROM recovery mode.
2. Whether Windows can identify the resulting `WL82 UBOOT1.00` device.
3. Whether the temporary recovery loader communicates successfully with DEV-01.
4. Whether the complete original 1 MiB flash can be read twice with matching results.
5. Whether a preserved image contains everything required for device-specific recovery.
6. Whether the recovery utility can restore a physical V16 FM-1 successfully.
7. Whether the original V16 firmware and device-specific data can be recovered after an unsuccessful custom firmware installation.
8. Whether the future Vapor Loop firmware build is compatible with the physical FM-1 hardware.

These are open engineering questions, not completed milestones.

## 11. Current checkpoint and resumption procedure

**Hardware testing is paused after Diagnostic 34.**

The last observed state of DEV-01 was normal USB operation with the expected MIDI, audio, and composite interfaces.

No ROM-mode transition has been authorized.

### Next diagnostic: 35 — ROM-mode entry review

Before proceeding:

1. Review the exact recovery-mode entry command and MIDI port-selection behavior.
2. Confirm that only the intended DEV-01 is connected.
3. Review potential reconnection, driver, and recovery risks.
4. Obtain explicit approval before sending any SysEx command.
5. If authorized, verify the resulting USB identity independently.
6. Treat flash backup as a separate operation requiring its own approval.
7. Preserve the original image, record its SHA-256 fingerprint, and independently verify its integrity before any firmware deployment.

Do not automatically run:

```text
softkey
info
backup
restore
full-restore
repair-settings
```

A backup is not the same as a proven recovery procedure. Flashing decisions must account for the remaining recovery limitations.

## 12. Relationship to Vapor Loop firmware development

The browser prototype remains at v0.12.4.

The browser-only gain-staging work is frozen, and no additional browser development is required as a prerequisite to continuing hardware preparation.

The long-term objective remains to develop, test, and deploy an original Vapor Loop firmware implementation for the FM-1.

Hardware feasibility, memory constraints, synthesis performance, USB/MIDI behavior, and compatibility must be evaluated against the actual platform rather than inferred from browser behavior.

This recovery checkpoint establishes preparation evidence. It is not a firmware deployment milestone.

## 13. AI assistance and verification disclosure

This project is directed by Execode765 with assistance from OpenAI ChatGPT.

AI assistance contributed to recovery-tool research, source interpretation, PowerShell commands, test planning, diagnostic analysis, and documentation drafting.

The project owner executed the Windows diagnostics, supplied the observed outputs, and retained control over hardware operations and development decisions.

The recovery utility and its dependencies are independent third-party projects and must retain their own attribution and licensing.

The reported test outcomes are based on user-supplied terminal output. The results were not independently reproduced by ChatGPT on the user's physical hardware.

No claim of successful physical firmware recovery, completed flash backup, or working custom firmware installation is made.

---

## Final checkpoint

**Diagnostics completed:** 01–34  
**Automated test executions:** 69 passed, 5 skipped, 2 environment errors  
**Normal USB identity:** Verified  
**ROM recovery identity:** Not yet observed  
**Original flash backup:** Not completed  
**Hardware firmware writes:** None performed during these diagnostics  
**Next step:** Diagnostic 35 — review ROM-mode entry and obtain explicit authorization

**Build → Test → Understand → Document → Verify**
