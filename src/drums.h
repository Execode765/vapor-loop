/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef ECHOLOG_DRUMS_H
#define ECHOLOG_DRUMS_H
#include <stdint.h>

/* Small deterministic reference kit: synthesized kick, snare, and closed hat. */
typedef struct {
    uint32_t kick_phase, kick_increment, noise;
    int32_t kick_env, snare_env, hat_env, previous_noise;
} EchoDrums;

void echo_drums_init(EchoDrums *d);
void echo_drums_trigger(EchoDrums *d, uint32_t eighth_step);
void echo_drums_render(EchoDrums *d, int32_t *stereo, uint32_t frames);

#endif
