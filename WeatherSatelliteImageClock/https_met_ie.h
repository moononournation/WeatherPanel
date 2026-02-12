/**
 * @brief Satellite image
 * https://www.met.ie/latest-reports/satellites/europe-visible
 * 45 minutes delay
 * 15 minutes interval
 */
// const char *SATELLITE_URL_TEMPLATE = "https://www.met.ie/images/satellite/web17_sat_irl_vis_%d%02d%02d%02d%02d.jpeg";
// const char *SATELLITE_URL_TEMPLATE = "https://www.met.ie/images/satellite/web17_sat_eur_vis_%d%02d%02d%02d%02d.jpeg";
const char *SATELLITE_URL_TEMPLATE = "https://www.met.ie/images/satellite/web17_sat_full_vis_%d%02d%02d%02d%02d.jpeg";

#define SATELLITE_IMAGE_X 560
#define SATELLITE_IMAGE_Y 96

const char *SATELLITE_FOLDER_L1 = "/met_ie";
const char *SATELLITE_FOLDER_L2_TEMPLATE = "/met_ie/%d";
const char *SATELLITE_FOLDER_L3_TEMPLATE = "/met_ie/%d/%02d";
const char *SATELLITE_FOLDER_L4_TEMPLATE = "/met_ie/%d/%02d/%02d";
const char *SATELLITE_FILE_TEMPLATE = "/met_ie/%d/%02d/%02d/%02d%02d.jpg";

const char *rootCACertificate = R"string_literal(
-----BEGIN CERTIFICATE-----
MIIFBjCCAu6gAwIBAgIRAMISMktwqbSRcdxA9+KFJjwwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMjQwMzEzMDAwMDAw
WhcNMjcwMzEyMjM1OTU5WjAzMQswCQYDVQQGEwJVUzEWMBQGA1UEChMNTGV0J3Mg
RW5jcnlwdDEMMAoGA1UEAxMDUjEyMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIB
CgKCAQEA2pgodK2+lP474B7i5Ut1qywSf+2nAzJ+Npfs6DGPpRONC5kuHs0BUT1M
5ShuCVUxqqUiXXL0LQfCTUA83wEjuXg39RplMjTmhnGdBO+ECFu9AhqZ66YBAJpz
kG2Pogeg0JfT2kVhgTU9FPnEwF9q3AuWGrCf4yrqvSrWmMebcas7dA8827JgvlpL
Thjp2ypzXIlhZZ7+7Tymy05v5J75AEaz/xlNKmOzjmbGGIVwx1Blbzt05UiDDwhY
XS0jnV6j/ujbAKHS9OMZTfLuevYnnuXNnC2i8n+cF63vEzc50bTILEHWhsDp7CH4
WRt/uTp8n1wBnWIEwii9Cq08yhDsGwIDAQABo4H4MIH1MA4GA1UdDwEB/wQEAwIB
hjAdBgNVHSUEFjAUBggrBgEFBQcDAgYIKwYBBQUHAwEwEgYDVR0TAQH/BAgwBgEB
/wIBADAdBgNVHQ4EFgQUALUp8i2ObzHom0yteD763OkM0dIwHwYDVR0jBBgwFoAU
ebRZ5nu25eQBc4AIiMgaWPbpm24wMgYIKwYBBQUHAQEEJjAkMCIGCCsGAQUFBzAC
hhZodHRwOi8veDEuaS5sZW5jci5vcmcvMBMGA1UdIAQMMAowCAYGZ4EMAQIBMCcG
A1UdHwQgMB4wHKAaoBiGFmh0dHA6Ly94MS5jLmxlbmNyLm9yZy8wDQYJKoZIhvcN
AQELBQADggIBAI910AnPanZIZTKS3rVEyIV29BWEjAK/duuz8eL5boSoVpHhkkv3
4eoAeEiPdZLj5EZ7G2ArIK+gzhTlRQ1q4FKGpPPaFBSpqV/xbUb5UlAXQOnkHn3m
FVj+qYv87/WeY+Bm4sN3Ox8BhyaU7UAQ3LeZ7N1X01xxQe4wIAAE3JVLUCiHmZL+
qoCUtgYIFPgcg350QMUIWgxPXNGEncT921ne7nluI02V8pLUmClqXOsCwULw+PVO
ZCB7qOMxxMBoCUeL2Ll4oMpOSr5pJCpLN3tRA2s6P1KLs9TSrVhOk+7LX28NMUlI
usQ/nxLJID0RhAeFtPjyOCOscQBA53+NRjSCak7P4A5jX7ppmkcJECL+S0i3kXVU
y5Me5BbrU8973jZNv/ax6+ZK6TM8jWmimL6of6OrX7ZU6E2WqazzsFrLG3o2kySb
zlhSgJ81Cl4tv3SbYiYXnJExKQvzf83DYotox3f0fwv7xln1A2ZLplCb0O+l/AK0
YE0DS2FPxSAHi0iwMfW2nNHJrXcY3LLHD77gRgje4Eveubi2xxa+Nmk/hmhLdIET
iVDFanoCrMVIpQ59XWHkzdFmoHXHBV7oibVjGSO7ULSQ7MJ1Nz51phuDJSgAIU7A
0zrLnOrAj/dfrlEWRhCvAgbuwLZX1A2sjNjXoPOHbsPiy+lO1KF8/XY7
-----END CERTIFICATE-----
)string_literal";

time_t next_download_satellite_time = 0;
time_t rounding_time;
char url[1024];
char path[1024];

void https_set_url_and_path() {
  rounding_time /= (15 * 60); // round to 15 minutes
  rounding_time *= (15 * 60);
  next_download_satellite_time = rounding_time + (15 * 60);
  rounding_time -= (30 * 60); // satellite image delayed 30 minutes
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
}
