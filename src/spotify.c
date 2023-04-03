#include "pico/stdlib.h"
#include "pico/stdio.h"

#include "spotify.h"
#include "network.h"
#include "defs.h"

static bool playing = false;

void toggle_playback(){
  if(playing){
    pause();
  } else {
    play();
  }
  playing = !playing;
}

void play(){
  printf("PLAY");
}

void pause(){
  printf("PLAY");
}

void next(){
  printf("PLAY");
}

void previous(){
  printf("PLAY");
}
