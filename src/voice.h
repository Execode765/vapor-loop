/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef ECHOLOG_VOICE_H
#define ECHOLOG_VOICE_H
#include <stdint.h>
enum {
    ECHO_ENGINE_SINE = 0,
    ECHO_ENGINE_FM = 1,
    ECHO_ENGINE_VA = 2,
    ECHO_ENGINE_STRING = 3,
    ECHO_ENGINE_COUNT = 4
};
#define ECHO_STRING_MAX 2048
/* Portable, monophonic integer voice. Caller owns timing and serializes access. */
typedef struct {
    uint32_t phase, mod_phase, increment;
    int32_t level, target, filter;
    uint32_t noise;
    uint16_t string_length, string_position;
    int note, engine;
    int16_t string_buffer[ECHO_STRING_MAX];
} EchoVoice;
void echo_voice_init(EchoVoice *v);
void echo_voice_set_engine(EchoVoice *v, int engine);
void echo_voice_on(EchoVoice *v, int note, int velocity);
void echo_voice_off(EchoVoice *v, int note);
void echo_voice_stop(EchoVoice *v);
void echo_voice_render(EchoVoice *v, int32_t *stereo, uint32_t frames);
#endif
