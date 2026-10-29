/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef ECHOLOG_MEMORY_H
#define ECHOLOG_MEMORY_H
#include "voice.h"
#include "harmony.h"
#include "groove.h"
#include "phrase.h"
#include "bass_groove.h"
#define ECHO_MAX_NOTES 32u
#define ECHO_MAX_FRAMES (44100u * 8u)
#define ECHO_CANDIDATE_SLOTS 3u
#define ECHO_MAX_BASS_EVENTS 32u
enum { ECHO_IDLE, ECHO_ARMED, ECHO_RECORDING, ECHO_PLAYING };
enum { ECHO_NO_LIMIT, ECHO_NOTE_LIMIT, ECHO_TIME_LIMIT };
enum { ECHO_NO_PLAYBACK, ECHO_EXACT_PLAYBACK, ECHO_REPLY_PLAYBACK, ECHO_CANDIDATE_PLAYBACK };
enum { ECHO_BASS_OFF, ECHO_BASS_MANUAL, ECHO_BASS_ROOT, ECHO_BASS_FOLLOW, ECHO_BASS_MODE_COUNT };
enum { ECHO_BASS_PEDAL, ECHO_BASS_HALF, ECHO_BASS_QUARTER, ECHO_BASS_DENSITY_COUNT };
typedef struct {
    uint32_t onset, duration; /* 44.1 kHz audio frames, relative to first note */
    uint8_t pitch, velocity;
} EchoEvent;
typedef struct {
    uint8_t pitch[ECHO_MAX_NOTES], velocity[ECHO_MAX_NOTES];
    uint32_t onset[ECHO_MAX_NOTES], duration[ECHO_MAX_NOTES];
    uint32_t count, length, generation;
    int filled, root, scale, adventure, bpm, phrase_style, phrase_amount;
    int groove_style, groove_amount;
} EchoCandidate;
typedef struct {
    EchoVoice voice, bass_voice;
    EchoEvent events[ECHO_MAX_NOTES];
    uint8_t reply[ECHO_MAX_NOTES], reply_velocity[ECHO_MAX_NOTES];
    uint32_t reply_onset[ECHO_MAX_NOTES], reply_duration[ECHO_MAX_NOTES];
    uint32_t count, reply_count, length, reply_length, play_length, position, next;
    int mode, active, playing, limit, playback, root, scale, adventure;
    int groove_style, groove_amount, bpm;
    int phrase_style, phrase_amount;
    int last_reply_valid, candidate_slot;
    uint32_t variation, reply_generation;
    EchoCandidate candidates[ECHO_CANDIDATE_SLOTS];
    uint8_t bass_pitch[ECHO_MAX_BASS_EVENTS], bass_velocity[ECHO_MAX_BASS_EVENTS];
    uint32_t bass_onset[ECHO_MAX_BASS_EVENTS], bass_duration[ECHO_MAX_BASS_EVENTS];
    uint32_t bass_count, bass_next;
    int bass_mode, bass_density, bass_octave, bass_volume;
    int bass_groove_style, bass_groove_amount;
    int bass_playing, bass_manual, bass_manual_note;
} EchoMemory;
/* One serialized owner; a future threaded/interrupt adapter must queue commands. */
void echo_memory_init(EchoMemory *m);
void echo_memory_capture(EchoMemory *m);
void echo_memory_finish(EchoMemory *m);
int echo_memory_replay(EchoMemory *m);
int echo_memory_reply(EchoMemory *m);
int echo_memory_keep(EchoMemory *m, int slot);
int echo_memory_play_candidate(EchoMemory *m, int slot);
void echo_memory_clear_candidate(EchoMemory *m, int slot);
uint32_t echo_memory_candidate_mask(const EchoMemory *m);
void echo_memory_set_bass(EchoMemory *m, int mode, int density, int octave, int volume);
void echo_memory_set_bass_groove(EchoMemory *m, int style, int amount);
void echo_memory_bass_on(EchoMemory *m, int pitch, int velocity);
void echo_memory_bass_off(EchoMemory *m, int pitch);
void echo_memory_set_harmony(EchoMemory *m, int root, int scale);
void echo_memory_set_adventure(EchoMemory *m, int adventure);
void echo_memory_set_groove(EchoMemory *m, int bpm, int style, int amount);
void echo_memory_set_phrase(EchoMemory *m, int style, int amount);
void echo_memory_stop(EchoMemory *m);
void echo_memory_clear(EchoMemory *m);
void echo_memory_on(EchoMemory *m, int pitch, int velocity);
void echo_memory_off(EchoMemory *m, int pitch);
void echo_memory_render(EchoMemory *m, int32_t *stereo, uint32_t frames);
#endif
