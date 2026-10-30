/* SPDX-License-Identifier: GPL-3.0-only */
#include "keys.h"
#include "voice_tables.h"

static void init_voice(EchoKeysVoice *v) {
    v->phase = v->tine_phase = v->increment = v->age = 0;
    v->level = v->target = 0;
    v->note = -1; v->brightness = 0;
}

void echo_keys_init(EchoKeys *k) {
    k->age = 0;
    for (int i = 0; i < ECHO_KEYS_VOICES; ++i) init_voice(&k->voices[i]);
}

static int choose_voice(EchoKeys *k, int note) {
    for (int i = 0; i < ECHO_KEYS_VOICES; ++i)
        if (k->voices[i].note == note) return i;
    for (int i = 0; i < ECHO_KEYS_VOICES; ++i)
        if (k->voices[i].note < 0 && k->voices[i].level == 0) return i;
    int chosen = 0;
    for (int i = 1; i < ECHO_KEYS_VOICES; ++i)
        if (k->voices[i].note < 0 || k->voices[i].age < k->voices[chosen].age) chosen = i;
    return chosen;
}

void echo_keys_on(EchoKeys *k, int note, int velocity) {
    if (note < 0 || note > 127) return;
    if (velocity <= 0) { echo_keys_off(k, note); return; }
    if (velocity > 127) velocity = 127;
    EchoKeysVoice *v = &k->voices[choose_voice(k, note)];
    v->note = note; v->increment = echo_increments[note];
    v->target = velocity * 40;
    v->brightness = 18 + velocity * 46 / 127;
    v->age = ++k->age;
}

void echo_keys_off(EchoKeys *k, int note) {
    for (int i = 0; i < ECHO_KEYS_VOICES; ++i) {
        EchoKeysVoice *v = &k->voices[i];
        if (v->note == note) { v->note = -1; v->target = 0; }
    }
}

void echo_keys_stop(EchoKeys *k) {
    for (int i = 0; i < ECHO_KEYS_VOICES; ++i) {
        k->voices[i].note = -1; k->voices[i].target = 0;
    }
}

int echo_keys_active(const EchoKeys *k) {
    int active = 0;
    for (int i = 0; i < ECHO_KEYS_VOICES; ++i) if (k->voices[i].note >= 0) ++active;
    return active;
}

void echo_keys_render(EchoKeys *k, int32_t *stereo, uint32_t frames) {
    for (uint32_t i = 0; i < frames; ++i) {
        int32_t mixed = 0;
        for (int n = 0; n < ECHO_KEYS_VOICES; ++n) {
            EchoKeysVoice *v = &k->voices[n];
            int32_t delta = v->target - v->level;
            v->level += delta / 96 + (delta > 0 ? 1 : delta < 0 ? -1 : 0);
            v->phase += v->increment;
            v->tine_phase += v->increment * 2u;
            int32_t body = echo_sine[v->phase >> 24];
            int32_t tine = echo_sine[v->tine_phase >> 24];
            int32_t tone = (body * (100 - v->brightness) + tine * v->brightness) / 100;
            mixed += tone * v->level / 32768;
        }
        stereo[2u * i] += mixed;
        stereo[2u * i + 1u] += mixed;
    }
}
