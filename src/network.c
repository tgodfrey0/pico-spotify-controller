#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "hardware/timer.h"
#include "lwip/dns.h"
#include "lwip/err.h"
#include "lwip/ip_addr.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/pbuf.h"
#include "lwip/altcp.h"
#include "lwip/altcp_tcp.h"
#include "lwip/altcp_tls.h"
#include "lwip/dns.h"
#include "hardware/gpio.h"
#include "hardware/sync.h"

#include "defs.h"

#include "network.h"
#include "spotify.h"
#include "credentials.h"

struct altcp_pcb *pcb;

err_t altcp_client_connect(void *arg, struct altcp_pcb *pcb, err_t err){
  printf("Connected to API\n");
  sync_playing();
  return ERR_OK;
}

void altcp_dns_callback(const char *name, const ip_addr_t *ip, void *arg){
  altcp_connect(pcb, ip, 443, altcp_client_connect);
}


err_t altcp_client_close(void *arg){

  err_t err = ERR_OK;
  if(pcb != NULL){
    altcp_arg(pcb, NULL);
    altcp_sent(pcb, NULL);
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
 * Called when the client acknowledges the sent data
 *
 * @param arg	    the state struct
 * @param pcb	  the connection PCB for which data has been acknowledged
 * @param len	    the amount of bytes acknowledged
 */
err_t altcp_client_sent(void *arg, struct altcp_pcb *pcb, u16_t len) {
  return ERR_OK;
}

/**
 * Function to send the data to the client
 *
 * @param arg	        the state struct
 * @param altcp_pcb     the client PCB
 * @param data	      the data to send
 */
err_t altcp_client_send_data(void *arg, struct altcp_pcb *pcb, char *data)
{
  
  cyw43_arch_lwip_check();

  // Write data for sending but does not send it immediately
  // To force writing we can call altcp_output after altcp_write
  err_t err = altcp_write(pcb, data, strlen(data), TCP_WRITE_FLAG_COPY);
  altcp_output(pcb);
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
 * @param pcb	  the connection PCB which received data
 * @param p	  the received data
 * @param err	  an error code if there has been an error receiving
 */
err_t altcp_client_recv(void *arg, struct altcp_pcb *pcb, struct pbuf *p, err_t err) {
  if (!p) {
    printf("No data received");
    return ERR_VAL;
  }

  cyw43_arch_lwip_check();
  
  pbuf_free(p);

  return ERR_OK;
}

/**
 * The function called when a critical error occurs
 *
 * @param arg	  the state struct
 * @param err	  the error code
 */
void altcp_client_err(void *arg, err_t err) {
  if (err != ERR_ABRT) {
    printf("TCP Client ERROR %d\n", err);
  }
}

struct altcp_pcb* altcp_client_init(char* hostname){
  struct altcp_tls_config *conf = altcp_tls_create_config_client(CERTIFICATE, strlen(CERTIFICATE) + 1);
  altcp_allocator_t allocator = {altcp_tls_alloc, conf};
  pcb = altcp_new(&allocator);

  altcp_arg(pcb, NULL);

  altcp_err(pcb, altcp_client_err);
  altcp_recv(pcb, altcp_client_recv);
  altcp_sent(pcb, altcp_client_sent);

  ip_addr_t *addr;
  dns_gethostbyname(hostname, addr, altcp_dns_callback, NULL);
}

