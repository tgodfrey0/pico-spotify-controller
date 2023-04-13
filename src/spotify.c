#include "hardware/timer.h"
#include "pico/stdlib.h"

#include "src/spotify.h"

#include "lwjson/lwjson.h"

#include "base64.h"
#include "network.h"
#include <stdint.h>

extern char* authorisation_code;
extern char *client_id;
extern char *client_secret;
extern char* access_token;
extern char* token_type;
extern uint16_t token_expiry;
extern char *server;
extern char *cmd_get_playback;
extern char *cmd_play;
extern char *cmd_pause;
extern char *cmd_next;
extern char *cmd_previous;

static lwjson_token_t tokens[128];
static lwjson_t lwjson;

volatile bool playing = false;
bool initialised = false;

void refresh_token(){
  char *encoded_token;
  uint16_t *encoded_token_length;
  char *token_secret;
  sprintf(token_secret, "%s:%s", client_id, client_secret);
  printf("%s\n", token_secret);

  encoded_token = base64_encode(token_secret, strlen(token_secret), encoded_token_length);
  printf("%s\n", encoded_token);

  char msg[1024];

  sprintf(msg, "GET /api/token HTTP/1.1\r\nHost: accounts.spotify.com\r\nAuthorization: Basic \r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n{\n\t'code': '%s',\n\t'redirect_uri': 'http://localhost:8888/callback',\n\t'grant_type': 'authorization_code'\n}\r\n", authorisation_code);
  tls_client_send_data_raw(msg);
}

void spotify_init(){
  if(initialised) return;
  // Init lwjson
  lwjson_init(&lwjson, tokens, LWJSON_ARRAYSIZE(tokens));
  refresh_token();
}

void sync_playback(){
  if(initialised) return;
  tls_client_send_data(cmd_get_playback);
}

void togglePlayback(){
  if(initialised) return;
  if(playing){
    pause();
  } else {
    play();
  }
}

void play(){
  if(initialised) return;
  tls_client_send_data(cmd_play);
}

void pause(){
  if(initialised) return;
  tls_client_send_data(cmd_pause);
}

void next(){
  if(initialised) return;
  tls_client_send_data(cmd_next);
}

void previous(){
  if(initialised) return;
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
    } else if((t = lwjson_find(&lwjson, "access_token")) != NULL){
      access_token = t->u.str.token_value;
      if((t = lwjson_find(&lwjson, "token_type")) != NULL){
        token_type = t->u.str.token_value;
      }
      
      if((t = lwjson_find(&lwjson, "expires_in")) != NULL){
        token_expiry = t->u.num_int;
      }
      sync_playback();
    } else {
      printf("Token not found!\n");
    }

  } else {
    printf("Failed to parse response\n");
  }
}
