#include "pico/stdlib.h"
#include "src/spotify.h"

#include "lwjson/lwjson.h"

#include "network.h"

extern char *cmd_get_playback;
extern char *cmd_play;
extern char *cmd_pause;
extern char *cmd_next;
extern char *cmd_previous;

static lwjson_token_t tokens[128];
static lwjson_t lwjson;

volatile bool playing = false;
bool initialised = false;

void spotify_init(){
  if(initialised) return;

  // Init lwjson
  lwjson_init(&lwjson, tokens, LWJSON_ARRAYSIZE(tokens));

  tls_client_send_data(cmd_get_playback);
}

void togglePlayback(){}
void play(){}
void pause(){}
void next(){}
void previous(){}

void parse_response(void *arg){
  char *res = (char*)arg;

  if (lwjson_parse(&lwjson, res) == lwjsonOK) {
    const lwjson_token_t* t;
    printf("JSON parsed..\r\n");

    /* Find custom key in JSON */
    if ((t = lwjson_find(&lwjson, "mykey")) != NULL) {
      printf("Key %s found with data type: %d\r\n", t->token_value, (int)t->type);
    }

  }
}
