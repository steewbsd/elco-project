#include "wifi.h"
#include "control.h"
#include "cam.h"
#include "Arduino.h"

void setup() {
   	Serial.begin(115200);
    /* TODO: pass required structures to the functions or return them (WiFiUdp struct etc.) */
    //setup_cam_udp();
    //setup_tcp();
    wifi_ap_setup();
}

void loop()
{
/*   // Captura de imagen
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
  esp_camera_fb_return(fb); */
}
