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
    uint32_t go[3]={0,7000,15000}, gd[3]={3000,4000,5000};
    uint32_t ro[3]={0}, rd[3]={0}, gl=0;
    echo_groove_apply(go,gd,ro,rd,3,22000,120,ECHO_GROOVE_ORIGINAL,100,&gl);
    for(int i=0;i<3;i++) assert(ro[i]==go[i] && rd[i]==gd[i]);
    assert(gl==22000);
    echo_groove_apply(go,gd,ro,rd,3,22000,120,ECHO_GROOVE_PULSE,100,&gl);
    assert(ro[0]==0 && ro[1]==5512 && ro[2]==16536 && rd[0]==4134);
    uint32_t swing_o[3]={0,5512,11024};
    echo_groove_apply(swing_o,gd,ro,rd,3,16000,120,ECHO_GROOVE_SWING,100,&gl);
    assert(ro[1]==7349 && ro[0]<ro[1] && ro[1]<ro[2]);
    echo_groove_apply(go,gd,ro,rd,3,22000,120,ECHO_GROOVE_HALF_TIME,100,&gl);
    assert(ro[1]==14000 && ro[2]==30000 && rd[2]==10000 && gl==44000);
    for(int style=0;style<ECHO_GROOVE_COUNT;style++) for(int amount=0;amount<=100;amount+=10) {
        echo_groove_apply(go,gd,ro,rd,3,22000,97,style,amount,&gl);
        assert(ro[0]<ro[1] && ro[1]<ro[2] && rd[0] && rd[1] && rd[2]);
        assert(gl<=ECHO_MAX_REPLY_FRAMES);
    }
    echo_memory_set_groove(&m,96,ECHO_GROOVE_POCKET,70);
    assert(m.bpm==96 && m.groove_style==ECHO_GROOVE_POCKET && m.groove_amount==70 && m.variation==0);
    /* Phrase architecture changes event count, rests, accents, and repetition. */
    uint8_t pp[32]={0}, pv[32]={0}, sv[5]={100,90,80,90,100};
    uint32_t po[32]={0}, pd[32]={0}, pc=0, pl=0;
    uint32_t so[5]={0,1000,2000,3000,4000}, sd[5]={500,500,500,500,500};
    echo_phrase_apply(expected,sv,so,sd,5,5000,pp,pv,po,pd,&pc,&pl,120,ECHO_PHRASE_ORIGINAL,100,0);
    assert(pc==5 && pl==5000);
    for(int i=0;i<5;i++) assert(pp[i]==expected[i] && pv[i]==sv[i] && po[i]==so[i] && pd[i]==sd[i]);
    echo_phrase_apply(expected,sv,so,sd,5,5000,pp,pv,po,pd,&pc,&pl,120,ECHO_PHRASE_TRAP_POCKET,100,0);
    assert(pc==10 && po[0]==0 && po[1]>po[0] && po[9]<pl && pp[4]==pp[5]);
    int has_short=0, has_rest=0;
    for(uint32_t i=0;i<pc;i++) { assert(pd[i]>0 && pv[i]>0); if(pd[i]<5000) has_short=1; if(i && po[i]>po[i-1]+pd[i-1]) has_rest=1; }
    assert(has_short && has_rest);
    echo_phrase_apply(expected,sv,so,sd,5,5000,pp,pv,po,pd,&pc,&pl,120,ECHO_PHRASE_RNB_FLOW,100,1);
    assert(pc==8 && pd[1]>pd[0]);
    echo_phrase_apply(expected,sv,so,sd,5,5000,pp,pv,po,pd,&pc,&pl,120,ECHO_PHRASE_TECHNO_MOTION,100,2);
    assert(pc==10 && pp[0]==expected[2] && pp[5]==expected[2]);
    for(int style=0;style<ECHO_PHRASE_COUNT;style++) for(int amount=0;amount<=100;amount+=10) {
        echo_phrase_apply(expected,sv,so,sd,5,5000,pp,pv,po,pd,&pc,&pl,73,style,amount,4);
        assert(pc>0 && pc<=32 && pl<=ECHO_MAX_REPLY_FRAMES);
        for(uint32_t i=0;i<pc;i++) { assert(pd[i]>0 && pv[i]>0 && (!i || po[i]>po[i-1])); }
    }
    echo_memory_set_phrase(&m,ECHO_PHRASE_TRAP_POCKET,100);
    assert(m.phrase_style==ECHO_PHRASE_TRAP_POCKET && m.phrase_amount==100 && m.variation==0);
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
    puts("PASS memory/harmony/phrase/groove: independent reply count, rests, motifs, ratchets, accents, Adventure, timing and lifecycle");
}
