#include "wifi.h"
#include "control.h"
#include "cam.h"
#include "Arduino.h"
#include <WiFiUdp.h>
#include <esp_camera.h>

#define LISTEN_PORT 1000

WiFiUDP udp;
camera_config_t camconf;
void setup() {
   	Serial.begin(9600);
    /* TODO: pass required structures to the functions or return them (WiFiUdp struct etc.) */
    /* Listen on port 1000 */
    setup_cam_udp(&camconf);
    //setup_tcp();
    wifi_ap_setup();
    udp.begin(LISTEN_PORT);
}

void loop()
{
  // Captura de imagen
 /*  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb)
  {
    Serial.println("Camera capture failed");
    return;
  } */

  // Envío de imagen por UDP
  //int packetSize = fb->len;
  //int remoteIp = "10.0.0.3";
  int remotePort = 1000;
  //for (int i = 0; i < packetSize; i += BUFSIZ)
  //{
    //int packetLength = min(BUFSIZ, packetSize - i);
    char packetBuffer[] = "Hola, mundo\n";
    //memcpy(packetBuffer, fb->buf + i, packetLength);
    udp.beginPacket("10.0.0.3", 1000);
    udp.write((const unsigned char *)packetBuffer, sizeof(packetBuffer));
    udp.endPacket();
    delay(100);
  //}

  // Liberación de memoria
  //esp_camera_fb_return(fb);
}
