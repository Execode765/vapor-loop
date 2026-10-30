/* SPDX-License-Identifier: GPL-3.0-only */
#include "drums.h"
#include "voice_tables.h"

void echo_drums_init(EchoDrums *d) {
    d->kick_phase = d->kick_increment = 0;
    d->kick_env = d->snare_env = d->hat_env = 0;
    d->noise = 0x6d2b79f5u;
    d->previous_noise = 0;
}

void echo_drums_trigger(EchoDrums *d, uint32_t step) {
    /* Eighth-note reference: kick on 1/3, snare on 2/4, hat on every eighth. */
    if ((step & 3u) == 0u) {
        d->kick_phase = 0;
        d->kick_increment = echo_increments[48]; /* starts near C3, falls toward C2 */
        d->kick_env = 7600;
    }
    if ((step & 3u) == 2u) d->snare_env = 5200;
    d->hat_env = (step & 1u) ? 1900 : 2600;
}

static int32_t decay(int32_t value, int shift) {
    if (value <= 0) return 0;
    value -= (value >> shift) + 1;
    return value > 0 ? value : 0;
}

void echo_drums_render(EchoDrums *d, int32_t *stereo, uint32_t frames) {
    const uint32_t kick_floor = echo_increments[36];
    for (uint32_t i = 0; i < frames; ++i) {
        d->noise ^= d->noise << 13;
        d->noise ^= d->noise >> 17;
        d->noise ^= d->noise << 5;
        int32_t noise = (int32_t)(d->noise >> 16) - 32768;
        int32_t high = noise - d->previous_noise;
        d->previous_noise = noise;

        d->kick_phase += d->kick_increment;
        int32_t kick = (int32_t)echo_sine[d->kick_phase >> 24] * d->kick_env / 32768;
        int32_t snare = noise * d->snare_env / 65536;
        int32_t hat = high * d->hat_env / 131072;
        int32_t sample = kick + snare + hat;
        stereo[2u * i] += sample;
        stereo[2u * i + 1u] += sample;

        if (d->kick_increment > kick_floor)
            d->kick_increment -= (d->kick_increment - kick_floor) >> 8;
        d->kick_env = decay(d->kick_env, 13);
        d->snare_env = decay(d->snare_env, 12);
        d->hat_env = decay(d->hat_env, 9);
    }
}
