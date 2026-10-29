/* SPDX-License-Identifier: GPL-3.0-only */
#include "memory.h"
void echo_memory_init(EchoMemory *m) {
    echo_voice_init(&m->voice);
    echo_voice_init(&m->bass_voice);
    m->count = m->reply_count = m->length = m->reply_length = m->play_length = m->position = m->next = 0;
    m->mode = ECHO_IDLE; m->active = m->playing = -1; m->limit = ECHO_NO_LIMIT;
    m->playback = ECHO_NO_PLAYBACK;
    m->root = 0; m->scale = ECHO_SCALE_MAJOR; m->adventure = 0; m->variation = 0;
    m->bpm = 120; m->groove_style = ECHO_GROOVE_ORIGINAL; m->groove_amount = 0;
    m->phrase_style = ECHO_PHRASE_ORIGINAL; m->phrase_amount = 0;
    m->last_reply_valid = 0; m->candidate_slot = 0; m->reply_generation = 0;
    for (uint32_t i = 0; i < ECHO_CANDIDATE_SLOTS; ++i) m->candidates[i].filled = 0;
    m->bass_mode = ECHO_BASS_OFF; m->bass_density = ECHO_BASS_HALF;
    m->bass_octave = 1; m->bass_volume = 70;
    m->bass_groove_style = ECHO_BASS_GROOVE_AUTO; m->bass_groove_amount = 55;
    m->bass_count = m->bass_next = 0; m->bass_playing = -1;
    m->bass_manual = 0; m->bass_manual_note = -1;
}
static void close_event(EchoMemory *m) {
    if (m->active < 0) return;
    EchoEvent *e = &m->events[m->active];
    e->duration = m->position - e->onset;
    /* No rendered frames elapsed: this gesture was inaudible; do not store a zero-length note. */
    if (!e->duration) --m->count;
    m->active = -1;
}
void echo_memory_finish(EchoMemory *m) {
    if (m->mode != ECHO_ARMED && m->mode != ECHO_RECORDING) return;
    close_event(m);
    m->length = m->count ? m->position : 0;
    m->mode = ECHO_IDLE;
    echo_voice_stop(&m->voice);
}
void echo_memory_stop(EchoMemory *m) {
    echo_memory_finish(m);
    m->mode = ECHO_IDLE; m->playing = -1; m->playback = ECHO_NO_PLAYBACK;
    echo_voice_stop(&m->voice);
    echo_voice_stop(&m->bass_voice);
    m->bass_count = m->bass_next = 0; m->bass_playing = -1;
    m->bass_manual = 0; m->bass_manual_note = -1;
}
void echo_memory_clear(EchoMemory *m) {
    echo_memory_stop(m);
    m->count = m->reply_count = m->length = m->reply_length = m->play_length = m->position = m->next = 0;
    m->active = -1; m->limit = ECHO_NO_LIMIT;
    m->last_reply_valid = 0;
}
void echo_memory_capture(EchoMemory *m) {
    echo_memory_clear(m); m->variation = 0; echo_voice_init(&m->voice); m->mode = ECHO_ARMED;
}
int echo_memory_replay(EchoMemory *m) {
    echo_memory_stop(m);
    if (!m->count || !m->length) return 0;
    echo_voice_init(&m->voice);
    m->mode = ECHO_PLAYING; m->position = m->next = 0; m->playing = -1;
    m->play_length = m->length;
    m->playback = ECHO_EXACT_PLAYBACK;
    echo_memory_set_bass(m, m->bass_mode, m->bass_density, m->bass_octave, m->bass_volume);
    return 1;
}
int echo_memory_reply(EchoMemory *m) {
    echo_memory_stop(m);
    if (!m->count || !m->length) return 0;
    uint8_t source[ECHO_MAX_NOTES];
    for (uint32_t i = 0; i < m->count; ++i) source[i] = m->events[i].pitch;
    uint32_t generation = m->variation++;
    m->reply_generation = generation;
    uint8_t harmony[ECHO_MAX_NOTES], velocity[ECHO_MAX_NOTES];
    echo_harmony_reply(source, harmony, m->count, m->root, m->scale,
                       m->adventure, generation);
    uint32_t source_onset[ECHO_MAX_NOTES], source_duration[ECHO_MAX_NOTES];
    for (uint32_t i = 0; i < m->count; ++i) {
        source_onset[i] = m->events[i].onset; source_duration[i] = m->events[i].duration;
        velocity[i] = m->events[i].velocity;
    }
    uint32_t phrase_onset[ECHO_MAX_NOTES], phrase_duration[ECHO_MAX_NOTES], phrase_length;
    echo_phrase_apply(harmony, velocity, source_onset, source_duration,
                      m->count, m->length, m->reply, m->reply_velocity,
                      phrase_onset, phrase_duration, &m->reply_count, &phrase_length,
                      m->bpm, m->phrase_style, m->phrase_amount, generation);
    echo_groove_apply(phrase_onset, phrase_duration, m->reply_onset, m->reply_duration,
                      m->reply_count, phrase_length, m->bpm, m->groove_style,
                      m->groove_amount, &m->reply_length);
    echo_voice_init(&m->voice);
    m->mode = ECHO_PLAYING; m->position = m->next = 0; m->playing = -1;
    m->play_length = m->reply_length;
    m->playback = ECHO_REPLY_PLAYBACK;
    m->last_reply_valid = 1;
    echo_memory_set_bass(m, m->bass_mode, m->bass_density, m->bass_octave, m->bass_volume);
    return 1;
}
int echo_memory_keep(EchoMemory *m, int slot) {
    if (slot < 0 || slot >= (int)ECHO_CANDIDATE_SLOTS || !m->last_reply_valid || !m->reply_count) return 0;
    EchoCandidate *c = &m->candidates[slot];
    for (uint32_t i = 0; i < m->reply_count; ++i) {
        c->pitch[i] = m->reply[i]; c->velocity[i] = m->reply_velocity[i];
        c->onset[i] = m->reply_onset[i]; c->duration[i] = m->reply_duration[i];
    }
    c->count = m->reply_count; c->length = m->reply_length; c->generation = m->reply_generation;
    c->root = m->root; c->scale = m->scale; c->adventure = m->adventure; c->bpm = m->bpm;
    c->phrase_style = m->phrase_style; c->phrase_amount = m->phrase_amount;
    c->groove_style = m->groove_style; c->groove_amount = m->groove_amount;
    c->filled = 1; m->candidate_slot = slot;
    return 1;
}
int echo_memory_play_candidate(EchoMemory *m, int slot) {
    echo_memory_stop(m);
    if (slot < 0 || slot >= (int)ECHO_CANDIDATE_SLOTS || !m->candidates[slot].filled) return 0;
    m->candidate_slot = slot;
    echo_voice_init(&m->voice);
    m->mode = ECHO_PLAYING; m->position = m->next = 0; m->playing = -1;
    m->play_length = m->candidates[slot].length;
    m->playback = ECHO_CANDIDATE_PLAYBACK;
    echo_memory_set_bass(m, m->bass_mode, m->bass_density, m->bass_octave, m->bass_volume);
    return 1;
}
void echo_memory_clear_candidate(EchoMemory *m, int slot) {
    if (slot < 0 || slot >= (int)ECHO_CANDIDATE_SLOTS) return;
    if (m->mode == ECHO_PLAYING && m->playback == ECHO_CANDIDATE_PLAYBACK && m->candidate_slot == slot)
        echo_memory_stop(m);
    m->candidates[slot].filled = 0;
}
uint32_t echo_memory_candidate_mask(const EchoMemory *m) {
    uint32_t mask = 0;
    for (uint32_t i = 0; i < ECHO_CANDIDATE_SLOTS; ++i) if (m->candidates[i].filled) mask |= 1u << i;
    return mask;
}
void echo_memory_set_harmony(EchoMemory *m, int root, int scale) {
    m->root = root >= 0 && root < 12 ? root : 0;
    m->scale = scale >= 0 && scale < ECHO_SCALE_COUNT ? scale : ECHO_SCALE_MAJOR;
}
void echo_memory_set_adventure(EchoMemory *m, int adventure) {
    if (adventure < 0) adventure = 0;
    if (adventure > 100) adventure = 100;
    m->adventure = adventure;
    m->variation = 0;
}
void echo_memory_set_groove(EchoMemory *m, int bpm, int style, int amount) {
    if (bpm < 40) bpm = 40;
    if (bpm > 240) bpm = 240;
    if (style < 0 || style >= ECHO_GROOVE_COUNT) style = ECHO_GROOVE_ORIGINAL;
    if (amount < 0) amount = 0;
    if (amount > 100) amount = 100;
    m->bpm = bpm; m->groove_style = style; m->groove_amount = amount;
    m->variation = 0;
}
void echo_memory_set_phrase(EchoMemory *m, int style, int amount) {
    if (style < 0 || style >= ECHO_PHRASE_COUNT) style = ECHO_PHRASE_ORIGINAL;
    if (amount < 0) amount = 0;
    if (amount > 100) amount = 100;
    m->phrase_style = style; m->phrase_amount = amount; m->variation = 0;
}
static uint32_t playback_count(const EchoMemory *m) {
    if (m->playback == ECHO_REPLY_PLAYBACK) return m->reply_count;
    if (m->playback == ECHO_CANDIDATE_PLAYBACK) return m->candidates[m->candidate_slot].count;
    return m->count;
}
static int playback_pitch(const EchoMemory *m, uint32_t index) {
    if (m->playback == ECHO_REPLY_PLAYBACK) return m->reply[index];
    if (m->playback == ECHO_CANDIDATE_PLAYBACK) return m->candidates[m->candidate_slot].pitch[index];
    return m->events[index].pitch;
}
static int playback_velocity(const EchoMemory *m, uint32_t index) {
    if (m->playback == ECHO_REPLY_PLAYBACK) return m->reply_velocity[index];
    if (m->playback == ECHO_CANDIDATE_PLAYBACK) return m->candidates[m->candidate_slot].velocity[index];
    return m->events[index].velocity;
}
static uint32_t playback_onset(const EchoMemory *m, uint32_t index) {
    if (m->playback == ECHO_REPLY_PLAYBACK) return m->reply_onset[index];
    if (m->playback == ECHO_CANDIDATE_PLAYBACK) return m->candidates[m->candidate_slot].onset[index];
    return m->events[index].onset;
}
static uint32_t playback_duration(const EchoMemory *m, uint32_t index) {
    if (m->playback == ECHO_REPLY_PLAYBACK) return m->reply_duration[index];
    if (m->playback == ECHO_CANDIDATE_PLAYBACK) return m->candidates[m->candidate_slot].duration[index];
    return m->events[index].duration;
}
static int playback_root(const EchoMemory *m) {
    return m->playback == ECHO_CANDIDATE_PLAYBACK ? m->candidates[m->candidate_slot].root : m->root;
}
static int playback_scale(const EchoMemory *m) {
    return m->playback == ECHO_CANDIDATE_PLAYBACK ? m->candidates[m->candidate_slot].scale : m->scale;
}
static int playback_bpm(const EchoMemory *m) {
    return m->playback == ECHO_CANDIDATE_PLAYBACK ? m->candidates[m->candidate_slot].bpm : m->bpm;
}
static int playback_phrase_style(const EchoMemory *m) {
    return m->playback == ECHO_CANDIDATE_PLAYBACK ? m->candidates[m->candidate_slot].phrase_style : m->phrase_style;
}
static int pitch_distance(int a, int b) {
    int d = a - b; if (d < 0) d = -d; return d > 6 ? 12 - d : d;
}
static int bass_follow_pitch(const EchoMemory *m, uint32_t frame) {
    uint32_t count = playback_count(m), chosen = 0;
    for (uint32_t i = 0; i < count && playback_onset(m, i) <= frame; ++i) chosen = i;
    int pc = playback_pitch(m, chosen) % 12;
    int root = playback_root(m);
    int scale = playback_scale(m);
    int third = root + ((scale == ECHO_SCALE_NATURAL_MINOR || scale == ECHO_SCALE_MINOR_PENTATONIC) ? 3 : 4);
    int tones[3] = {root % 12, third % 12, (root + 7) % 12};
    int best = tones[0], distance = pitch_distance(pc, tones[0]);
    for (int i = 1; i < 3; ++i) { int d = pitch_distance(pc, tones[i]); if (d < distance) { distance = d; best = tones[i]; } }
    return 12 * (m->bass_octave + 1) + best;
}
static void prepare_bass(EchoMemory *m) {
    echo_voice_stop(&m->bass_voice);
    m->bass_count = m->bass_next = 0; m->bass_playing = -1;
    if (m->bass_mode != ECHO_BASS_ROOT && m->bass_mode != ECHO_BASS_FOLLOW) return;
    if (m->mode != ECHO_PLAYING || !m->play_length) return;
    int bpm = playback_bpm(m); if (bpm < 40) bpm = 40; if (bpm > 240) bpm = 240;
    uint32_t beat = 2646000u / (uint32_t)bpm;
    uint32_t spacing = m->bass_density == ECHO_BASS_QUARTER ? beat :
                       m->bass_density == ECHO_BASS_HALF ? beat * 2u : m->play_length;
    if (!spacing) spacing = 1;
    for (uint32_t onset = 0; onset < m->play_length && m->bass_count < ECHO_MAX_BASS_EVENTS; onset += spacing) {
        uint32_t i = m->bass_count++;
        m->bass_onset[i] = onset;
        uint32_t remaining = m->play_length - onset;
        uint32_t gate = m->bass_density == ECHO_BASS_PEDAL ? remaining : spacing * 4u / 5u;
        m->bass_duration[i] = gate < remaining ? gate : remaining;
        m->bass_velocity[i] = 105;
        /* Follow begins from solid ground; later boundaries may answer with a third or fifth. */
        m->bass_pitch[i] = (uint8_t)(m->bass_mode == ECHO_BASS_ROOT || onset == 0 ?
            12 * (m->bass_octave + 1) + playback_root(m) : bass_follow_pitch(m, onset));
    }
    int style = m->bass_groove_style;
    if (style == ECHO_BASS_GROOVE_AUTO) {
        int phrase = playback_phrase_style(m);
        style = phrase == ECHO_PHRASE_TRAP_POCKET ? ECHO_BASS_GROOVE_TRAP :
                phrase == ECHO_PHRASE_RNB_FLOW ? ECHO_BASS_GROOVE_RNB :
                phrase == ECHO_PHRASE_TECHNO_MOTION ? ECHO_BASS_GROOVE_TECHNO :
                ECHO_BASS_GROOVE_STRAIGHT;
    }
    if (m->bass_density != ECHO_BASS_PEDAL)
        echo_bass_groove_apply(m->bass_pitch, m->bass_velocity, m->bass_onset,
                               m->bass_duration, &m->bass_count, m->play_length,
                               spacing, style, m->bass_groove_amount);
    /* A control change during playback joins at the next boundary, never rewinds to frame zero. */
    while (m->bass_next < m->bass_count && m->bass_onset[m->bass_next] < m->position) ++m->bass_next;
}
void echo_memory_set_bass_groove(EchoMemory *m, int style, int amount) {
    if (style < 0 || style >= ECHO_BASS_GROOVE_COUNT) style = ECHO_BASS_GROOVE_AUTO;
    if (amount < 0) amount = 0;
    if (amount > 100) amount = 100;
    m->bass_groove_style = style; m->bass_groove_amount = amount;
    if (!m->bass_manual) prepare_bass(m);
}
void echo_memory_set_bass(EchoMemory *m, int mode, int density, int octave, int volume) {
    if (mode < 0 || mode >= ECHO_BASS_MODE_COUNT) mode = ECHO_BASS_OFF;
    if (density < 0 || density >= ECHO_BASS_DENSITY_COUNT) density = ECHO_BASS_HALF;
    if (octave < 0) octave = 0;
    if (octave > 3) octave = 3;
    if (volume < 0) volume = 0;
    if (volume > 100) volume = 100;
    m->bass_mode = mode; m->bass_density = density; m->bass_octave = octave; m->bass_volume = volume;
    m->bass_manual = 0; m->bass_manual_note = -1;
    prepare_bass(m);
}
void echo_memory_bass_on(EchoMemory *m, int pitch, int velocity) {
    if (m->bass_mode == ECHO_BASS_OFF || pitch < 0 || pitch > 127) return;
    if (velocity <= 0) { echo_memory_bass_off(m, pitch); return; }
    if (velocity > 127) velocity = 127;
    m->bass_manual = 1; m->bass_manual_note = pitch; m->bass_playing = -1;
    echo_voice_on(&m->bass_voice, pitch, velocity);
}
void echo_memory_bass_off(EchoMemory *m, int pitch) {
    if (!m->bass_manual || m->bass_manual_note != pitch) return;
    echo_voice_off(&m->bass_voice, pitch);
    m->bass_manual = 0; m->bass_manual_note = -1;
    while (m->bass_next < m->bass_count && m->bass_onset[m->bass_next] <= m->position) ++m->bass_next;
}
void echo_memory_on(EchoMemory *m, int pitch, int velocity) {
    if (pitch < 0 || pitch > 127) return;
    if (velocity <= 0) { echo_memory_off(m, pitch); return; }
    if (velocity > 127) velocity = 127;
    /* Playing live interrupts replay, retaining the recorded phrase. */
    if (m->mode == ECHO_PLAYING) echo_memory_stop(m);
    if (m->mode == ECHO_ARMED) { m->mode = ECHO_RECORDING; m->position = 0; }
    if (m->mode == ECHO_RECORDING) {
        close_event(m);
        if (m->count == ECHO_MAX_NOTES) {
            m->limit = ECHO_NOTE_LIMIT;
            echo_memory_finish(m);
        } else {
            m->active = (int)m->count;
            EchoEvent *e = &m->events[m->count++];
            e->onset = m->position; e->duration = 0;
            e->pitch = (uint8_t)pitch; e->velocity = (uint8_t)velocity;
        }
    }
    echo_voice_on(&m->voice, pitch, velocity);
}
void echo_memory_off(EchoMemory *m, int pitch) {
    if (m->mode == ECHO_PLAYING) return; /* stale live releases cannot silence replay */
    if (m->mode == ECHO_RECORDING && m->active >= 0 &&
        m->events[m->active].pitch == pitch) {
        close_event(m);
        if (m->count == ECHO_MAX_NOTES) {
            m->limit = ECHO_NOTE_LIMIT;
            echo_memory_finish(m);
        }
    }
    echo_voice_off(&m->voice, pitch);
}
void echo_memory_render(EchoMemory *m, int32_t *out, uint32_t frames) {
    uint32_t offset = 0;
    while (offset < frames) {
        uint32_t n = frames - offset;
        if (m->mode == ECHO_RECORDING) {
            if (m->position == ECHO_MAX_FRAMES) {
                m->limit = ECHO_TIME_LIMIT; echo_memory_finish(m); continue;
            }
            uint32_t remaining = ECHO_MAX_FRAMES - m->position;
            if (n > remaining) n = remaining;
        } else if (m->mode == ECHO_PLAYING) {
            if (!m->bass_manual && m->bass_playing >= 0 &&
                m->position >= m->bass_onset[m->bass_playing] + m->bass_duration[m->bass_playing]) {
                echo_voice_off(&m->bass_voice, m->bass_pitch[m->bass_playing]); m->bass_playing = -1;
            }
            if (!m->bass_manual && m->bass_next < m->bass_count && m->bass_onset[m->bass_next] == m->position) {
                echo_voice_on(&m->bass_voice, m->bass_pitch[m->bass_next], m->bass_velocity[m->bass_next]);
                m->bass_playing = (int)m->bass_next++;
            }
            /* End before onset: adjacent notes and retriggers have unambiguous ownership. */
            if (m->playing >= 0) {
                if (m->position >= playback_onset(m, (uint32_t)m->playing) +
                                   playback_duration(m, (uint32_t)m->playing)) {
                    echo_voice_off(&m->voice, playback_pitch(m, (uint32_t)m->playing)); m->playing = -1;
                }
            }
            if (m->next < playback_count(m) && playback_onset(m, m->next) == m->position) {
                echo_voice_on(&m->voice, playback_pitch(m, m->next), playback_velocity(m, m->next));
                m->playing = (int)m->next++;
            }
            if (m->position >= m->play_length) { echo_memory_stop(m); continue; }
            uint32_t boundary = m->play_length;
            if (m->playing >= 0) {
                uint32_t end = playback_onset(m, (uint32_t)m->playing) +
                               playback_duration(m, (uint32_t)m->playing);
                if (end < boundary) boundary = end;
            }
            if (m->next < playback_count(m) && playback_onset(m, m->next) < boundary)
                boundary = playback_onset(m, m->next);
            if (!m->bass_manual && m->bass_playing >= 0) {
                uint32_t end = m->bass_onset[m->bass_playing] + m->bass_duration[m->bass_playing];
                if (end < boundary) boundary = end;
            }
            if (!m->bass_manual && m->bass_next < m->bass_count && m->bass_onset[m->bass_next] < boundary)
                boundary = m->bass_onset[m->bass_next];
            if (n > boundary - m->position) n = boundary - m->position;
        }
        uint32_t mixed = 0;
        while (mixed < n) {
            uint32_t chunk = n - mixed; if (chunk > 128u) chunk = 128u;
            int32_t bass[256] = {0};
            echo_voice_render(&m->voice, out + 2u * (offset + mixed), chunk);
            echo_voice_render(&m->bass_voice, bass, chunk);
            for (uint32_t i = 0; i < 2u * chunk; ++i) {
                int32_t value = out[2u * (offset + mixed) + i] + bass[i] * m->bass_volume / 100;
                if (value > 8128) value = 8128;
                if (value < -8128) value = -8128;
                out[2u * (offset + mixed) + i] = value;
            }
            mixed += chunk;
        }
        offset += n;
        if (m->mode == ECHO_RECORDING || m->mode == ECHO_PLAYING) m->position += n;
    }
    /* Resolve end boundaries even when they coincide with the end of a render block. */
    if (m->mode == ECHO_RECORDING && m->position == ECHO_MAX_FRAMES) {
        m->limit = ECHO_TIME_LIMIT; echo_memory_finish(m);
    }
    if (m->mode == ECHO_PLAYING && m->position == m->play_length) echo_memory_stop(m);
}
