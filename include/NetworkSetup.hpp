#ifndef __NetworkSetup_H__
#define __NetworkSetup_H__

#include <string>
#include <vector>
#include <Arduino.h>
#include <Ticker.h>
#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncDNSServer.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266mDNS.h>

/* --------- Configuration Constants --------- */

#ifndef DNS_PORT
#define DNS_PORT 53
#endif

#ifndef HTTP_PORT
#define HTTP_PORT 80
#endif

#ifndef DISCOVERY_TIME
#define DISCOVERY_TIME 10000
#endif

#ifndef EEPROM_START_ADDRESS
#define EEPROM_START_ADDRESS 0x00
#endif

/* --------- System routes --------- */

#ifndef SELECT_NETWORK_ROUTE
#define SELECT_NETWORK_ROUTE [](AsyncWebServerRequest *request) { request->redirect("/setup/networks"); }
#endif

#ifndef FINISH_ROUTE
#define FINISH_ROUTE [](AsyncWebServerRequest *request) { request->redirect("/setup/finish"); }
#endif

/* --------- Class definitions --------- */

/**
 * @struct NetworkSetupDatabase
 * @brief Store SSID and PASSWORD information for connections
 */
struct NetworkSetupDatabase
{
	char ssid[32];
	char password[32];
};

#define NETWORK_DATABASE_SIZE sizeof(NetworkSetupDatabase)

class NetworkSetup
{
public:
	/* --------- Typedefs --------- */
	using RoutesList = std::vector<ArRequestHandlerFunction>;

private:
	/* --------- Network constants --------- */
	const static inline IPAddress address{192, 168, 100, 24};
	const static inline IPAddress netmask{255, 255, 0, 0};

	/* --------- Variables --------- */
	std::string network_ssid;
	NetworkSetupDatabase database;

public:
	/* --------- Servers --------- */
	AsyncWebServer server; /**< HTTP Server, it is public so you can add your own routes */
	AsyncDNSServer dns;	   /**< DNS Server, public if you need to tweek it */

	/* --------- Tickers --------- */
	Ticker network_discovery_ticker{}; /**< Periodically check networks, loops can block it and yield() won't solve it, so it's public in order to manually trigger it*/

private:
	/* --------- Routes --------- */
	std::size_t current_route;
	RoutesList routes;

	/* --------- Inner state variables --------- */
	std::string network_list;

	/* --------- Private functions --------- */
private:
	void init_access_point();
	void init_captive_portal();
	void init_network_discovery();
	void eeprom_clear();
	void eeprom_save();

	/* --------- Public functions --------- */
public:
	NetworkSetup(std::string &&ssid, RoutesList &&routes = {SELECT_NETWORK_ROUTE, FINISH_ROUTE});
	void begin();
	void reset();
	bool is_setup() const;
	NetworkSetupDatabase get_network_setup() const;
};

#endif // __NetworkSetup_H__