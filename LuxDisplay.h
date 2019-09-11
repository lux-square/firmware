#ifndef lux_display_h
#define lux_display_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <FastLED_NeoMatrix.h>
#include <FastLED.h>
#include <LEDMatrix.h>

#include "./common.h"
#include "./constants.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp32-hal.h"

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

enum states_t
{
    TEXT,
    IMAGE
};

struct updateFrame
{
    bool x;
    bool y;
};

struct rateData
{
    int8_t offset;
    int8_t rate;
    int8_t start;
    int8_t end;
};

struct cursorData
{
    rateData x;
    rateData y;
};

class LuxDisplay
{
public:
    LuxDisplay();
    void setup(QueueHandle_t handle, portMUX_TYPE *mux);
    void loop();
    void adjustFrame();

    struct cursorData cursor;

private:
    CRGB *leds;
    FastLED_NeoMatrix *matrix;
    portMUX_TYPE *timerMux;
    using JsonDocument = StaticJsonDocument<1 * 1024>; // 1KB
    JsonDocument jsonDoc;

    states_t currentState;
    // Frames are 1-FRAME_RATE
    uint8_t currentFrame;
    uint8_t lastFrame;
    // scroll speed is 1-FRAME_RATE cols per second
    uint8_t scrollSpeed;

    struct updateFrame shouldUpdateFrame();
    void matrixClear();
    void displayFrame();
    void displayText();
    // TODO
    void displayImage();
    void updateFrame();
    void consumeQueue();

    QueueData queueData;
    QueueHandle_t queue;
};

#endif