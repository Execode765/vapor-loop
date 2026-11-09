/* SPDX-License-Identifier: GPL-3.0-only */
#include "harmony.h"

static const uint8_t intervals[ECHO_SCALE_COUNT][7] = {
    {0, 2, 4, 5, 7, 9, 11},
    {0, 2, 3, 5, 7, 8, 10},
    {0, 2, 4, 7, 9, 0, 0},
    {0, 3, 5, 7, 10, 0, 0}
};
static const uint8_t lengths[ECHO_SCALE_COUNT] = {7, 7, 5, 5};

static int normalize_root(int root) {
    root %= 12;
    return root < 0 ? root + 12 : root;
}
static int valid_scale(int scale) {
    return scale >= 0 && scale < ECHO_SCALE_COUNT ? scale : ECHO_SCALE_MAJOR;
}
static int scale_pitch(int index, int root, int scale) {
    int len = lengths[scale];
    int octave = index / len;
    int degree = index % len;
    if (degree < 0) { degree += len; --octave; }
    return root + 12 * octave + intervals[scale][degree];
}
static int nearest_index(int pitch, int root, int scale) {
    int best = 0, distance = 1000;
    int len = lengths[scale];
    for (int index = -len; index <= 12 * len; ++index) {
        int d = scale_pitch(index, root, scale) - pitch;
        if (d < 0) d = -d;
        if (d < distance) { best = index; distance = d; }
    }
    return best;
}
static int register_index(int index, int root, int scale) {
    int len = lengths[scale];
    while (scale_pitch(index, root, scale) < 36) index += len;
    while (scale_pitch(index, root, scale) > 96) index -= len;
    return index;
}
static int cadence_pitch(int target, int root, int scale) {
    int third = (scale == ECHO_SCALE_NATURAL_MINOR ||
                 scale == ECHO_SCALE_MINOR_PENTATONIC) ? 3 : 4;
    int classes[3] = {root, (root + third) % 12, (root + 7) % 12};
    int best = target, distance = 1000;
    for (int pitch = 36; pitch <= 96; ++pitch) {
        int pc = pitch % 12;
        if (pc != classes[0] && pc != classes[1] && pc != classes[2]) continue;
        int d = pitch - target;
        if (d < 0) d = -d;
        if (d < distance) { best = pitch; distance = d; }
    }
    return best;
}
static uint32_t next_random(uint32_t *state) {
    uint32_t x = *state;
    x ^= x << 13; x ^= x >> 17; x ^= x << 5;
    return *state = x ? x : 0x6d2b79f5u;
}

int echo_harmony_contains(int pitch, int root, int scale) {
    root = normalize_root(root); scale = valid_scale(scale);
    int pc = (pitch - root) % 12;
    if (pc < 0) pc += 12;
    for (int i = 0; i < lengths[scale]; ++i)
        if (intervals[scale][i] == pc) return 1;
    return 0;
}

static int pitch_class_distance(int a, int b) {
    int d = normalize_root(a - b);
    return d > 6 ? 12 - d : d;
}

static int is_strong_beat(uint32_t onset, uint32_t beat) {
    if (!beat) return 0;
    uint32_t remainder = onset % beat;
    uint32_t distance = remainder < beat - remainder ? remainder : beat - remainder;
    return distance <= beat / 16u;
}

int echo_harmony_detect(const uint8_t *pitch, const uint32_t *onset,
                        const uint32_t *duration, uint32_t count, uint32_t beat,
                        int *root, int *scale) {
    if (!pitch || !count || !root || !scale) return 0;
    uint32_t longest = 1;
    for (uint32_t i = 0; i < count; ++i)
        if (duration && duration[i] > longest) longest = duration[i];
    int best_score = -2147483647, second_score = -2147483647;
    int best_root = 0, best_scale = ECHO_SCALE_MAJOR;
    for (int candidate_root = 0; candidate_root < 12; ++candidate_root) {
        for (int candidate_scale = 0; candidate_scale < ECHO_SCALE_COUNT; ++candidate_scale) {
            int minor = candidate_scale == ECHO_SCALE_NATURAL_MINOR ||
                        candidate_scale == ECHO_SCALE_MINOR_PENTATONIC;
            int third = (candidate_root + (minor ? 3 : 4)) % 12;
            int fifth = (candidate_root + 7) % 12;
            int score = candidate_scale >= ECHO_SCALE_MAJOR_PENTATONIC ? 12 : 0;
            for (uint32_t i = 0; i < count; ++i) {
                int pc = pitch[i] % 12;
                int weight = 4 + (int)((duration ? duration[i] : 1u) * 8u / longest);
                if (echo_harmony_contains(pc, candidate_root, candidate_scale)) score += 10 * weight;
                else score -= 18 * weight;
                if (pc == candidate_root) score += 5 * weight;
                else if (pc == fifth) score += 4 * weight;
                else if (pc == third) score += 3 * weight;
                if (onset && is_strong_beat(onset[i], beat)) {
                    if (pc == candidate_root) score += 16;
                    else if (pc == third || pc == fifth) score += 7;
                    else if (!echo_harmony_contains(pc, candidate_root, candidate_scale)) score -= 10;
                }
            }
            int first = pitch[0] % 12, last = pitch[count - 1u] % 12;
            if (first == candidate_root) score += 34;
            else if (first == fifth) score += 14;
            else if (first == third) score += 9;
            if (last == candidate_root) score += 42;
            else if (last == fifth) score += 17;
            else if (last == third) score += 11;
            /* Close ties prefer a root near the phrase boundaries. */
            score -= pitch_class_distance(candidate_root, first);
            score -= pitch_class_distance(candidate_root, last);
            if (score > best_score) {
                second_score = best_score; best_score = score;
                best_root = candidate_root; best_scale = candidate_scale;
            } else if (score > second_score) second_score = score;
        }
    }
    *root = best_root; *scale = best_scale;
    if (best_score <= 0) return 0;
    int gap = best_score - second_score;
    int confidence = 45 + gap * 100 / (best_score + 1);
    if (confidence < 1) confidence = 1;
    if (confidence > 100) confidence = 100;
    return confidence;
}

void echo_harmony_triad(int anchor, int root, int scale, uint8_t chord[3]) {
    root = normalize_root(root); scale = valid_scale(scale);
    int index = nearest_index(anchor, root, scale);
    int len = lengths[scale];
    while (scale_pitch(index, root, scale) < 48) index += len;
    while (scale_pitch(index, root, scale) > 67) index -= len;
    chord[0] = (uint8_t)scale_pitch(index, root, scale);
    chord[1] = (uint8_t)scale_pitch(index + 2, root, scale);
    chord[2] = (uint8_t)scale_pitch(index + 4, root, scale);
    while (chord[2] > 84) {
        chord[0] -= 12; chord[1] -= 12; chord[2] -= 12;
    }
}

void echo_harmony_reply(const uint8_t *source, uint8_t *reply, uint32_t count,
                        int root, int scale, int adventure, uint32_t variation) {
    if (!count) return;
    root = normalize_root(root); scale = valid_scale(scale);
    if (adventure < 0) adventure = 0;
    if (adventure > 100) adventure = 100;
    uint32_t random = 0x811c9dc5u ^ variation ^ (uint32_t)(root * 131 + scale * 977);
    for (uint32_t i = 0; i < count; ++i) random = (random ^ source[i]) * 16777619u;
    if (!random) random = 1;
    int source_index = nearest_index(source[0], root, scale);
    int reply_index = register_index(nearest_index(source[0] + 7, root, scale), root, scale);
    reply[0] = (uint8_t)scale_pitch(reply_index, root, scale);
    for (uint32_t i = 1; i < count; ++i) {
        int next_source = nearest_index(source[i], root, scale);
        reply_index = register_index(reply_index - (next_source - source_index), root, scale);
        /* Protect the opening and cadence; vary only interior scale degrees. */
        if (i + 1 < count && adventure && (int)(next_random(&random) % 100u) < adventure) {
            int magnitude = 1;
            if (adventure >= 67 && next_random(&random) % 100u < (uint32_t)(adventure - 50))
                magnitude = 2;
            reply_index += next_random(&random) & 1u ? magnitude : -magnitude;
            reply_index = register_index(reply_index, root, scale);
        }
        reply[i] = (uint8_t)scale_pitch(reply_index, root, scale);
        source_index = next_source;
    }
    reply[count - 1] = (uint8_t)cadence_pitch(reply[count - 1], root, scale);
}

