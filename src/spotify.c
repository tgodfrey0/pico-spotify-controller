#include "hardware/timer.h"
#include "pico/stdlib.h"

#include "src/spotify.h"

#include "lwjson/lwjson.h"

#include "base64.h"
#include "network.h"
#include <stdbool.h>
#include <stdint.h>

extern char* authorisation_code;
extern char *client_id;
extern char *client_secret;

char *access_token;
char *token_type;
uint16_t token_expiry;
char* refresh_token;

extern bool ready;

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

void renew_token(){
  printf("Renewing token\n");
  char token_secret[strlen(client_id) + 1 + strlen(client_secret)];
  sprintf(token_secret, "%s:%s", client_id, client_secret);

  char *encoded_token = b64_encode(token_secret, strlen(token_secret));

  char body[strlen(refresh_token) + 39];
  sprintf(body, "grant_type=refresh_token&refresh_token=%s", refresh_token);
  uint16_t body_size = strlen(body);

  char msg[strlen(encoded_token) + body_size + 2 + 49];
  sprintf(msg, "POST /api/token HTTP/1.1\r\nHost: accounts.spotify.com\r\nAuthorization: Basic %s\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: %d\r\n\r\n%s\r\n", encoded_token, body_size, body);

  tls_client_send_data_raw(msg);
}

void get_token(){
  char token_secret[strlen(client_id) + 1 + strlen(client_secret)];
  sprintf(token_secret, "%s:%s", client_id, client_secret);

  char *encoded_token = b64_encode(token_secret, strlen(token_secret));

  char body[strlen(authorisation_code) + 79];
  sprintf(body, "grant_type=authorization_code&code=%s&redirect_uri=http://localhost:8888/callback", authorisation_code);
  uint16_t body_size = strlen(body);

  char msg[1024];
  sprintf(msg, "POST /api/token HTTP/1.1\r\nHost: accounts.spotify.com\r\nAuthorization: Basic %s\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: %d\r\n\r\n%s\r\n", encoded_token, strlen(body), body);

  tls_client_send_data_raw(msg);
}

void spotify_init(){
  if(initialised) return;
  // Init lwjson
  lwjson_init(&lwjson, tokens, LWJSON_ARRAYSIZE(tokens));
  get_token();
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
  if(!arg){
    printf("Null data passed to parser");
    return;
  }

  char *res = (char*)arg;

  char *body = strtok(res, "\r\n");
  body = strtok(NULL, "\r\n");
  body = strtok(NULL, "\r\n");
  body = strtok(NULL, "\r\n");
  body = strtok(NULL, "\r\n");
  body = strtok(NULL, "\r\n");
  body = strtok(NULL, "\r\n");
  body = strtok(NULL, "\r\n");
  body = strtok(NULL, "\r\n");
  body = strtok(NULL, "\r\n");
  body = strtok(NULL, "\r\n");
  body = strtok(NULL, "\r\n");
  body = strtok(NULL, "\r\n");
  body = strtok(NULL, "\r\n");
  body = strtok(NULL, "\r\n");

  if (lwjson_parse(&lwjson, body) == lwjsonOK) {
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
      access_token[t->u.str.token_value_len] = '\0';
      if((t = lwjson_find(&lwjson, "token_type")) != NULL){
        token_type = t->u.str.token_value;
        token_type[t->u.str.token_value_len] = '\0';
      } else {
        printf("Cannot find token type\n");
        return;
      }
      
      if((t = lwjson_find(&lwjson, "expires_in")) != NULL){
        token_expiry = t->u.num_int;
      } else {
        printf("Cannot find token expiration\n");
        return;
      }

      if((t = lwjson_find(&lwjson, "refresh_token")) != NULL){
        refresh_token = t->u.str.token_value;
        refresh_token[t->u.str.token_value_len] = '\0';
      }

      printf("\nToken: %s\nExpires in: %d\nRefresh Token: %s\n\n", access_token, token_expiry, refresh_token);
      //sync_playback();
      if(!ready) ready = true;
    } else {
      printf("Token not found!\n");
    }

  } else {
    printf("Failed to parse response\n");
  }
}
