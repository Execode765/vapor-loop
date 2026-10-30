// SPDX-License-Identifier: GPL-3.0-only
// The FM-1 in an AudioWorklet: firmware.wasm is your code on platform/web.c. Each render quantum
// asks it for 128 frames, which runs the device's clock forward (loop() once per millisecond);
// about 30 times a second the screen, the LEDs, MIDI out, print() and the settings go to the page.

class FM1 extends AudioWorkletProcessor {
  constructor() {
    super()
    this.ex = null
    this.port.onmessage = (e) => this.onMessage(e.data)
  }

  async onMessage(m) {
    if (m.type === 'load') {
      this.ex = null
      try {
        // This build has no imports. Fail explicitly if an unexpected dependency appears.
        const { instance } = await WebAssembly.instantiate(m.wasm, {})
        const ex = instance.exports
        this.mem = ex.memory
        if (m.store) {
          new Uint8Array(this.mem.buffer, ex.web_store(), m.store.length).set(m.store)
          ex.web_store_set_len(m.store.length)
        }
        ex.web_master(m.master)
        this.sentFb = -1
        this.sentStore = ex.web_store_gen()
        this.last = 0
        this.midi = []
        ex.web_boot()
        this.ex = ex
        this.port.postMessage({ type: 'ready' })
      } catch (err) {
        this.crash(err)
      }
      return
    }
    const ex = this.ex
    if (!ex) return
    if (m.type === 'keys') ex.web_keys(m.mask >>> 0)
    else if (m.type === 'buttons') ex.web_buttons(m.mask >>> 0)
    else if (m.type === 'encoder') ex.web_encoder(m.id, m.n | 0)
    else if (m.type === 'master') ex.web_master(m.value | 0)
    else if (m.type === 'midi') ex.web_midi_in(m.data[0], m.data[1] || 0, m.data[2] || 0)
    else if (m.type === 'harmony' && ex.echo_set_harmony) ex.echo_set_harmony(m.root | 0, m.scale | 0)
    else if (m.type === 'adventure' && ex.echo_set_adventure) ex.echo_set_adventure(m.value | 0)
    else if (m.type === 'groove' && ex.echo_set_groove) ex.echo_set_groove(m.bpm | 0, m.style | 0, m.amount | 0)
    else if (m.type === 'phrase' && ex.echo_set_phrase) ex.echo_set_phrase(m.style | 0, m.amount | 0)
    else if (m.type === 'capture-clock' && ex.echo_set_capture_clock) ex.echo_set_capture_clock(m.bars | 0, m.countInBars | 0, m.quantize | 0, m.metronome | 0)
    else if (m.type === 'rhythm-reference' && ex.echo_set_rhythm_reference) ex.echo_set_rhythm_reference(m.enabled | 0, m.volume | 0)
    else if (m.type === 'drum-mode' && ex.echo_set_drum_mode) ex.echo_set_drum_mode(m.mode | 0, m.volume | 0)
    else if (m.type === 'drum-toggle' && ex.echo_toggle_drum) ex.echo_toggle_drum(m.step | 0, m.voice | 0)
    else if (m.type === 'input-track' && ex.echo_set_input_track) ex.echo_set_input_track(m.track | 0)
    else if (m.type === 'track-record' && ex.echo_record_track) ex.echo_record_track(m.track | 0)
    else if (m.type === 'sequence-resolution' && ex.echo_set_sequence_resolution) ex.echo_set_sequence_resolution(m.resolution | 0)
    else if (m.type === 'keep' && ex.echo_keep) ex.echo_keep(m.slot | 0)
    else if (m.type === 'candidate-play' && ex.echo_play_candidate) ex.echo_play_candidate(m.slot | 0)
    else if (m.type === 'candidate-clear' && ex.echo_clear_candidate) ex.echo_clear_candidate(m.slot | 0)
    else if (m.type === 'bass' && ex.echo_set_bass) ex.echo_set_bass(m.mode | 0, m.density | 0, m.octave | 0, m.volume | 0)
    else if (m.type === 'bass-groove' && ex.echo_set_bass_groove) ex.echo_set_bass_groove(m.style | 0, m.amount | 0)
    else if (m.type === 'bass-on' && ex.echo_bass_on) ex.echo_bass_on(m.pitch | 0, m.velocity | 0)
    else if (m.type === 'bass-off' && ex.echo_bass_off) ex.echo_bass_off(m.pitch | 0)
  }

  crash(err) {
    this.ex = null
    this.port.postMessage({ type: 'crash', message: String((err && err.message) || err) })
  }

  publish() {
    const ex = this.ex
    const msg = { type: 'frame', ms: ex.web_now(), keys: ex.web_led_keys() >>> 0, buttons: ex.web_led_buttons() >>> 0, backlight: ex.web_led_backlight ? ex.web_led_backlight() : 0 }
    if (ex.echo_mode) msg.phrase = {mode:ex.echo_mode(),count:ex.echo_count(),replyCount:ex.echo_reply_count?ex.echo_reply_count():ex.echo_count(),length:ex.echo_length(),playLength:ex.echo_play_length?ex.echo_play_length():ex.echo_length(),position:ex.echo_position(),limit:ex.echo_limit(),playback:ex.echo_playback?ex.echo_playback():0,root:ex.echo_root?ex.echo_root():0,scale:ex.echo_scale?ex.echo_scale():0,adventure:ex.echo_adventure?ex.echo_adventure():0,variation:ex.echo_variation?ex.echo_variation():0,bpm:ex.echo_bpm?ex.echo_bpm():120,grooveStyle:ex.echo_groove_style?ex.echo_groove_style():0,grooveAmount:ex.echo_groove_amount?ex.echo_groove_amount():0,phraseStyle:ex.echo_phrase_style?ex.echo_phrase_style():0,phraseAmount:ex.echo_phrase_amount?ex.echo_phrase_amount():0,candidateMask:ex.echo_candidate_mask?ex.echo_candidate_mask():0,candidateSlot:ex.echo_candidate_slot?ex.echo_candidate_slot():0,lastReplyValid:ex.echo_last_reply_valid?ex.echo_last_reply_valid():0,candidateCounts:ex.echo_candidate_count?[ex.echo_candidate_count(0),ex.echo_candidate_count(1),ex.echo_candidate_count(2)]:[0,0,0],bassMode:ex.echo_bass_mode?ex.echo_bass_mode():0,bassDensity:ex.echo_bass_density?ex.echo_bass_density():1,bassOctave:ex.echo_bass_octave?ex.echo_bass_octave():1,bassVolume:ex.echo_bass_volume?ex.echo_bass_volume():70,bassGrooveStyle:ex.echo_bass_groove_style?ex.echo_bass_groove_style():1,bassGrooveAmount:ex.echo_bass_groove_amount?ex.echo_bass_groove_amount():55,bassCount:ex.echo_bass_count?ex.echo_bass_count():0,manualBassCount:ex.echo_manual_bass_count?ex.echo_manual_bass_count():0,trackRecording:ex.echo_track_recording?ex.echo_track_recording():0,inputTrack:ex.echo_input_track?ex.echo_input_track():0,sequenceResolution:ex.echo_sequence_resolution?ex.echo_sequence_resolution():16,drumMode:ex.echo_drum_mode?ex.echo_drum_mode():0,captureBars:ex.echo_capture_bars?ex.echo_capture_bars():0,countInBars:ex.echo_count_in_bars?ex.echo_count_in_bars():1,captureQuantize:ex.echo_capture_quantize?ex.echo_capture_quantize():0,metronomeEnabled:ex.echo_metronome_enabled?ex.echo_metronome_enabled():0,rhythmEnabled:ex.echo_rhythm_enabled?ex.echo_rhythm_enabled():0,rhythmVolume:ex.echo_rhythm_volume?ex.echo_rhythm_volume():55,captureLength:ex.echo_capture_length?ex.echo_capture_length():0,tempoBeat:ex.echo_tempo_beat?ex.echo_tempo_beat():0,clockBar:ex.echo_clock_bar?ex.echo_clock_bar():0,clockBeat:ex.echo_clock_beat?ex.echo_clock_beat():0}
    const transfer = []
    const gen = ex.web_fb_gen()
    if (gen !== this.sentFb) {
      this.sentFb = gen
      msg.fb = new Uint16Array(this.mem.buffer, ex.web_fb(), 240 * 240).slice()
      transfer.push(msg.fb.buffer)
    }
    const sg = ex.web_store_gen()
    if (sg !== this.sentStore) {
      this.sentStore = sg
      msg.store = new Uint8Array(this.mem.buffer, ex.web_store(), ex.web_store_len()).slice()
    }
    const p = ex.web_log()
    const bytes = new Uint8Array(this.mem.buffer, p)
    let n = 0
    while (bytes[n]) n++
    if (n) msg.log = String.fromCharCode(...bytes.subarray(0, n)) // (no TextDecoder in a worklet)
    if (this.midi.length) {
      msg.midi = this.midi
      this.midi = []
    }
    this.port.postMessage(msg, transfer)
  }

  process(inputs, outputs) {
    const ex = this.ex
    if (!ex) return true
    const out = outputs[0]
    const n = out[0].length
    try {
      ex.web_render(n)
      for (let m; (m = ex.web_midi_out()) >= 0;) this.midi.push(m)
    } catch (err) {
      this.crash(err)
      return true
    }
    out[0].set(new Float32Array(this.mem.buffer, ex.web_out_l(), n))
    if (out[1]) out[1].set(new Float32Array(this.mem.buffer, ex.web_out_r(), n))
    if (currentTime - this.last >= 1 / 30) {
      this.last = currentTime
      this.publish()
    }
    return true
  }
}

registerProcessor('fm1', FM1)


