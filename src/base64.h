#ifndef BASE64_H
#define BASE64_H

#include "pico/stdlib.h"
#include <stddef.h>
#include <stdint.h>

size_t b64_encoded_size(size_t len);
char *b64_encode(char *in, size_t len);

#endif
