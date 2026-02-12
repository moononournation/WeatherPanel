/**
 * @brief Satellite image
 * 45 minutes delay
 * 15 minutes interval
 */
const char *SATELLITE_URL_TEMPLATE = "https://image.nmc.cn/product/%d/%02d/%02d/WXBL/medium/SEVP_NSMC_WXBL_FY4B_ETCC_ACHN_LNO_PY_%d%02d%02d%02d%02d00000.JPG";

#define SATELLITE_IMAGE_X 220
#define SATELLITE_IMAGE_Y 280

const char *SATELLITE_FOLDER_L1 = "/fy4b";
const char *SATELLITE_FOLDER_L2_TEMPLATE = "/fy4b/%d";
const char *SATELLITE_FOLDER_L3_TEMPLATE = "/fy4b/%d/%02d";
const char *SATELLITE_FOLDER_L4_TEMPLATE = "/fy4b/%d/%02d/%02d";
const char *SATELLITE_FILE_TEMPLATE = "/fy4b/%d/%02d/%02d/%02d%02d.jpg";

/* https://www.nmc.cn/publish/satellite/fy4b-visible.htm */
const char *rootCACertificate = R"string_literal(
-----BEGIN CERTIFICATE-----
MIIFjTCCA3WgAwIBAgIEGErM1jANBgkqhkiG9w0BAQsFADBWMQswCQYDVQQGEwJD
TjEwMC4GA1UECgwnQ2hpbmEgRmluYW5jaWFsIENlcnRpZmljYXRpb24gQXV0aG9y
aXR5MRUwEwYDVQQDDAxDRkNBIEVWIFJPT1QwHhcNMTIwODA4MDMwNzAxWhcNMjkx
MjMxMDMwNzAxWjBWMQswCQYDVQQGEwJDTjEwMC4GA1UECgwnQ2hpbmEgRmluYW5j
aWFsIENlcnRpZmljYXRpb24gQXV0aG9yaXR5MRUwEwYDVQQDDAxDRkNBIEVWIFJP
T1QwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIKAoICAQDXXWvNED8fBVnVBU03
sQ7smCuOFR36k0sXgiFxEFLXUWRwFsJVaU2OFW2fvwwbwuCjZ9YMrM8irq93VCpL
TIpTUnrD7i7es3ElweldPe6hL6P3KjzJIx1qqx2hp/Hz7KDVRM8Vz3IvHWOX6Jn5
/ZOkVIBMUtRSqy5J35DNuF++P96hyk0g1CXohClTt7GIH//62pCfCqktQT+x8Rgp
7hZZLDRJGqgG16iI0gNyejLi6mhNbiyWZXvKWfry4t3uMCz7zEasxGPrb382KzRz
EpR/38wmnvFyXVBlWY9ps4deMm/DGIq1lY+wejfeWkU7xzbh72fROdOXW3NiGUgt
hxwG+3SYIElz8AXSG7Ggo7cbcNOIabla1jj0Ytwli3i/+Oh+uFzJlU9fpy25IGvP
a931DfSCt/SyZi4QKPaXWnuWFo8BGS1sbn85WAZkgwGDg8NNkt0yxoekN+kWzqot
aK8KgWU6cMGbrU1tVMoqLUuFG7OA5nBFDWteNfB/O7ic5ARwiRIlk9oKmSJgamNg
TnYGmE69g60dWIolhdLHZR4tjsbftsbhf4oEIRUpdPA+nJCdDC7xij5aqgwJHsfV
PKPtl8MeNPo4+QgO48BdK4PRVmrJtqhUUy54Mmc9gn900PvhtgVguXDbjgv5E1hv
cWAQUhC5wUEJ73IfZzF4/5YFjQIDAQABo2MwYTAfBgNVHSMEGDAWgBTj/i39KNAL
tbq2osS/BqoFjJP7LzAPBgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBBjAd
BgNVHQ4EFgQU4/4t/SjQC7W6tqLEvwaqBYyT+y8wDQYJKoZIhvcNAQELBQADggIB
ACXGumvrh8vegjmWPfBEp2uEcwPenStPuiB/vHiyz5ewG5zz13ku9Ui20vsXiObT
ej/tUxPQ4i9qecsAIyjmHjdXNYmEwnZPNDatZ8POQQaIxffu2Bq41gt/UP+TqhdL
jOztUmCypAbqTuv0axn96/Ua4CUqmtzHQTb3yHQFhDmVOdYLO6Qn+gjYXB74BGBS
ESgoA//vU2YApUo0FmZ8/Qmkrp5nGm9BC2sGE5uPhnEFtC+NiWYzKXZUmhH4J/qy
P5Hgzg0b8zAarb8iXRvTvyUFTeGSGn+ZnzxEk8rUQElsgIfXBDrDMlI1Dlb4pd19
xIsNER9Tyx6yF7Zod1rg1MvIB671Oi6ON7fQAUtDKXeMOZePglr4UeWJoBjnaH9d
Ci77o0cOPaYjesYBx4/IXr9tgFa+iiS6M+qf4TIRnvHST4D2G0CvOJ4RUHlzEhLN
5mydLIhyPDCBBpEi6lmt2hkuIsKNuYyH4Ga8cyNfIWRjgEj1oDwYPZTISEEdQLpe
/v5WOaHIz16eGWRGENoXkbcFgKyLmZJ956LYBws2J+dIeWCKw9cTXPhyQN9Ky8+Z
AAoACxGV2lZFA4gKn2fQ1XmxqI1AbQ3CekD6819kR5LLU7m7Wc5P/dAVUwHY3+vZ
5nbv0CO7O6l5s9UCKc2Jo5YPSjXnTkLAdc0Hz+Ys63su
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
}
