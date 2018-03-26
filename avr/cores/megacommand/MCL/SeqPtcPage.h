/* Justin Mammarella jmamma@gmail.com 2018 */

#ifndef SEQPTCPAGE_H__
#define SEQPTCPAGE_H__

#include "SeqPage.h"
#include "Scales.h"
extern scale_t *scales[16];
class SeqPtcMidiEvents : public MidiCallback {
public:
  bool state;

  void setup_callbacks();
  void remove_callbacks();

  void onNoteOnCallback_Midi2(uint8_t *msg);
  void onNoteOffCallback_Midi2(uint8_t *msg);
};

class SeqPtcPage : public SeqPage {

public:
  uint8_t poly_count = 0;
  uint8_t poly_max = 1;

  bool record_mode = false;
  SeqPtcMidiEvents midi_events;
  SeqPtcPage(Encoder *e1 = NULL, Encoder *e2 = NULL, Encoder *e3 = NULL,
             Encoder *e4 = NULL)
      : SeqPage(e1, e2, e3, e4) {}
  bool handleEvent(gui_event_t *event);
  void pattern_len_handler(Encoder *enc);
  uint8_t seq_ext_pitch(uint8_t note_num);
  virtual void display();
  uint8_t get_machine_pitch(uint8_t track, uint8_t pitch);
  uint8_t get_next_track();
  uint8_t calc_pitch(uint8_t note_num);
  void setup();
  void cleanup();
  void init();
};

#endif /* SEQPTCPAGE_H__ */