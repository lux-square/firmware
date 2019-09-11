/***************************************************************************************
* Title: LuxSquare
* Author: Rowan Baker-French
* Date: August 8, 2019
* Code version: 0.0.0
***************************************************************************************/
#include <Arduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "./common.h"
#include "./LuxServer.h"
#include "./LuxDisplay.h"

volatile bool nextFrameFlag;
uint8_t lastSecond = 0;
hw_timer_t *frameRateTimer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

TaskHandle_t WebHandle, DisplayHandle;
QueueHandle_t queueHandle;

static LuxDisplay lDisplay;
LuxServer lServer;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

void IRAM_ATTR updateFrame();

void setup()
{
  Serial.begin(115200);

  nextFrameFlag = false;

  queueHandle = xQueueCreate(MAX_QUEUE_SIZE, sizeof(char)); // max of 1024 chars
  if (queueHandle == NULL)
  {
    Serial.println("Error creating the queue");
  }

  lServer.setup(queueHandle);
  lDisplay.setup(queueHandle, &timerMux);

  frameRateTimer = timerBegin(0, 80, true);
  timerAttachInterrupt(frameRateTimer, &updateFrame, true);
  timerAlarmWrite(frameRateTimer, 16667, true); // 60x per second
  timerAlarmEnable(frameRateTimer);

  xTaskCreatePinnedToCore(
      webTask,                       /* Function to implement the task */
      "WebTask",                     /* Name of the task */
      20000,                         /* Stack size in words */
      static_cast<void *>(&lServer), /* Task input parameter */
      1,                             /* Priority of the task */
      &WebHandle,                    /* Task handle. */
      1                              /* Core where the task should run */
  );
  delay(10);
  timeClient.begin();
}

void loop()
{
  // Needed to keep NTP time valid
  while (!timeClient.update())
  {
    timeClient.forceUpdate();
  }
  if (nextFrameFlag)
  {
    const uint8_t currentSecond = timeClient.getSeconds();
    if (currentSecond != lastSecond)
    {
      // TODO
      lDisplay.adjustFrame();
    }
    lastSecond = currentSecond;
    lDisplay.loop();
    nextFrameFlag = false;
  }
};

void webTask(void *pvParameters)
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

void IRAM_ATTR updateFrame()
{
  portENTER_CRITICAL_ISR(&timerMux);
  nextFrameFlag = true;
  portEXIT_CRITICAL_ISR(&timerMux);
}
