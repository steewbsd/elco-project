#include "wifi.h"
#include "control.h"
#include "cam.h"
#include "message.h"

#include <WiFiUdp.h>
#include <HardwareSerial.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define LISTEN_PORT 1000

HardwareSerial IPC(1);

WiFiUDP udp;
camera_config_t camconf;
void setup()
{
  // Local ESP32 serial monitor
  Serial.begin(9600);
  // Inter board UART communication with pins 18 (rx) and 19 (tx)
  IPC.begin(115200, SERIAL_8N1, 18, 19);
  /* Listen on port 1000 */
  setup_cam_udp(&camconf);
  wifi_ap_setup();
  udp.begin(LISTEN_PORT);
}

void loop()
{
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
      IPC.write(b);
    }

    // Send control message
  }

  // Serial.println(recv);
  delay(1000);
}
