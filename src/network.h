#ifndef NETWORK_H
#define NETWORK_H

#include "lwip/err.h"
#include "defs.h"

err_t tcp_client_connect(void *arg, struct tcp_pcb *tpcb, err_t err);
err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
void tcp_client_err(void *arg, err_t err);
struct tcp_pcb* tcp_client_init(int ip_1, int ip_2, int ip_3, int ip_4);
err_t tcp_client_send_data(void *arg, struct tcp_pcb *tpcb, char *data);
err_t tcp_client_close(void *arg);

#endif
