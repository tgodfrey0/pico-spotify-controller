#ifndef SPOTIFY_H
#define SPOTIFY_H

#include "lwjson/lwjson.h"

void parse_response(void *arg);
void get_token(void);
void renew_token(void);
void spotify_init(void);
void sync_playback(void);
void togglePlayback(uint gpio, uint32_t events);
void play();
void pause();
void next(uint gpio, uint32_t events);
void previous(uint gpio, uint32_t events);

#endif
