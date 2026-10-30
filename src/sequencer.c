/* SPDX-License-Identifier: GPL-3.0-only */
#include "sequencer.h"

static int valid_resolution(int resolution) {
    return resolution == ECHO_RES_QUARTER || resolution == ECHO_RES_EIGHTH ||
           resolution == ECHO_RES_SIXTEENTH;
}

void echo_seq_init(EchoSequencer *s) {
    s->count = 0; s->bars = 2; s->resolution = ECHO_RES_SIXTEENTH;
    s->length_ticks = (uint16_t)(s->bars * 4u * ECHO_PPQN);
    s->recording = 0; s->record_track = ECHO_TRACK_LEAD; s->open_event = -1;
    for (uint32_t i = 0; i < ECHO_DRUM_STEPS; ++i) s->drum_steps[i] = 0;
}

void echo_seq_configure(EchoSequencer *s, int bars, int resolution) {
    if (bars != 1 && bars != 2 && bars != 4 && bars != 8 && bars != 16) bars = 2;
    if (!valid_resolution(resolution)) resolution = ECHO_RES_SIXTEENTH;
    s->bars = (uint8_t)bars; s->resolution = (uint8_t)resolution;
    s->length_ticks = (uint16_t)(bars * 4u * ECHO_PPQN);
}

void echo_seq_clear_track(EchoSequencer *s, int track) {
    uint32_t out = 0;
    for (uint32_t i = 0; i < s->count; ++i)
        if (s->events[i].track != track) s->events[out++] = s->events[i];
    s->count = out; s->open_event = -1;
    if (track == ECHO_TRACK_DRUMS)
        for (uint32_t i = 0; i < ECHO_DRUM_STEPS; ++i) s->drum_steps[i] = 0;
}

uint16_t echo_seq_frame_to_tick(uint32_t frame, int bpm) {
    if (bpm < 40) bpm = 40;
    if (bpm > 240) bpm = 240;
    uint64_t tick = (uint64_t)frame * (uint32_t)bpm * ECHO_PPQN / (44100u * 60u);
    return tick > 65535u ? 65535u : (uint16_t)tick;
}

uint32_t echo_seq_tick_to_frame(uint16_t tick, int bpm) {
    if (bpm < 40) bpm = 40;
    if (bpm > 240) bpm = 240;
    return (uint32_t)((uint64_t)tick * 44100u * 60u / ((uint32_t)bpm * ECHO_PPQN));
}

static uint16_t quantized_tick(const EchoSequencer *s, uint32_t frame, int bpm) {
    uint32_t tick = echo_seq_frame_to_tick(frame, bpm);
    uint32_t step = ECHO_PPQN * 4u / s->resolution;
    tick = ((tick + step / 2u) / step) * step;
    if (tick >= s->length_ticks) tick = s->length_ticks ? s->length_ticks - 1u : 0u;
    return (uint16_t)tick;
}

void echo_seq_start_record(EchoSequencer *s, int track) {
    if (track < 0 || track >= ECHO_TRACK_COUNT) return;
    echo_seq_clear_track(s, track);
    s->recording = 1; s->record_track = (uint8_t)track; s->open_event = -1;
}

int echo_seq_note_on(EchoSequencer *s, int track, int value, int velocity,
                     uint32_t frame, int bpm) {
    if (!s->recording || s->record_track != track || s->count >= ECHO_SEQ_MAX_EVENTS) return 0;
    if (value < 0 || value > 127 || velocity <= 0) return 0;
    if (velocity > 127) velocity = 127;
    if (s->open_event >= 0) {
        EchoSeqEvent *old = &s->events[s->open_event];
        uint16_t end = quantized_tick(s, frame, bpm);
        old->duration = end > old->tick ? (uint16_t)(end - old->tick) : (uint16_t)1u;
    }
    EchoSeqEvent *e = &s->events[s->count];
    e->tick = quantized_tick(s, frame, bpm); e->duration = 1;
    e->track = (uint8_t)track; e->value = (uint8_t)value; e->velocity = (uint8_t)velocity;
    s->open_event = (int)s->count++;
    return 1;
}

void echo_seq_note_off(EchoSequencer *s, int track, int value,
                       uint32_t frame, int bpm) {
    if (!s->recording || s->open_event < 0) return;
    EchoSeqEvent *e = &s->events[s->open_event];
    if (e->track != track || e->value != value) return;
    uint16_t end = quantized_tick(s, frame, bpm);
    e->duration = end > e->tick ? (uint16_t)(end - e->tick) : (uint16_t)1u;
    s->open_event = -1;
}

void echo_seq_stop_record(EchoSequencer *s, uint32_t frame, int bpm) {
    if (s->open_event >= 0) {
        EchoSeqEvent *e = &s->events[s->open_event];
        uint16_t end = echo_seq_frame_to_tick(frame, bpm);
        if (end > s->length_ticks) end = s->length_ticks;
        e->duration = end > e->tick ? (uint16_t)(end - e->tick) : (uint16_t)1u;
    }
    s->recording = 0; s->open_event = -1;
}

uint32_t echo_seq_track_count(const EchoSequencer *s, int track) {
    uint32_t count = 0;
    for (uint32_t i = 0; i < s->count; ++i) if (s->events[i].track == track) ++count;
    return count;
}

void echo_seq_toggle_drum(EchoSequencer *s, int step, int voice) {
    if (step < 0 || step >= (int)ECHO_DRUM_STEPS || voice < 0 || voice >= ECHO_DRUM_VOICE_COUNT) return;
    s->drum_steps[step] ^= (uint8_t)(1u << voice);
}

uint8_t echo_seq_drum_mask(const EchoSequencer *s, int step) {
    return step >= 0 && step < (int)ECHO_DRUM_STEPS ? s->drum_steps[step] : 0;
}
