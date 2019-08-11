#ifndef lux_display_h
#define lux_display_h

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <FastLED_NeoMatrix.h>
#include <FastLED.h>
#include <LEDMatrix.h>

#define LED_RED_MEDIUM (15 << 11)

// Used by LEDMatrix
const uint8_t MATRIX_TILE_WIDTH = 8;  // width of EACH NEOPIXEL MATRIX (not total display)
const uint8_t MATRIX_TILE_HEIGHT = 8; // height of each matrix
const uint8_t MATRIX_TILE_H = 1;      // number of matrices arranged horizontally
const uint8_t MATRIX_TILE_V = 1;      // number of matrices arranged vertically

// Used by NeoMatrix
const uint8_t mw = (MATRIX_TILE_WIDTH * MATRIX_TILE_H);
const uint8_t mh = (MATRIX_TILE_HEIGHT * MATRIX_TILE_V);
const uint8_t NUMMATRIX = (mw * mh);

// Compat for some other demos
const uint8_t NUM_LEDS = NUMMATRIX;
const uint8_t MATRIX_HEIGHT = mh;
const uint8_t MATRIX_WIDTH = mw;

const uint8_t BRIGHTNESS = 8;

const uint8_t LED_DATA_PIN = 26;
const uint8_t CHAR_WIDTH = 6;
const uint8_t CHAR_HEIGHT = 9;

struct POS
{
    int8_t x;
    int8_t y;
};

class LuxDisplay
{
public:
    LuxDisplay();
    void setup();
    void loop();

    struct POS cursor;

private:
    CRGB *leds;
    FastLED_NeoMatrix *matrix;

    void matrix_clear();
    void display_circle();
    void display_text(String text, struct POS coordinates);
    String message;
};

#endif