/* SPDX-License-Identifier: GPL-3.0-only */
#include "memory.h"
void echo_memory_init(EchoMemory *m) {
    echo_voice_init(&m->voice);
    m->count = m->length = m->position = m->next = 0;
    m->mode = ECHO_IDLE; m->active = m->playing = -1; m->limit = ECHO_NO_LIMIT;
    m->playback = ECHO_NO_PLAYBACK;
    m->root = 0; m->scale = ECHO_SCALE_MAJOR; m->adventure = 0; m->variation = 0;
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
}
void echo_memory_clear(EchoMemory *m) {
    echo_memory_stop(m);
    m->count = m->length = m->position = m->next = 0;
    m->active = -1; m->limit = ECHO_NO_LIMIT;
}
void echo_memory_capture(EchoMemory *m) {
    echo_memory_clear(m); m->variation = 0; echo_voice_init(&m->voice); m->mode = ECHO_ARMED;
}
int echo_memory_replay(EchoMemory *m) {
    echo_memory_stop(m);
    if (!m->count || !m->length) return 0;
    echo_voice_init(&m->voice);
    m->mode = ECHO_PLAYING; m->position = m->next = 0; m->playing = -1;
    m->playback = ECHO_EXACT_PLAYBACK;
    return 1;
}
int echo_memory_reply(EchoMemory *m) {
    echo_memory_stop(m);
    if (!m->count || !m->length) return 0;
    uint8_t source[ECHO_MAX_NOTES];
    for (uint32_t i = 0; i < m->count; ++i) source[i] = m->events[i].pitch;
    echo_harmony_reply(source, m->reply, m->count, m->root, m->scale,
                       m->adventure, m->variation++);
    echo_voice_init(&m->voice);
    m->mode = ECHO_PLAYING; m->position = m->next = 0; m->playing = -1;
    m->playback = ECHO_REPLY_PLAYBACK;
    return 1;
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
static int playback_pitch(const EchoMemory *m, const EchoEvent *e) {
    if (m->playback != ECHO_REPLY_PLAYBACK) return e->pitch;
    return m->reply[e - m->events];
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
            /* End before onset: adjacent notes and retriggers have unambiguous ownership. */
            if (m->playing >= 0) {
                EchoEvent *e = &m->events[m->playing];
                if (m->position >= e->onset + e->duration) {
                    echo_voice_off(&m->voice, playback_pitch(m, e)); m->playing = -1;
                }
            }
            if (m->next < m->count && m->events[m->next].onset == m->position) {
                EchoEvent *e = &m->events[m->next];
                echo_voice_on(&m->voice, playback_pitch(m, e), e->velocity);
                m->playing = (int)m->next++;
            }
            if (m->position >= m->length) { echo_memory_stop(m); continue; }
            uint32_t boundary = m->length;
            if (m->playing >= 0) {
                EchoEvent *e = &m->events[m->playing];
                uint32_t end = e->onset + e->duration;
                if (end < boundary) boundary = end;
            }
            if (m->next < m->count && m->events[m->next].onset < boundary)
                boundary = m->events[m->next].onset;
            if (n > boundary - m->position) n = boundary - m->position;
        }
        echo_voice_render(&m->voice, out + 2u * offset, n);
        offset += n;
        if (m->mode == ECHO_RECORDING || m->mode == ECHO_PLAYING) m->position += n;
    }
    /* Resolve end boundaries even when they coincide with the end of a render block. */
    if (m->mode == ECHO_RECORDING && m->position == ECHO_MAX_FRAMES) {
        m->limit = ECHO_TIME_LIMIT; echo_memory_finish(m);
    }
    if (m->mode == ECHO_PLAYING && m->position == m->length) echo_memory_stop(m);
}
