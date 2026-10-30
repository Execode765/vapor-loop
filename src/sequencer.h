/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef VAPOR_LOOP_SEQUENCER_H
#define VAPOR_LOOP_SEQUENCER_H
#include <stdint.h>

#define ECHO_PPQN 96u
#define ECHO_SEQ_MAX_EVENTS 128u
#define ECHO_DRUM_STEPS 16u

enum { ECHO_TRACK_LEAD, ECHO_TRACK_BASS, ECHO_TRACK_DRUMS, ECHO_TRACK_CHORDS, ECHO_TRACK_COUNT };
enum { ECHO_TRACK_OFF, ECHO_TRACK_MANUAL, ECHO_TRACK_COMPANION };
enum { ECHO_RES_QUARTER = 4, ECHO_RES_EIGHTH = 8, ECHO_RES_SIXTEENTH = 16 };
enum { ECHO_DRUM_KICK, ECHO_DRUM_SNARE, ECHO_DRUM_HAT, ECHO_DRUM_VOICE_COUNT };

typedef struct {
    uint16_t tick, duration;
    uint8_t track, value, velocity;
} EchoSeqEvent;

typedef struct {
    EchoSeqEvent events[ECHO_SEQ_MAX_EVENTS];
    uint32_t count;
    uint16_t length_ticks;
    uint8_t bars, resolution, recording, record_track;
    int open_event;
    uint8_t drum_steps[ECHO_DRUM_STEPS];
} EchoSequencer;

void echo_seq_init(EchoSequencer *s);
void echo_seq_configure(EchoSequencer *s, int bars, int resolution);
void echo_seq_clear_track(EchoSequencer *s, int track);
void echo_seq_start_record(EchoSequencer *s, int track);
void echo_seq_stop_record(EchoSequencer *s, uint32_t frame, int bpm);
int echo_seq_note_on(EchoSequencer *s, int track, int value, int velocity,
                     uint32_t frame, int bpm);
void echo_seq_note_off(EchoSequencer *s, int track, int value,
                       uint32_t frame, int bpm);
uint32_t echo_seq_track_count(const EchoSequencer *s, int track);
uint16_t echo_seq_frame_to_tick(uint32_t frame, int bpm);
uint32_t echo_seq_tick_to_frame(uint16_t tick, int bpm);
void echo_seq_toggle_drum(EchoSequencer *s, int step, int voice);
uint8_t echo_seq_drum_mask(const EchoSequencer *s, int step);

#endif
