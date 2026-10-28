/* SPDX-License-Identifier: GPL-3.0-only */
#include "voice.h"
#include "voice_tables.h"
void echo_voice_init(EchoVoice *v) {
    v->phase = v->increment = 0;
    v->level = v->target = 0;
    v->note = -1;
}
void echo_voice_on(EchoVoice *v, int note, int velocity) {
    if (note < 0 || note > 127) return;
    if (velocity <= 0) { echo_voice_off(v, note); return; }
    if (velocity > 127) velocity = 127;
    v->note = note;
    v->increment = echo_increments[note];
    v->target = velocity * 64; /* peak at most 8128 / 32768 before platform volume */
}
void echo_voice_off(EchoVoice *v, int note) {
    if (v->note == note) { v->note = -1; v->target = 0; }
}
void echo_voice_stop(EchoVoice *v) {
    v->note = -1;
    v->target = 0;
}
void echo_voice_render(EchoVoice *v, int32_t *stereo, uint32_t frames) {
    for (uint32_t i = 0; i < frames; ++i) {
        int32_t delta = v->target - v->level;
        /* Finite linear settling near zero; avoid a stuck quiet release. */
        v->level += delta / 64 + (delta > 0 ? 1 : delta < 0 ? -1 : 0);
        v->phase += v->increment;
        int32_t s = ((int32_t)echo_sine[v->phase >> 24] * v->level) / 32768;
        stereo[2*i] += s;
        stereo[2*i+1] += s;
    }
}
