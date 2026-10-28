/* SPDX-License-Identifier: GPL-3.0-only */
#include "fm1.h"
#include "memory.h"
static EchoMemory memory;
static int shown_note = -2, shown_mode = -1, shown_count = -1, shown_limit = -1;
void setup(void) {
    echo_memory_init(&memory);
    clear(RGB(10, 20, 24));
    text(24, 24, "ECHOLOG", RGB(141, 238, 192), FONT_BIG);
    text(24, 66, "ADVENTURE 0.2.0", WHITE, FONT_SMALL);
    text(24, 198, "32 NOTES / 8 SEC", WHITE, FONT_SMALL);
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
        if (key_released(k)) echo_memory_off(&memory, KEY_FIRST_NOTE + k);
        if (key_pressed(k)) echo_memory_on(&memory, KEY_FIRST_NOTE + k, 100);
    }
    uint8_t st, note, vel;
    while (midi_read(&st, &note, &vel)) {
        if ((st & 0xf0) == 0x90) echo_memory_on(&memory, note, vel);
        if ((st & 0xf0) == 0x80) echo_memory_off(&memory, note);
        if ((st & 0xf0) == 0xb0 && (note == 120 || note == 123)) echo_memory_stop(&memory);
    }
    if (shown_note != memory.voice.note || shown_mode != memory.mode ||
        shown_count != (int)memory.count || shown_limit != memory.limit) {
        shown_note = memory.voice.note; shown_mode = memory.mode;
        shown_count = (int)memory.count; shown_limit = memory.limit;
        rect(16, 96, 216, 92, RGB(10, 20, 24));
        const char *state = memory.mode == ECHO_ARMED ? "PLAY TO BEGIN" :
            memory.mode == ECHO_RECORDING ? "RECORDING" :
            memory.mode == ECHO_PLAYING ?
                (memory.playback == ECHO_REPLY_PLAYBACK ? "REPLYING" : "REPLAYING") :
                memory.count ? "PHRASE READY" : "PLAY OR CAPTURE";
        text(16, 100, state, RGB(141, 238, 192), FONT_SMALL);
        char num[12]; format_int(num, (int32_t)memory.count);
        text(16, 128, num, WHITE, FONT_BIG);
        text(80, 138, "NOTES", WHITE, FONT_SMALL);
        if (memory.limit) text(16, 168, memory.limit == ECHO_TIME_LIMIT ? "TIME LIMIT" : "NOTE LIMIT", RGB(245, 215, 148), FONT_SMALL);
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
ECHO_EXPORT(echo_set_harmony) void echo_set_harmony(int root, int scale) {
    echo_memory_set_harmony(&memory, root, scale);
}
ECHO_EXPORT(echo_set_adventure) void echo_set_adventure(int adventure) {
    echo_memory_set_adventure(&memory, adventure);
}
#endif
