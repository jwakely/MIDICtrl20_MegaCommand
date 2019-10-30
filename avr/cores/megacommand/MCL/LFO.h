/* Copyright Justin Mammarella jmamma@gmail.com 2018 */

#ifndef LFOOSC_H__
#define LFOOSC_H__

#include "MCL.h"
#include "Math.h"

#define EXP_LFO 1
#define LFO_LENGTH 48

class LFO {
public:
  uint8_t amplitude;
  virtual uint8_t get_sample(uint8_t sample_number);
};

class ExpLFO : public LFO {

  float time_constant;
public:
  ExpLFO(float time_constant_ = 20) { time_constant = 1.00 / time_constant_; }
  uint8_t get_sample(uint8_t sample_number);
};


class IExpLFO : public LFO {

  float time_constant;
public:
  IExpLFO(float time_constant_ = 20) { time_constant = 1.00 / time_constant_; }
  uint8_t get_sample(uint8_t sample_number);
};

class RampLFO : public LFO {

public:
  RampLFO() { }
  uint8_t get_sample(uint8_t sample_number);
};

class IRampLFO : public LFO {

public:
  IRampLFO() { }
  uint8_t get_sample(uint8_t sample_number);
};


class TriLFO : public LFO {

public:
  TriLFO() { }
  uint8_t get_sample(uint8_t sample_number);
};

class SinLFO : public LFO {

public:
  SinLFO() { }
  uint8_t get_sample(uint8_t sample_number);
};


#endif /* LFOOSC_H__ */
