#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "hardware/timer.h"
#include "lwip/err.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "hardware/gpio.h"
#include "hardware/sync.h"

#include "defs.h"

#include "network.h"
#include "spotify.h"

struct tcp_pcb *tpcb;

err_t tcp_client_connect(void *arg, struct tcp_pcb *tpcb, err_t err){
  printf("Connected to API\n");
  sync_playing();
  return ERR_OK;
}

struct tcp_pcb* tcp_client_init(int ip_1, int ip_2, int ip_3, int ip_4){
  struct ip_addr_t ip;
  IP4_ADDR(&ip, ip_1, ip_2, ip_3, ip_4);

  tpcb = tcp_new();

  if(!tpcb){
    printf("Failed to create PCB\n");
    return NULL;
  }

  tcp_arg(tpcb, NULL); // No extra state is carried

  tcp_err(tpcb, tcp_client_err);
  tcp_sent(tpcb, tcp_client_sent);
  tcp_recv(tpcb, tcp_client_recv);

  tcp_connect(tpcb, &ip, 80, tcp_client_connect);
}


err_t tcp_client_close(void *arg){
  struct tcp_pcb *tpcb = (struct tcp_pcb*) arg;

  err_t err = ERR_OK;
  if(tpcb != NULL){
    tcp_arg(tpcb, NULL);
    tcp_sent(tpcb, NULL);
    tcp_recv(tpcb, NULL);
    tcp_err(tpcb, NULL);
    err = tcp_close(tpcb);
    if (err != ERR_OK) {
      printf("Close failed %d, calling abort\n", err);
      tcp_abort(tpcb);
      err = ERR_ABRT;
    }
    tpcb = NULL;
  }
  return err;
}


/**
 * Called when the client acknowledges the sent data
 *
 * @param arg	    the state struct
 * @param tpcb	  the connection PCB for which data has been acknowledged
 * @param len	    the amount of bytes acknowledged
 */
err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
  TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
  state->sent_len += len;

  if (state->sent_len >= BUF_SIZE) {

    // We should get the data back from the client
    state->recv_len = 0;
    printf("Waiting for buffer from client\n");
  }

  return ERR_OK;
}

/**
 * Function to send the data to the client
 *
 * @param arg	        the state struct
 * @param tcp_pcb     the client PCB
 * @param data	      the data to send
 */
err_t tcp_client_send_data(void *arg, struct tcp_pcb *tpcb, char *data)
{
  TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
  
  memset(state->buffer_sent, 0, sizeof(state->buffer_sent));
  memcpy(state->buffer_sent, data, strlen(data));

  state->sent_len = 0;
  printf("Writing %ld bytes to client\n", BUF_SIZE);

  cyw43_arch_lwip_check();

  // Write data for sending but does not send it immediately
  // To force writing we can call tcp_output after tcp_write
  err_t err = tcp_write(tpcb, state->buffer_sent, BUF_SIZE, TCP_WRITE_FLAG_COPY);
  tcp_output(tpcb);
  if (err != ERR_OK) {
    printf("Failed to write data %d\n", err);
    return ERR_VAL;
  }
  return ERR_OK;
}

/**
 * The method called when data is received at the host
 *
 * @param arg	  the state struct
 * @param tpcb	  the connection PCB which received data
 * @param p	  the received data
 * @param err	  an error code if there has been an error receiving
 */
err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
  TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
  if (!p) {
    printf("No data received");
    return ERR_VAL;
  }

  cyw43_arch_lwip_check();
  
  if(p->tot_len > 0){
    printf("Data: %s", ((char*) p->payload));
    printf("tcp_client_recv %d/%d err %d\n", p->tot_len, state->recv_len, err);

    // Receive the buffer
    const uint16_t buffer_left = BUF_SIZE - state->recv_len;
    state->recv_len += pbuf_copy_partial(p, state->buffer_recv + state->recv_len,
					 p->tot_len > buffer_left ? buffer_left : p->tot_len, 0);

    // Called once data has been processed to advertise a larger window
    tcp_recved(tpcb, p->tot_len);
  }
  pbuf_free(p);

  return ERR_OK;
}

/**
 * The function called when a critical error occurs
 *
 * @param arg	  the state struct
 * @param err	  the error code
 */
void tcp_client_err(void *arg, err_t err) {
  if (err != ERR_ABRT) {
    printf("TCP Client ERROR %d\n", err);
  }
}

