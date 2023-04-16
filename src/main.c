#include <stdint.h>
#include <string.h>
#include <time.h>

#include "hardware/gpio.h"
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

void button_isr(uint gpio, uint32_t state){
  //next();
  //busy_wait_ms(3000);
  sync_playback();
  //busy_wait_ms(3000);
  //previous();
  //busy_wait_ms(3000);
  return;
  switch (gpio) {
    case 10:
      printf("Button 10 pressed\n");
      previous();
      break;
    case 11:
      printf("Button 11 pressed\n");
      togglePlayback();
      break;
    case 12:
      printf("Button 12 pressed\n");
      next();
      break;
  }
}

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

  //togglePlayback();
  printf("IN MAIN\n");
 
  gpio_init(10);
  gpio_set_dir(10, GPIO_IN);
  gpio_pull_down(10);
  gpio_set_irq_enabled_with_callback(10, GPIO_IRQ_EDGE_FALL, true, &button_isr);
  gpio_init(11);
  gpio_set_dir(11, GPIO_IN);
  gpio_pull_down(11);
  gpio_set_irq_enabled_with_callback(11, GPIO_IRQ_EDGE_FALL, true, &button_isr);
  gpio_init(12);
  gpio_set_dir(12, GPIO_IN);
  gpio_pull_down(12);
  gpio_set_irq_enabled_with_callback(12, GPIO_IRQ_EDGE_FALL, true, &button_isr);

  while(true){
    sleep_ms((token_expiry - 30) * 1000);
    renew_token();
  }

  lwjson_free(&lwjson);
  cyw43_arch_deinit();
  return 0;
}
