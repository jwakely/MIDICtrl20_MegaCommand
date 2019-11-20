#include "MixerPage.h"
#include "MCL.h"

#define FADER_LEN 16
#define FADE_RATE 16

void MixerPage::set_display_mode(uint8_t param) {
  display_mode = param;
  switch (param) {
  case MODEL_FLTF:
    strcpy(info_line2, "FLTF");
    break;
  case MODEL_FLTW:
    strcpy(info_line2, "FLTW");
    break;
  case MODEL_FLTQ:
    strcpy(info_line2, "FLTQ");
    break;
  case MODEL_LEVEL:
  default:
    display_mode = MODEL_LEVEL;
    strcpy(info_line2, "VOLUME");
    break;
  }
}

#ifdef OLED_DISPLAY
static void oled_draw_routing() {
  for (int i = 0; i < 16; ++i) {
    // draw routing
    if (note_interface.notes[i] > 0) {

      oled_display.fillRect(0 + i * 8, 2, 6, 6, WHITE);
    }

    else if (mcl_cfg.routing[i] == 6) {

      oled_display.fillRect(0 + i * 8, 2, 6, 6, BLACK);
      oled_display.drawRect(0 + i * 8, 2, 6, 6, WHITE);

    }

    else {

      oled_display.fillRect(0 + i * 8, 2, 6, 6, BLACK);
      oled_display.drawLine(+i * 8, 5, 5 + (i * 8), 5, WHITE);
    }
  }
}

#endif

void MixerPage::setup() {
  encoders[0]->handler = encoder_level_handle;
  encoders[1]->handler = encoder_filtf_handle;
  encoders[2]->handler = encoder_filtw_handle;
  encoders[3]->handler = encoder_filtq_handle;
  if (route_page.encoders[0]->cur == 0) {
    route_page.encoders[0]->cur = 2;
  }
  create_chars_mixer();
#ifdef OLED_DISPLAY
  classic_display = false;
  oled_display.clearDisplay();
#endif
}

void MixerPage::init() {
  level_pressmode = 0;
  encoders[0]->cur = 60;
  encoders[1]->cur = 60;
  encoders[2]->cur = 60;
  encoders[3]->cur = 60;
  bool switch_tracks = false;
  note_interface.ni_active = true;
  midi_events.setup_callbacks();
  for (uint8_t i = 0; i < 16; i++) {
    for (uint8_t c = 0; c < 3; c++) {
      params[i][c] = MD.kit.params[i][MODEL_FLTF + c];
    }
  }

#ifdef OLED_DISPLAY
  oled_display.clearDisplay();
  oled_draw_routing();
  set_display_mode(MODEL_LEVEL);
  initializing = true;
  for (uint8_t i = 0; i < 16; i++) {
    uint8_t scaled_level =
        (uint8_t)(((float)MD.kit.levels[i] / (float)127) * (float)FADER_LEN);

    oled_display.drawRect(0 + i * 8, 12 + (FADER_LEN - scaled_level), 6,
                          scaled_level + 1, WHITE);
    disp_levels[i] = 0;
  }
#endif
}

void MixerPage::cleanup() {
//  md_exploit.off();
#ifdef OLED_DISPLAY
  oled_display.clearDisplay();
#endif
  note_interface.ni_active = false;

  midi_events.remove_callbacks();
}

void MixerPage::set_level(int curtrack, int value) {
  // in_sysex = 1;
  MD.kit.levels[curtrack] = value;
  USE_LOCK();
  SET_LOCK();
  MD.setTrackParam(curtrack, 33, value);
  CLEAR_LOCK();
  // in_sysex = 0;
}

void MixerPage::loop() {}

void MixerPage::draw_levels() {}

void encoder_level_handle(Encoder *enc) {

  bool redraw_frame = (mixer_page.display_mode != MODEL_LEVEL);

  mixer_page.set_display_mode(MODEL_LEVEL);

  int dir = enc->getValue() - enc->old;
  int track_newval;

  for (int i = 0; i < 16; i++) {
    if (note_interface.notes[i] == 1) {
      track_newval = MD.kit.levels[i] + dir;
      if (track_newval < 0) {
        track_newval = 0;
      }
      if (track_newval > 127) {
        track_newval = 127;
      }
      for (uint8_t level = MD.kit.levels[i]; level < track_newval; level++) {
        mixer_page.set_level(i, level);
      }
      for (uint8_t level = MD.kit.levels[i]; level > track_newval; level--) {
        mixer_page.set_level(i, level);
      }
      // if ((MD.kit.levels[i] < 127) && (MD.kit.levels[i] > 0)) {
      mixer_page.set_level(i, track_newval);
    }

    if (note_interface.notes[i] == 1 || redraw_frame) {
#ifdef OLED_DISPLAY
      uint8_t scaled_level = (MD.kit.levels[i] / 127.0f) * FADER_LEN;

      oled_display.fillRect(0 + i * 8, 12, 6, FADER_LEN, BLACK);
      oled_display.drawRect(0 + i * 8, 12 + (FADER_LEN - scaled_level), 6,
                            scaled_level + 1, WHITE);
#endif
    }
  }
  enc->cur = 64 + dir;
  enc->old = 64;
}

void encoder_filtf_handle(Encoder *enc) {
  mixer_page.adjust_param(enc, MODEL_FLTF);
}

void encoder_filtw_handle(Encoder *enc) {
  mixer_page.adjust_param(enc, MODEL_FLTW);
}

void encoder_filtq_handle(Encoder *enc) {
  mixer_page.adjust_param(enc, MODEL_FLTQ);
}

void encoder_lastparam_handle(Encoder *enc) {
  mixer_page.adjust_param(enc, MD.midi_events.last_md_param);
}

void MixerPage::adjust_param(Encoder *enc, uint8_t param) {

  if (initializing) {
    if (param == MODEL_FLTQ) {
      initializing = false;
    }
  } else {
    set_display_mode(param);
  }

  int dir = enc->getValue() - enc->old;
  int newval;

  for (int i = 0; i < 16; i++) {
    if (note_interface.notes[i] == 1) {
      newval = MD.kit.params[i][param] + dir;
      if (newval < 0) {
        newval = 0;
      }
      if (newval > 127) {
        newval = 127;
      }
      for (uint8_t value = MD.kit.params[i][param]; value < newval; value++) {
        USE_LOCK();
        SET_LOCK();
        MD.setTrackParam(i, param, value);
        MD.kit.params[i][param] = value;
        CLEAR_LOCK();
      }
      for (uint8_t value = MD.kit.params[i][param]; value > newval; value--) {
        USE_LOCK();
        SET_LOCK();
        MD.setTrackParam(i, param, value);
        MD.kit.params[i][param] = value;
        CLEAR_LOCK();
      }
      USE_LOCK();
      SET_LOCK();
      MD.setTrackParam(i, param, newval);
      MD.kit.params[i][param] = newval;
      CLEAR_LOCK();

#ifdef OLED_DISPLAY
      uint8_t scaled_level = (newval / 127.0f) * FADER_LEN;

      oled_display.fillRect(0 + i * 8, 12, 6, FADER_LEN, BLACK);
      oled_display.fillRect(0 + i * 8, 12 + (FADER_LEN - scaled_level), 6,
                            scaled_level + 1, WHITE);
#endif
    }
  }
  enc->cur = 64 + dir;
  enc->old = 64;
}

#ifndef OLED_DISPLAY
void MixerPage::display() {
  note_interface.draw_notes(0);
  if (!classic_display) {
    LCD.goLine(0);
    LCD.puts(GUI.lines[0].data);
  }
  GUI.setLine(GUI.LINE2);
  uint8_t scaled_level;
  uint8_t scaled_level2;
  char str[17] = "                ";
  for (int i = 0; i < 16; i++) {
    str[i] = (char)219;

    if (mcl_cfg.routing[i] == 6) {

      str[i] = (char)'-';
    }
    if (note_interface.notes[i] > 0) {

      str[i] = (char)255;
    }
  }
  GUI.put_string_at(0, str);

  uint8_t dec = MidiClock.get_tempo() / FADE_RATE;
  for (uint8_t n = 0; n < 16; n++) {
    if (disp_levels[n] < dec) {
      disp_levels[n] = 0;
    } else {
      disp_levels[n] -= dec;
    }
  }
}
#else

void MixerPage::display() {

  auto oldfont = oled_display.getFont();

  uint8_t fader_level;
  uint8_t meter_level;
  uint8_t fader_x = 0;
  for (int i = 0; i < 16; i++) {

    if (display_mode == MODEL_LEVEL) {
      fader_level = MD.kit.levels[i];
    } else {
      fader_level = MD.kit.params[i][display_mode];
    }

    fader_level = (fader_level / 127.0f) * FADER_LEN + 1;
    meter_level = (disp_levels[i] / 127.0f) * FADER_LEN + 1;

    if (display_mode == MODEL_LEVEL) {

      if (note_interface.notes[i] == 1) {
        oled_display.fillRect(fader_x, 13 + (FADER_LEN - fader_level), 6,
                              fader_level, WHITE);
      } else {

        // draw meter only if not pressed
        oled_display.fillRect(fader_x + 1, 14 + (FADER_LEN - fader_level), 4,
                              FADER_LEN - meter_level, BLACK);
        oled_display.fillRect(fader_x + 1, 13 + (FADER_LEN - meter_level), 4,
                              meter_level, WHITE);
      }
    } else {
      meter_level = min(fader_level, meter_level);
      oled_display.fillRect(0 + i * 8, 12, 6, FADER_LEN, BLACK);
      oled_display.fillRect(fader_x, 13 + (FADER_LEN - fader_level), 6,
                            fader_level, WHITE);
      oled_display.fillRect(fader_x + 1, 14 + (FADER_LEN - meter_level), 4,
                            meter_level - 2, BLACK);
    }

    fader_x += 8;
  }

  uint8_t dec = MidiClock.get_tempo() / FADE_RATE;
  for (uint8_t n = 0; n < 16; n++) {
    if (disp_levels[n] < dec) {
      disp_levels[n] = 0;
    } else {
      disp_levels[n] -= dec;
    }
  }

  oled_display.display();
  oled_display.setFont(oldfont);
}
#endif

bool MixerPage::handleEvent(gui_event_t *event) {
  if (note_interface.is_event(event)) {
    uint8_t mask = event->mask;
    uint8_t port = event->port;
    uint8_t device = midi_active_peering.get_device(port);

    uint8_t track = event->source - 128;

    if (track > 16) {
      return false;
    }
    if (event->mask == EVENT_BUTTON_PRESSED) {
#ifdef OLED_DISPLAY

      if (note_interface.notes[track] > 0) {

        oled_display.fillRect(0 + track * 8, 2, 6, 6, WHITE);
      }

#endif

      return true;
    }

    if (event->mask == EVENT_BUTTON_RELEASED) {
#ifndef OLED_DISPLAY
      note_interface.draw_notes(0);
#else
      uint8_t i = track;
      uint8_t scaled_level = (MD.kit.levels[i] / 127.0f) * FADER_LEN;
      oled_display.fillRect(0 + i * 8, 12, 6, FADER_LEN, BLACK);
      oled_display.drawRect(0 + i * 8, 12 + (FADER_LEN - scaled_level), 6,
                            scaled_level + 1, WHITE);

#endif

      if (note_interface.notes_all_off_md()) {
        if (BUTTON_DOWN(Buttons.BUTTON4)) {
          route_page.toggle_routes_batch();
        }
        if (BUTTON_DOWN(Buttons.BUTTON1)) {
          route_page.toggle_routes_batch(true);
        }
        note_interface.init_notes();
#ifdef OLED_DISPLAY
        oled_draw_routing();
#endif
      }
      return true;
    }
  }
  /*
    if (EVENT_PRESSED(event, Buttons.BUTTON4)) {
            route_page.toggle_routes_batch();
          note_interface.init_notes();
  #ifdef OLED_DISPLAY
         route_page.draw_routes(0);
  #endif
    }
  */
  if (EVENT_PRESSED(event, Buttons.BUTTON2)) {
    route_page.update_globals();
    md_exploit.off();
    md_exploit.on();
    GUI.setPage(&page_select_page);
    return true;
  }
  if (EVENT_PRESSED(event, Buttons.BUTTON3)) {
    for (uint8_t i = 0; i < 16; i++) {
      if (note_interface.notes[i] == 1) {
        for (uint8_t c = 0; c < 3; c++) {
          if (MD.kit.params[i][MODEL_FLTF + c] != params[i][c]) {
            USE_LOCK();
            SET_LOCK();
            MD.setTrackParam(i, MODEL_FLTF + c, params[i][c]);
            MD.kit.params[i][MODEL_FLTF + c] = params[i][c];
            CLEAR_LOCK();
          }
        }
      }
    }
    return true;
  }

  if (EVENT_PRESSED(event, Buttons.ENCODER1) ||
      EVENT_PRESSED(event, Buttons.ENCODER2) ||
      EVENT_PRESSED(event, Buttons.ENCODER3) ||
      EVENT_PRESSED(event, Buttons.ENCODER4)) {
    if (note_interface.notes_count() == 0) {
      route_page.update_globals();
      GUI.setPage(&grid_page);
    }
    return true;
  }

  return false;
}

void MixerMidiEvents::setup_callbacks() {
  if (state) {
    return;
  }
  Midi.addOnNoteOnCallback(
      this, (midi_callback_ptr_t)&MixerMidiEvents::onNoteOnCallback_Midi);
  Midi.addOnNoteOffCallback(
      this, (midi_callback_ptr_t)&MixerMidiEvents::onNoteOffCallback_Midi);

  state = true;
}

void MixerMidiEvents::remove_callbacks() {
  if (!state) {
    return;
  }

  DEBUG_PRINTLN("remove calblacks");
  Midi.removeOnNoteOnCallback(
      this, (midi_callback_ptr_t)&MixerMidiEvents::onNoteOnCallback_Midi);
  Midi.removeOnNoteOffCallback(
      this, (midi_callback_ptr_t)&MixerMidiEvents::onNoteOffCallback_Midi);

  state = false;
}
uint8_t MixerMidiEvents::note_to_trig(uint8_t note_num) {
  uint8_t trig_num = 0;
  for (uint8_t i = 0; i < sizeof(MD.global.drumMapping); i++) {
    if (note_num == MD.global.drumMapping[i]) {
      trig_num = i;
    }
  }
  return trig_num;
}
void MixerMidiEvents::onNoteOnCallback_Midi(uint8_t *msg) {
  uint8_t note_num = msg[1];
  uint8_t channel = MIDI_VOICE_CHANNEL(msg[0]);

  uint8_t n = note_to_trig(msg[1]);
  if (msg[0] != 153) {
    mixer_page.disp_levels[n] = MD.kit.levels[n];
  }
}
void MixerMidiEvents::onNoteOffCallback_Midi(uint8_t *msg) {}
