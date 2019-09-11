#ifndef web_server_h
#define web_server_h

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <FirebaseESP32.h>

#include "./common.h"
#include "./constants.h"
#include "./secrets.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

class LuxServer
{
public:
  LuxServer();
  void setup(QueueHandle_t handle);
  void loop();

private:
  // WiFiServer *server;
  FirebaseData *firebaseData;

  void produceQueue();

  // TODO: set these programmaticly
  bool dbUpdate;
  char ssid[64];
  char password[64];
  const char *ntpServer = "pool.ntp.org";

  QueueHandle_t queue;
  char jsonData[MAX_QUEUE_SIZE];
};

#endif