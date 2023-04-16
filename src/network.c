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
	return ERR_OK;
}

/**
 * Send the data over the TLS/TCP socket
 */
err_t tls_client_send_data_raw(const char *msg){
	return ERR_OK;
}

/**
 * Convert a command into a HTTP request and send it
 *
 * @param data is the command to include 
 */
err_t tls_client_send_data(char *data){
	return ERR_OK;
}

/**
 * Make a HTTP request with extra headers and send it
 *
 * @param data									the command to include
 * @param additional_headers		the headers to include
 */
err_t tls_client_send_data_with_headers(char *data, char *additional_headers){
	return ERR_OK;
}

/**
 * Make a HTTP request with a body
 *
 * @param data						the command to include
 * @param data_format			the content type of the body
 * @param body						the body to include
 */
err_t tls_client_send_data_with_body(char *cmd, char *data_format, char* body){
	return ERR_OK;
}

/**
 * The callback for when it connects to the server
 *
 * @param arg			the designated callback argument
 * @param pcb			the PCB that connected
 * @param err			any error codes
 */
err_t tls_client_connected(void *arg, struct altcp_pcb *pcb, err_t err) {
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
	return ERR_OK;
}

/**
 * Connect the socket to the server
 *
 * @param ipaddr			the IP of the server
 */
void tls_client_connect_to_server_ip(const ip_addr_t *ipaddr){}

/**
 * The callback when the DNS server returns an IP address
 *
 * @param hostname			the name that was looked up
 * @param	ipaddr				the IP address of the hostname
 * @param arg						the designated callback argument
 */
void tls_client_dns_found(const char* hostname, const ip_addr_t *ipaddr, void *arg){}

/**
 * Open a TLS client
 *
 * @param hostname			the hostname to find
 */
bool tls_client_open(const char *hostname) {
	return true;
}

/**
 * Initialise a TLS client
 */
bool tls_client_init(void) {
	return true;
}
