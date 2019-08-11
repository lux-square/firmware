#ifndef web_server_h
#define web_server_h

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <FirebaseESP32.h>

#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

class LuxServer
{
public:
  LuxServer();
  void setup();
  void loop();

private:
  // WiFiServer *server;
  FirebaseData *firebaseData;
  const char *ssid;
  const char *password;

  String header;
  String LED1;
  String LED2;
};

const String html_header = R"rawText(
HTTP/1.1 200 OK
Content-type:text/html
Connection: close

)rawText";

const String html_body = R"rawText(
<!DOCTYPE html><html>
<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">
  <link rel=\"icon\" href=\"data:,\">
  <style>
    html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}
    .button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;
    text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}
    .button2 {background-color: #555555;}
  </style>
</head>
<body>
  <h1>ESP32 Web Server</h1>

)rawText";

#endif