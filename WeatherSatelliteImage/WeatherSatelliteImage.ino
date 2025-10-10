const char *SSID_NAME = "YourAP";
const char *SSID_PASSWORD = "PleaseInputYourPasswordHere";
const char *SATELLITE_URL_TEMPLATE = "https://image.nmc.cn/product/%d/%02d/%02d/WXBL/medium/SEVP_NSMC_WXBL_FY4B_ETCC_ACHN_LNO_PY_%d%02d%02d%02d%02d00000.JPG";

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>

#include <Arduino_JSON.h>
char url[1024];

#include "HTTPS.h"
#include "JPEG.h"

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
void setClock() {
  configTime(0, 0, "pool.ntp.org");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print(F("Current time: "));
  char buf[26];
  Serial.print(asctime_r(&timeinfo, buf));
}

// pixel drawing callback
int JPEGDraw(JPEGDRAW *pDraw)
{
  // Serial.printf("Draw pos = %d,%d. size = %d x %d\n", pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight);
  gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  return 1;
}

WiFiMulti WiFiMulti;

void setup() {
  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println("Weather Satellite Image");

  // Init Display
  if (!gfx->begin())
  {
    Serial.println("gfx->begin() failed!");
  }
  gfx->fillScreen(RGB565_BLACK);

#ifdef GFX_BL
  pinMode(GFX_BL, OUTPUT);
  // digitalWrite(GFX_BL, HIGH);
  digitalWrite(GFX_BL, LOW);
#endif

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(SSID_NAME, SSID_PASSWORD);

  // wait for WiFi connection
  Serial.print("Waiting for WiFi to connect...");
  while ((WiFiMulti.run() != WL_CONNECTED)) {
    Serial.print(".");
  }
  Serial.println(" connected");

  setClock();
}

#define UPDATE_INTERVAL (15 * 60 * 1000)
unsigned long last_update = 0;
void loop() {
  if ((last_update == 0) || ((millis() - last_update) > UPDATE_INTERVAL)) {
    Serial.printf("ESP.getFreeHeap(): %d, ESP.getFreePsram(): %d\n", ESP.getFreeHeap(), ESP.getFreePsram());
    time_t now;
    time(&now);
    now -= 45 * 60;
    now /= (15 * 60);
    now *= (15 * 60);
    struct tm *tmNow = localtime(&now);

    // show satellite photo
    sprintf(url, SATELLITE_URL_TEMPLATE, tmNow->tm_year + 1900, tmNow->tm_mon + 1, tmNow->tm_mday, tmNow->tm_year + 1900, tmNow->tm_mon + 1, tmNow->tm_mday, tmNow->tm_hour, tmNow->tm_min);
    Serial.printf("url: %s\n", url);
    https_get_response_buf(url);
    if (jpeg.openRAM(https_response_buf, https_response_len, JPEGDraw)) { // pass the data and its size
        jpeg.setPixelType(RGB565_BIG_ENDIAN);
        jpeg.decode(-220 /* x */, -280 /* y */, 0 /* options */); // 860 x 540
        gfx->flush();
    }
    jpeg.close();
    free(https_response_buf);

    last_update = millis();
  }

  // Serial.println();
  // Serial.println("Waiting 1s before the next round...");
  delay(1000);
}
