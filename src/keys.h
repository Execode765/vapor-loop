/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef VAPOR_LOOP_KEYS_H
#define VAPOR_LOOP_KEYS_H
#include <stdint.h>

#define ECHO_KEYS_VOICES 4

typedef struct {
    uint32_t phase, tine_phase, increment, age;
    int32_t level, target;
    int note, brightness;
} EchoKeysVoice;

typedef struct {
    EchoKeysVoice voices[ECHO_KEYS_VOICES];
    uint32_t age;
} EchoKeys;

void echo_keys_init(EchoKeys *k);
void echo_keys_on(EchoKeys *k, int note, int velocity);
void echo_keys_off(EchoKeys *k, int note);
void echo_keys_stop(EchoKeys *k);
int echo_keys_active(const EchoKeys *k);
void echo_keys_render(EchoKeys *k, int32_t *stereo, uint32_t frames);

#endif
