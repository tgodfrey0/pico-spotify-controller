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
char *server = "api.spotify.com";

char *cmd_get_playback = "GET /me/player";
char *cmd_play = "PUT /me/player/play";
char *cmd_pause = "PUT /me/player/pause";
char *cmd_next = "PUT /me/player/next";
char *cmd_previous = "PUT /me/player/previous";

#endif /* DEFS_H */
