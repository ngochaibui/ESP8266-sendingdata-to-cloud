#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <Libraries/DHT/DHT.h>

// If you want, you can define WiFi settings globally in Eclipse Environment Variables
#ifndef WIFI_SSID
	#define WIFI_SSID "Maker Ha Noi" // Put you SSID and Password here
	#define WIFI_PWD "makerhanoi123"
#endif
#define WORK_PIN 14 // GPIO14

DHT dht(WORK_PIN,DHT11);
Timer procTimer;
int sensorValue = 0;
HttpClient thingSpeak;

void onDataSent(HttpClient& client, bool successful)
{
	if (successful)
		Serial.println("Success sent");
	else
		Serial.println("Failed");

	String response = client.getResponseString();
	Serial.println("Server response: '" + response + "'");
	if (response.length() > 0)
	{
		int intVal = response.toInt();

		if (intVal == 0)
			Serial.println("Sensor value wasn't accepted. May be we need to wait a little?");
	}
}

void sendData()
{
	if (thingSpeak.isProcessing()) return; // We need to wait while request processing was completed

	// Read our sensor value :)
	sensorValue = dht.readHumidity();
	Serial.println(sensorValue);
	thingSpeak.downloadString("http://api.thingspeak.com/update?key=69TT2XX59AN4FO1F&field1=" + String(sensorValue), onDataSent);
}

// Will be called when WiFi station was connected to AP
void connectOk()
{
	Serial.println("I'm CONNECTED");

	// Start send data loop
	procTimer.initializeMs(2 * 1000, sendData).start(); // every 25 seconds
}

// Will be called when WiFi station timeout was reached
void connectFail()
{
	Serial.println("I'm NOT CONNECTED. Need help :(");

	// Start soft access point
	WifiAccessPoint.enable(true);
	WifiAccessPoint.config("CONFIG ME PLEEEEASE...", "", AUTH_OPEN);

	// .. some you code for configuration ..
}

void init()
{
	spiffs_mount(); // Mount file system, in order to work with files

	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(false); // Disable debug output to serial

	WifiStation.config(WIFI_SSID, WIFI_PWD);
	WifiStation.enable(true);
	WifiAccessPoint.enable(false);
	WDT.enable(false);
	delay(1000);
	dht.begin();
	// Run our method when station was connected to AP (or not connected)
	WifiStation.waitConnection(connectOk, 20, connectFail); // We recommend 20+ seconds for connection timeout at start



}
