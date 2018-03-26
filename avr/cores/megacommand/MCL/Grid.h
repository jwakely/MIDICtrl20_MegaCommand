/* Copyright Justin Mammarella jmamma@gmail.com 2018 */

#ifndef GRID_H__
#define GRID_H__

#include "A4Track.h"
#include "GridPages.h"

#define GRID_LENGTH 130
#define GRID_WIDTH 22
#define GRID_SLOT_BYTES 4096

#define A4_TRACK_TYPE 2
#define MD_TRACK_TYPE 1
#define EXT_TRACK_TYPE 3
#define EMPTY_TRACK_TYPE 0

class Grid {
public:
  float row_name_offset = 0;
  char row_name[17] = "                ";
  uint8_t get_slot_model(int column, int row, bool load, A4Track *track_buf);

  void setup();
  bool clear_slot(uint16_t i);
  bool clear_row(uint16_t row);
  char *get_slot_kit(int column, int row, bool load, bool scroll);
};
extern Grid grid;

#endif /* GRID_H__ */