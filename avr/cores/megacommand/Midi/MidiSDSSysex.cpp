#include "WProgram.h"

#include "MCLSd.h"
#include "Math.h"
#include "MidiSDSMessages.hh"
#include "MidiSDSSysex.hh"
#include "helpers.h"
MidiSDSSysexListenerClass MidiSDSSysexListener;

void MidiSDSSysexListenerClass::start() {

  msgType = 255;
  isSDSMessage = false;
}

void MidiSDSSysexListenerClass::handleByte(uint8_t byte) {}

#define ELEKTRON_ID 0x3C
#define MD_ID 0x02
#define MD_SDS_NAME 0x73

void MidiSDSSysexListenerClass::end() {
  if (MidiSysex.data[0] == 0x7E) {
    isSDSMessage = true;
  } else {
    isSDSMessage = false;
    return;
  }

  msgType = MidiSysex.data[2];
  switch (msgType) {

  case MIDI_SDS_DUMPHEADER:

    if (midi_sds.state != SDS_READY) {
      DEBUG_PRINTLN("header received, not in ready");
      return;
    }
    midi_sds.state = SDS_REC;

    dump_header();

    break;

  case MIDI_SDS_DATAPACKET:
    if (midi_sds.state != SDS_REC) {
      DEBUG_PRINTLN("not in sds rec mode");
      midi_sds.sendCancelMessage();
      midi_sds.cancel();
      return;
    }
    data_packet();

    break;
  }
}
void MidiSDSSysexListenerClass::end_immediate() {

  if ((MidiSysex.data[2] == ELEKTRON_ID) && (MidiSysex.data[3] == MD_ID) &&
      (MidiSysex.data[5] == MD_SDS_NAME)) {
    sds_slot = MidiSysex.data[6];
    sds_name[0] = MidiSysex.data[7];
    sds_name[1] = MidiSysex.data[8];
    sds_name[2] = MidiSysex.data[9];
    sds_name[3] = MidiSysex.data[10];
    sds_name_rec = true;
    DEBUG_PRINTLN("sample name received");

    return;
  }

  if (MidiSysex.data[0] == 0x7E) {
    isSDSMessage = true;
  } else {
    isSDSMessage = false;
    return;
  }

  msgType = MidiSysex.data[2];

  switch (msgType) {
  case MIDI_SDS_DUMPREQUEST:
    dump_request();
    break;

  case MIDI_SDS_ACK:
    ack();
    break;

  case MIDI_SDS_NAK:
    nak();
    break;

  case MIDI_SDS_CANCEL:
    cancel();
    break;

  case MIDI_SDS_WAIT:

    wait();
    break;

  case MIDI_SDS_EOF:
    eof();
    break;
  }
}
void MidiSDSSysexListenerClass::wait() {}
void MidiSDSSysexListenerClass::eof() {}

void MidiSDSSysexListenerClass::cancel() {}

void MidiSDSSysexListenerClass::nak() {}

void MidiSDSSysexListenerClass::ack() {}
void MidiSDSSysexListenerClass::dump_request() {}

void MidiSDSSysexListenerClass::dump_header() {
  sds_name_rec = false;
  uint8_t i = 3;

  midi_sds.sampleNumber = MidiSysex.data[i++];
  midi_sds.sampleNumber |= ((uint32_t)MidiSysex.data[i++] << 7);

  midi_sds.sampleFormat = MidiSysex.data[i++];

  midi_sds.samplePeriod = MidiSysex.data[i++];
  midi_sds.samplePeriod |= ((uint32_t)MidiSysex.data[i++] << 7);
  midi_sds.samplePeriod |= ((uint32_t)MidiSysex.data[i++] << 14);

  // SampleLength in words;
  DEBUG_PRINTLN(MidiSysex.data[i]);
  midi_sds.sampleLength = (uint32_t)(MidiSysex.data[i++]);
  midi_sds.sampleLength |= ((uint32_t)MidiSysex.data[i++] << 7);
  midi_sds.sampleLength |= ((uint32_t)MidiSysex.data[i++] << 14);

  DEBUG_PRINTLN(midi_sds.sampleLength);
  uint32_t sampleRate = 1000000000 / midi_sds.samplePeriod + 1;

  DEBUG_PRINTLN("MidiSDS DumpHeader Received");
  DEBUG_PRINTLN(midi_sds.sampleNumber);
  DEBUG_PRINTLN(midi_sds.sampleFormat);
  DEBUG_PRINTLN(midi_sds.samplePeriod);
  DEBUG_PRINTLN(sampleRate);
  char my_string[16] = "___.wav";

  my_string[0] = (midi_sds.sampleNumber % 1000) / 100 + '0';
  my_string[1] = (midi_sds.sampleNumber % 100) / 10 + '0';
  my_string[2] = (midi_sds.sampleNumber % 10) + '0';
  if ((midi_sds.sampleFormat != 8) && (midi_sds.sampleFormat != 16) &&
      (midi_sds.sampleFormat != 24)) {
    midi_sds.sendNakMessage();
    return;
  }
  bool overwrite = true;
  if (!midi_sds.wav_file.open(my_string, overwrite, 1, sampleRate,
                              midi_sds.sampleFormat)) {
    midi_sds.sendCancelMessage();
    midi_sds.cancel();
  }
  midi_sds.samplesSoFar = 0;
  midi_sds.packetNumber = 0;

  //  midi_sds.sample_offset = (pow(2, midi_sds.sampleFormat) / 2) + 1;

  midi_sds.sample_offset = (pow(2, midi_sds.sampleFormat) / 2);
  midi_sds.midiBytes_per_word = midi_sds.sampleFormat / 7;
  midi_sds.bytes_per_word = midi_sds.sampleFormat / 8;
  if (midi_sds.sampleFormat % 8 > 0) {
    midi_sds.bytes_per_word++;
  }
  if (midi_sds.sampleFormat % 7 > 0) {
    midi_sds.midiBytes_per_word++;
  }
  // temp_file.open("temp_file.sds", FILE_WRITE | O_CREAT);
  ///  temp_file.close();
  midi_sds.sendAckMessage();
}

void MidiSDSSysexListenerClass::data_packet() {
  uint8_t checksum = 0;
  uint8_t packetNumber = MidiSysex.data[3];
  if (packetNumber != midi_sds.packetNumber) {
    DEBUG_PRINTLN("Packet received out of order");
    midi_sds.sendNakMessage();
    return;
  }
  for (uint16_t b = 0; b < MidiSysex.len - 1; b++) {
    checksum ^= MidiSysex.data[b];
  }
  if ((MidiSysex.len == 125) &&
      (MidiSysex.data[MidiSysex.len - 1] == checksum)) {
    // 120 byte data stream divided in to m words.
    // 7bits per data midi data byte.
    // For an 8bit sample (smallest bit rate), 2 midi data bytes are required.
    // Therefore a maximum of 60 samples can be transfered with a single data
    // packet.
    // temp_file.open("temp_file.sds", FILE_WRITE | O_APPEND);
    //    if (temp_file.write(&MidiSysex, MidiSysex.len) != MidiSysex.len) {
    //    DEBUG_PRINTLN("could not write");
    //}
    // temp_file.close();
    // midi_sds.sendAckMessage();
    // midi_sds.packetNumber += 1;
    //  if (midi_sds.packetNumber > 0x7F) {
    //       midi_sds.packetNumber = 0;
    //    }
    midi_sds.sendWaitMessage();

    uint8_t samples[120];

    uint32_t num_of_samples = 0;
    uint8_t byte_count = 0;

    uint32_t decode_val = 0;
    int16_t signed_val = 0;
    for (uint8_t n = 0;
         n <= 120 - midi_sds.midiBytes_per_word &&
         (num_of_samples + midi_sds.samplesSoFar) < midi_sds.sampleLength;
         n = n + midi_sds.midiBytes_per_word) {
      decode_val = 0;
      signed_val = 0;
      // Decode 7 bit midiBytes in to value
      uint8_t shift;
      //  DEBUG_PRINTLN("MIDI_BIN");
      for (shift = 0; shift < midi_sds.midiBytes_per_word; shift++) {

        decode_val = decode_val << 7;
        decode_val |= (int32_t)(0x7F & MidiSysex.data[n + 4 + shift]);
        //  Serial.println(MidiSysex.data[n + 4 + shift], HEX);
      }
      // Remove 0 padding.
      decode_val = decode_val >> (8 - shift);
      // For bit depth greater than 8 we need to convert the sample
      // from unsigned to signed by subtracting offset

      if (midi_sds.bytes_per_word > 1) {
        signed_val = decode_val - midi_sds.sample_offset;
        // decode_val -= midi_sds.sample_offset;
      } else {
        signed_val = decode_val;
      }
      // Shift the value in to b, byte values for wav file.
      //DEBUG_PRINTLN(" ");
      //DEBUG_PRINT(decode_val);
      //DEBUG_PRINT(",");
      //DEBUG_PRINT(signed_val);
      for (uint8_t b = 0; b < midi_sds.bytes_per_word; b++) {
        samples[byte_count + b] = (uint8_t)((signed_val >> (8 * (b))) & 0xFF);
      }

      byte_count += midi_sds.bytes_per_word;
      num_of_samples++;
    }

    bool ret = false;
    bool write_header = false;
    if (midi_sds.wav_file.write_samples(
            &samples, num_of_samples, midi_sds.samplesSoFar, 0, write_header)) {
      midi_sds.samplesSoFar += num_of_samples;
      midi_sds.sendAckMessage();
      midi_sds.incPacketNumber();
    } else {
      DEBUG_PRINTLN("error writing sds to SDCard");
      midi_sds.sendCancelMessage();
      midi_sds.state = SDS_READY;
    }
    // DEBUG_PRINT(" ");
    if (midi_sds.samplesSoFar == midi_sds.sampleLength) {
      DEBUG_PRINTLN("Sample receive finished");
      DEBUG_PRINTLN(midi_sds.wav_file.header.subchunk2Size);
      bool write_header = true;
      midi_sds.wav_file.close(write_header);
      if (sds_name_rec) {
        midi_sds.wav_file.rename(sds_name);
      }
      midi_sds.state = SDS_READY;
    }

  } else {
    DEBUG_PRINTLN("sds packet checksum error");
    DEBUG_PRINTLN(midi_sds.packetNumber);
    DEBUG_PRINTLN(MidiSysex.len);
    DEBUG_PRINTLN(MidiSysex.data[MidiSysex.len - 1]);
    DEBUG_PRINT(" ");
    DEBUG_PRINT(checksum);
    midi_sds.sendNakMessage();
  }
}

void MidiSDSSysexListenerClass::setup() { MidiSysex.addSysexListener(this); }
