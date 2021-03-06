#!/bin/bash
AVR_DIR=$(cd "../../../../tools/avr"; pwd)
DEV=/dev/$(ls /dev | grep usb | head -n 1)
make -j8
if [ $? -eq 0 ]; then
  ${AVR_DIR}/bin/avrdude -C${AVR_DIR}/etc/avrdude.conf -v -V -patmega2560 -cwiring -P${DEV} -b115200 -D -Uflash:w:./main.hex
fi
