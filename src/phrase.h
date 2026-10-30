/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef ECHOLOG_PHRASE_H
#define ECHOLOG_PHRASE_H
#include <stdint.h>

enum {
    ECHO_PHRASE_ORIGINAL,
    ECHO_PHRASE_TRAP_POCKET,
    ECHO_PHRASE_RNB_FLOW,
    ECHO_PHRASE_TECHNO_MOTION,
    ECHO_PHRASE_COUNT
};

/* May remove, repeat, or reposition reply notes. Arrays have capacity 128. */
void echo_phrase_apply(const uint8_t *source_pitch, const uint8_t *source_velocity,
                       const uint32_t *source_onset, const uint32_t *source_duration,
                       uint32_t source_count, uint32_t source_length,
                       uint8_t *reply_pitch, uint8_t *reply_velocity,
                       uint32_t *reply_onset, uint32_t *reply_duration,
                       uint32_t *reply_count, uint32_t *reply_length,
                       int bpm, int style, int amount, uint32_t variation);
#endif
