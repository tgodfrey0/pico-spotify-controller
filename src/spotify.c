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
  if(!initialised) return;

  // Init lwjson
  lwjson_init(&lwjson, tokens, LWJSON_ARRAYSIZE(tokens));

  tls_client_send_data(cmd_get_playback);
}

void sync_playback(){
  if(!initialised) return;
  tls_client_send_data(cmd_get_playback);
}

void togglePlayback(){
  if(!initialised) return;
  if(playing){
    pause();
  } else {
    play();
  }
}

void play(){
  if(!initialised) return;
  tls_client_send_data(cmd_play);
}

void pause(){
  if(!initialised) return;
  tls_client_send_data(cmd_pause);
}

void next(){
  if(!initialised) return;
  tls_client_send_data(cmd_next);
}

void previous(){
  if(!initialised) return;
  tls_client_send_data(cmd_previous);
}

void parse_response(void *arg){
  char *res = (char*)arg;

  if (lwjson_parse(&lwjson, res) == lwjsonOK) {
    const lwjson_token_t* t;

    /* Find custom key in JSON */
    if ((t = lwjson_find(&lwjson, "is_playing")) != NULL) {
      printf("Token found\n");
      if(t->type == LWJSON_TYPE_TRUE){
        playing = true;
      } else if(t->type == LWJSON_TYPE_FALSE){
        playing = false;
      } else {
        printf("Invalid is_playing value\n");
      }
    } else {
      printf("Token not found!\n");
    }

  } else {
    printf("Failed to parse response\n");
  }
}
