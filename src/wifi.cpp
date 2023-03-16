#include <WiFi.h>

void wifi_ap_setup()
{
	const char *ssid = "AC-ESP32";
	const char *passphrase = "Nabucono2sor";
	IPAddress local_IP(10,0,0,2);
	IPAddress gateway(10,0,0,1);
	IPAddress subnet(255,255,255,0);


 	Serial.println();
 	Serial.print("Setting soft-AP configuration ... ");
 	Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
 	Serial.print("Setting soft-AP ... ");
 	Serial.println(WiFi.softAP(ssid,passphrase) ? "Ready" : "Failed!");
 	WiFi.softAP(ssid);
 	WiFi.softAP(ssid, passphrase, 10, false, 10);
 	
 	Serial.print("Soft-AP IP address = ");
 	Serial.println(WiFi.softAPIP());
}