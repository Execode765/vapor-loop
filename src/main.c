/* SPDX-License-Identifier: GPL-3.0-only */
#include "fm1.h"
#include "memory.h"
static EchoMemory memory;
static int shown_note = -2, shown_mode = -1, shown_count = -1, shown_limit = -1;
static int shown_bar = -1, shown_beat = -1;
static void performance_on(int note, int velocity) {
    if (memory.input_track == ECHO_TRACK_BASS ||
        (memory.input_track == ECHO_INPUT_SPLIT && note < memory.split_point))
        echo_memory_bass_on(&memory, note, velocity);
    else if (memory.input_track == ECHO_TRACK_CHORDS)
        echo_memory_keys_on(&memory, note, velocity);
    else echo_memory_on(&memory, note, velocity);
}
static void performance_off(int note) {
    if (memory.input_track == ECHO_TRACK_BASS ||
        (memory.input_track == ECHO_INPUT_SPLIT && note < memory.split_point))
        echo_memory_bass_off(&memory, note);
    else if (memory.input_track == ECHO_TRACK_CHORDS)
        echo_memory_keys_off(&memory, note);
    else echo_memory_off(&memory, note);
}
void setup(void) {
    echo_memory_init(&memory);
    clear(RGB(10, 20, 24));
    text(24, 24, "VAPOR LOOP", RGB(141, 238, 192), FONT_BIG);
    text(24, 66, "PERFORMANCE 0.10.1", WHITE, FONT_SMALL);
    text(24, 198, "SPLIT / 4-VOICE KEYS", WHITE, FONT_SMALL);
}
void loop(void) {
    /* Commands precede input so a note arriving with Capture belongs to the new take. */
    if (button_pressed(BTN_PLAY)) echo_memory_stop(&memory);
    if (button_pressed(BTN_HOME)) echo_memory_clear(&memory);
    if (button_pressed(BTN_REC)) echo_memory_capture(&memory);
    if (button_pressed(BTN_SEL)) echo_memory_finish(&memory);
    if (button_pressed(BTN_SEQ)) echo_memory_replay(&memory);
    if (button_pressed(BTN_FX)) echo_memory_reply(&memory);
    for (int k = 0; k < KEY_COUNT; ++k) {
        if (key_released(k)) performance_off(KEY_FIRST_NOTE + k);
        if (key_pressed(k)) performance_on(KEY_FIRST_NOTE + k, 100);
    }
    uint8_t st, note, vel;
    while (midi_read(&st, &note, &vel)) {
        int channel = st & 0x0f;
        if ((st & 0xf0) == 0x90) {
            if (channel == 1) echo_memory_bass_on(&memory, note, vel); /* MIDI channel 2 */
            else if (channel == 2) echo_memory_keys_on(&memory, note, vel); /* MIDI channel 3 */
            else echo_memory_on(&memory, note, vel);
        }
        if ((st & 0xf0) == 0x80) {
            if (channel == 1) echo_memory_bass_off(&memory, note);
            else if (channel == 2) echo_memory_keys_off(&memory, note);
            else echo_memory_off(&memory, note);
        }
        if ((st & 0xf0) == 0xb0 && (note == 120 || note == 123)) echo_memory_stop(&memory);
    }
    int clock_bar = (int)echo_memory_clock_bar(&memory);
    int clock_beat = (int)echo_memory_clock_beat(&memory);
    if (shown_note != memory.voice.note || shown_mode != memory.mode ||
        shown_count != (int)memory.count || shown_limit != memory.limit ||
        shown_bar != clock_bar || shown_beat != clock_beat) {
        shown_note = memory.voice.note; shown_mode = memory.mode;
        shown_count = (int)memory.count; shown_limit = memory.limit;
        shown_bar = clock_bar; shown_beat = clock_beat;
        rect(16, 96, 216, 92, RGB(10, 20, 24));
        const char *state = memory.mode == ECHO_COUNT_IN ? "COUNT IN" :
            memory.mode == ECHO_ARMED ? "PLAY TO BEGIN" :
            memory.mode == ECHO_RECORDING ? "RECORDING" :
            memory.mode == ECHO_PLAYING ?
                (memory.playback == ECHO_REPLY_PLAYBACK ? "REPLYING" :
                 memory.playback == ECHO_CANDIDATE_PLAYBACK ? "KEPT PHRASE" : "REPLAYING") :
                memory.count ? "PHRASE READY" : "PLAY OR CAPTURE";
        text(16, 100, state, RGB(141, 238, 192), FONT_SMALL);
        char num[12]; format_int(num, (int32_t)memory.count);
        text(16, 128, num, WHITE, FONT_BIG);
        text(80, 138, "NOTES", WHITE, FONT_SMALL);
        if (clock_bar && clock_beat) {
            char bar[12], beat[12]; format_int(bar, clock_bar); format_int(beat, clock_beat);
            text(16, 168, "BAR", RGB(245, 215, 148), FONT_SMALL);
            text(54, 168, bar, WHITE, FONT_SMALL);
            text(90, 168, "BEAT", RGB(245, 215, 148), FONT_SMALL);
            text(140, 168, beat, WHITE, FONT_SMALL);
        } else if (memory.limit) text(16, 168,
            memory.limit == ECHO_TIME_LIMIT ? "TIME LIMIT" :
            memory.limit == ECHO_BAR_COMPLETE ? "BARS COMPLETE" : "NOTE LIMIT",
            RGB(245, 215, 148), FONT_SMALL);
    }
}
void audio(int32_t *out, uint32_t frames) { echo_memory_render(&memory, out, frames); }
#if defined(__wasm__)
#define ECHO_EXPORT(name) __attribute__((export_name(#name)))
ECHO_EXPORT(echo_mode) int echo_mode(void) { return memory.mode; }
ECHO_EXPORT(echo_count) uint32_t echo_count(void) { return memory.count; }
ECHO_EXPORT(echo_length) uint32_t echo_length(void) { return memory.length; }
ECHO_EXPORT(echo_position) uint32_t echo_position(void) { return memory.position; }
ECHO_EXPORT(echo_limit) int echo_limit(void) { return memory.limit; }
ECHO_EXPORT(echo_playback) int echo_playback(void) { return memory.playback; }
ECHO_EXPORT(echo_root) int echo_root(void) { return memory.root; }
ECHO_EXPORT(echo_scale) int echo_scale(void) { return memory.scale; }
ECHO_EXPORT(echo_adventure) int echo_adventure(void) { return memory.adventure; }
ECHO_EXPORT(echo_variation) uint32_t echo_variation(void) { return memory.variation; }
ECHO_EXPORT(echo_play_length) uint32_t echo_play_length(void) { return memory.play_length; }
ECHO_EXPORT(echo_bpm) int echo_bpm(void) { return memory.bpm; }
ECHO_EXPORT(echo_groove_style) int echo_groove_style(void) { return memory.groove_style; }
ECHO_EXPORT(echo_groove_amount) int echo_groove_amount(void) { return memory.groove_amount; }
ECHO_EXPORT(echo_reply_count) uint32_t echo_reply_count(void) { return memory.reply_count; }
ECHO_EXPORT(echo_phrase_style) int echo_phrase_style(void) { return memory.phrase_style; }
ECHO_EXPORT(echo_phrase_amount) int echo_phrase_amount(void) { return memory.phrase_amount; }
ECHO_EXPORT(echo_candidate_mask) uint32_t echo_candidate_mask(void) { return echo_memory_candidate_mask(&memory); }
ECHO_EXPORT(echo_candidate_slot) int echo_candidate_slot(void) { return memory.candidate_slot; }
ECHO_EXPORT(echo_candidate_count) uint32_t echo_candidate_count(int slot) {
    return slot >= 0 && slot < (int)ECHO_CANDIDATE_SLOTS && memory.candidates[slot].filled ? memory.candidates[slot].count : 0;
}
ECHO_EXPORT(echo_last_reply_valid) int echo_last_reply_valid(void) { return memory.last_reply_valid; }
ECHO_EXPORT(echo_bass_mode) int echo_bass_mode(void) { return memory.bass_mode; }
ECHO_EXPORT(echo_bass_density) int echo_bass_density(void) { return memory.bass_density; }
ECHO_EXPORT(echo_bass_octave) int echo_bass_octave(void) { return memory.bass_octave; }
ECHO_EXPORT(echo_bass_volume) int echo_bass_volume(void) { return memory.bass_volume; }
ECHO_EXPORT(echo_bass_count) uint32_t echo_bass_count(void) { return memory.bass_count; }
ECHO_EXPORT(echo_bass_groove_style) int echo_bass_groove_style(void) { return memory.bass_groove_style; }
ECHO_EXPORT(echo_bass_groove_amount) int echo_bass_groove_amount(void) { return memory.bass_groove_amount; }
ECHO_EXPORT(echo_capture_bars) int echo_capture_bars(void) { return memory.capture_bars; }
ECHO_EXPORT(echo_capture_quantize) int echo_capture_quantize(void) { return memory.capture_quantize; }
ECHO_EXPORT(echo_metronome_enabled) int echo_metronome_enabled(void) { return memory.metronome_enabled; }
ECHO_EXPORT(echo_count_in_bars) int echo_count_in_bars(void) { return memory.count_in_bars; }
ECHO_EXPORT(echo_capture_length) uint32_t echo_capture_length(void) { return memory.capture_length; }
ECHO_EXPORT(echo_tempo_beat) uint32_t echo_tempo_beat(void) { return memory.tempo_beat; }
ECHO_EXPORT(echo_clock_bar) uint32_t echo_clock_bar(void) { return echo_memory_clock_bar(&memory); }
ECHO_EXPORT(echo_clock_beat) uint32_t echo_clock_beat(void) { return echo_memory_clock_beat(&memory); }
ECHO_EXPORT(echo_rhythm_enabled) int echo_rhythm_enabled(void) { return memory.rhythm_enabled; }
ECHO_EXPORT(echo_rhythm_volume) int echo_rhythm_volume(void) { return memory.rhythm_volume; }
ECHO_EXPORT(echo_drum_mode) int echo_drum_mode(void) { return memory.drum_mode; }
ECHO_EXPORT(echo_input_track) int echo_input_track(void) { return memory.input_track; }
ECHO_EXPORT(echo_track_recording) int echo_track_recording(void) { return memory.sequencer.recording; }
ECHO_EXPORT(echo_sequence_resolution) int echo_sequence_resolution(void) { return memory.sequencer.resolution; }
ECHO_EXPORT(echo_manual_bass_count) uint32_t echo_manual_bass_count(void) {
    return echo_seq_track_count(&memory.sequencer, ECHO_TRACK_BASS);
}
ECHO_EXPORT(echo_manual_keys_count) uint32_t echo_manual_keys_count(void) {
    return echo_seq_track_count(&memory.sequencer, ECHO_TRACK_CHORDS);
}
ECHO_EXPORT(echo_keys_mode) int echo_keys_mode(void) { return memory.keys_mode; }
ECHO_EXPORT(echo_keys_volume) int echo_keys_volume(void) { return memory.keys_volume; }
ECHO_EXPORT(echo_keys_active) int echo_keys_active_export(void) { return echo_keys_active(&memory.keys); }
ECHO_EXPORT(echo_split_point) int echo_split_point(void) { return memory.split_point; }
ECHO_EXPORT(echo_recording_track) int echo_recording_track(void) {
    return memory.sequencer.recording ? memory.sequencer.record_track : -1;
}
ECHO_EXPORT(echo_drum_step_mask) int echo_drum_step_mask(int step) {
    return echo_seq_drum_mask(&memory.sequencer, step);
}
ECHO_EXPORT(echo_set_harmony) void echo_set_harmony(int root, int scale) {
    echo_memory_set_harmony(&memory, root, scale);
}
ECHO_EXPORT(echo_set_adventure) void echo_set_adventure(int adventure) {
    echo_memory_set_adventure(&memory, adventure);
}
ECHO_EXPORT(echo_set_groove) void echo_set_groove(int bpm, int style, int amount) {
    echo_memory_set_groove(&memory, bpm, style, amount);
}
ECHO_EXPORT(echo_set_phrase) void echo_set_phrase(int style, int amount) {
    echo_memory_set_phrase(&memory, style, amount);
}
ECHO_EXPORT(echo_set_capture_clock) void echo_set_capture_clock(int bars, int count_in_bars,
                                                               int quantize, int metronome) {
    echo_memory_set_capture_clock(&memory, bars, count_in_bars, quantize, metronome);
}
ECHO_EXPORT(echo_set_rhythm_reference) void echo_set_rhythm_reference(int enabled, int volume) {
    echo_memory_set_rhythm_reference(&memory, enabled, volume);
}
ECHO_EXPORT(echo_set_drum_mode) void echo_set_drum_mode(int mode, int volume) {
    echo_memory_set_drum_mode(&memory, mode, volume);
}
ECHO_EXPORT(echo_toggle_drum) void echo_toggle_drum(int step, int voice) {
    echo_memory_toggle_drum(&memory, step, voice);
}
ECHO_EXPORT(echo_set_input_track) void echo_set_input_track(int track) {
    echo_memory_set_input_track(&memory, track);
}
ECHO_EXPORT(echo_set_split_point) void echo_set_split_point(int note) {
    echo_memory_set_split_point(&memory, note);
}
ECHO_EXPORT(echo_record_track) int echo_record_track(int track) {
    return echo_memory_record_track(&memory, track);
}
ECHO_EXPORT(echo_set_sequence_resolution) void echo_set_sequence_resolution(int resolution) {
    echo_memory_set_sequence_resolution(&memory, resolution);
}
ECHO_EXPORT(echo_keep) int echo_keep(int slot) { return echo_memory_keep(&memory, slot); }
ECHO_EXPORT(echo_play_candidate) int echo_play_candidate(int slot) { return echo_memory_play_candidate(&memory, slot); }
ECHO_EXPORT(echo_clear_candidate) void echo_clear_candidate(int slot) { echo_memory_clear_candidate(&memory, slot); }
ECHO_EXPORT(echo_set_bass) void echo_set_bass(int mode, int density, int octave, int volume) {
    echo_memory_set_bass(&memory, mode, density, octave, volume);
}
ECHO_EXPORT(echo_set_bass_groove) void echo_set_bass_groove(int style, int amount) {
    echo_memory_set_bass_groove(&memory, style, amount);
}
ECHO_EXPORT(echo_bass_on) void echo_bass_on(int pitch, int velocity) { echo_memory_bass_on(&memory, pitch, velocity); }
ECHO_EXPORT(echo_bass_off) void echo_bass_off(int pitch) { echo_memory_bass_off(&memory, pitch); }
ECHO_EXPORT(echo_set_keys) void echo_set_keys(int mode, int volume) {
    echo_memory_set_keys(&memory, mode, volume);
}
ECHO_EXPORT(echo_keys_on) void echo_keys_on_export(int pitch, int velocity) {
    echo_memory_keys_on(&memory, pitch, velocity);
}
ECHO_EXPORT(echo_keys_off) void echo_keys_off_export(int pitch) {
    echo_memory_keys_off(&memory, pitch);
}
#endif



