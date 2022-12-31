/* --------- Libraries --------- */
#include <NetworkSetup.hpp>

/* --------- Global Objects --------- */

/**
 * 1. Select the network
 * 2. Finish configuration
 */
NetworkSetup networkSetup{"ESP8266-IoT", {SELECT_NETWORK_ROUTE, FINISH_ROUTE}};

/* --------- Functions --------- */

void setup()
{
	Serial.begin(115200);
	delay(5000);

	Serial.println("NetworkSetup::begin();");
	networkSetup.begin();

	Serial.print("NetworkSetup: is setup required? -> ");
	Serial.println(!networkSetup.is_setup());

	if (networkSetup.is_setup())
	{
		Serial.println("NetworkSetup is saved across reboots!");

		Serial.println("Setup configuration will be erased in 10s");
		delay(10000);

		Serial.println("Erasing configuration...");
		networkSetup.reset();
	}
	else
	{
		Serial.print("Setup is required");
	}
}

void loop()
{
}