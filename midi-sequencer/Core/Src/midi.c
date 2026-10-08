#include "midi.h"

#include "main.h"
#include <string.h>
#include "ff.h"


const char
  CHUNK_TYPE_HEADER_IDENT[] = "MThd",
  CHUNK_TYPE_TRACK_IDENT[]  = "MTrk";


enum Event {
  MIDI,
  SYSEX,
  META = 0XFF
};

// Starts with 0xFF, code, and then arguments.
enum MetaEvent {
  SEQUENCE_NUMBER = 0x00,// 02 ssss
  TEXT = 0x01,// len text
  COPYRIGHT = 0x02,// len text
  TRACK_NAME = 0x03,// len text
  INSTRUMENT_NAME = 0x04,// len text
  LYRIC = 0x05,// len text
  MARKER = 0x06,// len text
  CUE_POINT = 0x07,// len text
  DEVICE_NAME = 0x09,
  CHANNEL_PREFIX = 0x20,// 01 cc
  PORT = 0x21,
  END_OF_TRACK = 0x2F,// 00
  SET_TEMPO = 0x51,// 03 tttttt
  SMPTE_OFFSET = 0x54,// 05 hr mn se fr ff
  TIME_SIGNATURE = 0x58,// 04 nn dd cc bb
  KEY_SIGNATURE = 0x59,// 02 sf mi
  SEQUENCER_SPECIFIC = 0x7F// len data
};

struct ChunkHeader {
  char type[4];// MThd
  uint32_t len;
  uint16_t format;
  uint16_t ntrks;
  uint16_t division;
};

struct ChunkTrack {
  char type[4];// MTrk
};

//#define SET_SIGN(var)SET_BIT(var,1<<(sizeof(val)-1))
//#define CLEAR_SIGN(var)CLEAR_BIT(var,1<<(sizeof(val)-1))


uint16_t FORMAT, NTRKS, DIVISION;

uint32_t read_var_len(FIL *fp);

void handle_meta_event(FIL *fp, uint8_t event);

MRESULT midi_verify(FIL *fp)
{
  UINT br;

  {
    struct ChunkHeader header;
    if (FR_OK != f_read(fp, &header, sizeof(struct ChunkHeader), &br)) {
      return M_DISK_ERR;
    }

    if (0 == strstr(header.type, CHUNK_TYPE_HEADER_IDENT)) {
      return M_INVALID_FORMAT;
    }

    if (6 != header.len) {
      return M_INVALID_FORMAT;
    }

    FORMAT = header.format;
    NTRKS = header.ntrks;
    DIVISION = header.division;
  }

  return M_OK;
}

void handle_meta_event(FIL *fp, uint8_t event)
{
  switch (event) {
    case SEQUENCE_NUMBER:
      // TODO: create handle.
      Error_Handler();
      break;
    case TEXT:
    case LYRIC:
    case COPYRIGHT:
    case CUE_POINT:
    case INSTRUMENT_NAME:
    case TRACK_NAME: {
      // Skip reading those bytes.
      uint32_t len = read_var_len(fp);
      f_lseek(fp, len);
      break;
    }
    case MARKER:
      // TODO: create handle.
      Error_Handler();
      break;
    case DEVICE_NAME:
      // TODO: create handle.
      Error_Handler();
      break;
    case CHANNEL_PREFIX:
      // TODO: create handle.
      Error_Handler();
      break;
    case PORT:
      // TODO: create handle.
      Error_Handler();
      break;
    case END_OF_TRACK:
      // TODO: create handle.
      Error_Handler();
      break;
    case SET_TEMPO:
      // TODO: create handle.
      Error_Handler();
      break;
    case SMPTE_OFFSET:
      // TODO: create handle.
      Error_Handler();
      break;
    case TIME_SIGNATURE:
      // TODO: create handle.
      Error_Handler();
      break;
    case KEY_SIGNATURE:
      // TODO: create handle.
      Error_Handler();
      break;
    case SEQUENCER_SPECIFIC:
      // TODO: create handle.
      Error_Handler();
      break;

    default:
      // Unhandled event.
      Error_Handler();
  }
}


uint32_t read_var_len(FIL *fp)
{
  const uint8_t sign_bit = 0x80, sign_mask = 0x7f;
  UINT br;
  uint32_t val;

  // Get first byte.
  f_read(fp, &val, 1, &br);

  if (val & sign_bit) {
    val &= sign_mask;

    uint8_t c;

    do {
      // Get next byte.
      f_read(fp, &c, sizeof(c), &br);
      val = (val << 7) + (c & sign_mask);
    } while (c & sign_bit);
  }

  return val;
}
