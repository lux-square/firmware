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
  message = " Hello World";

  cursor.x = 0;
  cursor.y = 0;
}

void LuxDisplay::matrix_clear()
{
  // FastLED.clear does not work properly with multiple matrices connected via parallel inputs
  // on ESP8266 (not sure about other chips).
  memset(static_cast<void *>(leds), 0, NUMMATRIX * 3);
}

void LuxDisplay::display_circle()
{
  matrix_clear();
  matrix->drawCircle(mw / 2, mh / 2, 2, LED_RED_MEDIUM);

  matrix->show();
}

void LuxDisplay::setup()
{
  FastLED.addLeds<NEOPIXEL, LED_DATA_PIN>(leds, NUMMATRIX).setCorrection(TypicalLEDStrip);

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
  matrix_clear();
  matrix->setCursor(cursor.x, cursor.y);
  matrix->print(message);

  if (--cursor.x < -(message.length() * CHAR_WIDTH))
  {
    cursor.x = 0;
  }
  matrix->show();
  delay(250);
}
