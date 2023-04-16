#include <stdint.h>
#include <string.h>
#include <time.h>

#include "hardware/timer.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/pbuf.h"
#include "lwip/altcp_tcp.h"
#include "lwip/altcp_tls.h"
#include "lwip/dns.h"

#include "lwjson/lwjson.h"

#include "defs.h"
#include "credentials.h"
#include "network.h"
#include "spotify.h"

struct altcp_pcb *pcb;
struct altcp_tls_config *tls_config = NULL;

extern uint16_t token_expiry;

extern lwjson_t lwjson;

bool ready = false;

int main() {
  stdio_init_all();

  if (cyw43_arch_init()) {
    printf("Initialisation failure\n");
    return 1;
  }

  cyw43_arch_enable_sta_mode();

  if (cyw43_arch_wifi_connect_timeout_ms(ssid, pass, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
    printf("Connection failure\n");
    return 1;
  }

  if(!tls_client_init()){
    printf("Failed to initialise TLS client");
  }

  // Setup interrupts
  
  while(!ready){
    busy_wait_ms(100);
  }

  busy_wait_ms(3000);
  //togglePlayback();
  //pause();
  //busy_wait_ms(3000);
  //play();
  //busy_wait_ms(3000);
  //next();
  //busy_wait_ms(3000);
  //previous();
  //busy_wait_ms(3000);
  return 0;

  while(true){
    sleep_ms((token_expiry - 30) * 1000);
    renew_token();
  }

  lwjson_free(&lwjson);
  cyw43_arch_deinit();
  return 0;
}
