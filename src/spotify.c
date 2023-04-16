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

/**
* Get an access token using the authorisation token
*/
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

/**
* Initialise the spotify structures and get a token
*/
void spotify_init(){
  if(initialised) return;
  // Init lwjson
  access_token = calloc(512, sizeof(char));
  refresh_token = calloc(512, sizeof(char));
  lwjson_init(&lwjson, tokens, LWJSON_ARRAYSIZE(tokens));
  get_token();
}

void play(){
  if(!initialised) return;
  uint32_t state = save_and_disable_interrupts();
  tls_client_send_data_with_body(cmd_play, "application/json", "{\"position_ms\": 0}");
  restore_interrupts(state);
}

void pause(){
  if(!initialised) return;
  uint32_t state = save_and_disable_interrupts();
  tls_client_send_data(cmd_pause);
  restore_interrupts(state);
}

void next(){
  if(!initialised) return;
  uint32_t state = save_and_disable_interrupts();
  busy_wait_ms(500);
  tls_client_send_data(cmd_next);
  restore_interrupts(state);
}

void previous(){
  if(!initialised) return;
  uint32_t state = save_and_disable_interrupts();
  tls_client_send_data(cmd_previous);
  restore_interrupts(state);
}

/**
* Parse the response received from the API server
*/
void parse_response(void *arg){
  if(!arg){
    printf("Null data passed to parser");
    return;
  }

  char *res = (char*)arg;

  char status[4];
  status[0] = res[9];
  status[1] = res[10];
  status[2] = res[11];
  status[3] = '\0';
  printf("Status code: %s\n", status);
  if(strncmp(status, "204", 3) == 0){
    printf("Successful response - no content\n");
    return;
  } else if(strncmp(status, "2xx", 1) == 0){
    printf("Successful response\n");
  } else if(strncmp(status, "401", 3) == 0){
    printf("Bad token\n");
    renew_token();
    return;
  } else if(strncmp(status, "4xx", 1) == 0){
    printf("Error response\n");
    return;
  }

  char *body;
  for(uint16_t i = 0; i < strlen(res); i += 4){
    if(res[i] == '\r' && res[i+1] == '\n' && res[i+2] == '\r' && res[i+3] == '\n'){
      body = &res[i+4];
      break;
    }
  }

  if(strcmp(body, "empty response") == 0){
    printf("Empty response\n");
    return;
  }

  if (lwjson_parse(&lwjson, body) == lwjsonOK) {
    const lwjson_token_t* t;

    /* Find custom key in JSON */
    if ((t = lwjson_find(&lwjson, "error_description")) != NULL) {
      char *e = t->u.str.token_value;
      e[t->u.str.token_value_len] = '\0';
      printf("ERROR: %s\n", e);
    } else if((t = lwjson_find(&lwjson, "access_token")) != NULL){
      strncpy(access_token, t->u.str.token_value, t->u.str.token_value_len);
      strncat(access_token, "\0", 1);

      if((t = lwjson_find(&lwjson, "expires_in")) != NULL){
        token_expiry = t->u.num_int;
      } else {
        printf("Cannot find token expiration\n");
        return;
      }

      if((t = lwjson_find(&lwjson, "refresh_token")) != NULL){
        strncpy(refresh_token, t->u.str.token_value, t->u.str.token_value_len);
        strncat(refresh_token, "\0", 1);
      }

      printf("\nToken: %s\nExpires in: %d\nRefresh Token: %s\n\n", access_token, token_expiry, refresh_token);
      if(!ready) ready = true;
      if(!initialised) initialised = true;
    } else {
      printf("Token not found!\n");
    }
  } else {
    printf("Failed to parse response\n");
  }
}
