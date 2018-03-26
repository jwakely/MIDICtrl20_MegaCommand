#include "SeqRlckPage.h"
#include "MCL.h"

void SeqRlckPage::setup() { SeqPage::setup(); }

void SeqRlckPage::init() {
  md_exploit.off();
  note_interface.state = false;

  ((MCLEncoder*)encoders[0])->max = 4;
  ((MCLEncoder*)encoders[1])->max = 64;
  ((MCLEncoder*)encoders[2])->max = 64;
  ((MCLEncoder*)encoders[3])->max = 11;
  encoders[2]->cur = mcl_seq.md_tracks[last_md_track].length;

  curpage = SEQ_RTRK_PAGE;
  midi_events.setup_callbacks();
}
void SeqRlckPage::cleanup() { midi_events.remove_callbacks(); }
void SeqRlckPage::display() {
  GUI.setLine(GUI.LINE1);
  GUI.put_value_at1(15, page_select + 1);

  GUI.put_string_at(0, "RLCK");

  const char *str1 = getMachineNameShort(MD.kit.models[grid_page.cur_col], 1);
  const char *str2 = getMachineNameShort(MD.kit.models[grid_page.cur_col], 2);
  if (grid_page.cur_col < 16) {
    GUI.put_p_string_at(9, str1);
    GUI.put_p_string_at(11, str2);
    GUI.put_value_at(5, encoders[2]->getValue());
  } else {
    GUI.put_value_at(5, (encoders[2]->getValue() /
                         (2 / mcl_seq.ext_tracks[last_ext_track].resolution)));
    if (Analog4.connected) {
      GUI.put_string_at(9, "A4T");
    } else {
      GUI.put_string_at(9, "MID");
    }
    GUI.put_value_at1(12, grid_page.cur_col - 16 + 1);
  }

  draw_lock_mask(page_select * 16);
}
bool SeqRlckPage::handleEvent(gui_event_t *event) {

  if (note_interface.is_event(event)) {
    return true;
  }

  if (EVENT_RELEASED(event, Buttons.BUTTON1)) {
    md_exploit.on();
    note_interface.state = false;
    curpage = SEQ_RTRK_PAGE;
    GUI.setPage(&seq_rtrk_page);
    return true;
  }

  if ((EVENT_PRESSED(event, Buttons.BUTTON3) && BUTTON_DOWN(Buttons.BUTTON4)) ||
      (EVENT_PRESSED(event, Buttons.BUTTON4) && BUTTON_DOWN(Buttons.BUTTON3))) {

    for (uint8_t n = 0; n < mcl_seq.num_md_tracks; n++) {
      mcl_seq.md_tracks[n].clear_seq_locks();
    }
    return true;
  }

  if (EVENT_RELEASED(event, Buttons.BUTTON4)) {
    mcl_seq.md_tracks[grid_page.cur_col].clear_seq_locks();
    return true;
  }

  if (SeqPage::handleEvent(event)) {
    return true;
  }

  return false;
}

void SeqRlckPageMidiEvents::onControlChangeCallback_Midi(uint8_t *msg) {
  uint8_t channel = MIDI_VOICE_CHANNEL(msg[0]);
  uint8_t param = msg[1];
  uint8_t value = msg[2];
  uint8_t track;
  uint8_t track_param;
  uint8_t param_true = 0;
  if (param >= 16) {
    param_true = 1;
  }
  if (param < 63) {
    param = param - 16;
    track = (param / 24) + (channel - MD.global.baseChannel) * 4;
    track_param = param - ((param / 24) * 24);
  } else if (param >= 72) {
    param = param - 72;
    track = (param / 24) + 2 + (channel - MD.global.baseChannel) * 4;
    track_param = param - ((param / 24) * 24);
  }

  if (param_true) {

    MD.kit.params[track][track_param] = value;
  }
  grid_page.cur_col = track;
  last_md_track = track;
  seq_rlck_page.encoders[2]->cur = mcl_seq.md_tracks[grid_page.cur_col].length;
  mcl_seq.md_tracks[track].record_track_locks(track_param, value);
}

void SeqRlckPageMidiEvents::onControlChangeCallback_Midi2(uint8_t *msg) {}

void SeqRlckPageMidiEvents::setup_callbacks() {
  if (state) {
    return;
  }
  Midi.addOnControlChangeCallback(
      this,
      (midi_callback_ptr_t)&SeqRlckPageMidiEvents::onControlChangeCallback_Midi);
  Midi2.addOnControlChangeCallback(this,
                                   (midi_callback_ptr_t)&SeqRlckPageMidiEvents::
                                       onControlChangeCallback_Midi2);

  state = true;
}

void SeqRlckPageMidiEvents::remove_callbacks() {

  if (!state) {
    return;
  }
  Midi.removeOnControlChangeCallback(
      this,
      (midi_callback_ptr_t)&SeqRlckPageMidiEvents::onControlChangeCallback_Midi);
  Midi2.removeOnControlChangeCallback(
      this, (midi_callback_ptr_t)&SeqRlckPageMidiEvents::
                onControlChangeCallback_Midi2);

  state = false;
}