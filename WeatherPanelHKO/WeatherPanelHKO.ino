const char *SSID_NAME = "YourAP";
const char *SSID_PASSWORD = "PleaseInputYourPasswordHere";
const long gmtOffset_sec = 8 * 60 * 60;

/**
 * @brief Weather photo
 * 5 minutes delay
 * 5 minutes interval
 */
const char *PHOTO_URL_TEMPLATE = "https://www.hko.gov.hk/wxinfo/aws/hko_mica/hmm/latest_HMM.jpg?v=%lu123";
const char *PHOTO_FOLDER_L1 = "/hmm";
const char *PHOTO_FOLDER_L2_TEMPLATE = "/hmm/%d";
const char *PHOTO_FOLDER_L3_TEMPLATE = "/hmm/%d/%02d";
const char *PHOTO_FOLDER_L4_TEMPLATE = "/hmm/%d/%02d/%02d";
const char *PHOTO_FILE_TEMPLATE = "/hmm/%d/%02d/%02d/%02d%02d.jpg";
// const char *PHOTO_URL_TEMPLATE = "https://www.hko.gov.hk/wxinfo/aws/hko_mica/ic1/latest_IC1.jpg?v=%lu123";
// const char *PHOTO_FOLDER_L1 = "/ic1";
// const char *PHOTO_FOLDER_L2_TEMPLATE = "/ic1/%d";
// const char *PHOTO_FOLDER_L3_TEMPLATE = "/ic1/%d/%02d";
// const char *PHOTO_FOLDER_L4_TEMPLATE = "/ic1/%d/%02d/%02d";
// const char *PHOTO_FILE_TEMPLATE = "/ic1/%d/%02d/%02d/%02d%02d.jpg";

/**
 * @brief Radar image
 * 6 minutes delay
 * 6 minutes interval
 */
// const char *RADAR_URL_TEMPLATE = "https://www.hko.gov.hk/wxinfo/radars/rad_064_png/2d064nradar_%d%02d%02d%02d%02d.jpg";
// const char *RADAR_FOLDER_L1 = "/rad_064";
// const char *RADAR_FOLDER_L2_TEMPLATE = "/rad_064/%d";
// const char *RADAR_FOLDER_L3_TEMPLATE = "/rad_064/%d/%02d";
// const char *RADAR_FOLDER_L4_TEMPLATE = "/rad_064/%d/%02d/%02d";
// const char *RADAR_FILE_TEMPLATE = "/rad_064/%d/%02d/%02d/%02d%02d.jpg";
const char *RADAR_URL_TEMPLATE = "https://www.hko.gov.hk/wxinfo/radars/rad_256_png/2d256nradar_%d%02d%02d%02d%02d.jpg";
const char *RADAR_FOLDER_L1 = "/rad_256";
const char *RADAR_FOLDER_L2_TEMPLATE = "/rad_256/%d";
const char *RADAR_FOLDER_L3_TEMPLATE = "/rad_256/%d/%02d";
const char *RADAR_FOLDER_L4_TEMPLATE = "/rad_256/%d/%02d/%02d";
const char *RADAR_FILE_TEMPLATE = "/rad_256/%d/%02d/%02d/%02d%02d.jpg";

/**
 * @brief Satellite image
 * 60 minutes delay
 * 10 minutes interval
 */
const char *SATELLITE_URL_TEMPLATE = "https://www.hko.gov.hk/wxinfo/intersat/satellite/image/images/h8_ir_x2M_%d%02d%02d%02d%02d00.jpg";
const char *SATELLITE_FOLDER_L1 = "/h8_ir_x2M";
const char *SATELLITE_FOLDER_L2_TEMPLATE = "/h8_ir_x2M/%d";
const char *SATELLITE_FOLDER_L3_TEMPLATE = "/h8_ir_x2M/%d/%02d";
const char *SATELLITE_FOLDER_L4_TEMPLATE = "/h8_ir_x2M/%d/%02d/%02d";
const char *SATELLITE_FILE_TEMPLATE = "/h8_ir_x2M/%d/%02d/%02d/%02d%02d.jpg";
// const char *SATELLITE_URL_TEMPLATE = "https://www.hko.gov.hk/wxinfo/intersat/satellite/image/images/h8_ir_x8M_%d%02d%02d%02d%02d00.jpg";
// const char *SATELLITE_FOLDER_L1 = "/h8_ir_x8M";
// const char *SATELLITE_FOLDER_L2_TEMPLATE = "/h8_ir_x8M/%d";
// const char *SATELLITE_FOLDER_L3_TEMPLATE = "/h8_ir_x8M/%d/%02d";
// const char *SATELLITE_FOLDER_L4_TEMPLATE = "/h8_ir_x8M/%d/%02d/%02d";
// const char *SATELLITE_FILE_TEMPLATE = "/h8_ir_x8M/%d/%02d/%02d/%02d%02d.jpg";

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>
WiFiMulti WiFiMulti;

char url[1024];
char path[1024];
time_t rounding_time;
time_t next_download_photo_time = 0;
time_t next_download_radar_time = 0;
time_t next_download_satellite_time = 0;

#include <SD_MMC.h>
#define SD_CS 1
#define SD_MOSI 2
#define SD_SCK 3
#define SD_MISO 4

#include "FILESYSTEM.h"
#include "HTTPS.h"
#include "JPEG.h"

/*******************************************************************************
 * Start of Arduino_GFX setting
 ******************************************************************************/
#include <Arduino_GFX_Library.h>

Arduino_DataBus *bus1 = new Arduino_ESP32QSPI(
    6 /* CS */, 7 /* SCK */, 8 /* D0 */, 9 /* D1 */, 10 /* D2 */, 11 /* D3 */, true /* is_shared_interface */);
Arduino_GFX *g = new Arduino_ST77916(
    bus1, 12 /* RST */, 0 /* rotation */, true /* IPS */, 360 /* width */, 360 /* height */,
    0 /* col offset 1 */, 0 /* row offset 1 */, 0 /* col offset 2 */, 0 /* row offset 2 */,
    st77916_150_init_operations, sizeof(st77916_150_init_operations));
#define CANVAS
Arduino_Canvas *gfx1 = new Arduino_Canvas(
    360 /* width */, 360 /* height */, g);

Arduino_DataBus *bus2 = new Arduino_ESP32QSPI(
    5 /* CS */, 7 /* SCK */, 8 /* D0 */, 9 /* D1 */, 10 /* D2 */, 11 /* D3 */, true /* is_shared_interface */);
Arduino_GFX *gfx2 = new Arduino_ST77916(
    bus2, GFX_NOT_DEFINED /* RST */, 0 /* rotation */, true /* IPS */, 360 /* width */, 360 /* height */,
    0 /* col offset 1 */, 0 /* row offset 1 */, 0 /* col offset 2 */, 0 /* row offset 2 */,
    st77916_150_init_operations, sizeof(st77916_150_init_operations));

Arduino_DataBus *bus3 = new Arduino_ESP32QSPI(
    13 /* CS */, 7 /* SCK */, 8 /* D0 */, 9 /* D1 */, 10 /* D2 */, 11 /* D3 */, true /* is_shared_interface */);
Arduino_GFX *gfx3 = new Arduino_ST77916(
    bus3, GFX_NOT_DEFINED /* RST */, 0 /* rotation */, true /* IPS */, 360 /* width */, 360 /* height */,
    0 /* col offset 1 */, 0 /* row offset 1 */, 0 /* col offset 2 */, 0 /* row offset 2 */,
    st77916_150_init_operations, sizeof(st77916_150_init_operations));
/*******************************************************************************
 * End of Arduino_GFX setting
 ******************************************************************************/

// Not sure if NetworkClientSecure checks the validity date of the certificate.
// Setting clock just to be sure...
void setClock()
{
  configTime(0, 0, "pool.ntp.org");

  Serial.print("Waiting for NTP time sync: ");
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2)
  {
    delay(500);
    Serial.print(".");
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print("Current time: ");
  char buf[26];
  Serial.println(asctime_r(&timeinfo, buf));
}

// pixel drawing callback
int JPEGDraw1(JPEGDRAW *pDraw)
{
  // Serial.printf("Draw pos = %d,%d. size = %d x %d\n", pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight);
  gfx1->draw16bitRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  return 1;
}

// pixel drawing callback
int JPEGDraw2(JPEGDRAW *pDraw)
{
  // Serial.printf("Draw pos = %d,%d. size = %d x %d\n", pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight);
  gfx2->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  return 1;
}

// pixel drawing callback
int JPEGDraw3(JPEGDRAW *pDraw)
{
  // Serial.printf("Draw pos = %d,%d. size = %d x %d\n", pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight);
  gfx3->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  return 1;
}

char timeStr[6];
void drawClock()
{
  time(&rounding_time);
  rounding_time += gmtOffset_sec;
  struct tm *tmLocal = localtime(&rounding_time);
  strftime(timeStr, sizeof(timeStr), "%H:%M", tmLocal);

  bool shown_time = false;
  rounding_time /= (30 * 60); // round to 30 minutes, LCM of 5, 6 and 10 minutes
  rounding_time *= (30 * 60);
  unsigned long startMs = millis();

  /* draw photo */
  time_t minute_i = rounding_time - (3 * 60 * 60); // show 3 hours timelapse
  while (minute_i < rounding_time)
  {
    struct tm *tmPast = localtime(&minute_i);
    sprintf(path, PHOTO_FILE_TEMPLATE, tmPast->tm_year + 1900, tmPast->tm_mon + 1, tmPast->tm_mday, tmPast->tm_hour, tmPast->tm_min);
    // Serial.println(path);
    if (SD_MMC.exists(path))
    {
      if (jpeg.open(path, jpegOpenSD_MMC, jpegClose, jpegRead, jpegSeek, JPEGDraw1))
      {
        jpeg.setPixelType(RGB565_LITTLE_ENDIAN);
        jpeg.setCropArea(180, 44, 360, 360); // requested area
        jpeg.decode(0 /* x */, 0 /* y */, 0 /* options */);
        jpeg.close();
        gfx1->setCursor(35, 145);
        gfx1->setTextColor(RGB565_BLACK);
        gfx1->setTextSize(10, 10, 2);
        gfx1->print(timeStr);
        gfx1->setCursor(36, 146);
        gfx1->setTextColor(RGB565_WHITE);
        gfx1->setTextSize(10, 10, 4);
        gfx1->print(timeStr);
        gfx1->flush();
        shown_time = true;
      }
      else
      {
        Serial.print("error = ");
        Serial.println(jpeg.getLastError(), DEC);
      }
    }
    minute_i += (5 * 60);
  }

  /* draw radar image */
  minute_i = rounding_time - (3 * 60 * 60); // show 3 hours timelapse
  while (minute_i < rounding_time)
  {
    struct tm *tmPast = localtime(&minute_i);
    sprintf(path, RADAR_FILE_TEMPLATE, tmPast->tm_year + 1900, tmPast->tm_mon + 1, tmPast->tm_mday, tmPast->tm_hour, tmPast->tm_min);
    // Serial.println(path);
    if (SD_MMC.exists(path))
    {
      if (jpeg.open(path, jpegOpenSD_MMC, jpegClose, jpegRead, jpegSeek, JPEGDraw2))
      {
        jpeg.setPixelType(RGB565_BIG_ENDIAN);
        jpeg.setCropArea(20, 20, 360, 360); // requested area
        jpeg.decode(0 /* x */, 0 /* y */, 0 /* options */);
        jpeg.close();
      }
      else
      {
        Serial.print("error = ");
        Serial.println(jpeg.getLastError(), DEC);
      }
    }
    minute_i += (6 * 60);
  }

  /* draw satellite image */
  minute_i = rounding_time - (3 * 60 * 60); // show 3 hours timelapse
  while (minute_i < rounding_time)
  {
    struct tm *tmPast = localtime(&minute_i);
    sprintf(path, SATELLITE_FILE_TEMPLATE, tmPast->tm_year + 1900, tmPast->tm_mon + 1, tmPast->tm_mday, tmPast->tm_hour, tmPast->tm_min);
    // Serial.println(path);
    if (SD_MMC.exists(path))
    {
      if (jpeg.open(path, jpegOpenSD_MMC, jpegClose, jpegRead, jpegSeek, JPEGDraw3))
      {
        jpeg.setPixelType(RGB565_BIG_ENDIAN);
        jpeg.setCropArea(192, 192, 360, 360); // requested area
        jpeg.decode(0 /* x */, 0 /* y */, 0 /* options */);
        jpeg.close();
      }
      else
      {
        Serial.print("error = ");
        Serial.println(jpeg.getLastError(), DEC);
      }
    }
    minute_i += (10 * 60);
  }

  if (!shown_time)
  {
    gfx1->fillScreen(RGB565_NAVY);
    gfx1->setCursor(35, 145);
    gfx1->setTextColor(RGB565_BLACK);
    gfx1->setTextSize(10, 10, 2);
    gfx1->print(timeStr);
    gfx1->setCursor(36, 146);
    gfx1->setTextColor(RGB565_WHITE);
    gfx1->setTextSize(10, 10, 4);
    gfx1->print(timeStr);
    gfx1->flush();
  }
  Serial.printf("drawClock() used: %d ms\n", millis() - startMs);
}

void setup()
{
  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println("Weather Panel HKO");

  delay(2000); // wait 2 seconds

  // Init Display
  Serial.println("gfx1->begin()");
  if (!gfx1->begin())
  {
    Serial.println("gfx1->begin() failed!");
  }
  gfx1->fillScreen(RGB565_NAVY);
  gfx1->setCursor(125, 145);
  gfx1->setTextColor(RGB565_RED);
  gfx1->setTextSize(10, 10, 4);
  gfx1->print("SD");
  gfx1->flush();

  Serial.println("gfx2->begin()");
  if (!gfx2->begin(GFX_SKIP_DATABUS_UNDERLAYING_BEGIN))
  {
    Serial.println("gfx2->begin() failed!");
  }
  gfx2->fillScreen(RGB565_DARKCYAN);
  gfx2->setCursor(65, 145);
  gfx2->setTextColor(RGB565_RED);
  gfx2->setTextSize(10, 10, 4);
  gfx2->print("WiFi");

  Serial.println("gfx3->begin()");
  if (!gfx3->begin(GFX_SKIP_DATABUS_UNDERLAYING_BEGIN))
  {
    Serial.println("gfx3->begin() failed!");
  }
  gfx3->fillScreen(RGB565_BLUE);
  gfx3->setCursor(95, 145);
  gfx3->setTextColor(RGB565_RED);
  gfx3->setTextSize(10, 10, 4);
  gfx3->print("NTP");

#ifdef GFX_BL
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
#endif

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(SSID_NAME, SSID_PASSWORD);

  SD_MMC.setPins(SD_SCK, SD_MOSI, SD_MISO);
  if (!SD_MMC.begin("/root", true /* mode1bit */, false /* format_if_mount_failed */, SDMMC_FREQ_HIGHSPEED))
  {
    Serial.println("Card Mount Failed");
    delay(24 * 60 * 60 * 1000);
  }
  else
  {
    Serial.printf("SD_MMC Card Mounted, space usage: %llu / %llu MB\n", SD_MMC.usedBytes() / (1024 * 1024), SD_MMC.totalBytes() / (1024 * 1024));
    listDir(SD_MMC, "/", 4);
    gfx1->setCursor(125, 145);
    gfx1->setTextColor(RGB565_GREEN);
    gfx1->print("SD");
    gfx1->flush();
  }

  // wait for WiFi connection
  Serial.print("Waiting for WiFi to connect...");
  while ((WiFiMulti.run() != WL_CONNECTED))
  {
    Serial.print(".");
  }
  Serial.println(" connected");
  gfx2->setCursor(65, 145);
  gfx2->setTextColor(RGB565_GREEN);
  gfx2->print("WiFi");

  setClock();
  gfx3->setCursor(95, 145);
  gfx3->setTextColor(RGB565_GREEN);
  gfx3->print("NTP");
}

void loop()
{
  drawClock();

  /* Check and download latest photo */
  time(&rounding_time);
  if (rounding_time > next_download_photo_time)
  {
    Serial.printf("ESP.getFreeHeap(): %d, ESP.getFreePsram(): %d\n", ESP.getFreeHeap(), ESP.getFreePsram());
    rounding_time /= 5 * 60; // round to 5 minutes
    rounding_time *= 5 * 60;
    next_download_photo_time = rounding_time + (5 * 60);
    rounding_time -= 5 * 60; // photo delayed 5 minutes
    sprintf(url, PHOTO_URL_TEMPLATE, rounding_time);

    rounding_time += gmtOffset_sec;
    struct tm *tmLocal = gmtime(&rounding_time);
    SD_MMC.mkdir(PHOTO_FOLDER_L1);
    sprintf(path, PHOTO_FOLDER_L2_TEMPLATE, tmLocal->tm_year + 1900);
    SD_MMC.mkdir(path);
    sprintf(path, PHOTO_FOLDER_L3_TEMPLATE, tmLocal->tm_year + 1900, tmLocal->tm_mon + 1);
    SD_MMC.mkdir(path);
    sprintf(path, PHOTO_FOLDER_L4_TEMPLATE, tmLocal->tm_year + 1900, tmLocal->tm_mon + 1, tmLocal->tm_mday);
    SD_MMC.mkdir(path);
    // Serial.println(path);
    sprintf(path, PHOTO_FILE_TEMPLATE, tmLocal->tm_year + 1900, tmLocal->tm_mon + 1, tmLocal->tm_mday, tmLocal->tm_hour, tmLocal->tm_min);
    if (!SD_MMC.exists(path))
    {
      https_fs_download(url, SD_MMC, path);
    }
  }

  /* Check and download latest radar image */
  time(&rounding_time);
  if (rounding_time > next_download_radar_time)
  {
    Serial.printf("ESP.getFreeHeap(): %d, ESP.getFreePsram(): %d\n", ESP.getFreeHeap(), ESP.getFreePsram());
    rounding_time /= 6 * 60; // round to 6 minutes
    rounding_time *= 6 * 60;
    next_download_radar_time = rounding_time + (6 * 60);
    rounding_time -= 6 * 60; // radar image delayed 6 minutes
    rounding_time += gmtOffset_sec;
    struct tm *tmLocal = gmtime(&rounding_time);
    sprintf(url, RADAR_URL_TEMPLATE, tmLocal->tm_year + 1900, tmLocal->tm_mon + 1, tmLocal->tm_mday, tmLocal->tm_hour, tmLocal->tm_min);

    SD_MMC.mkdir(RADAR_FOLDER_L1);
    sprintf(path, RADAR_FOLDER_L2_TEMPLATE, tmLocal->tm_year + 1900);
    SD_MMC.mkdir(path);
    sprintf(path, RADAR_FOLDER_L3_TEMPLATE, tmLocal->tm_year + 1900, tmLocal->tm_mon + 1);
    SD_MMC.mkdir(path);
    sprintf(path, RADAR_FOLDER_L4_TEMPLATE, tmLocal->tm_year + 1900, tmLocal->tm_mon + 1, tmLocal->tm_mday);
    SD_MMC.mkdir(path);
    // Serial.println(path);
    sprintf(path, RADAR_FILE_TEMPLATE, tmLocal->tm_year + 1900, tmLocal->tm_mon + 1, tmLocal->tm_mday, tmLocal->tm_hour, tmLocal->tm_min);
    if (!SD_MMC.exists(path))
    {
      https_fs_download(url, SD_MMC, path);
    }
  }

  /* Check and download latest satellite image */
  time(&rounding_time);
  if (rounding_time > next_download_satellite_time)
  {
    Serial.printf("ESP.getFreeHeap(): %d, ESP.getFreePsram(): %d\n", ESP.getFreeHeap(), ESP.getFreePsram());
    rounding_time /= (10 * 60); // round to 10 minutes
    rounding_time *= (10 * 60);
    next_download_satellite_time = rounding_time + (10 * 60);
    rounding_time -= (60 * 60); // satellite image delayed 60 minutes
    struct tm *tmGm = gmtime(&rounding_time);
    sprintf(url, SATELLITE_URL_TEMPLATE, tmGm->tm_year + 1900, tmGm->tm_mon + 1, tmGm->tm_mday, tmGm->tm_hour, tmGm->tm_min);

    rounding_time += gmtOffset_sec;
    struct tm *tmLocal = gmtime(&rounding_time);
    SD_MMC.mkdir(SATELLITE_FOLDER_L1);
    sprintf(path, SATELLITE_FOLDER_L2_TEMPLATE, tmLocal->tm_year + 1900);
    SD_MMC.mkdir(path);
    sprintf(path, SATELLITE_FOLDER_L3_TEMPLATE, tmLocal->tm_year + 1900, tmLocal->tm_mon + 1);
    SD_MMC.mkdir(path);
    sprintf(path, SATELLITE_FOLDER_L4_TEMPLATE, tmLocal->tm_year + 1900, tmLocal->tm_mon + 1, tmLocal->tm_mday);
    SD_MMC.mkdir(path);
    // Serial.println(path);
    sprintf(path, SATELLITE_FILE_TEMPLATE, tmLocal->tm_year + 1900, tmLocal->tm_mon + 1, tmLocal->tm_mday, tmLocal->tm_hour, tmLocal->tm_min);
    if (!SD_MMC.exists(path))
    {
      https_fs_download(url, SD_MMC, path);
    }
  }

  // Serial.println();
  // Serial.println("Waiting 5s before the next round...");
  delay(5 * 1000);
}
