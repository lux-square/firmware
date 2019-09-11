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
  queueData.size = 100;
  queueData.color = 11111;
  strcpy(queueData.text, " Hello World");

  cursor.x.offset = 0;
  cursor.x.rate = 1;
  cursor.x.start = 0;
  cursor.x.end = 0;

  cursor.y.offset = 0;
  cursor.y.rate = 0;
  cursor.y.start = 0;
  cursor.y.end = 0;

  currentState = TEXT;
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

    if (currentFrame > FRAME_RATE)
    {
      currentFrame = 1;
    }

    displayFrame();
  }

  consumeQueue();
}

void LuxDisplay::matrixClear()
{
  // FastLED.clear does not work properly with multiple matrices connected via parallel inputs
  // on ESP8266 (not sure about other chips).
  memset(static_cast<void *>(leds), 0, NUMMATRIX * 3);
}

struct updateFrame LuxDisplay::shouldUpdateFrame()
{
  struct updateFrame result = {false, false};
  if ((cursor.x.rate != 0) && (currentFrame % (FRAME_RATE / cursor.x.rate) == 0))
  {
    cursor.x.rate > 0 ? ++cursor.x.offset : --cursor.x.offset;
    if (cursor.x.offset == cursor.x.end)
      cursor.x.offset = cursor.x.start;
    result.x = true;
  }
  if ((cursor.y.rate != 0) && (currentFrame % (FRAME_RATE / cursor.y.rate) == 0))
  {
    cursor.y.rate > 0 ? ++cursor.y.offset : --cursor.y.offset;
    if (cursor.y.offset == cursor.y.end)
      cursor.y.offset = cursor.y.start;
    result.y = true;
  }
  return result;
}

void LuxDisplay::displayFrame()
{
  struct updateFrame update = shouldUpdateFrame();
  if (update.x || update.y)
  {
    matrixClear();
    Serial.print(cursor.x.offset);
    Serial.print(" ");
    Serial.println(cursor.y.offset);
    matrix->setCursor(cursor.x.offset, cursor.y.offset);
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
}

void LuxDisplay::displayText()
{
  Serial.println(queueData.text);
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

    // Serial.println(static_cast<const char *>(jsonDoc["cursor"]["x"][0]));
    // Serial.println(static_cast<const char *>(jsonDoc["cursor"]["x"][1]));
    // Serial.println(static_cast<const char *>(jsonDoc["cursor"]["x"][2]));
    // Serial.println(static_cast<const char *>(jsonDoc["cursor"]["x"][3]));

    cursor.x.offset = jsonDoc["cursor"]["x"][0];
    cursor.x.rate = jsonDoc["cursor"]["x"][1];
    if (cursor.x.rate)
    {
      cursor.x.rate = FRAME_RATE / cursor.x.rate;
    }
    cursor.x.start = jsonDoc["cursor"]["x"][2];
    cursor.x.end = jsonDoc["cursor"]["x"][3];

    cursor.y.offset = jsonDoc["cursor"]["y"][0];
    cursor.y.rate = jsonDoc["cursor"]["y"][1];
    if (cursor.y.rate)
    {
      cursor.y.rate = FRAME_RATE / cursor.y.rate;
    }
    cursor.y.start = jsonDoc["cursor"]["y"][2];
    cursor.y.end = jsonDoc["cursor"]["y"][3];

    strcpy(queueData.text, jsonDoc["text"]);
    xQueueReset(queue);

    Serial.println();
    Serial.println("Begin json");
    Serial.println(cursor.x.offset);
    Serial.println(cursor.x.rate);
    Serial.println(cursor.x.start);
    Serial.println(cursor.x.end);
    Serial.println("End json");
    Serial.println();
  }
};

void LuxDisplay::adjustFrame()
{
  // TODO
}

void LuxDisplay::updateFrame()
{
  currentFrame++;
  if (currentFrame > FRAME_RATE)
  {
    currentFrame = 1;
  }
}
