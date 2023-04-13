#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "lwip/altcp.h"
#include "lwip/err.h"
#include "lwip/tcpbase.h"
#include "mbedtls/x509_crt.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/pbuf.h"
#include "lwip/altcp_tcp.h"
#include "lwip/altcp_tls.h"
#include "lwip/dns.h"

#include "network.h"
#include "spotify.h"

extern char* access_token;
extern char* token_type;
extern uint16_t token_expiry;

extern const char *cert;
extern const char *server;
extern struct altcp_pcb *pcb;
extern struct altcp_tls_config *tls_config;

err_t tls_client_close() {
	err_t err = ERR_OK;

	if (pcb != NULL) {
		altcp_arg(pcb, NULL);
		altcp_poll(pcb, NULL, 0);
		altcp_recv(pcb, NULL);
		altcp_err(pcb, NULL);
		err = altcp_close(pcb);
		if (err != ERR_OK) {
			printf("Close failed %d, calling abort\n", err);
			altcp_abort(pcb);
			err = ERR_ABRT;
		}
		pcb = NULL;
	}
	return err;
}

err_t tls_client_send_data_raw(char *msg){
	printf("Sending %s", msg);

	err_t err = altcp_write(pcb, msg, strlen(msg), TCP_WRITE_FLAG_COPY);
	if(err != ERR_OK){
		printf("Error writing data");
		return tls_client_close();
	}

	return ERR_OK;
}

err_t tls_client_send_data(char *data){
	char msg[strlen(data) + strlen(server) + 40];
	sprintf(msg, "%s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", data, server);
	tls_client_send_data_raw(msg);
}

err_t tls_client_connected(void *arg, struct altcp_pcb *pcb, err_t err) {
	if (err != ERR_OK) {
		printf("Connect failed %d\n", err);
		return tls_client_close();
	}
	printf("Connected to server\n");
	spotify_init();
	return ERR_OK;
}

void tls_client_err(void *arg, err_t err) {
	printf("CRITICAL ERROR - %d\n", err);
	pcb = NULL; /* pcb freed by lwip when _err function is called */
}

err_t tls_client_recv(void *arg, struct altcp_pcb *pcb, struct pbuf *p, err_t err) {
	if (!p) {
		printf("connection closed\n");
		return tls_client_close();
	}

	if (p->tot_len > 0) {
		/* For simplicity this examples creates a buffer on stack the size of the data pending here, 
					 and copies all the data to it in one go.
					 Do be aware that the amount of data can potentially be a bit large (TLS record size can be 16 KB),
					 so you may want to use a smaller fixed size buffer and copy the data to it using a loop, if memory is a concern */
		char buf[p->tot_len + 1];

		pbuf_copy_partial(p, buf, p->tot_len, 0);
		buf[p->tot_len] = 0;

		printf("***\nnew data received from server:\n***\n\n%s\n", buf);
		parse_response(p->payload);
		altcp_recved(pcb, p->tot_len);
	}
	pbuf_free(p);

	return ERR_OK;
}

void tls_client_connect_to_server_ip(const ip_addr_t *ipaddr)
{
	err_t err;
	u16_t port = 443;

	printf("connecting to server IP %s port %d\n", ipaddr_ntoa(ipaddr), port);
	err = altcp_connect(pcb, ipaddr, port, tls_client_connected);
	if (err != ERR_OK)
	{
		fprintf(stderr, "error initiating connect, err=%d\n", err);
		tls_client_close();
	}
}

void tls_client_dns_found(const char* hostname, const ip_addr_t *ipaddr, void *arg)
{
	if (ipaddr)
		{
			printf("DNS resolving complete\n");
			tls_client_connect_to_server_ip(ipaddr);
		}
	else
		{
		printf("error resolving hostname %s\n", hostname);
		tls_client_close();
	}
}


bool tls_client_open(const char *hostname) {
	err_t err;
	ip_addr_t server_ip;

	pcb = altcp_tls_new(tls_config, IPADDR_TYPE_ANY);
	if (!pcb) {
		printf("failed to create pcb\n");
		return false;
	}

	altcp_arg(pcb, NULL); // Don't need a state passed around
	altcp_recv(pcb, tls_client_recv);
	altcp_err(pcb, tls_client_err);

	/* Set SNI */
	mbedtls_ssl_set_hostname(altcp_tls_context(pcb), hostname);

	printf("resolving %s\n", hostname);

	// cyw43_arch_lwip_begin/end should be used around calls into lwIP to ensure correct locking.
	// You can omit them if you are in a callback from lwIP. Note that when using pico_cyw_arch_poll
	// these calls are a no-op and can be omitted, but it is a good practice to use them in
	// case you switch the cyw43_arch type later.
	cyw43_arch_lwip_begin();

	err = dns_gethostbyname(hostname, &server_ip, tls_client_dns_found, NULL);
	if (err == ERR_OK){
		tls_client_connect_to_server_ip(&server_ip);
		}
	else if (err != ERR_INPROGRESS){
		printf("error initiating DNS resolving, err=%d\n", err);
		tls_client_close();
	}

	cyw43_arch_lwip_end();

	return err == ERR_OK || err == ERR_INPROGRESS;
}

bool tls_client_init(void) {
	tls_config = altcp_tls_create_config_client(NULL, 0);

	pcb = calloc(1, sizeof(struct altcp_pcb));

	if (!pcb) {
		printf("Failed to allocate state\n");
		return false;
	}

	if (!tls_client_open(server)) {
		return false;
	}
	return true;
}
