#include "lwip/tcp.h"
#include "pico/stdlib.h"
#include "pico/stdio.h"

#include "spotify.h"
#include "network.h"
#include "defs.h"

static bool playing = false;
static struct tcp_pcb *pcb = NULL;

static void send(char* cmd){
  
}

void sync_playing(){
  
}

void init_spotify(){
}

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
