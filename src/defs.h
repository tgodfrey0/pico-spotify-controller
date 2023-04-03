#ifndef DEFS_H
#define DEFS_H

#include "pico/stdlib.h"

// Constants
#define SWITCH_A_PLAYBACK     16
#define SWITCH_B              17
#define SWITCH_X_NEXT         19
#define SWITCH_Y_PREVIOUS     20

#define LED_R                 9
#define LED_G                 10
#define LED_B                 11

#define TCP_PORT              4444
#define BUF_SIZE              2048
#define POLL_TIME_S           10
#define TEST_ITERATIONS       10

// Structs
typedef struct TCP_SERVER_T_ {
  struct tcp_pcb *server_pcb;
  struct tcp_pcb *client_pcb;
  uint8_t buffer_sent[BUF_SIZE];
  uint8_t buffer_recv[BUF_SIZE];
  int sent_len;
  int recv_len;
} TCP_SERVER_T;

// Request Strings
char *api_url = "https://api/spotify.com";

char *cmd_get_playback = "GET /me/player";
char *cmd_play = "PUT /me/player/play";
char *cmd_pause = "PUT /me/player/pause";
char *cmd_next = "PUT /me/player/next";
char *cmd_previous = "PUT /me/player/previous";

#endif /* DEFS_H */
