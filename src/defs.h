#ifndef DEFS_H
#define DEFS_H

#include "pico/stdlib.h"
#include <stdint.h>

#define TLS_CLIENT_TIMEOUT_SECS  15

// Request Strings
const char* const server = "api.spotify.com";

const char* const cmd_play = "PUT /v1/me/player/play";
const char* const cmd_pause = "PUT /v1/me/player/pause";
const char* const cmd_next = "POST /v1/me/player/next";
const char* const cmd_previous = "POST /v1/me/player/previous";

#endif /* DEFS_H */
