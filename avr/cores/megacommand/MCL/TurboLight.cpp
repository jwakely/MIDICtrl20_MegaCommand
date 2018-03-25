#include "TurboLight.h"

uint8_t TurboLight::lookup_speed(uint8_t speed) {
  switch (speed) {
    case 0:
      return 1;

    case 1:
      return 2;

    case 2:
      return 4;

    case 3:
      return 7;
  }
}

void TurboLight::send_header(uint8_t cmd, MidiUartClass *MidiUart_) {
  for (uint8_t x = 0 ; x  < 6; x++) {
    MidiUart_->m_putc_immediate(turbomidi_sysex_header[x]);

  }

  MidiUart_->m_putc_immediate(cmd);
}

void TurboLight::set_speed(uint8_t speed, uint8_t port) {
  MidiUartClass *MidiUart_;

  if (port == 1) {
    MidiUart_ = (MidiUartClass*) &MidiUart;
  }
  else {
    MidiUart_ = (MidiUartClass*) &MidiUart2;
  }

  if (MidiUart_->speed == tmSpeeds[speed]) {
    return;
  }

  //USE_LOCK();
  // SET_LOCK();

  sendturbomidiHeader(0x20, MidiUart_);
  MidiUart_->m_putc_immediate(speed);

  MidiUart_->m_putc_immediate(0xF7);
  if (port == 1) {
    while (!IS_BIT_SET8(UCSR1A, UDRE1));
  }
  else {
    while (!IS_BIT_SET8(UCSR2A, UDRE2));
  }
  delay(50);
  MidiUart.setSpeed(tmSpeeds[speed ], port);
  //delay(50);
  MidiUart_->setActiveSenseTimer(150);
  //  MidiUart_->m_putc_immediate(0xF8);
  //MidiUart_->m_putc_immediate(0xFE);

  // MidiUart_->sendActiveSenseTimer = 10;
  //   CLEAR_LOCK();

}



TurboLight turbo_light;
