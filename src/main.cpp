#include "wifi.h"
#include "control.h"
#include "cam.h"

void setup() {
   	Serial.begin(115200);
    /* TODO: pass required structures to the functions or return them (WiFiUdp struct etc.) */
    setup_cam_udp();
    setup_tcp();
    wifi_ap_setup();
}

void loop() {

}