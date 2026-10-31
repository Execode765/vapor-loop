/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef ECHOLOG_HARMONY_H
#define ECHOLOG_HARMONY_H
#include <stdint.h>
enum {
    ECHO_SCALE_MAJOR,
    ECHO_SCALE_NATURAL_MINOR,
    ECHO_SCALE_MAJOR_PENTATONIC,
    ECHO_SCALE_MINOR_PENTATONIC,
    ECHO_SCALE_COUNT
};
void echo_harmony_reply(const uint8_t *source, uint8_t *reply, uint32_t count,
                        int root, int scale, int adventure, uint32_t variation);
int echo_harmony_contains(int pitch, int root, int scale);
void echo_harmony_triad(int anchor, int root, int scale, uint8_t chord[3]);
#endif
