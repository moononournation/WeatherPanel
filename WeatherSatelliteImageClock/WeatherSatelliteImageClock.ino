const char *SSID_NAME = "YourAP";
const char *SSID_PASSWORD = "PleaseInputYourPasswordHere";
const long gmtOffset_sec = 8 * 60 * 60;

/**
 * @brief Satellite image
 * 45 minutes delay
 * 15 minutes interval
 */
const char *SATELLITE_URL_TEMPLATE = "https://image.nmc.cn/product/%d/%02d/%02d/WXBL/medium/SEVP_NSMC_WXBL_FY4B_ETCC_ACHN_LNO_PY_%d%02d%02d%02d%02d00000.JPG";

const char *SATELLITE_FOLDER_L1 = "/fy4b";
const char *SATELLITE_FOLDER_L2_TEMPLATE = "/fy4b/%d";
const char *SATELLITE_FOLDER_L3_TEMPLATE = "/fy4b/%d/%02d";
const char *SATELLITE_FOLDER_L4_TEMPLATE = "/fy4b/%d/%02d/%02d";
const char *SATELLITE_FILE_TEMPLATE = "/fy4b/%d/%02d/%02d/%02d%02d.jpg";

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>
WiFiMulti WiFiMulti;

char url[1024];
char path[1024];
time_t rounding_time;
time_t next_download_satellite_time = 0;

#include <SD_MMC.h>
#define SD_CS 38
#define SD_MOSI 39
#define SD_SCK 41
#define SD_MISO 40

#include "FILESYSTEM.h"
#include "HTTPS.h"
#include "JPEG.h"

#include <U8g2lib.h>
/*******************************************************************************
 * Start of Arduino_GFX setting
 ******************************************************************************/
#include <Arduino_GFX_Library.h>
#define GFX_BL 8
Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    9 /* CS */, 10 /* SCK */, 11 /* D0 */, 12 /* D1 */, 13 /* D2 */, 14 /* D3 */);
Arduino_GFX *g = new Arduino_AXS15231B(
    bus, 21 /* RST */, 0 /* rotation */, false /* IPS */, 172 /* width */, 640 /* height */,
    0 /* col offset 1 */, 0 /* row offset 1 */, 0 /* col offset 2 */, 0 /* row offset 2 */,
    axs15231b_180640_init_operations, sizeof(axs15231b_180640_init_operations));
#define CANVAS
Arduino_Canvas *gfx = new Arduino_Canvas(
    172 /* width */, 640 /* height */, g, 0 /* output_x */, 0 /* output_y */, 1 /* rotation */);
/*******************************************************************************
 * End of Arduino_GFX setting
 ******************************************************************************/

// Not sure if NetworkClientSecure checks the validity date of the certificate.
// Setting clock just to be sure...
void setClock()
{
  configTime(0, 0, "pool.ntp.org");

  Serial.print("Waiting for NTP time sync: ");
  gfx->print("Waiting for NTP time sync: ");
  gfx->flush();
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2)
  {
    delay(500);
    Serial.print(".");
    gfx->print(".");
    gfx->flush();
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  gfx->println();
  gfx->flush();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print("Current time: ");
  gfx->print("Current time: ");
  gfx->flush();
  char buf[26];
  Serial.println(asctime_r(&timeinfo, buf));
  gfx->println(asctime_r(&timeinfo, buf));
  gfx->flush();
}

// pixel drawing callback
int JPEGDraw(JPEGDRAW *pDraw)
{
  // Serial.printf("Draw pos = %d,%d. size = %d x %d\n", pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight);
  gfx->draw16bitRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  return 1;
}

char dateStr[32];
char timeStr[6];
void drawClock()
{
  time(&rounding_time);
  rounding_time += gmtOffset_sec;
  struct tm *tmLocal = localtime(&rounding_time);
  strftime(dateStr, sizeof(dateStr), "%Y %B %d, %A", tmLocal);
  strftime(timeStr, sizeof(timeStr), "%H:%M", tmLocal);

  bool shown_time = false;
  rounding_time /= (15 * 60); // round to 15 minutes
  rounding_time *= (15 * 60);
  unsigned long startMs = millis();
  /* draw satellite image */
  time_t minute_i = rounding_time - (8 * 60 * 60); // show 8 hours timelapse
  while (minute_i < rounding_time)
  {
    struct tm *tmPast = localtime(&minute_i);
    sprintf(path, SATELLITE_FILE_TEMPLATE, tmPast->tm_year + 1900, tmPast->tm_mon + 1, tmPast->tm_mday, tmPast->tm_hour, tmPast->tm_min);
    // Serial.println(path);
    if (SD_MMC.exists(path))
    {
      if (jpeg.open(path, jpegOpenSD_MMC, jpegClose, jpegRead, jpegSeek, JPEGDraw))
      {
        jpeg.setPixelType(RGB565_LITTLE_ENDIAN);
        jpeg.setCropArea(220, 280, 640, 172); // requested area
        jpeg.decode(0 /* x */, 0 /* y */, 0 /* options */);
        jpeg.close();
        gfx->setFont();
        gfx->shade(0, 162, 152, 10, 0b0101101011101011);
        gfx->setCursor(1, 163);
        gfx->setTextColor(RGB565_WHITE);
        gfx->print(path);

        gfx->shade(10, 10, 300, 128, 0b1011110111110111);
        gfx->setFont(u8g2_font_fub14_tf);
        gfx->setCursor(12, 28);
        gfx->setTextColor(RGB565_WHITE);
        gfx->print(dateStr);
        gfx->setFont(u8g2_font_logisoso92_tn);
        gfx->setCursor(4, 130);
        gfx->setTextColor(RGB565_WHITE);
        gfx->print(timeStr);
        gfx->flush();
        shown_time = true;
      }
      else
      {
        Serial.print("error = ");
        Serial.println(jpeg.getLastError(), DEC);
      }
    }
    minute_i += (15 * 60);
  }

  if (!shown_time)
  {
    gfx->fillScreen(RGB565_NAVY);
    gfx->setFont(u8g2_font_fub14_tf);
    gfx->setCursor(12, 28);
    gfx->setTextColor(RGB565_WHITE);
    gfx->print(dateStr);
    gfx->setFont(u8g2_font_logisoso92_tn);
    gfx->setCursor(4, 130);
    gfx->setTextColor(RGB565_WHITE);
    gfx->print(timeStr);
    gfx->flush();
  }
  Serial.printf("drawClock() used: %d ms\n", millis() - startMs);
}

void setup()
{
  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println("Weather Satellite Image Clock");

  delay(2000); // wait 2 seconds

  // Init Display
  if (!gfx->begin())
  {
    Serial.println("gfx->begin() failed!");
  }
  gfx->fillScreen(RGB565_BLACK);
  gfx->setTextColor(RGB565_RED);
  gfx->println("Weather Satellite Image");
  gfx->flush();

#ifdef GFX_BL
  pinMode(GFX_BL, OUTPUT);
  // digitalWrite(GFX_BL, HIGH);
  digitalWrite(GFX_BL, LOW);
#endif

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(SSID_NAME, SSID_PASSWORD);

  gfx->setTextColor(RGB565_ORANGE);
  SD_MMC.setPins(SD_SCK, SD_MOSI, SD_MISO);
  if (!SD_MMC.begin("/root", true /* mode1bit */, false /* format_if_mount_failed */, SDMMC_FREQ_HIGHSPEED))
  {
    Serial.println("SD_MMC Card Mount Failed");
    gfx->println("SD_MMC Card Mount Failed");
    gfx->flush();
    delay(24 * 60 * 60 * 1000);
  }
  else
  {
    Serial.printf("SD_MMC Card Mounted, space usage: %llu / %llu MB\n", SD_MMC.usedBytes() / (1024 * 1024), SD_MMC.totalBytes() / (1024 * 1024));
    gfx->printf("SD_MMC Card Mounted, space usage: %llu / %llu MB\n", SD_MMC.usedBytes() / (1024 * 1024), SD_MMC.totalBytes() / (1024 * 1024));
    gfx->flush();
    listDir(SD_MMC, "/", 4);
  }

  // wait for WiFi connection
  gfx->setTextColor(RGB565_YELLOW);
  Serial.print("Waiting for WiFi to connect...");
  gfx->print("Waiting for WiFi to connect...");
  gfx->flush();
  while ((WiFiMulti.run() != WL_CONNECTED))
  {
    Serial.print(".");
    gfx->print(".");
    gfx->flush();
  }
  Serial.println(" connected");
  gfx->println(" connected");
  gfx->flush();

  gfx->setTextColor(RGB565_GREEN);
  setClock();
}

void loop()
{
  drawClock();

  /* Check and download latest satellite image */
  time(&rounding_time);
  if (rounding_time > next_download_satellite_time)
  {
    Serial.printf("ESP.getFreeHeap(): %d, ESP.getFreePsram(): %d\n", ESP.getFreeHeap(), ESP.getFreePsram());
    rounding_time /= (15 * 60); // round to 15 minutes
    rounding_time *= (15 * 60);
    next_download_satellite_time = rounding_time + (15 * 60);
    rounding_time -= (45 * 60); // satellite image delayed 45 minutes
    struct tm *tmGm = gmtime(&rounding_time);
    sprintf(url, SATELLITE_URL_TEMPLATE, tmGm->tm_year + 1900, tmGm->tm_mon + 1, tmGm->tm_mday, tmGm->tm_year + 1900, tmGm->tm_mon + 1, tmGm->tm_mday, tmGm->tm_hour, tmGm->tm_min);

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
