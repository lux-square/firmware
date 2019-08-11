/***************************************************************************************
* Title: LuxSquare
* Author: Rowan Baker-French
* Date: August 8, 2019
* Code version: 0.0.0
***************************************************************************************/
#include <Arduino.h>
#include "./LuxServer.h"
#include "./LuxDisplay.h"

TaskHandle_t WebServerHandle, DisplayHandle;

LuxDisplay lDisplay;
LuxServer lServer;

void setup()
{
  Serial.begin(115200);
  delay(10);

  lServer.setup();
  lDisplay.setup();

  xTaskCreatePinnedToCore(
      displayTask,                    /* Function to implement the task */
      "DisplayTask",                  /* Name of the task */
      10000,                          /* Stack size in words */
      static_cast<void *>(&lDisplay), /* Task input parameter */
      1,                              /* Priority of the task */
      &DisplayHandle,                 /* Task handle. */
      0                               /* Core where the task should run */
  );
  delay(10);

  xTaskCreatePinnedToCore(
      webServerTask,                 /* Function to implement the task */
      "WebServerTask",               /* Name of the task */
      10000,                         /* Stack size in words */
      static_cast<void *>(&lServer), /* Task input parameter */
      1,                             /* Priority of the task */
      &WebServerHandle,              /* Task handle. */
      1                              /* Core where the task should run */
  );
  delay(10);
}

void loop()
{
  TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
  TIMERG0.wdt_feed = 1;
  TIMERG0.wdt_wprotect = 0;
};

void displayTask(void *pvParameters)
{
  LuxDisplay display = *(static_cast<LuxDisplay *>(pvParameters));
  while (true)
  {
    TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_feed = 1;
    TIMERG0.wdt_wprotect = 0;
    // handle display here
    display.loop();
  }
}

void webServerTask(void *pvParameters)
{
  LuxServer server = *(static_cast<LuxServer *>(pvParameters));
  while (true)
  {
    TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_feed = 1;
    TIMERG0.wdt_wprotect = 0;
    // handle server code here
    server.loop();
  }
}