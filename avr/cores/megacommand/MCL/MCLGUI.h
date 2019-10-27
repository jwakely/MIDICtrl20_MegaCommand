/* Copyright Justin Mammarella jmamma@gmail.com 2018 */

#ifndef MCLGUI_H__
#define MCLGUI_H__

#include "TextInputPage.h"
#include "QuestionDialogPage.h"

class MCLGUI {
public:
  // fills dst buffer with input text. ensures that:
  // 1. dst is null-terminated
  // 2. dst has no trailing spaces 
  bool wait_for_input(char *dst, const char *title, uint8_t len);
  void draw_vertical_dashline(uint8_t x, uint8_t from = 1, uint8_t to = 32);
  void draw_horizontal_dashline(uint8_t y, uint8_t from, uint8_t to);
  bool wait_for_confirm(const char *title, const char* text);
  void draw_infobox(const char* line1, const char* line2, const int line2_offset = 0);
  void draw_vertical_separator(uint8_t x);
  void draw_vertical_scrollbar(uint8_t x, uint8_t n_items, uint8_t n_window, uint8_t n_current);
  ///  Clear the content area of a popup
  void clear_popup();
  void draw_popup(const char* title, bool deferred_display = false);
  void draw_progress(const char* msg, uint8_t cur, uint8_t _max, bool deferred_display = false);

  void clear_leftpane();
  void clear_rightpane();

  void draw_encoder(uint8_t x, uint8_t y, uint8_t value);
  void draw_encoder(uint8_t x, uint8_t y, Encoder *encoder);

  bool show_encoder_value(Encoder *encoder);

  void draw_md_encoder(uint8_t x, uint8_t y, Encoder *encoder, const char *name);
  void draw_md_encoder(uint8_t x, uint8_t y, uint8_t value, const char *name, bool show_value);
  void draw_light_encoder(uint8_t x, uint8_t y, Encoder *encoder, const char *name);
  void draw_light_encoder(uint8_t x, uint8_t y, uint8_t value, const char *name, bool show_value);
  void draw_keyboard(uint8_t x, uint8_t y, uint8_t note_width, uint8_t note_height, uint8_t num_of_notes, uint64_t note_mask);

  static constexpr uint8_t s_menu_w = 96;
  static constexpr uint8_t s_menu_h = 24;
  static constexpr uint8_t s_menu_x = (128 - s_menu_w) / 2;
  static constexpr uint8_t s_menu_y = (32 - s_menu_h) / 2;
  static constexpr uint8_t s_title_x = 31;
  static constexpr uint8_t s_title_w = 64;

  static constexpr uint8_t s_rightpane_offset_x = 43;
  static constexpr uint8_t s_rightpane_offset_y = 8;

  static constexpr auto info_y1 = 2;
  static constexpr auto info_y2 = 27;
  static constexpr auto info_x1 = 12;
  static constexpr auto info_x2 = 124;
  static constexpr auto circle_x = info_x1 + 10;
  static constexpr auto circle_y = info_y1 + 15;

  static constexpr auto info_w = info_x2 - info_x1 + 1;
  static constexpr auto info_h = info_y2 - info_y1 + 1;

};

extern MCLGUI mcl_gui;
/*
// 'encoder_medium_1', 15x15px
const unsigned char encoder_medium_0 [] PROGMEM = {
	0x07, 0xc0, 0x18, 0x30, 0x20, 0x08, 0x40, 0x04, 0x40, 0x04, 0x80, 0x02, 0x80, 0x02, 0x80, 0x02,
	0x80, 0x02, 0x80, 0x02, 0x40, 0x04, 0x43, 0x84, 0x23, 0x88, 0x18, 0x30, 0x07, 0xc0
};
// 'encoder_medium_2', 15x15px
const unsigned char encoder_medium_1 [] PROGMEM = {
	0x07, 0xc0, 0x18, 0x30, 0x20, 0x08, 0x40, 0x04, 0x40, 0x04, 0x80, 0x02, 0x80, 0x02, 0x80, 0x02,
	0x80, 0x02, 0x80, 0x02, 0x40, 0x04, 0x47, 0x04, 0x23, 0x08, 0x18, 0x30, 0x07, 0xc0
};
// 'encoder_medium_3', 15x15px
const unsigned char encoder_medium_2 [] PROGMEM = {
	0x07, 0xc0, 0x18, 0x30, 0x20, 0x08, 0x40, 0x04, 0x40, 0x04, 0x80, 0x02, 0x80, 0x02, 0x80, 0x02,
	0x80, 0x02, 0x80, 0x02, 0x48, 0x04, 0x46, 0x04, 0x26, 0x08, 0x18, 0x30, 0x07, 0xc0
};
// 'encoder_medium_4', 15x15px
const unsigned char encoder_medium_3 [] PROGMEM = {
	0x07, 0xc0, 0x18, 0x30, 0x20, 0x08, 0x40, 0x04, 0x40, 0x04, 0x80, 0x02, 0x80, 0x02, 0x80, 0x02,
	0x80, 0x02, 0x90, 0x02, 0x58, 0x04, 0x4c, 0x04, 0x20, 0x08, 0x18, 0x30, 0x07, 0xc0
};
// 'encoder_medium_5', 15x15px
const unsigned char encoder_medium_4 [] PROGMEM = {
	0x07, 0xc0, 0x18, 0x30, 0x20, 0x08, 0x40, 0x04, 0x40, 0x04, 0x80, 0x02, 0x80, 0x02, 0x80, 0x02,
	0xb0, 0x02, 0xb0, 0x02, 0x50, 0x04, 0x40, 0x04, 0x20, 0x08, 0x18, 0x30, 0x07, 0xc0
};
// 'encoder_medium_6', 15x15px
const unsigned char encoder_medium_5 [] PROGMEM = {
	0x07, 0xc0, 0x18, 0x30, 0x20, 0x08, 0x40, 0x04, 0x40, 0x04, 0x80, 0x02, 0x80, 0x02, 0xa0, 0x02,
	0xb0, 0x02, 0xb0, 0x02, 0x40, 0x04, 0x40, 0x04, 0x20, 0x08, 0x18, 0x30, 0x07, 0xc0
};
*/
// 'encoder0', 11x11px
const unsigned char encoder_small_0 [] PROGMEM = {
	0x0e, 0x00, 0x31, 0x80, 0x40, 0x40, 0x40, 0x40, 0x80, 0x20, 0x80, 0x20, 0x80, 0x20, 0x4e, 0x40,
	0x4e, 0x40, 0x31, 0x80, 0x0e, 0x00
};
// 'encoder1', 11x11px
const unsigned char encoder_small_1 [] PROGMEM = {
	0x0e, 0x00, 0x31, 0x80, 0x40, 0x40, 0x40, 0x40, 0x80, 0x20, 0x80, 0x20, 0x80, 0x20, 0x5c, 0x40,
	0x4c, 0x40, 0x31, 0x80, 0x0e, 0x00
};
// 'encoder2', 11x11px
const unsigned char encoder_small_2 [] PROGMEM = {
	0x0e, 0x00, 0x31, 0x80, 0x40, 0x40, 0x40, 0x40, 0x80, 0x20, 0x80, 0x20, 0x90, 0x20, 0x58, 0x40,
	0x48, 0x40, 0x31, 0x80, 0x0e, 0x00
};
// 'encoder3', 11x11px
const unsigned char encoder_small_3 [] PROGMEM = {
	0x0e, 0x00, 0x31, 0x80, 0x40, 0x40, 0x40, 0x40, 0x80, 0x20, 0x80, 0x20, 0xb0, 0x20, 0x58, 0x40,
	0x40, 0x40, 0x31, 0x80, 0x0e, 0x00
};
// 'encoder4', 11x11px
const unsigned char encoder_small_4 [] PROGMEM = {
	0x0e, 0x00, 0x31, 0x80, 0x40, 0x40, 0x40, 0x40, 0x80, 0x20, 0xb0, 0x20, 0xb0, 0x20, 0x58, 0x40,
	0x40, 0x40, 0x31, 0x80, 0x0e, 0x00
};
// 'encoder5', 11x11px
const unsigned char encoder_small_5 [] PROGMEM = {
	0x0e, 0x00, 0x31, 0x80, 0x40, 0x40, 0x40, 0x40, 0x80, 0x20, 0xb0, 0x20, 0xb0, 0x20, 0x50, 0x40,
	0x40, 0x40, 0x31, 0x80, 0x0e, 0x00
};
// 'encoder6', 11x11px
const unsigned char encoder_small_6 [] PROGMEM = {
	0x0e, 0x00, 0x31, 0x80, 0x40, 0x40, 0x40, 0x40, 0xb0, 0x20, 0xb0, 0x20, 0xb0, 0x20, 0x40, 0x40, 
	0x40, 0x40, 0x31, 0x80, 0x0e, 0x00
};


#endif /* MCLGUI_H__ */
