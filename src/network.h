#ifndef NETWORK_H
#define NETWORK_H

#include "lwip/pbuf.h"
#include "lwip/altcp_tcp.h"
#include "lwip/altcp_tls.h"
#include "lwip/dns.h"

err_t tls_client_close();
err_t tls_client_connected(void *arg, struct altcp_pcb *pcb, err_t err);
err_t tls_client_send_data_raw(char *msg);
err_t tls_client_send_data(char *msg);
void tls_client_err(void *arg, err_t err);
err_t tls_client_recv(void *arg, struct altcp_pcb *pcb, struct pbuf *p, err_t err);
void tls_client_connect_to_server_ip(const ip_addr_t *ipaddr);
void tls_client_dns_found(const char *hostname, const ip_addr_t *ipaddr, void *arg);
bool tls_client_open(const char *hostname);
bool tls_client_init(void);

#endif
