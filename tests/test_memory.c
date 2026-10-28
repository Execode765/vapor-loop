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
    /* Reply retains timing/velocity while moving in C-major scale degrees. */
    assert(echo_memory_reply(&m)); assert(m.playback==ECHO_REPLY_PLAYBACK);
    assert(m.reply[0]==67 && m.reply[1]==64);
    advance(&m,1); assert(m.voice.note==67 && m.voice.target==6400);
    advance(&m,1000); assert(m.voice.note==-1);
    advance(&m,500); assert(m.voice.note==64 && m.voice.target==5120);
    echo_memory_stop(&m);
    uint8_t source[5]={60,62,64,62,60}, reply[5]={0};
    echo_harmony_reply(source,reply,5,0,ECHO_SCALE_MAJOR,0,0);
    const uint8_t expected[5]={67,65,64,65,67};
    for(int i=0;i<5;i++) assert(reply[i]==expected[i]);
    uint8_t faithful[5]={0}, bold_a[5]={0}, bold_b[5]={0};
    echo_harmony_reply(source,faithful,5,0,ECHO_SCALE_MAJOR,0,99);
    echo_harmony_reply(source,bold_a,5,0,ECHO_SCALE_MAJOR,100,0);
    echo_harmony_reply(source,bold_b,5,0,ECHO_SCALE_MAJOR,100,1);
    int changed_a=0, changed_b=0, variants_differ=0;
    for(int i=0;i<5;i++) {
        assert(faithful[i]==expected[i]);
        assert(echo_harmony_contains(bold_a[i],0,ECHO_SCALE_MAJOR));
        assert(echo_harmony_contains(bold_b[i],0,ECHO_SCALE_MAJOR));
        if(bold_a[i]!=expected[i]) changed_a=1;
        if(bold_b[i]!=expected[i]) changed_b=1;
        if(bold_a[i]!=bold_b[i]) variants_differ=1;
    }
    assert(bold_a[0]==expected[0] && bold_b[0]==expected[0]);
    int cadence_a=bold_a[4]%12, cadence_b=bold_b[4]%12;
    assert((cadence_a==0 || cadence_a==4 || cadence_a==7) &&
           (cadence_b==0 || cadence_b==4 || cadence_b==7));
    assert(changed_a && changed_b && variants_differ);
    /* Every root/scale combination stays in scale and in the reply register. */
    uint8_t chromatic[32], constrained[32];
    for(int i=0;i<32;i++) chromatic[i]=(uint8_t)(36+(i*7)%61);
    for(int root=0;root<12;root++) for(int scale=0;scale<ECHO_SCALE_COUNT;scale++) {
        echo_harmony_reply(chromatic,constrained,32,root,scale,100,7);
        for(int i=0;i<32;i++) {
            assert(constrained[i]>=36 && constrained[i]<=96);
            assert(echo_harmony_contains(constrained[i],root,scale));
        }
    }
    echo_memory_set_harmony(&m,2,ECHO_SCALE_NATURAL_MINOR);
    assert(m.root==2 && m.scale==ECHO_SCALE_NATURAL_MINOR);
    echo_memory_set_adventure(&m,80); assert(m.adventure==80 && m.variation==0);
    echo_memory_set_adventure(&m,200); assert(m.adventure==100);
    echo_memory_set_adventure(&m,-1); assert(m.adventure==0);
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
    puts("PASS memory/harmony: faithful baseline, distinct Adventure variants, 48 key/scale contexts, protected anchor/cadence/register, timing/velocity and lifecycle");
}
