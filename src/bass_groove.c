/* SPDX-License-Identifier: GPL-3.0-only */
#include "bass_groove.h"

static int blend(int from, int to, int amount) {
    return from + (to - from) * amount / 100;
}

void echo_bass_groove_apply(uint8_t *pitch, uint8_t *velocity,
                            uint32_t *onset, uint32_t *duration,
                            uint32_t *count, uint32_t length, uint32_t spacing,
                            int style, int amount) {
    if (!count || !*count || !length || !spacing) return;
    if (style < 0 || style >= ECHO_BASS_GROOVE_COUNT) style = ECHO_BASS_GROOVE_STRAIGHT;
    if (amount < 0) amount = 0;
    if (amount > 100) amount = 100;
    if (style == ECHO_BASS_GROOVE_STRAIGHT || style == ECHO_BASS_GROOVE_AUTO || !amount) return;

    uint32_t input_count = *count, output_count = 0;
    for (uint32_t i = 0; i < input_count; ++i) {
        uint32_t phase = i & 7u;
        int play = 1, offset_percent = 0, gate_percent = 80, target_velocity = 105;

        if (style == ECHO_BASS_GROOVE_TRAP) {
            /* Space on beat two, a late answer, and short/long punctuation. */
            if (amount >= 30 && (phase == 1u || phase == 5u)) play = 0;
            if (amount >= 75 && phase == 6u) play = 0;
            if (phase == 2u || phase == 6u) offset_percent = 12;
            gate_percent = (phase == 0u || phase == 4u) ? 68 : (phase & 1u) ? 42 : 55;
            target_velocity = (phase == 0u || phase == 4u) ? 118 : (phase & 1u) ? 88 : 101;
        } else if (style == ECHO_BASS_GROOVE_RNB) {
            /* A gentle behind/ahead pocket with one optional breath per two bars. */
            if (amount >= 65 && phase == 5u) play = 0;
            if (phase == 1u || phase == 6u) offset_percent = 8;
            if (phase == 3u || phase == 7u) offset_percent = -12;
            gate_percent = (phase == 0u || phase == 4u) ? 88 : (phase == 2u || phase == 6u) ? 72 : 50;
            target_velocity = (phase == 0u || phase == 4u) ? 113 : (phase == 2u || phase == 6u) ? 101 : 86;
        } else if (style == ECHO_BASS_GROOVE_TECHNO) {
            /* Preserve propulsion; use alternating gates and restrained off-beat drag. */
            if (amount >= 90 && phase == 7u) play = 0;
            if (phase & 1u) offset_percent = 5;
            gate_percent = (phase & 1u) ? 44 : 72;
            target_velocity = (phase == 0u || phase == 4u) ? 120 : (phase & 1u) ? 91 : 104;
        }
        if (!play) continue;

        uint32_t shaped_onset = onset[i];
        int64_t shift = (int64_t)spacing * offset_percent * amount / 10000;
        int64_t shifted = (int64_t)shaped_onset + shift;
        if (shifted < 0) shifted = 0;
        if ((uint64_t)shifted >= length) shifted = (int64_t)length - 1;
        shaped_onset = (uint32_t)shifted;
        if (output_count && shaped_onset <= onset[output_count - 1])
            shaped_onset = onset[output_count - 1] + 1u;
        if (shaped_onset >= length) continue;

        int shaped_gate = blend(80, gate_percent, amount);
        uint32_t shaped_duration = spacing * (uint32_t)shaped_gate / 100u;
        if (!shaped_duration) shaped_duration = 1;
        uint32_t remaining = length - shaped_onset;
        if (shaped_duration > remaining) shaped_duration = remaining;

        pitch[output_count] = pitch[i];
        velocity[output_count] = (uint8_t)blend(105, target_velocity, amount);
        onset[output_count] = shaped_onset;
        duration[output_count] = shaped_duration;
        ++output_count;
    }
    *count = output_count;
}
