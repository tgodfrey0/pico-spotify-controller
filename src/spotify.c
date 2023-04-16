#include "hardware/timer.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/sync.h"
#include "pico/time.h"
#include "src/spotify.h"

#include "lwjson/lwjson.h"

#include "base64.h"
#include "network.h"
#include <stdbool.h>
#include <stdint.h>

extern char* authorisation_code;
extern char *client_id;
extern char *client_secret;

char *access_token = NULL;
char *token_type = NULL;
uint16_t token_expiry;
char *refresh_token = NULL;

extern bool ready;

extern char *server;
extern char *cmd_get_playback;
extern char *cmd_play;
extern char *cmd_pause;
extern char *cmd_next;
extern char *cmd_previous;

static lwjson_token_t tokens[128];
static lwjson_t lwjson;

bool initialised = false;

/**
* Use the refresh token to get a new access token
*/
void renew_token(){}

/**
* Get an access token using the authorisation token
*/
void get_token(){}

/**
* Initialise the spotify structures and get a token
*/
void spotify_init(){}

void play(){}

void pause(){}

void next(){}

void previous(){}

/**
* Parse the response received from the API server
*/
void parse_response(void *arg){}
