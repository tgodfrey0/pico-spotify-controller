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

#define BUFSIZE								4096

extern char* access_token;
extern char* token_type;
extern uint16_t token_expiry;
extern char* refresh_token;

extern const char *cert;
extern const char *server;
extern struct altcp_pcb *pcb;
extern struct altcp_tls_config *tls_config;

extern bool connected;

static const char *packet;

/**
 * Close the TLS client
 */
err_t tls_client_close() {
	err_t err = ERR_OK;
	if(packet != NULL){
		free((void*)packet);
		packet = NULL;
	}
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

/**
 * Send the data over the TLS/TCP socket
 */
err_t tls_client_send_data_raw(const char *msg){
	printf("Sending %s", msg);
	err_t err = altcp_write(pcb, msg, strlen(msg), TCP_WRITE_FLAG_COPY);
	printf("Writen to stream\n");
	//altcp_output(pcb);
	printf("Stream pushed\n");
	if(err != ERR_OK){
		printf("Error writing data");
		return tls_client_close();
	}

	printf("Data sent\n");

	return ERR_OK;
}

/**
 * Convert a command into a HTTP request and send it
 *
 * @param data is the command to include 
 */
err_t tls_client_send_data(char *data){
	uint16_t msg_size = strlen(data) + strlen(server) + strlen(access_token) + 64;
	if(msg_size > BUFSIZE){
		printf("Message too big!\n");
		return ERR_RST;
	}

	printf("Clearing message buffer\n");
	memset(packet, 0, BUFSIZE);
	sprintf(packet, "%s HTTP/1.1\r\nHost: %s\r\nContent-Length: 0\r\nAuthorization: Bearer %s\r\n\r\n", data, server, access_token);
	printf("Message written to buffer\n");
	err_t err = tls_client_send_data_raw(packet);
	return err;
}

/**
 * Make a HTTP request with extra headers and send it
 *
 * @param data									the command to include
 * @param additional_headers		the headers to include
 */
err_t tls_client_send_data_with_headers(char *data, char *additional_headers){
	uint16_t msg_size = strlen(data) + strlen(server) + strlen(access_token) + strlen(additional_headers) + 64;
	if(msg_size > BUFSIZE){
		printf("Message too big!\n");
		return ERR_RST;
	}
	memset(packet, 0, BUFSIZE);
	snprintf(packet, msg_size, "%s HTTP/1.1\r\nHost: %s\r\nAuthorization: Bearer %s\r\nContent-Length: 0\r\n%s\r\n", data, server, access_token, additional_headers);
	err_t err = tls_client_send_data_raw(packet);
	return err;
}

/**
 * Make a HTTP request with a body
 *
 * @param data						the command to include
 * @param data_format			the content type of the body
 * @param body						the body to include
 */
err_t tls_client_send_data_with_body(char *cmd, char *data_format, char* body){
	uint16_t msg_size = strlen(cmd) + strlen(server) + strlen(access_token) + strlen(data_format) + sizeof(strlen(body)) + strlen(body) + 81;
	if(msg_size > BUFSIZE){
		printf("Message too big!\n");
		return ERR_RST;
	}
	memset(packet, 0, BUFSIZE);
	snprintf(packet, msg_size, "%s HTTP/1.1\r\nHost: %s\r\nAuthorization: Bearer %s\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n%s\r\n", cmd, server, access_token, data_format, strlen(body), body);
	err_t err = tls_client_send_data_raw(packet);
	return err;
}

/**
 * The callback for when it connects to the server
 *
 * @param arg			the designated callback argument
 * @param pcb			the PCB that connected
 * @param err			any error codes
 */
err_t tls_client_connected(void *arg, struct altcp_pcb *pcb, err_t err) {
	if (err != ERR_OK) {
		printf("Connect failed %d\n", err);
		return tls_client_close();
	}
	printf("Connected to server\n");
	spotify_init();
	return ERR_OK;
}

/**
 * Callback for a critical error
 *
 * @param arg			the designated callback argument
 * param err			the error code
 */
void tls_client_err(void *arg, err_t err) {
	printf("CRITICAL ERROR - %d\n", err);
	pcb = NULL; /* pcb freed by lwip when _err function is called */
}

/**
 * The callback when data is received
 *
 * @param arg			the designated callback argument
 * @param pcb			the PCB which received data
 * @param p				the PBUF struct holding the response
 * @param err			any error codes
 */
err_t tls_client_recv(void *arg, struct altcp_pcb *pcb, struct pbuf *p, err_t err) {
	printf("DATA RECEIVED\n");
	if (!p) {
		printf("connection closed\n");
		return tls_client_close();
	}

	char* data;

	if (p->tot_len > 0) {
		data = (char*)p->payload;
		printf("***\nnew data received from server:\n***\n\n%s\n\n***\n", data);
		altcp_recved(pcb, p->tot_len);
	}

	pbuf_free(p);
	parse_response(data);

	return ERR_OK;
}

/**
 * Connect the socket to the server
 *
 * @param ipaddr			the IP of the server
 */
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

/**
 * The callback when the DNS server returns an IP address
 *
 * @param hostname			the name that was looked up
 * @param	ipaddr				the IP address of the hostname
 * @param arg						the designated callback argument
 */
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

/**
 * Open a TLS client
 *
 * @param hostname			the hostname to find
 */
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

/**
 * Initialise a TLS client
 */
bool tls_client_init(void) {
	tls_config = altcp_tls_create_config_client(NULL, 0);

	pcb = calloc(1, sizeof(struct altcp_pcb));
	packet = calloc(0, sizeof(char));

	if (!pcb) {
		printf("Failed to allocate state\n");
		return false;
	}

	if (!tls_client_open(server)) {
		return false;
	}
	return true;
}
