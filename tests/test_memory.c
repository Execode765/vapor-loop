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
    /* Keep owns a deep copy; later replies cannot overwrite it. */
    assert(!echo_memory_keep(&m,-1) && !echo_memory_keep(&m,3));
    assert(echo_memory_keep(&m,0));
    assert(echo_memory_candidate_mask(&m)==1u && m.candidates[0].count==2);
    uint8_t kept_first=m.candidates[0].pitch[0];
    uint32_t kept_onset=m.candidates[0].onset[1];
    echo_memory_set_phrase(&m,ECHO_PHRASE_TRAP_POCKET,100);
    assert(echo_memory_reply(&m) && m.reply_count==10); echo_memory_stop(&m);
    assert(m.candidates[0].pitch[0]==kept_first && m.candidates[0].onset[1]==kept_onset && m.candidates[0].count==2);
    assert(echo_memory_keep(&m,1) && echo_memory_candidate_mask(&m)==3u && m.candidates[1].count==10);
    assert(echo_memory_play_candidate(&m,0) && m.playback==ECHO_CANDIDATE_PLAYBACK && m.play_length==m.candidates[0].length);
    advance(&m,1); assert(m.voice.note==kept_first); echo_memory_stop(&m);
    echo_memory_clear_candidate(&m,0); assert(echo_memory_candidate_mask(&m)==2u && !echo_memory_play_candidate(&m,0));
    /* Bass Companion is a simultaneous second voice with an exact Straight bypass. */
    echo_memory_set_bass_groove(&m,ECHO_BASS_GROOVE_STRAIGHT,100);
    echo_memory_set_bass(&m,ECHO_BASS_ROOT,ECHO_BASS_QUARTER,1,70);
    assert(echo_memory_play_candidate(&m,1) && m.bass_count>0 && m.bass_pitch[0]==24);
    uint32_t straight_bass_count=m.bass_count;
    for(uint32_t i=0;i<m.bass_count;i++) assert(m.bass_velocity[i]==105);
    advance(&m,1); assert(m.voice.note>=0 && m.bass_voice.note==24 && m.mode==ECHO_PLAYING);
    echo_memory_bass_on(&m,30,110); assert(m.bass_manual && m.bass_voice.note==30 && m.mode==ECHO_PLAYING);
    echo_memory_bass_off(&m,30); assert(!m.bass_manual && m.mode==ECHO_PLAYING); echo_memory_stop(&m);
    /* Auto reads the kept Trap context and introduces space, accents, and gate changes. */
    echo_memory_set_bass_groove(&m,ECHO_BASS_GROOVE_AUTO,60);
    echo_memory_set_bass(&m,ECHO_BASS_FOLLOW,ECHO_BASS_QUARTER,1,80);
    assert(echo_memory_play_candidate(&m,1) && m.bass_count>0);
    assert(m.bass_count<straight_bass_count);
    assert(m.bass_pitch[0]%12==m.candidates[1].root);
    int bass_accent=0, bass_gate_change=0;
    for(uint32_t i=0;i<m.bass_count;i++) {
        int pc=m.bass_pitch[i]%12; assert(pc==0 || pc==4 || pc==7);
        assert(m.bass_pitch[i]>=24 && m.bass_pitch[i]<=35);
        if(m.bass_velocity[i]!=105) bass_accent=1;
        if(i && m.bass_duration[i]!=m.bass_duration[0]) bass_gate_change=1;
    }
    assert(bass_accent && bass_gate_change);
    echo_memory_stop(&m);
    /* Follow anchors to the final transformed Reply on the first generation, not a free-running grid. */
    EchoMemory synced; echo_memory_init(&synced);
    synced.count=8; synced.length=160000;
    for(uint32_t i=0;i<synced.count;i++) {
        synced.events[i].pitch=(uint8_t)(60+(i%5)); synced.events[i].velocity=100;
        synced.events[i].onset=i*20000u; synced.events[i].duration=9000;
    }
    echo_memory_set_phrase(&synced,ECHO_PHRASE_TRAP_POCKET,60);
    echo_memory_set_groove(&synced,77,ECHO_GROOVE_SWING,80);
    echo_memory_set_bass_groove(&synced,ECHO_BASS_GROOVE_STRAIGHT,100);
    echo_memory_set_bass(&synced,ECHO_BASS_FOLLOW,ECHO_BASS_HALF,3,70);
    for(int generation=0;generation<2;generation++) {
        assert(echo_memory_reply(&synced) && synced.bass_count>=2);
        int moved_off_grid=0;
        uint32_t spacing=5292000u/77u;
        for(uint32_t i=0;i<synced.bass_count;i++) {
            int matches_reply=0;
            for(uint32_t j=0;j<synced.reply_count;j++)
                if(synced.bass_onset[i]==synced.reply_onset[j]) matches_reply=1;
            assert(matches_reply);
            if(synced.bass_onset[i]!=i*spacing) moved_off_grid=1;
            if(i) assert(synced.bass_onset[i]>synced.bass_onset[i-1]);
        }
        assert(moved_off_grid && synced.bass_pitch[0]%12==synced.root);
        echo_memory_stop(&synced);
    }
    echo_memory_set_bass(&m,ECHO_BASS_MANUAL,ECHO_BASS_HALF,1,65);
    echo_memory_bass_on(&m,28,100); assert(m.mode==ECHO_IDLE && m.bass_voice.note==28);
    advance(&m,64); echo_memory_bass_off(&m,28); advance(&m,4096); assert(m.bass_voice.level==0);
    echo_memory_set_bass(&m,99,99,99,999);
    assert(m.bass_mode==ECHO_BASS_OFF && m.bass_density==ECHO_BASS_HALF && m.bass_octave==3 && m.bass_volume==100);
    echo_memory_set_bass_groove(&m,99,999);
    assert(m.bass_groove_style==ECHO_BASS_GROOVE_AUTO && m.bass_groove_amount==100);
    echo_memory_set_bass(&m,ECHO_BASS_OFF,ECHO_BASS_HALF,1,70);
    echo_memory_set_phrase(&m,ECHO_PHRASE_ORIGINAL,0);
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
    /* Bass-groove profiles remain ordered, audible, bounded, and bypassable. */
    for(int style=0;style<ECHO_BASS_GROOVE_COUNT;style++) for(int amount=0;amount<=100;amount+=10) {
        uint8_t bp[8],bv[8]; uint32_t bo[8],bd[8],bc=8;
        for(int i=0;i<8;i++){bp[i]=(uint8_t)(24+i%3);bv[i]=105;bo[i]=(uint32_t)i*1000u;bd[i]=800;}
        echo_bass_groove_apply(bp,bv,bo,bd,&bc,8000,1000,style,amount);
        assert(bc>0 && bc<=8);
        for(uint32_t i=0;i<bc;i++) {
            assert(bo[i]<8000 && bd[i]>0 && bo[i]+bd[i]<=8000 && bv[i]>0 && bv[i]<=127);
            if(i) assert(bo[i]>bo[i-1]);
        }
        if(style==ECHO_BASS_GROOVE_STRAIGHT || amount==0) {
            assert(bc==8); for(int i=0;i<8;i++) assert(bo[i]==(uint32_t)i*1000u && bd[i]==800 && bv[i]==105);
        }
    }
    /* Short phrases retain both opportunities; groove shapes rather than erases the answer. */
    for(int style=ECHO_BASS_GROOVE_TRAP;style<ECHO_BASS_GROOVE_COUNT;style++) {
        uint8_t bp[2]={24,31},bv[2]={105,105}; uint32_t bo[2]={0,1000},bd[2]={800,800},bc=2;
        echo_bass_groove_apply(bp,bv,bo,bd,&bc,2000,1000,style,100);
        assert(bc==2 && bo[0]==0 && bo[1]>bo[0] && bd[0] && bd[1]);
    }
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
    /* Capacity: last release finishes, never writes a 129th event. */
    echo_memory_capture(&m);
    for(uint32_t i=0;i<ECHO_MAX_NOTES;i++) {
        echo_memory_on(&m,60,100); advance(&m,100); echo_memory_off(&m,60);
    }
    assert(m.mode==ECHO_IDLE && m.count==128 && m.limit==ECHO_NOTE_LIMIT);
    echo_memory_on(&m,61,100); assert(m.count==128);
    /* Capacity with overlapping notes also finalizes safely. */
    echo_memory_capture(&m);
    for(uint32_t i=0;i<129;i++){echo_memory_on(&m,60+(int)(i%12),100);advance(&m,10);}
    assert(m.count==128 && m.limit==ECHO_NOTE_LIMIT);
    echo_memory_replay(&m); echo_memory_on(&m,72,100);
    assert(m.mode==ECHO_IDLE && m.voice.note==72 && m.count==128);
    echo_memory_capture(&m); echo_memory_on(&m,60,100); advance(&m,100); echo_memory_stop(&m);
    assert(m.count==1 && m.events[0].duration==100 && m.mode==ECHO_IDLE);
    advance(&m,4096); assert(m.voice.level==0);
    echo_memory_clear(&m); assert(!m.count && !m.length && !echo_memory_replay(&m));
    assert(echo_memory_candidate_mask(&m)==2u); /* phrase Clear does not erase kept work */
    echo_memory_clear_candidate(&m,1); assert(!echo_memory_candidate_mask(&m));
    echo_memory_on(&m,-1,100); echo_memory_on(&m,128,100); assert(m.voice.note==-1);
    /* Tempo-aware capture owns beat one, preserves an opening rest, and corrects to sixteenths. */
    EchoMemory clock; echo_memory_init(&clock);
    echo_memory_set_groove(&clock,120,ECHO_GROOVE_ORIGINAL,0);
    echo_memory_set_capture_clock(&clock,2,2,ECHO_QUANTIZE_TIGHT,1);
    echo_memory_set_rhythm_reference(&clock,1,55);
    echo_memory_capture(&clock);
    assert(clock.mode==ECHO_COUNT_IN && clock.tempo_beat==22050 && clock.count_in_length==176400);
    assert(echo_memory_clock_bar(&clock)==1 && echo_memory_clock_beat(&clock)==1);
    int32_t click[512]={0}; echo_memory_render(&clock,click,256);
    int heard_click=0; for(int i=0;i<512;i++) if(click[i]) heard_click=1; assert(heard_click);
    advance(&clock,clock.count_in_length-256); assert(clock.mode==ECHO_RECORDING && clock.position==0);
    int32_t drums[512]={0}; echo_memory_render(&clock,drums,256);
    int heard_drums=0; for(int i=0;i<512;i++) if(drums[i]) heard_drums=1; assert(heard_drums);
    assert(echo_memory_clock_bar(&clock)==1 && echo_memory_clock_beat(&clock)==1);
    advance(&clock,2744); echo_memory_on(&clock,60,100); advance(&clock,2000); echo_memory_off(&clock,60);
    advance(&clock,5000); echo_memory_on(&clock,64,90); advance(&clock,2500); echo_memory_off(&clock,64);
    advance(&clock,clock.capture_length-clock.position);
    assert(clock.mode==ECHO_IDLE && clock.limit==ECHO_BAR_COMPLETE && clock.length==176400 && clock.count==2);
    assert(clock.events[0].onset==5512 && clock.events[1].onset==11024);
    assert(clock.events[0].duration>0 && clock.events[1].duration>0);
    echo_memory_set_capture_clock(&clock,3,3,99,0);
    assert(clock.capture_bars==2 && clock.count_in_bars==2 &&
           clock.capture_quantize==ECHO_QUANTIZE_OFF && !clock.metronome_enabled);
    echo_memory_set_groove(&clock,240,ECHO_GROOVE_ORIGINAL,0);
    echo_memory_set_capture_clock(&clock,16,1,ECHO_QUANTIZE_LIGHT,0);
    echo_memory_capture(&clock);
    assert(clock.count_in_length==44100 && clock.capture_length==705600);
    echo_memory_finish(&clock);
    echo_memory_set_rhythm_reference(&clock,0,999);
    assert(!clock.rhythm_enabled && clock.rhythm_volume==100);
    /* v0.9.0: one 96-PPQN timeline owns a manual bass pass and manual drum lane. */
    EchoMemory seq; echo_memory_init(&seq);
    echo_memory_set_groove(&seq,120,ECHO_GROOVE_ORIGINAL,0);
    echo_memory_set_capture_clock(&seq,1,1,ECHO_QUANTIZE_TIGHT,1);
    echo_memory_set_sequence_resolution(&seq,ECHO_RES_SIXTEENTH);
    assert(seq.sequencer.length_ticks==384 && echo_seq_tick_to_frame(96,120)==22050);
    echo_memory_capture(&seq); advance(&seq,seq.count_in_length);
    echo_memory_on(&seq,60,100); advance(&seq,11025); echo_memory_off(&seq,60);
    advance(&seq,seq.capture_length-seq.position);
    assert(seq.mode==ECHO_IDLE && seq.count==1 && seq.length==88200);
    echo_memory_set_bass(&seq,ECHO_BASS_MANUAL,ECHO_BASS_HALF,1,75);
    echo_memory_set_input_track(&seq,ECHO_TRACK_BASS);
    assert(echo_memory_record_track(&seq,ECHO_TRACK_BASS));
    assert(seq.mode==ECHO_COUNT_IN && seq.track_record_pending);
    advance(&seq,seq.count_in_length);
    assert(seq.mode==ECHO_PLAYING && seq.sequencer.recording);
    echo_memory_bass_on(&seq,24,110); advance(&seq,11025); echo_memory_bass_off(&seq,24);
    advance(&seq,11025); echo_memory_bass_on(&seq,31,96); advance(&seq,11025); echo_memory_bass_off(&seq,31);
    advance(&seq,seq.play_length-seq.position);
    assert(seq.mode==ECHO_IDLE && !seq.sequencer.recording);
    assert(echo_seq_track_count(&seq.sequencer,ECHO_TRACK_BASS)==2);
    assert(seq.sequencer.events[0].tick==0 && seq.sequencer.events[0].duration==48);
    assert(seq.sequencer.events[1].tick==96 && seq.sequencer.events[1].value==31);
    echo_memory_set_drum_mode(&seq,ECHO_TRACK_MANUAL,60);
    echo_memory_toggle_drum(&seq,0,ECHO_DRUM_KICK);
    echo_memory_toggle_drum(&seq,2,ECHO_DRUM_HAT);
    echo_memory_toggle_drum(&seq,4,ECHO_DRUM_SNARE);
    assert(echo_seq_drum_mask(&seq.sequencer,0)==1u);
    assert(echo_seq_drum_mask(&seq.sequencer,2)==4u);
    assert(echo_memory_replay(&seq) && seq.bass_count==2);
    int32_t sequence_audio[512]={0}; echo_memory_render(&seq,sequence_audio,256);
    int heard_sequence=0; for(int i=0;i<512;i++) if(sequence_audio[i]) heard_sequence=1;
    assert(heard_sequence && seq.bass_voice.note==24 && seq.drums.kick_env>0);
    echo_memory_stop(&seq);
    echo_memory_set_drum_mode(&seq,99,999);
    assert(seq.drum_mode==ECHO_TRACK_OFF && seq.rhythm_volume==100);
    puts("PASS memory: 0.9.0 track/sequencer foundation plus all 0.8.1 regressions");
}

