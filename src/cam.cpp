#include <WiFi.h>
#include <WiFiUdp.h>
#include <esp_camera.h>

void setup_cam_udp(camera_config_t * camconf)
{
  //char packetBuffer[UDP_TX_PACKET_MAX_SIZE];

  // Inicialización de la cámara
  camconf->ledc_channel = LEDC_CHANNEL_0;
  camconf->ledc_timer = LEDC_TIMER_0;
  camconf->pin_d0 = 5;
  camconf->pin_d1 = 18;
  camconf->pin_d2 = 19;
  camconf->pin_d3 = 21;
  camconf->pin_d4 = 36;
  camconf->pin_d5 = 39;
  camconf->pin_d6 = 34;
  camconf->pin_d7 = 35;
  camconf->pin_xclk = 0;
  camconf->pin_pclk = 22;
  camconf->pin_vsync = 25;
  camconf->pin_href = 23;
  camconf->pin_sscb_sda = 26;
  camconf->pin_sscb_scl = 27;
  camconf->pin_pwdn = 32;
  camconf->pin_reset = -1;
  camconf->xclk_freq_hz = 20000000;
  camconf->pixel_format = PIXFORMAT_JPEG;
  camconf->frame_size = FRAMESIZE_QVGA;
  camconf->jpeg_quality = 12;
  camconf->fb_count = 1;

  esp_err_t err = esp_camera_init(camconf);
  if (err != ESP_OK)
  {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
}

