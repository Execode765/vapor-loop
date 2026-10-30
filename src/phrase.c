/* SPDX-License-Identifier: GPL-3.0-only */
#include "phrase.h"

#define PHRASE_CAPACITY 128u
#define PHRASE_MAX_FRAMES (44100u * 96u)

static uint8_t velocity_scaled(uint8_t value, int percent) {
    int v = (int)value * percent / 100;
    if (v < 1) v = 1;
    if (v > 127) v = 127;
    return (uint8_t)v;
}

static void emit(const uint8_t *source_pitch, const uint8_t *source_velocity,
                 uint32_t source_count, uint8_t *pitch, uint8_t *velocity,
                 uint32_t *onset, uint32_t *duration, uint32_t *count,
                 uint32_t step, uint32_t grid, uint32_t pitch_index,
                 uint32_t length_steps, int accent) {
    if (*count >= PHRASE_CAPACITY || !source_count) return;
    uint32_t i = (*count)++;
    pitch[i] = source_pitch[pitch_index % source_count];
    velocity[i] = velocity_scaled(source_velocity[pitch_index % source_count], accent);
    onset[i] = step * grid;
    duration[i] = length_steps ? length_steps * grid : grid / 2u;
    if (!duration[i]) duration[i] = 1;
}

void echo_phrase_apply(const uint8_t *source_pitch, const uint8_t *source_velocity,
                       const uint32_t *source_onset, const uint32_t *source_duration,
                       uint32_t source_count, uint32_t source_length,
                       uint8_t *reply_pitch, uint8_t *reply_velocity,
                       uint32_t *reply_onset, uint32_t *reply_duration,
                       uint32_t *reply_count, uint32_t *reply_length,
                       int bpm, int style, int amount, uint32_t variation) {
    if (bpm < 40) bpm = 40;
    if (bpm > 240) bpm = 240;
    if (style < 0 || style >= ECHO_PHRASE_COUNT) style = ECHO_PHRASE_ORIGINAL;
    if (amount < 0) amount = 0;
    if (amount > 100) amount = 100;
    *reply_count = 0;
    *reply_length = 0;
    if (!source_count) return;

    if (style == ECHO_PHRASE_ORIGINAL || amount == 0) {
        for (uint32_t i = 0; i < source_count && i < PHRASE_CAPACITY; ++i) {
            reply_pitch[i] = source_pitch[i];
            reply_velocity[i] = source_velocity[i];
            reply_onset[i] = source_onset[i];
            reply_duration[i] = source_duration[i] ? source_duration[i] : 1u;
        }
        *reply_count = source_count > PHRASE_CAPACITY ? PHRASE_CAPACITY : source_count;
        *reply_length = source_length;
        return;
    }

    const uint32_t grid = 661500u / (uint32_t)bpm; /* sixteenth note */
    uint32_t motif = variation % source_count;
    if (style == ECHO_PHRASE_TRAP_POCKET) {
        const uint8_t steps[] = {0, 3, 6, 7, 10, 12, 15};
        for (uint32_t i = 0; i < sizeof steps; ++i)
            emit(source_pitch, source_velocity, source_count, reply_pitch, reply_velocity,
                 reply_onset, reply_duration, reply_count, steps[i], grid, motif + i,
                 i == 4 ? 1u : 0u, (i == 0 || i == 4) ? 115 : 82);
        if (amount >= 40)
            emit(source_pitch, source_velocity, source_count, reply_pitch, reply_velocity,
                 reply_onset, reply_duration, reply_count, 11, grid, motif + 4, 0, 68);
        if (amount >= 70) {
            emit(source_pitch, source_velocity, source_count, reply_pitch, reply_velocity,
                 reply_onset, reply_duration, reply_count, 13, grid, motif + 5, 0, 74);
            emit(source_pitch, source_velocity, source_count, reply_pitch, reply_velocity,
                 reply_onset, reply_duration, reply_count, 14, grid, motif + 5, 0, 64);
        }
    } else if (style == ECHO_PHRASE_RNB_FLOW) {
        const uint8_t steps[] = {0, 2, 5, 7, 10, 14};
        const uint8_t lengths[] = {1, 2, 1, 2, 3, 2};
        for (uint32_t i = 0; i < sizeof steps; ++i)
            emit(source_pitch, source_velocity, source_count, reply_pitch, reply_velocity,
                 reply_onset, reply_duration, reply_count, steps[i], grid, motif + i,
                 lengths[i], i == 0 ? 108 : (i & 1u ? 78 : 94));
        if (amount >= 55)
            emit(source_pitch, source_velocity, source_count, reply_pitch, reply_velocity,
                 reply_onset, reply_duration, reply_count, 12, grid, motif + 4, 1, 70);
        if (amount >= 85)
            emit(source_pitch, source_velocity, source_count, reply_pitch, reply_velocity,
                 reply_onset, reply_duration, reply_count, 15, grid, motif + 5, 0, 62);
    } else { /* Techno Motion */
        const uint8_t steps[] = {0, 2, 4, 6, 8, 10, 12, 14};
        for (uint32_t i = 0; i < sizeof steps; ++i)
            emit(source_pitch, source_velocity, source_count, reply_pitch, reply_velocity,
                 reply_onset, reply_duration, reply_count, steps[i], grid, motif + (i % 4u),
                 0, (i % 4u) == 0 ? 118 : 88);
        if (amount >= 50)
            emit(source_pitch, source_velocity, source_count, reply_pitch, reply_velocity,
                 reply_onset, reply_duration, reply_count, 7, grid, motif + 3, 0, 72);
        if (amount >= 80)
            emit(source_pitch, source_velocity, source_count, reply_pitch, reply_velocity,
                 reply_onset, reply_duration, reply_count, 15, grid, motif + 3, 0, 70);
    }

    /* Pattern construction above is musical-order, but optional notes can be inserted late. */
    for (uint32_t i = 1; i < *reply_count; ++i) {
        uint32_t j = i;
        while (j && reply_onset[j] < reply_onset[j - 1]) {
            uint32_t u;
            uint8_t b;
            u = reply_onset[j]; reply_onset[j] = reply_onset[j-1]; reply_onset[j-1] = u;
            u = reply_duration[j]; reply_duration[j] = reply_duration[j-1]; reply_duration[j-1] = u;
            b = reply_pitch[j]; reply_pitch[j] = reply_pitch[j-1]; reply_pitch[j-1] = b;
            b = reply_velocity[j]; reply_velocity[j] = reply_velocity[j-1]; reply_velocity[j-1] = b;
            --j;
        }
    }
    uint32_t end = grid * 16u;
    for (uint32_t i = 0; i < *reply_count; ++i) {
        uint32_t next = i + 1u < *reply_count ? reply_onset[i + 1u] : end;
        if (reply_duration[i] > next - reply_onset[i]) reply_duration[i] = next - reply_onset[i];
        if (!reply_duration[i]) reply_duration[i] = 1;
    }
    *reply_length = end > PHRASE_MAX_FRAMES ? PHRASE_MAX_FRAMES : end;
}
