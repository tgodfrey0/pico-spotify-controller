#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "hardware/timer.h"
#include "lwip/err.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "hardware/gpio.h"
#include "hardware/sync.h"

// credentials.h stores the ssid and password as char[]. You must make this youself for obvious reasons
#include "credentials.h"
#include "defs.h"
#include "utils.h"
#include "network.h"

int main() {
  stdio_init_all();

  gpio_init(LED);
  gpio_set_dir(LED, GPIO_OUT);

  if(cyw43_arch_init_with_country(CYW43_COUNTRY_UK)){
    printf("Failed to initialise\n");
    return 1;
  }

  cyw43_arch_enable_sta_mode();

  if(cyw43_arch_wifi_connect_timeout_ms(ssid, pass, CYW43_AUTH_WPA2_AES_PSK, 10000)){
    printf("Failed to connect\n");
    gpio_put(LED, 1);
    return 1;
  }

  printf("Connected to WiFi\n");

  TCP_SERVER_T *state = init_server();
  
  if(!state){
    printf("Shutting down\n");
    return 1;
  }
  
  listen(state);

  tcp_server_close(state);
  free(state);
  cyw43_arch_deinit();
  return 0;
}
