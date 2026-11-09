# Shared Harmonic Context — prototype 0.12.2

Vapor Loop now has one explicit source of harmonic truth for generated music. The active key and scale drive Reply, Bass Root/Follow, and Chord Root/Phrase Follow.

## Auto mode

After a Lead capture finishes, the engine scores all roots across major, natural minor, major pentatonic, and minor pentatonic. The bounded integer analysis considers:

- whether each pitch belongs to the candidate scale;
- note duration;
- notes near strong beats;
- tonic, third, and fifth emphasis;
- opening and ending notes.

The interface reports the selected key/scale and a relative confidence value. Short or harmonically ambiguous phrases may have several reasonable interpretations, so the result is a musical estimate rather than a claim of perfect key recognition.

## Manual override

Select **Manual override** to choose the root and scale directly. Generated Reply, Bass, and Chord parts immediately use that shared context. Returning to **Auto** re-analyzes the current captured Lead.

## What never changes automatically

- Replay preserves the captured Lead pitches and timing exactly.
- A recorded manual bass pass preserves its MIDI notes.
- A recorded manual chord pass preserves its MIDI notes.

If the shared context changes after a manual pass was recorded, Vapor Loop shows a warning. The pass is not deleted, quantized to a new scale, or silently transposed. The player can restore the original context, keep the deliberate tension, or re-record the pass.

## Candidate memory

Each kept candidate stores the root and scale that created it. Playing a candidate restores that candidate’s context for its generated Bass and Chord companions without changing the global manual selection or rewriting the candidate notes.

## Scope

This is browser-prototype logic designed for bounded, deterministic execution and later FM-1 feasibility review. It does not add hardware firmware, persistence across page reloads, chromatic chord analysis, modulation tracking, or automatic reharmonization of manual performances.
