#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

#include "web_server.h"
#include "state.h"
#include "web/dashboard.h"   // INDEX_HTML (stored in flash)


static WebServer server(80);


// ------------------------------------------------------------
// GET /  -> the dashboard page
// ------------------------------------------------------------

static void handleRoot() {

  server.send_P(
    200,
    "text/html",
    INDEX_HTML
  );
}


// ------------------------------------------------------------
// GET /api  -> live readings as JSON (see docs/api.md)
// ------------------------------------------------------------

static void handleAPI() {

  String json = "{";

  json += "\"rawLight\":" + String(state.rawLight) + ",";
  json += "\"rawPot\":" + String(state.rawPot) + ",";

  json += "\"brightness\":" + String(state.brightness, 1) + ",";
  json += "\"darkness\":" + String(state.darkness, 1) + ",";

  json += "\"threshold\":" + String(state.threshold, 1) + ",";

  json += "\"nightMode\":";
  json += state.nightMode ? "true" : "false";
  json += ",";

  json += "\"ledBrightness\":" + String(state.ledBrightness) + ",";

  json += "\"uptime\":" + String(millis());

  json += "}";

  server.send(
    200,
    "application/json",
    json
  );
}


void webBegin() {

  server.on("/", HTTP_GET, handleRoot);
  server.on("/api", HTTP_GET, handleAPI);

  server.begin();

  Serial.println("Web server started.");
  Serial.println();
  Serial.println("Open this address in your browser:");
  Serial.print("http://");
  Serial.println(WiFi.localIP());
}


void webHandle() {

  server.handleClient();
}
