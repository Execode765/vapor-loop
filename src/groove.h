/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef ECHOLOG_GROOVE_H
#define ECHOLOG_GROOVE_H
#include <stdint.h>
enum {
    ECHO_GROOVE_ORIGINAL,
    ECHO_GROOVE_PULSE,
    ECHO_GROOVE_SWING,
    ECHO_GROOVE_POCKET,
    ECHO_GROOVE_HALF_TIME,
    ECHO_GROOVE_COUNT
};
#define ECHO_MAX_REPLY_FRAMES (44100u * 16u)
void echo_groove_apply(const uint32_t *source_onset, const uint32_t *source_duration,
                       uint32_t *reply_onset, uint32_t *reply_duration,
                       uint32_t count, uint32_t source_length,
                       int bpm, int style, int amount, uint32_t *reply_length);
#endif
