#ifndef BASE64_H
#define BASE64_H

#include "pico/stdlib.h"
#include <stdint.h>

char *base64_encode(const unsigned char *data, uint8_t input_length, uint16_t *output_length);

#endif
