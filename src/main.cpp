#include "wifi.h"
#include "control.h"
#include "cam.h"
#include "Arduino.h"
#include "message.h"
#include <WiFiUdp.h>
#include <esp_camera.h>
#include <HardwareSerial.h>

#define LISTEN_PORT 1000
#define CAMERA_MODEL_AI_THINKER

HardwareSerial interboard(1);

WiFiUDP udp;
camera_config_t camconf;
void setup()
{
  Serial.begin(9600);
  interboard.begin(115200, SERIAL_8N1, 18, 19);
  /* TODO: pass required structures to the functions or return them (WiFiUdp struct etc.) */
  /* Listen on port 1000 */
  setup_cam_udp(&camconf);
  // setup_tcp();
  wifi_ap_setup();
  udp.begin(LISTEN_PORT);
}

void loop()
{
  /* // Captura de imagen
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb)
  {
    Serial.println("Camera capture failed");
    return;
  }

  // Envío de imagen por UDP
  int packetSize = fb->len;
  //int remoteIp = "10.0.0.3";
  int remotePort = 1000;
  char packetBuffer[1024];
  for (int i = 0; i < packetSize; i += 100)
  {
    int packetLength = min(100, packetSize - i);
    memcpy(packetBuffer, fb->buf + i, packetLength);
    //char packetBuffer[] = "Hola, mundo\n";
    //memcpy(packetBuffer, fb->buf + i, packetLength);
    udp.beginPacket("10.0.0.4", 1000);
    udp.write((const unsigned char *)packetBuffer, packetLength);
    udp.endPacket();
    delay(100);
  }

  // Liberación de memoria
  esp_camera_fb_return(fb); */
  /* Read from the arduino */
  if (Serial.available() > 0)
  {
    char recv = Serial.read();
    Serial.println(recv);
    Message msg = {0};
    if (recv == '1')
    {
      Serial.println("FWD_R");
      msg = msg_gen(MOTOR, FWD_R);
    }
    else if (recv == '2')
    {
      Serial.println("FWD_L");
      msg = msg_gen(MOTOR, FWD_L);
    }
    else if (recv == '3')
    {
      Serial.println("RWD_R");
      msg = msg_gen(MOTOR, RWD_R);
    }
    else if (recv == '4')
    {
      Serial.println("RWD_L");
      msg = msg_gen(MOTOR, RWD_L);
    }
    else if (recv == '5')
    {
      Serial.println("FWD");
      msg = msg_gen(MOTOR, FWD_R | FWD_L);
    }
    else if (recv == '6')
    {
      Serial.println("BWD");
      msg = msg_gen(MOTOR, RWD_R | RWD_L);
    }
    else if (recv == '7')
    {
      Serial.println("ALL");
      msg = msg_gen(MOTOR, FWD_R | FWD_L | RWD_R | RWD_L);
    }
    uint32_t raw = 0;
    raw = msg_to_bytes(&msg);
    for (int i = 0; i < 4; i++)
    {
      byte b = (raw >> (8 * i)) & 0xFF;
      Serial.printf("%x\n", b);
      interboard.write(b);
    }

    // Send control message
  }

  // Serial.println(recv);
  delay(1000);
}
