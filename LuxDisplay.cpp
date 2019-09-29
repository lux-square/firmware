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
  queueData.size = 13;
  queueData.color = 11111;
  strcpy(queueData.text, " Hello World ");

  cursor.x.offset = 0;
  cursor.x.rate = -0.15;
  cursor.x.start = 0;
  cursor.x.end = -12 * 6;

  cursor.y.offset = 0;
  cursor.y.rate = 0;
  cursor.y.start = 0;
  cursor.y.end = 0;
}

void LuxDisplay::setup(QueueHandle_t queueHandle, portMUX_TYPE *mux)
{
  queue = queueHandle;
  timerMux = mux;
  FastLED.addLeds<NEOPIXEL, LED_DATA_PIN>(leds, NUMMATRIX).setCorrection(TypicalLEDStrip);

  currentFrame = 1;
  finalFrame = 300;
  scrollSpeed = 1;

  delay(10);
  matrix->begin();
  matrix->setTextWrap(false);
  matrix->setBrightness(BRIGHTNESS);
  delay(10);
}

void LuxDisplay::loop()
{
  updateFrame();

  displayFrame();

  // consumeQueue();
}

void LuxDisplay::matrixClear()
{
  // FastLED.clear does not work properly with multiple matrices connected via parallel inputs
  // on ESP8266 (not sure about other chips).
  memset(static_cast<void *>(leds), 0, NUMMATRIX * 3);
}

/*
 * description: 
 *  calculates the x and y offset as a function of the current frame
 * params:
 *  void
 * return: 
 *  void
 */
void LuxDisplay::updateCursor()
{
  cursor.x.offset = (int)floor(currentFrame * cursor.x.rate);
  if (cursor.x.offset <= cursor.x.end)
  {
    cursor.x.offset = cursor.x.start;
  }

  cursor.y.offset = (int)floor(currentFrame * cursor.y.rate);
  if (cursor.y.offset <= cursor.y.end)
  {
    cursor.y.offset = cursor.y.start;
  }
}

/*
 * DESCRIPTION:
 *  Prints text and image to the matrix
 * PARAMS:
 *  void
 * RETURN:
 *  void
 */
void LuxDisplay::displayFrame()
{
  updateCursor();

  matrixClear();
  matrix->setCursor(cursor.x.offset, cursor.y.offset);

  // TODO displayImage();
  displayText();

  matrix->show();
}

void LuxDisplay::displayText()
{
  matrix->print(queueData.text);
}

void LuxDisplay::consumeQueue()
{
  uint16_t messagesWaiting = uxQueueMessagesWaiting(queue);
  if (messagesWaiting > 0)
  {
    char data[MAX_QUEUE_SIZE];
    Serial.print("messagesWaiting: ");
    Serial.println(messagesWaiting);
    for (uint16_t i = 0; i < messagesWaiting; i++)
    {
      xQueueReceive(queue, &data[i], 0);
    }
    Serial.print("Display: ");
    Serial.println(data);

    deserializeJson(jsonDoc, data);

    queueData.color = jsonDoc["color"];

    cursor.x.offset = jsonDoc["cursor"]["x"][0];
    cursor.x.rate = jsonDoc["cursor"]["x"][1];
    cursor.x.start = jsonDoc["cursor"]["x"][2];
    cursor.x.end = jsonDoc["cursor"]["x"][3];

    cursor.y.offset = jsonDoc["cursor"]["y"][0];
    cursor.y.rate = jsonDoc["cursor"]["y"][1];
    cursor.y.start = jsonDoc["cursor"]["y"][2];
    cursor.y.end = jsonDoc["cursor"]["y"][3];

    strcpy(queueData.text, jsonDoc["text"]);
    xQueueReset(queue);

    // Serial.println();
    // Serial.println("Begin json");
    // Serial.println(cursor.x.offset);
    // Serial.println(cursor.x.rate);
    // Serial.println(cursor.x.start);
    // Serial.println(cursor.x.end);
    // Serial.println("End json");
    // Serial.println();
  }
};

void LuxDisplay::updateFrame()
{
  if (++currentFrame > finalFrame)
  {
    currentFrame = 1;
  }
}
