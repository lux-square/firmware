#ifndef lux_common
#define lux_common

#include <Arduino.h>
#include <string.h>
#include "time.h"

const uint16_t MAX_QUEUE_SIZE = 1024;
const uint8_t FRAME_RATE = 60;

struct QueueData
{
    char text[1024];
    uint16_t color;
    uint16_t size;
};

void printUTCTime();

#endif
