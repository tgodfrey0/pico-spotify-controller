#ifndef DEFS_H
#define DEFS_H

#include "pico/stdlib.h"
#include <stdint.h>

#define TLS_CLIENT_HTTP_REQUEST  "GET /api/ip HTTP/1.1\r\n" \
	"Host: " TLS_CLIENT_server "\r\n" \
	"Connection: close\r\n" \
	"\r\n"
#define TLS_CLIENT_TIMEOUT_SECS  15

// Constants
#define SWITCH_A_PLAYBACK     16
#define SWITCH_B              17
#define SWITCH_X_NEXT         19
#define SWITCH_Y_PREVIOUS     20

#define LED_R                 9
#define LED_G                 10
#define LED_B                 11

// Request Strings
const char* const server = "api.spotify.com";

const char* const cmd_get_playback = "GET /v1/me/player";
const char* const cmd_play = "PUT /v1/me/player/play";
const char* const cmd_pause = "PUT /v1/me/player/pause";
const char* const cmd_next = "POST /v1/me/player/next";
const char* const cmd_previous = "POST /v1/me/player/previous";

#endif /* DEFS_H */
