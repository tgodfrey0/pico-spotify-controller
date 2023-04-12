#ifndef SPOTIFY_H
#define SPOTIFY_H

#include "lwjson/lwjson.h"

void parse_response(void *arg);

void spotify_init(void);
void sync_playback(void);
void togglePlayback(void);
void play(void);
void pause(void);
void next(void);
void previous(void);

#endif
