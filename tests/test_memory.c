/* SPDX-License-Identifier: GPL-3.0-only */
#include "memory.h"
#include <assert.h>
#include <stdio.h>
static void advance(EchoMemory *m, uint32_t frames) {
    while (frames) {
        int32_t out[1024] = {0};
        uint32_t n = frames > 512 ? 512 : frames;
        echo_memory_render(m, out, n); frames -= n;
        for (uint32_t i=0;i<2*n;i++) assert(out[i]>=-8128 && out[i]<=8128);
    }
}
int main(void) {
    EchoMemory m; echo_memory_init(&m);
    assert(!echo_memory_replay(&m));
    echo_memory_capture(&m); advance(&m, ECHO_MAX_FRAMES+1);
    assert(m.mode==ECHO_ARMED && m.count==0); /* waits for first note */
    echo_memory_on(&m,60,100); advance(&m,1000); echo_memory_off(&m,60);
    advance(&m,500); echo_memory_on(&m,64,80); advance(&m,2000); echo_memory_off(&m,64);
    advance(&m,250); echo_memory_finish(&m);
    assert(m.count==2 && m.length==3750);
    assert(m.events[0].onset==0 && m.events[0].duration==1000 && m.events[0].pitch==60);
    assert(m.events[1].onset==1500 && m.events[1].duration==2000 && m.events[1].velocity==80);
    assert(echo_memory_replay(&m));
    advance(&m,1); assert(m.voice.note==60 && m.voice.target==6400);
    advance(&m,1000); assert(m.voice.note==-1);
    advance(&m,500); assert(m.voice.note==64 && m.voice.target==5120);
    echo_memory_off(&m,64); assert(m.voice.note==64); /* stale live release */
    advance(&m,2000); assert(m.voice.note==-1 && m.mode==ECHO_PLAYING);
    advance(&m,249); assert(m.mode==ECHO_IDLE && m.count==2 && m.length==3750);
    /* Replaying twice yields exactly the same samples. */
    int32_t first[8000]={0}, second[8000]={0};
    echo_memory_replay(&m); echo_memory_render(&m,first,4000);
    echo_memory_replay(&m);
    for(int i=0;i<4000;i++) echo_memory_render(&m,second+2*i,1);
    for(int i=0;i<8000;i++) assert(first[i]==second[i]);
    /* Reply retains timing/velocity while mirroring intervals around first note + 7. */
    assert(echo_memory_reply(&m)); assert(m.playback==ECHO_REPLY_PLAYBACK);
    advance(&m,1); assert(m.voice.note==67 && m.voice.target==6400);
    advance(&m,1000); assert(m.voice.note==-1);
    advance(&m,500); assert(m.voice.note==63 && m.voice.target==5120);
    echo_memory_stop(&m);
    /* Same-pitch retrigger and overlapping input close the old event. */
    echo_memory_capture(&m); echo_memory_on(&m,60,100); advance(&m,100);
    echo_memory_on(&m,60,90); advance(&m,100); echo_memory_on(&m,65,90);
    echo_memory_off(&m,60); assert(m.voice.note==65);
    advance(&m,100); echo_memory_finish(&m);
    assert(m.count==3 && m.events[0].duration==100 && m.events[1].onset==100 && m.events[2].duration==100);
    /* Empty and inaudibly short takes have no zero-length events. */
    echo_memory_capture(&m); echo_memory_finish(&m); assert(!m.count && !m.length);
    echo_memory_capture(&m); echo_memory_on(&m,60,100); echo_memory_off(&m,60);
    echo_memory_finish(&m); assert(!m.count && !echo_memory_replay(&m));
    /* A held note is closed precisely at the time cap. */
    echo_memory_capture(&m); echo_memory_on(&m,60,100); advance(&m,ECHO_MAX_FRAMES+1000);
    assert(m.mode==ECHO_IDLE && m.limit==ECHO_TIME_LIMIT);
    assert(m.length==ECHO_MAX_FRAMES && m.events[0].duration==ECHO_MAX_FRAMES);
    /* Capacity: last release finishes, never writes a 33rd event. */
    echo_memory_capture(&m);
    for(uint32_t i=0;i<ECHO_MAX_NOTES;i++) {
        echo_memory_on(&m,60,100); advance(&m,100); echo_memory_off(&m,60);
    }
    assert(m.mode==ECHO_IDLE && m.count==32 && m.limit==ECHO_NOTE_LIMIT);
    echo_memory_on(&m,61,100); assert(m.count==32);
    /* Capacity with overlapping notes also finalizes safely. */
    echo_memory_capture(&m);
    for(uint32_t i=0;i<33;i++){echo_memory_on(&m,60+(int)(i%12),100);advance(&m,10);}
    assert(m.count==32 && m.limit==ECHO_NOTE_LIMIT);
    echo_memory_replay(&m); echo_memory_on(&m,72,100);
    assert(m.mode==ECHO_IDLE && m.voice.note==72 && m.count==32);
    echo_memory_capture(&m); echo_memory_on(&m,60,100); advance(&m,100); echo_memory_stop(&m);
    assert(m.count==1 && m.events[0].duration==100 && m.mode==ECHO_IDLE);
    advance(&m,4096); assert(m.voice.level==0);
    echo_memory_clear(&m); assert(!m.count && !m.length && !echo_memory_replay(&m));
    echo_memory_on(&m,-1,100); echo_memory_on(&m,128,100); assert(m.voice.note==-1);
    puts("PASS memory: exact replay, mirrored reply, timing/velocity, empty/short takes, overlap/retrigger, caps, interrupt/stop/clear");
}
