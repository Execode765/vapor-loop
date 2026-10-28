/* SPDX-License-Identifier: GPL-3.0-only */
#include "fm1.h"
#include "voice.h"
static EchoVoice voice;
static int shown = -2, panel_note = -1;
void setup(void) {
    echo_voice_init(&voice);
    clear(RGB(10, 20, 24));
    text(24, 24, "ECHOLOG", RGB(141, 238, 192), FONT_BIG);
    text(24, 66, "SOUND CHECK 0.0.1", WHITE, FONT_SMALL);
    text(24, 174, "PLAY A KEY", WHITE, FONT_SMALL);
    text(24, 198, "PLAY = STOP", WHITE, FONT_SMALL);
}
void loop(void) {
    for (int k = 0; k < KEY_COUNT; ++k) {
        if (key_pressed(k)) {
            panel_note = KEY_FIRST_NOTE + k;
            echo_voice_on(&voice, panel_note, 100);
        }
        if (key_released(k)) {
            echo_voice_off(&voice, KEY_FIRST_NOTE + k);
            if (panel_note == KEY_FIRST_NOTE + k) panel_note = -1;
        }
    }
    uint8_t st, note, vel;
    while (midi_read(&st, &note, &vel)) {
        if ((st & 0xf0) == 0x90) echo_voice_on(&voice, note, vel);
        if ((st & 0xf0) == 0x80) echo_voice_off(&voice, note);
        if ((st & 0xf0) == 0xb0 && (note == 120 || note == 123)) echo_voice_stop(&voice);
    }
    if (button_pressed(BTN_PLAY)) echo_voice_stop(&voice);
    if (shown != voice.note) {
        shown = voice.note;
        rect(24, 112, 192, 40, RGB(10, 20, 24));
        char num[12];
        format_int(num, shown);
        text(24, 116, shown < 0 ? "READY" : num, RGB(141, 238, 192), FONT_BIG);
    }
}
void audio(int32_t *out, uint32_t frames) { echo_voice_render(&voice, out, frames); }
