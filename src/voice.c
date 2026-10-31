/* SPDX-License-Identifier: GPL-3.0-only */
#include "voice.h"
#include "voice_tables.h"
void echo_voice_init(EchoVoice *v) {
    v->phase = v->mod_phase = v->increment = 0;
    v->level = v->target = v->filter = 0;
    v->noise = 0x6d2b79f5u;
    v->string_length = v->string_position = 0;
    v->note = -1;
    v->engine = ECHO_ENGINE_SINE;
    for (int i = 0; i < ECHO_STRING_MAX; ++i) v->string_buffer[i] = 0;
}
void echo_voice_set_engine(EchoVoice *v, int engine) {
    if (engine < 0 || engine >= ECHO_ENGINE_COUNT) engine = ECHO_ENGINE_SINE;
    if (v->engine == engine) return;
    v->engine = engine;
    v->phase = v->mod_phase = 0;
    v->filter = 0;
    v->string_position = 0;
}
void echo_voice_on(EchoVoice *v, int note, int velocity) {
    if (note < 0 || note > 127) return;
    if (velocity <= 0) { echo_voice_off(v, note); return; }
    if (velocity > 127) velocity = 127;
    v->note = note;
    v->increment = echo_increments[note];
    v->target = velocity * 64; /* peak at most 8128 / 32768 before platform volume */
    if (v->engine == ECHO_ENGINE_STRING) {
        uint32_t length = v->increment ? (uint32_t)(0x100000000ULL / v->increment) : ECHO_STRING_MAX;
        if (length < 8) length = 8;
        if (length > ECHO_STRING_MAX) length = ECHO_STRING_MAX;
        v->string_length = (uint16_t)length;
        v->string_position = 0;
        for (uint32_t i = 0; i < length; ++i) {
            v->noise ^= v->noise << 13; v->noise ^= v->noise >> 17; v->noise ^= v->noise << 5;
            v->string_buffer[i] = (int16_t)((int32_t)(v->noise & 0xffffu) - 32768);
        }
    }
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
        int32_t wave;
        if (v->engine == ECHO_ENGINE_FM) {
            v->mod_phase += v->increment * 2u;
            int32_t modulation = (int32_t)echo_sine[v->mod_phase >> 24] * 24576;
            uint32_t carrier = v->phase + (uint32_t)modulation;
            wave = echo_sine[carrier >> 24];
        } else if (v->engine == ECHO_ENGINE_VA) {
            int32_t saw = (int32_t)(v->phase >> 16) - 32768;
            v->filter += (saw - v->filter) / 7;
            wave = v->filter;
        } else if (v->engine == ECHO_ENGINE_STRING && v->string_length) {
            uint32_t p = v->string_position;
            uint32_t next = p + 1u == v->string_length ? 0u : p + 1u;
            int32_t current = v->string_buffer[p];
            int32_t averaged = (current + v->string_buffer[next]) / 2;
            averaged -= averaged / 256;
            v->string_buffer[p] = (int16_t)averaged;
            v->string_position = (uint16_t)next;
            wave = current;
        } else wave = echo_sine[v->phase >> 24];
        int32_t s = (wave * v->level) / 32768;
        stereo[2*i] += s;
        stereo[2*i+1] += s;
    }
}
