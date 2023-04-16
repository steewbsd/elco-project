/* ESP32 master program: 
 * Check & forward TCP messages to Arduino sent from the remote client
 * Establish a WiFi SoftAP 
 * Control other stuff, camera stream 
*/
#include "driver/uart.h"
#include "esp_event.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "message.h"
#include "nvs_flash.h"

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <lwip/sockets.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define EXAMPLE_ESP_WIFI_SSID "ELCO-BEACON"
#define EXAMPLE_ESP_WIFI_PASS "baconfrito5"
#define EXAMPLE_ESP_WIFI_CHANNEL 6
#define EXAMPLE_MAX_STA_CONN 4

#define TCP_PORT 6666

TaskHandle_t listenHandle = NULL;
TaskHandle_t listenTCPHandle = NULL;
// globals
static const char *vWifiTag = "wifi softAP";
static const uart_port_t IPC = UART_NUM_1;

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
  if (event_id == WIFI_EVENT_AP_STACONNECTED) {
    wifi_event_ap_staconnected_t *event =
        (wifi_event_ap_staconnected_t *)event_data;
    ESP_LOGI(vWifiTag, "station " MACSTR " join, AID=%d", MAC2STR(event->mac),
             event->aid);
  } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
    wifi_event_ap_stadisconnected_t *event =
        (wifi_event_ap_stadisconnected_t *)event_data;
    ESP_LOGI(vWifiTag, "station " MACSTR " leave, AID=%d", MAC2STR(event->mac),
             event->aid);
  }
}

void wifi_init_softap(void) {
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_ap();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));

  wifi_config_t wifi_config = {
      .ap =
          {
              .ssid = EXAMPLE_ESP_WIFI_SSID,
              .password = EXAMPLE_ESP_WIFI_PASS,
              .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
              .channel = EXAMPLE_ESP_WIFI_CHANNEL,
              .authmode = WIFI_AUTH_WPA2_PSK,
              .ssid_hidden = 0,
              .max_connection = EXAMPLE_MAX_STA_CONN,
              .pmf_cfg =
                  {
                      .required = true,
                  },
          },
  };
  if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
    wifi_config.ap.authmode = WIFI_AUTH_OPEN;
  }

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_LOGI(
      vWifiTag, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
      EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS, EXAMPLE_ESP_WIFI_CHANNEL);
}

void vControlListener(char *buf) {
  static const char *vcl = "vControlListener";
  char *newline = strchr(buf, '\n');
  if (newline) {
    *newline = '\0';
  }

  char *endptr;
  uint32_t cast = strtoul(buf, &endptr, 10);
  if (*endptr != '\0')
    ESP_LOGE(vcl, "Could not parse control message to known format.");
  struct Message msg = {0};
  uint8_t b[4] = {0};
  for (int i = 0; i < 4; i++) {
    b[i] = (cast >> (8 * i)) & 0xFF;
  }
  msg.header = b[3];
  msg.cmd_ident = b[2];
  msg.payload = b[1];
  msg.end = b[0];
  printf("Received: %x, %x, %x, %x\n", msg.header, msg.cmd_ident, msg.payload,
         msg.end);

  if ((msg.end != 0xFF) | (msg.header != 0xAA)) {
    ESP_LOGE(vcl,
             "Erroneous control message, header or sentinel are incorrect!");
    return;
  }

  /* Forward control message to the Arduino board */
  if (msg.cmd_ident == MOTOR) {
    printf("Received MOTOR command\n");
    for (int i = 3; i >= 0; i--)
      uart_write_bytes(IPC, b[i], sizeof(uint8_t));
    // nothing else we can do here
    return;
  }
}

// TODO: switch to FreeRTOS tcp implementation
void vTcpReceiver(void *arg) {
  int server_fd, new_socket, valread;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  char buffer[1024] = {0};

  // Creating socket fd
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(TCP_PORT);

  // Binding the socket to the specified address and port
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  // Listening for incoming connections
  if (listen(server_fd, 3) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  for (;;) {

    // Accepting incoming connections
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t *)&addrlen)) < 0) {
      perror("accept");
      exit(EXIT_FAILURE);
    }

    while ((valread = read(new_socket, buffer, 1024)) > 0) {
      // Reading data from the client
      buffer[valread] = '\0'; // Ensure the buffer is null-terminated
      printf("%s\n", buffer);
      // TODO: maybe parse
      vControlListener(buffer);
      memset(buffer, 0, sizeof(buffer));
    }
  }
}

void app_main(void) {
  /* ESP32 nvs */
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // UART1 Peripheral initialization
  uart_config_t uart_config = {
      .baud_rate = 115200,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
  };

  // Configure UART pins tx: 19, rx: 18
  uart_set_pin(UART_NUM_1, 19, 18, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  ESP_ERROR_CHECK(uart_param_config(IPC, &uart_config));

  wifi_init_softap();

  // FreeRTOS task initializer
  xTaskCreate(vTcpReceiver, "TCP Receiver", 8192, NULL, 2, &listenTCPHandle);
  while (1)
    vTaskDelay(10);
}
