/* ESP32 master program:
 * Check & forward TCP messages to Arduino sent from the remote client
 * Establish a WiFi SoftAP
 * Control other stuff, camera stream
 */
#include "driver/uart.h"
#include "esp_camera.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "message.h"
#include "nvs_flash.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <lwip/sockets.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define EXAMPLE_ESP_WIFI_SSID "ELCO-BEACON"
#define EXAMPLE_ESP_WIFI_PASS "baconfrito5"
#define EXAMPLE_ESP_WIFI_CHANNEL 6
#define EXAMPLE_MAX_STA_CONN 4
// Camera pins
/* #define CAM_PIN_PWDN 32 // power down is not used
#define CAM_PIN_RESET -1 // software reset will be performed
#define CAM_PIN_XCLK 0
#define CAM_PIN_SIOD 26
#define CAM_PIN_SIOC 27

#define CAM_PIN_D7 35
#define CAM_PIN_D6 34
#define CAM_PIN_D5 39
#define CAM_PIN_D4 36
#define CAM_PIN_D3 21
#define CAM_PIN_D2 19
#define CAM_PIN_D1 18
#define CAM_PIN_D0 5
#define CAM_PIN_VSYNC 25
#define CAM_PIN_HREF 23
#define CAM_PIN_PCLK 22
 */
#define CAM_PIN_PWDN 32  // power down is not used
#define CAM_PIN_RESET -1 // software reset will be performed
#define CAM_PIN_XCLK 0
#define CAM_PIN_SIOD 26
#define CAM_PIN_SIOC 27

#define CAM_PIN_D7 35
#define CAM_PIN_D6 34
#define CAM_PIN_D5 39
#define CAM_PIN_D4 36
#define CAM_PIN_D3 21
#define CAM_PIN_D2 19
#define CAM_PIN_D1 18
#define CAM_PIN_D0 5
#define CAM_PIN_VSYNC 25
#define CAM_PIN_HREF 23
#define CAM_PIN_PCLK 22

#define TCP_PORT 6666
#define UDP_PORT 6665
#define FPS 60
#define UDP_RECONNECT_MS 1000 // 1 second

TaskHandle_t listenHandle = NULL;
TaskHandle_t listenTCPHandle = NULL;
// globals
static const char *vWifiTag = "wifi softAP";
static const uart_port_t IPC = UART_NUM_1;
// camera buffer
static size_t _jpg_buf_len = 0;
static uint8_t *_jpg_buf = NULL;
static int udp_send_socket = -1;

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

esp_err_t xCameraConfig() {
  static const char *pcCamTag = "Camera Task";
  static camera_config_t camera_config = {
      .pin_pwdn = CAM_PIN_PWDN,
      .pin_reset = CAM_PIN_RESET,
      .pin_xclk = CAM_PIN_XCLK,
      .pin_sscb_sda = CAM_PIN_SIOD,
      .pin_sscb_scl = CAM_PIN_SIOC,
      .pin_d7 = CAM_PIN_D7,
      .pin_d6 = CAM_PIN_D6,
      .pin_d5 = CAM_PIN_D5,
      .pin_d4 = CAM_PIN_D4,
      .pin_d3 = CAM_PIN_D3,
      .pin_d2 = CAM_PIN_D2,
      .pin_d1 = CAM_PIN_D1,
      .pin_d0 = CAM_PIN_D0,
      .pin_vsync = CAM_PIN_VSYNC,
      .pin_href = CAM_PIN_HREF,
      .pin_pclk = CAM_PIN_PCLK,

      // XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
      .xclk_freq_hz = 20000000,
      .ledc_timer = LEDC_TIMER_0,
      .ledc_channel = LEDC_CHANNEL_0,

      .pixel_format = PIXFORMAT_RGB565, // YUV422,GRAYSCALE,RGB565,JPEG
      .frame_size = FRAMESIZE_QVGA, // QQVGA-UXGA Do not use sizes above QVGA
                                    // when not JPEG

      .jpeg_quality = 12, // 0-63 lower number means higher quality
      .fb_count = 1, // if more than one, i2c runs in continuous mode. Use only
                     // with JPEG
      .grab_mode = CAMERA_GRAB_WHEN_EMPTY,
  };

  // initialize the camera
  esp_err_t err = esp_camera_init(&camera_config);
  if (err != ESP_OK) {
    ESP_LOGE(pcCamTag, "Camera Init Failed");
    return ESP_FAIL;
  }
  return ESP_OK;
}

void vCameraTask(void *arg) {
  static const char *pcCamTag = "Camera Task";
  // compressed JPEG buffer

  for (;;) {
    /*  ESP_LOGI(pcCamTag, "Taking picture...");
     camera_fb_t *pic = esp_camera_fb_get();

     // use pic->buf to access the image
     ESP_LOGI(pcCamTag, "Picture taken! Its size was: %zu bytes", pic->len);
     // framebuffer compression to JPEG for reduced network traffic
     frame2jpg(pic, 80, &_jpg_buf, &_jpg_buf_len);
     esp_camera_fb_return(pic);

     // Delay according to frame rate (FPS) */
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

static void vUdpServer(void *pvParameters) {
  static const char *vUdp = "UDP Client";
  char *message = "Hello Server";
  int sockfd;
  struct sockaddr_in servaddr;

  // clear servaddr
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_addr.s_addr = inet_addr("192.168.4.2");
  servaddr.sin_port = htons(UDP_PORT);
  servaddr.sin_family = AF_INET;

  // create datagram socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    ESP_LOGE(vUdp, "Could not create UDP socket");

  bool connected = false;

  for (;;) {
    // connect to server
    if (connected == false) {
      if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        // ESP_LOGE(vUdp, "UDP connection failed, waiting... %d", errno);
        vTaskDelay(UDP_RECONNECT_MS / portTICK_PERIOD_MS);
        continue;
      } else {
        ESP_LOGI(vUdp, "Connected to UDP server");
        connected = true;
      }
    }

    // request to send datagram
    int err = send(sockfd, message, 13, 0);
    if (err < 0) {
      ESP_LOGE(vUdp, "Sending failed: %d", errno);
      vTaskDelay(UDP_RECONNECT_MS / portTICK_PERIOD_MS);
    }

    // close the descriptor
    // close(sockfd);
  }
}

void vControlListener(char *buf) {
  static const char *pcVcl = "vControlListener";
  char *newline = strchr(buf, '\n');
  if (newline) {
    *newline = '\0';
  }

  char *endptr;
  uint32_t cast = strtoul(buf, &endptr, 10);
  if (*endptr != '\0')
    ESP_LOGE(pcVcl, "Could not parse control message to known format.");
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
    ESP_LOGE(pcVcl,
             "Erroneous control message, header or sentinel are incorrect!");
    return;
  }

  /* Forward control message to the Arduino board */
  if (msg.cmd_ident == MOTOR) {
    printf("Received MOTOR command\n");
    // for (int i = 0; i < )
    uart_write_bytes(IPC, (const char *) &msg.payload, sizeof msg.payload);
    // uart_write_bytes(IPC, "CAFE", 5);
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
      // if (valread > 32)
      //  buffer >>= (valread - 32);
      vControlListener(buffer);
      memset(buffer, 0, sizeof(buffer));
    }
  }
}

void app_main() {
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
      .baud_rate = 9600,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .source_clk = UART_SCLK_DEFAULT,
      // .rx_flow_ctrl_thresh = 0,
  };

  // Configure UART pins tx: 19, rx: 18

  ESP_ERROR_CHECK(uart_param_config(IPC, &uart_config));
  ESP_ERROR_CHECK(uart_set_pin(IPC, 4, 5, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
  ESP_ERROR_CHECK(uart_driver_install(IPC, BUFSIZ * 2, BUFSIZ * 2, 0, NULL, 0));

  wifi_init_softap();
  esp_err_t cam_err = xCameraConfig();
  // FreeRTOS task initializer
  // maximum priority for control messages
  xTaskCreate(vTcpReceiver, "TCP Receiver", 8192, NULL, 9, &listenTCPHandle);
  xTaskCreate(vUdpServer, "UDP Server", 8192, NULL, 2, NULL);
  // Only create camera task if we could initialize it
  if (cam_err == ESP_OK) {
    xTaskCreate(vCameraTask, "Camera task", 8192, NULL, 3, NULL);
  }

  while (1)
    vTaskDelay(10);
}
