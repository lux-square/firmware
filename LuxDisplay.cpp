#include "./LuxDisplay.h"

LuxDisplay::LuxDisplay()
{
  cLEDMatrix<
      -MATRIX_TILE_WIDTH, -MATRIX_TILE_HEIGHT,
      HORIZONTAL_ZIGZAG_MATRIX, MATRIX_TILE_H, MATRIX_TILE_V,
      HORIZONTAL_BLOCKS>
      ledmatrix;

  leds = ledmatrix[0];

  matrix = new FastLED_NeoMatrix(
      leds, MATRIX_TILE_WIDTH, MATRIX_TILE_HEIGHT,
      MATRIX_TILE_H, MATRIX_TILE_V,
      NEO_MATRIX_TOP + NEO_MATRIX_LEFT);

  matrix->setTextColor(60605);
  dbMessage = " Hello World";

  currentState = TEXT;
  cursor.x = 0;
  cursor.y = 0;
}

void LuxDisplay::setup(QueueHandle_t queueHandle, portMUX_TYPE *mux)
{
  queue = queueHandle;
  timerMux = mux;
  FastLED.addLeds<NEOPIXEL, LED_DATA_PIN>(leds, NUMMATRIX).setCorrection(TypicalLEDStrip);

  currentFrame = 1;
  lastFrame = 0;
  scrollSpeed = 1;

  delay(100);
  Serial.print("Matrix Size: ");
  Serial.print(mw);
  Serial.print(" ");
  Serial.println(mh);
  matrix->begin();
  matrix->setTextWrap(false);
  matrix->setBrightness(BRIGHTNESS);
  delay(10);
}

void LuxDisplay::loop()
{
  updateFrame();

  if (currentFrame != lastFrame)
  {
    lastFrame = currentFrame;

    if (currentFrame > 60)
    {
      currentFrame = 1;
    }

    if (currentFrame % (60 / scrollSpeed) == 0)
    {
      displayFrame();
    }
  }

  consumeQueue();
}

void LuxDisplay::matrixClear()
{
  // FastLED.clear does not work properly with multiple matrices connected via parallel inputs
  // on ESP8266 (not sure about other chips).
  memset(static_cast<void *>(leds), 0, NUMMATRIX * 3);
}

void LuxDisplay::displayFrame()
{
  matrixClear();
  matrix->setCursor(cursor.x, cursor.y);
  switch (currentState)
  {
  case TEXT:
    Serial.println("TEXT case");
    displayText();
    break;
  case IMAGE:
    Serial.println("IMAGE case");
    // TODO displayImage();
    break;
  default:
    Serial.println("DEFAULT case");
    displayText();
  }

  matrix->show();
}

void LuxDisplay::displayText()
{
  matrix->print(dbMessage);

  if (--cursor.x < -(dbMessage.length() * CHAR_WIDTH))
  {
    cursor.x = 0;
  }
}

void LuxDisplay::consumeQueue()
{
  uint16_t messagesWaiting = uxQueueMessagesWaiting(queue);
  if (messagesWaiting > 0)
  {
    for (uint16_t i = 0; i < messagesWaiting; i++)
    {
      xQueueReceive(queue, &dbMessage, 0);
    }
    Serial.print("Display: ");
    Serial.println(dbMessage);
    xQueueReset(queue);
  }
};

void LuxDisplay::updateFrame()
{
  currentFrame++;
  if (currentFrame > 60)
  {
    currentFrame = 1;
  }
}
