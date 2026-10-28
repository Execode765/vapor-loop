/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef ECHOLOG_VOICE_H
#define ECHOLOG_VOICE_H
#include <stdint.h>
/* Portable, monophonic integer voice. Caller owns timing and serializes access. */
typedef struct {
    uint32_t phase, increment;
    int32_t level, target;
    int note;
} EchoVoice;
void echo_voice_init(EchoVoice *v);
void echo_voice_on(EchoVoice *v, int note, int velocity);
void echo_voice_off(EchoVoice *v, int note);
void echo_voice_stop(EchoVoice *v);
void echo_voice_render(EchoVoice *v, int32_t *stereo, uint32_t frames);
#endif
