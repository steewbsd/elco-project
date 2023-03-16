#include <WiFi.h>
#include <WiFiUdp.h>
#include <esp_camera.h>

void setup_cam_udp()
{
  IPAddress ip(192, 168, 1, 177);  // IP del ESP32
  unsigned int localPort = 5000;   // Puerto para recibir paquetes UDP
  unsigned int remotePort = 10000; // Puerto para enviar paquetes UDP
  char packetBuffer[UDP_TX_PACKET_MAX_SIZE];

  WiFiUDP udp;

  // Inicialización de la cámara
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = 5;
  config.pin_d1 = 18;
  config.pin_d2 = 19;
  config.pin_d3 = 21;
  config.pin_d4 = 36;
  config.pin_d5 = 39;
  config.pin_d6 = 34;
  config.pin_d7 = 35;
  config.pin_xclk = 0;
  config.pin_pclk = 22;
  config.pin_vsync = 25;
  config.pin_href = 23;
  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;
  config.pin_pwdn = 32;
  config.pin_reset = -1;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK)
  {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // Configuración de UDP
  udp.begin(localPort);
  Serial.printf("Local port: %d\n", udp.localPort());
}

void loop()
{
  // Captura de imagen
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb)
  {
    Serial.println("Camera capture failed");
    return;
  }

  // Envío de imagen por UDP
  int packetSize = fb->len;
  for (int i = 0; i < packetSize; i += UDP_TX_PACKET_MAX_SIZE)
  {
    int packetLength = min(UDP_TX_PACKET_MAX_SIZE, packetSize - i);
    memcpy(packetBuffer, fb->buf + i, packetLength);
    udp.beginPacket(ip, remotePort);
    udp.write(packetBuffer, packetLength);
    udp.endPacket();
    delay(1);
  }

  // Liberación de memoria
  esp_camera_fb_return(fb);
}
