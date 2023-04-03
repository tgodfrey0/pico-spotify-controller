#ifndef NETWORK_H
#define NETWORK_H

#include "lwip/err.h"
#include "defs.h"

err_t tcp_server_close(void *arg);
err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
err_t tcp_server_send_data(void *arg, struct tcp_pcb *tpcb, uint8_t data[]);
err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) ;
void tcp_server_err(void *arg, err_t err) ;
err_t tcp_server_accept(void *arg, struct tcp_pcb *client_pcb, err_t err) ;
TCP_SERVER_T* init_server(void);
void listen(TCP_SERVER_T *state);

#endif
