/***************************************************************************************
* Title: LuxSquare
* Author: Rowan Baker-French
* Date: August 8, 2019
* Code version: 0.0.0
***************************************************************************************/
#include <Arduino.h>
#include "./common.h"
#include "./LuxServer.h"
#include "./LuxDisplay.h"

volatile bool nextFrameFlag;
hw_timer_t *frameRateTimer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

TaskHandle_t WebHandle, DisplayHandle;
QueueHandle_t queueHandle;

static LuxDisplay lDisplay;
LuxServer lServer;

void IRAM_ATTR updateFrame();

void setup()
{
  Serial.begin(115200);

  nextFrameFlag = false;

  queueHandle = xQueueCreate(1024, sizeof(String)); // max of 1024 chars
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

  // xTaskCreatePinnedToCore(
  //     displayTask,                    /* Function to implement the task */
  //     "DisplayTask",                  /* Name of the task */
  //     10000,                          /* Stack size in words */
  //     static_cast<void *>(&lDisplay), /* Task input parameter */
  //     1,                              /* Priority of the task */
  //     &DisplayHandle,                 /* Task handle. */
  //     0                               /* Core where the task should run */
  // );
  // delay(10);

  xTaskCreatePinnedToCore(
      webTask,                       /* Function to implement the task */
      "WebTask",                     /* Name of the task */
      10000,                         /* Stack size in words */
      static_cast<void *>(&lServer), /* Task input parameter */
      1,                             /* Priority of the task */
      &WebHandle,                    /* Task handle. */
      1                              /* Core where the task should run */
  );
  delay(10);
}

void loop()
{
  if (nextFrameFlag)
  {
    lDisplay.loop();
    nextFrameFlag = false;
  }
};

// void displayTask(void *pvParameters)
// {
//   LuxDisplay display = *(static_cast<LuxDisplay *>(pvParameters));
//   while (true)
//   {
//     TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
//     TIMERG0.wdt_feed = 1;
//     TIMERG0.wdt_wprotect = 0;
//     // handle display here
//     display.loop();
//   }
// }

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
