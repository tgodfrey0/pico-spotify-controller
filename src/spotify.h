#ifndef SPOTIFY_H
#define SPOTIFY_H

void parse_response(void *arg);

void spotify_init(void);
void togglePlayback(void);
void play(void);
void pause(void);
void next(void);
void previous(void);

#endif
