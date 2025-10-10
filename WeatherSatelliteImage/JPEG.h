#pragma once

#include <SD.h>
#include <SD_MMC.h>
#include <JPEGDEC.h>
JPEGDEC jpeg;

// Functions to access a file on the SD card
File jpegFile;

void *jpegOpenSD(const char *filename, int32_t *size) {
  jpegFile = SD.open(filename);
  *size = jpegFile.size();
  return &jpegFile;
}

void *jpegOpenSD_MMC(const char *filename, int32_t *size) {
  jpegFile = SD_MMC.open(filename);
  *size = jpegFile.size();
  return &jpegFile;
}

void jpegClose(void *handle) {
  if (jpegFile) {
    jpegFile.close();
  }
}

int32_t jpegRead(JPEGFILE *handle, uint8_t *buffer, int32_t length) {
  if (!jpegFile) {
    return 0;
  }
  return jpegFile.read(buffer, length);
}

int32_t jpegSeek(JPEGFILE *handle, int32_t position) {
  if (!jpegFile) {
    return 0;
  }
  return jpegFile.seek(position);
}
