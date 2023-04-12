#include <string.h>
#include <time.h>

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

extern lwjson_t lwjson;

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

  spotify_init();

  // Setup interrupts

  for(;;){}

  lwjson_free(&lwjson);
  cyw43_arch_deinit();
  return 0;
}
