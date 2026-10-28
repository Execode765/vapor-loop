/* SPDX-License-Identifier: GPL-3.0-only */
#include "groove.h"

static uint32_t blend(uint32_t from, uint32_t to, int amount) {
    int64_t delta = (int64_t)to - from;
    int64_t value = (int64_t)from + delta * amount / 100;
    return value < 0 ? 0u : (uint32_t)value;
}
static uint32_t quantize(uint32_t value, uint32_t grid) {
    return ((value + grid / 2u) / grid) * grid;
}
void echo_groove_apply(const uint32_t *source_onset, const uint32_t *source_duration,
                       uint32_t *reply_onset, uint32_t *reply_duration,
                       uint32_t count, uint32_t source_length,
                       int bpm, int style, int amount, uint32_t *reply_length) {
    if (bpm < 40) bpm = 40;
    if (bpm > 240) bpm = 240;
    if (style < 0 || style >= ECHO_GROOVE_COUNT) style = ECHO_GROOVE_ORIGINAL;
    if (amount < 0) amount = 0;
    if (amount > 100) amount = 100;
    uint32_t grid = 661500u / (uint32_t)bpm; /* one sixteenth at 44.1 kHz */
    uint32_t end = 0, previous = 0;
    for (uint32_t i = 0; i < count; ++i) {
        uint32_t onset = source_onset[i], duration = source_duration[i];
        if (style == ECHO_GROOVE_PULSE) {
            onset = blend(onset, quantize(onset, grid), amount);
            duration = blend(duration, (grid * 3u) / 4u, amount);
        } else if (style == ECHO_GROOVE_SWING) {
            onset = blend(onset, quantize(onset, grid), amount);
            uint32_t step = (onset + grid / 2u) / grid;
            if (step & 1u) onset += grid * (uint32_t)amount / 300u;
            duration = blend(duration, (grid * 4u) / 5u, amount);
        } else if (style == ECHO_GROOVE_POCKET) {
            onset = blend(onset, quantize(onset, grid), amount / 2);
            if (i & 1u) onset += grid * (uint32_t)amount / 500u;
            uint32_t target = (i & 1u) ? (grid * 5u) / 4u : (grid * 3u) / 4u;
            duration = blend(duration, target, amount);
        } else if (style == ECHO_GROOVE_HALF_TIME) {
            onset = onset + (uint32_t)((uint64_t)onset * (uint32_t)amount / 100u);
            duration = duration + (uint32_t)((uint64_t)duration * (uint32_t)amount / 100u);
        }
        if (!duration) duration = 1;
        if (i && onset <= previous) onset = previous + 1u;
        if (onset >= ECHO_MAX_REPLY_FRAMES) onset = ECHO_MAX_REPLY_FRAMES - 1u;
        if (duration > ECHO_MAX_REPLY_FRAMES - onset) duration = ECHO_MAX_REPLY_FRAMES - onset;
        reply_onset[i] = onset; reply_duration[i] = duration;
        previous = onset;
        if (onset + duration > end) end = onset + duration;
    }
    uint32_t shaped_length = source_length;
    if (style == ECHO_GROOVE_HALF_TIME)
        shaped_length += (uint32_t)((uint64_t)source_length * (uint32_t)amount / 100u);
    if (shaped_length < end) shaped_length = end;
    if (shaped_length > ECHO_MAX_REPLY_FRAMES) shaped_length = ECHO_MAX_REPLY_FRAMES;
    *reply_length = count ? shaped_length : 0;
}
