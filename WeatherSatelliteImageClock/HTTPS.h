#pragma once

#include <FS.h>
#include <HTTPClient.h>
#include <NetworkClientSecure.h>

String https_get_string(String uri) {
  Serial.printf("https_get_string(%s)\n", uri.c_str());

  NetworkClientSecure *client = new NetworkClientSecure;
  String payload;
  if (client) {
    client->setCACert(rootCACertificate);
    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before NetworkClientSecure *client is
      HTTPClient https;

      Serial.print("[HTTPS] begin...\n");
      if (https.begin(*client, uri)) {  // HTTPS
        Serial.print("[HTTPS] GET...\n");
        // start connection and send HTTP header
        int httpCode = https.GET();

        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            payload = https.getString();
          }
        } else {
          Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }

        https.end();
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
      }
      // End extra scoping block
    }

    delete client;
  } else {
    Serial.println("Unable to create client");
  }
  return payload;
}

uint8_t *https_response_buf;
int https_response_len;
void https_get_response_buf(String uri) {
  Serial.printf("https_request(%s)\n", uri.c_str());

  NetworkClientSecure *client = new NetworkClientSecure;
  if (client) {
    client->setCACert(rootCACertificate);
    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before NetworkClientSecure *client is
      HTTPClient https;

      Serial.print("[HTTPS] begin...\n");
      if (https.begin(*client, uri)) {  // HTTPS
        Serial.print("[HTTPS] GET...\n");
        // start connection and send HTTP header
        int httpCode = https.GET();

        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            // String payload = https.getString();
            // Serial.println(payload);
            https_response_len = https.getSize();
            https_response_buf = (uint8_t *)malloc(https_response_len);
            if (https_response_buf) {
              int read = 0;
              int loop_count = 0;
              while ((read < https_response_len) && (loop_count < 2000)) {
                if (client->available()) {
                  read += client->readBytes(https_response_buf + read, https_response_len - read);
                  Serial.printf("https_response_len: %d, read: %d\n", https_response_len, read);
                } else {
                  delay(50);
                }
                ++loop_count;
              }
              Serial.printf("[HTTPS] GET loop_count: %d\n", loop_count);
            }
          }
        } else {
          Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }

        https.end();
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
      }
      // End extra scoping block
    }

    delete client;
  } else {
    Serial.println("Unable to create client");
  }
}

#define FILE_BUFFER_SIZE 4096
uint8_t file_buf[FILE_BUFFER_SIZE];
void https_fs_download(String uri, fs::FS &fs, String path) {
  Serial.printf("https_fs_download(%s, %s)\n", uri.c_str(), path.c_str());

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.printf("file open %s failed!\n", path.c_str());
  } else {
    NetworkClientSecure *client = new NetworkClientSecure;
    if (client) {
      client->setCACert(rootCACertificate);
      {
        // Add a scoping block for HTTPClient https to make sure it is destroyed before NetworkClientSecure *client is
        HTTPClient https;

        Serial.print("[HTTPS] begin...\n");
        if (https.begin(*client, uri)) {  // HTTPS
          Serial.print("[HTTPS] GET...\n");
          // start connection and send HTTP header
          int httpCode = https.GET();

          // httpCode will be negative on error
          if (httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

            // file found at server
            if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
              int remain = https.getSize();
              int read = 0;
              int loop_count = 0;
              while (remain && (loop_count < 2000)) {
                if (client->available()) {
                  read = client->readBytes(file_buf, (remain > FILE_BUFFER_SIZE) ? FILE_BUFFER_SIZE : remain);
                  file.write(file_buf, read);
                  remain -= read;
                  // Serial.printf("read: %d, remain: %d\n", read, remain);
                } else {
                  delay(50);
                }
                ++loop_count;
              }
              Serial.printf("[HTTPS] GET remain: %d, loop_count: %d\n", remain, loop_count);
            }
          } else {
            Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
          }

          https.end();
        } else {
          Serial.printf("[HTTPS] Unable to connect\n");
        }
        // End extra scoping block
      }

      delete client;
    } else {
      Serial.println("Unable to create client");
    }
    file.flush();
    file.close();
  }
}
