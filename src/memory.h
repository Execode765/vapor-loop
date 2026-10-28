/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef ECHOLOG_MEMORY_H
#define ECHOLOG_MEMORY_H
#include "voice.h"
#include "harmony.h"
#define ECHO_MAX_NOTES 32u
#define ECHO_MAX_FRAMES (44100u * 8u)
enum { ECHO_IDLE, ECHO_ARMED, ECHO_RECORDING, ECHO_PLAYING };
enum { ECHO_NO_LIMIT, ECHO_NOTE_LIMIT, ECHO_TIME_LIMIT };
enum { ECHO_NO_PLAYBACK, ECHO_EXACT_PLAYBACK, ECHO_REPLY_PLAYBACK };
typedef struct {
    uint32_t onset, duration; /* 44.1 kHz audio frames, relative to first note */
    uint8_t pitch, velocity;
} EchoEvent;
typedef struct {
    EchoVoice voice;
    EchoEvent events[ECHO_MAX_NOTES];
    uint8_t reply[ECHO_MAX_NOTES];
    uint32_t count, length, position, next;
    int mode, active, playing, limit, playback, root, scale;
} EchoMemory;
/* One serialized owner; a future threaded/interrupt adapter must queue commands. */
void echo_memory_init(EchoMemory *m);
void echo_memory_capture(EchoMemory *m);
void echo_memory_finish(EchoMemory *m);
int echo_memory_replay(EchoMemory *m);
int echo_memory_reply(EchoMemory *m);
void echo_memory_set_harmony(EchoMemory *m, int root, int scale);
void echo_memory_stop(EchoMemory *m);
void echo_memory_clear(EchoMemory *m);
void echo_memory_on(EchoMemory *m, int pitch, int velocity);
void echo_memory_off(EchoMemory *m, int pitch);
void echo_memory_render(EchoMemory *m, int32_t *stereo, uint32_t frames);
#endif
