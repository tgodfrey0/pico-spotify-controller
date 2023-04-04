#ifndef NETWORK_H
#define NETWORK_H

#include "lwip/err.h"
#include "defs.h"

err_t altcp_client_connect(void *arg, struct altcp_pcb *pcb, err_t err);
err_t altcp_client_sent(void *arg, struct altcp_pcb *pcb, u16_t len);
err_t altcp_client_recv(void *arg, struct altcp_pcb *pcb, struct pbuf *p, err_t err);
void altcp_client_err(void *arg, err_t err);
struct altcp_pcb* altcp_client_init(char *hostname);
err_t altcp_client_send_data(void *arg, struct altcp_pcb *pcb, char *data);
err_t altcp_client_close(void *arg);

#endif
