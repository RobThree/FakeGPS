extern "C"
{
#include "user_interface.h"  	  // Required for wifi_station_connect() to work
}

#include <ESP8266WiFi.h>          // https://github.com/esp8266/Arduino
#include <WiFiUdp.h>
#include <time.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          // https://github.com/tzapu/WiFiManager
#include <NTPClient.h>			  // https://github.com/arduino-libraries/NTPClient
#include <Timezone.h>    		  // https://github.com/JChristensen/Timezone
#include <config.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_SERVER, 0, NTP_INTERVAL);
WiFiManager wifiManager;

int firstrun = 1;

// Configure your timezone rules here how described on https://github.com/JChristensen/Timezone
TimeChangeRule tzDST = TZ_DST;
TimeChangeRule tzSTD = TZ_STD;
Timezone tzClock(tzDST, tzSTD);

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);   // Initialize the LED_BUILTIN pin as an output
	digitalWrite(LED_BUILTIN, HIGH);
	Serial.begin(DEBUG_SERIAL);

	wifiManager.setTimeout(180);

	// Fetches ssid and pass and tries to connect
	// If it does not connect it starts an access point with the specified name
	// And goes into a blocking loop awaiting configuration

	if (!wifiManager.autoConnect(AP_NAME))
	{
		Serial.println("Failed to connect; timed out");
		delay(3000);
		// Reset and try again, or maybe put it to deep sleep
		ESP.reset();
		delay(5000);
	}

	// If we get here you have connected to the WiFi
	Serial.println("Connected!");

	Serial1.begin(CLOCK_SERIAL);

	timeClient.update();
}

void loop()
{
	char tstr[128];
	unsigned char cs;
	unsigned int i;
	time_t rawtime, loctime;
	unsigned long amicros, umicros = 0;

	for (;;)
	{
		amicros = micros();
		if (timeClient.update())						// NTP-update
		{
			umicros = amicros;
			rawtime = timeClient.getEpochTime();		// Get NTP-time
			loctime = tzClock.toLocal(rawtime);			// Calc local time

			if ((!second(loctime)) || firstrun)			// Full minute or first cycle
			{
				digitalWrite(LED_BUILTIN, HIGH);		// Blink for sync
				sprintf(tstr, "$GPRMC,%02d%02d%02d,A,0000.0000,N,00000.0000,E,0.0,0.0,%02d%02d%02d,0.0,E,S",
						hour(loctime), minute(loctime), second(loctime), day(loctime), month(loctime), year(loctime) - 2000);
				cs = 0;
				for (i = 1; i < strlen(tstr); i++)		// Calculate checksum
					cs ^= tstr[i];
				sprintf(tstr + strlen(tstr), "*%02X", cs);
				Serial.println(tstr);					// Send to debug
				Serial1.println(tstr);					// Send to clock
				delay(100);
				digitalWrite(LED_BUILTIN, LOW);
				delay(58000 - ((micros() - amicros) / 1000) - (second(loctime) * 1000)); // Wait for end of minute
				firstrun = 0;
			}
		}
		delay(200);
		if (((amicros - umicros) / 1000000L) > 3600)	// If no sync for more than one hour
			digitalWrite(LED_BUILTIN, HIGH);			// Switch off LED
	}
}