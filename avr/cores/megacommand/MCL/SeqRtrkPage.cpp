#include "MCL.h"
#include "SeqRtrkPage.h"

void SeqRtrkPage::setup() { SeqPage::setup(); }

void SeqRtrkPage::init() {
  md_exploit.on();

  note_interface.state = false;

  ((MCLEncoder *)encoders[0])->max = 4;
  ((MCLEncoder *)encoders[1])->max = 64;
  ((MCLEncoder *)encoders[2])->max = 64;
  ((MCLEncoder *)encoders[3])->max = 11;
  encoders[2]->cur = mcl_seq.md_tracks[last_md_track].length;

  curpage = SEQ_RTRK_PAGE;
}

void SeqRtrkPage::display() {

  GUI.setLine(GUI.LINE1);
  GUI.put_value_at1(15, page_select + 1);

  GUI.put_string_at(0, "RTRK");

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

  draw_pattern_mask(page_select * 16, DEVICE_MD);
}
bool SeqRtrkPage::handleEvent(gui_event_t *event) {

  if (note_interface.is_event(event)) {
    uint8_t mask = event->mask;
    uint8_t port = event->port;
    uint8_t device = midi_active_peering.get_device(port);

    uint8_t track = event->source - 128;

    if (event->mask == EVENT_BUTTON_PRESSED) {
      grid_page.cur_col = track;
      last_md_track = track;

      encoders[2]->cur = mcl_seq.md_tracks[last_md_track].length;
      MD.triggerTrack(track, 127);
    if ((MidiClock.state == 2)) {
        mcl_seq.md_tracks[last_md_track].record_track(track, 127);

        return true;

    }
    }
    if (event->mask == EVENT_BUTTON_RELEASED) {
    }
  }
  if (EVENT_RELEASED(event, Buttons.BUTTON1)) {
    md_exploit.off();
    GUI.setPage(&seq_rlck_page);
    return true;
  }

  if (EVENT_PRESSED(event, Buttons.ENCODER2)) {
    md_exploit.off();
    GUI.setPage(&grid_page);
    return true;
  }

  if ((EVENT_PRESSED(event, Buttons.BUTTON3) && BUTTON_DOWN(Buttons.BUTTON4)) ||
      (EVENT_PRESSED(event, Buttons.BUTTON4) && BUTTON_DOWN(Buttons.BUTTON3))) {

    for (uint8_t n = 0; n < 16; n++) {
      mcl_seq.md_tracks[n].clear_seq_track();
    }
    return true;
  }

  if (EVENT_RELEASED(event, Buttons.BUTTON4)) {
    if (grid_page.cur_col < 16) {
      mcl_seq.md_tracks[grid_page.cur_col].clear_seq_track();
    } else {
      mcl_seq.ext_tracks[grid_page.cur_col].clear_track();
    }
    return true;
  }

  if (SeqPage::handleEvent(event)) {
    return true;
  }

  return false;
}