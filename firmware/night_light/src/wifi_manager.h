#pragma once

// Connect to Wi-Fi in station mode.
// Returns true when connected, false after the retry limit.
bool wifiConnect(const char* ssid, const char* password);
