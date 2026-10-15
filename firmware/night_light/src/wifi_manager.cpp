#include <Arduino.h>
#include <WiFi.h>

#include "wifi_manager.h"
#include "config.h"


bool wifiConnect(const char* ssid, const char* password) {

  Serial.println();
  Serial.println("Connecting to Wi-Fi...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int attempts = 0;

  while (
    WiFi.status() != WL_CONNECTED &&
    attempts < Timing::WIFI_MAX_ATTEMPTS
  ) {

    delay(Timing::WIFI_RETRY_DELAY_MS);

    Serial.print(".");

    attempts++;
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {

    Serial.println("Wi-Fi connected!");

    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    return true;
  }

  Serial.println("Wi-Fi connection failed.");

  return false;
}
