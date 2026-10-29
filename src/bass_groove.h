/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef ECHOLOG_BASS_GROOVE_H
#define ECHOLOG_BASS_GROOVE_H
#include <stdint.h>

enum {
    ECHO_BASS_GROOVE_STRAIGHT,
    ECHO_BASS_GROOVE_AUTO,
    ECHO_BASS_GROOVE_TRAP,
    ECHO_BASS_GROOVE_RNB,
    ECHO_BASS_GROOVE_TECHNO,
    ECHO_BASS_GROOVE_COUNT
};

/* Shapes an already-safe bass grid in place. Straight and amount zero are exact bypasses. */
void echo_bass_groove_apply(uint8_t *pitch, uint8_t *velocity,
                            uint32_t *onset, uint32_t *duration,
                            uint32_t *count, uint32_t length, uint32_t spacing,
                            int style, int amount);

#endif
