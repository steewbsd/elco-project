// #include "cam.h"
// #include "control.h"
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
// #include <WiFiUdp.h>

#define EXAMPLE_ESP_WIFI_SSID "ELCO-BEACON"
#define EXAMPLE_ESP_WIFI_PASS "baconfrito5"
#define EXAMPLE_ESP_WIFI_CHANNEL 6
#define EXAMPLE_MAX_STA_CONN 4

#define LISTEN_PORT 1000
#define TCP_PORT 6666

// Utilize UART1 to communicate with another board (Arduino)
// HardwareSerial IPC(1);
TaskHandle_t listenHandle = NULL;
TaskHandle_t listenTCPHandle = NULL;
// WiFiUDP udp;
static const char *vWifiTag = "wifi softAP";
// Globals
static char recv_char;

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

void vControlListener(void *arg) {
  static const char *vcl = "vControlListener";
  struct Message msg = {0};
  switch (recv_char) {
  case '1':
    ESP_LOGI(vcl, "FWD_R");
    msg = msg_gen(MOTOR, FWD_R);
    break;
  case '2':
    ESP_LOGI(vcl, "FWD_L");
    msg = msg_gen(MOTOR, FWD_L);
    break;
  case '3':
    ESP_LOGI(vcl, "RWD_R");
    msg = msg_gen(MOTOR, RWD_R);
    break;
  case '4':
    ESP_LOGI(vcl, "RWD_L");
    msg = msg_gen(MOTOR, RWD_L);
    break;
  case '5':
    ESP_LOGI(vcl, "FWD");
    msg = msg_gen(MOTOR, FWD_R | FWD_L);
    break;
  case '6':
    ESP_LOGI(vcl, "BWD");
    msg = msg_gen(MOTOR, RWD_R | RWD_L);
    break;
  case '7':
    ESP_LOGI(vcl, "ALL");
    msg = msg_gen(MOTOR, FWD_R | FWD_L | RWD_R | RWD_L);
    break;
  default:
    ESP_LOGI(vcl, "None");
    uint32_t raw = 0;
    raw = msg_to_bytes(&msg);
    for (int i = 0; i < 4; i++) {
      uint8_t b = (raw >> (8 * i)) & 0xFF;
      // Serial.printf("%x\n", b);
      // IPC.write(b);
    }
  }
  // wait to receive another command
  // vTaskDelayUntil(&last, xFrequency);
}

// TODO: switch to FreeRTOS tcp implementation
void vTcpReceiver(void *arg) {
  int server_fd, new_socket, valread;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);
  char buffer[1024] = {0};
  // char *hello = "Hello from server";

  // Creating socket fd
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  /*   // Attaching socket to the port 6666
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
      perror("setsockopt"); */
  // exit(EXIT_FAILURE);
  //}
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
      memset(buffer, 0, sizeof(buffer));
    }
  }

  /*  // Sending data to the client
   send(new_socket, hello, strlen(hello), 0);
   printf("Hello message sent\n"); */
}

void app_main(void) {
  // Local ESP32 serial monitor
  // Serial.begin(9600);
  // Inter board UART communication with pins 18 (rx) and 19 (tx)
  // IPC.begin(115200, SERIAL_8N1, 18, 19);
  /* Listen on port 1000 */
  // udp.begin(LISTEN_PORT);
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // FreeRTOS task initializer
  wifi_init_softap();
  // xTaskCreate(vControlListener, "Message control listener", 4096, NULL, 10,
  //            &listenHandle);
  xTaskCreate(vTcpReceiver, "TCP Receiver", 8192, NULL, 2, &listenTCPHandle);
  while (1)
    vTaskDelay(10);
}
