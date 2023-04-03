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
  struct ip_addr ip;
  IP4_ADDR(&ip, 35,186,224,25);
  pcb = tcp_new();

  tcp_arg(pcb, &pcb);
  tcp_err(pcb, tcp_server_err);
  tcp_recv(pcb, tcp_server_recv);
  tcp_sent(pcb, tcp_server_sent);

  tcp_connect(pcb, &ip, 80, tcp_server_connect);
  
  sync_playing();
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
