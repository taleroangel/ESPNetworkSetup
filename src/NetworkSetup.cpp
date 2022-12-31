#include <NetworkSetup.hpp>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <string>

/* --------- Constructors --------- */
NetworkSetup::NetworkSetup(std::string &&ssid, RoutesList &&routes)
	: network_ssid{ssid},
	  server{HTTP_PORT},
	  dns{},
	  current_route{0},
	  routes{routes}
{
}

void NetworkSetup::init_access_point()
{
	// Initialize WiFi in AP Mode
	WiFi.persistent(true);
	WiFi.setAutoConnect(true);
	WiFi.mode(WIFI_AP_STA);
	WiFi.softAPConfig(address, address, netmask);
	WiFi.softAP(network_ssid.c_str());

	// Initialize mDNS
	MDNS.begin(network_ssid.c_str());
	MDNS.addService(F("http"), F("tcp"), HTTP_PORT);
}

void NetworkSetup::init_captive_portal()
{
	// Initialize DNS Server
	dns.setErrorReplyCode(AsyncDNSReplyCode::NoError);
	dns.start(DNS_PORT, "*", address);

	server.onNotFound([](AsyncWebServerRequest *request)
					  { request->redirect(F("/setup/")); });

	server.on("/", [&](AsyncWebServerRequest *request)
			  { request->redirect(F("/setup/")); });

	// API Definition
	server.on("/setup/api/list", HTTP_GET, [&](AsyncWebServerRequest *request)
			  { request->send(200, F("application/json"), this->network_list.c_str()); });

	server.on("/setup/api/connect", HTTP_POST, [&](AsyncWebServerRequest *request)
			  {
				  auto ssid = request->getParam(F("ssid"))->value();		 // Get SSID
				  auto password = request->getParam(F("password"))->value(); // Get Password

				  // Store variables for future use
				  strcpy(this->database.ssid, ssid.c_str());
				  strcpy(this->database.password, password.c_str());

				  // Begin connection attempt
				  WiFi.disconnect();
				  WiFi.begin(ssid, password);
				  request->send(200); });

	server.on("/setup/api/status", HTTP_GET, [&](AsyncWebServerRequest *request)
			  { request->send(200, F("text/plain"), std::to_string(WiFi.status()).c_str()); });

	server.on("/setup/api/done", HTTP_POST, [&](AsyncWebServerRequest *request)
			  {  this->eeprom_save(); ESP.restart(); });

	// Next route
	server.on("/setup/next", [&](AsyncWebServerRequest *request)
			  {
				  this->routes[this->current_route](request);
				  this->current_route++;					  // Go to next route
				  this->current_route %= this->routes.size(); // Overflow
			  });

	// Frontend routes
	auto delegate_to_frontend = [&](AsyncWebServerRequest *request)
	{ request->send(LittleFS, F("setup/index.html"), F("text/html")); };

	server.on("/setup/", delegate_to_frontend);
	server.on("/setup/networks", delegate_to_frontend);
	server.on("/setup/connect", delegate_to_frontend);
	server.on("/setup/finish", delegate_to_frontend);

	server.begin();
}

void NetworkSetup::init_network_discovery()
{
	network_discovery_ticker.attach_ms_scheduled(
		DISCOVERY_TIME, [&]()
		{ WiFi.scanNetworksAsync(
			  [&](int n_networks)
			  {	DynamicJsonDocument document{1024}; // 1KB of DynamicJson
				// Add SSID's
				for (uint8_t i = 0; i < n_networks; i++)
				document.add(WiFi.SSID(i));
				// Serialize into JSON array
				this->network_list.clear();
				serializeJson(document, this->network_list); }); });
}

void NetworkSetup::eeprom_clear()
{
	// EEPROM Write 0's
	EEPROM.begin(sizeof(NetworkSetupDatabase));
	memset((void *)&(this->database), 0, sizeof(NetworkSetupDatabase));
	EEPROM.put(EEPROM_START_ADDRESS, this->database);
	EEPROM.commit();
	EEPROM.end();
}

void NetworkSetup::eeprom_save()
{
	EEPROM.begin(sizeof(NetworkSetupDatabase));
	EEPROM.put(EEPROM_START_ADDRESS, database);
	EEPROM.commit();
	EEPROM.end();
}

void NetworkSetup::begin()
{
	// Initialize and read NetworkSetup
	EEPROM.begin(sizeof(NetworkSetupDatabase));
	EEPROM.get<NetworkSetupDatabase>(EEPROM_START_ADDRESS, database);
	EEPROM.end();

	if (!is_setup())
	{
		//* Setup Required
		init_network_discovery(); // Initialize network discovery
		LittleFS.begin();		  // Begin File System
		init_access_point();	  // Initialize AP
		init_captive_portal();	  // Initialize mDNS
	}
	else
	{
		WiFi.mode(WIFI_STA);
		WiFi.begin(database.ssid, database.password);
		WiFi.setAutoConnect(true);
		WiFi.setAutoReconnect(true);
	}
}

void NetworkSetup::reset()
{
	eeprom_clear();
	WiFi.setAutoConnect(false);
	ESP.reset(); // Reset the ESP
}

bool NetworkSetup::is_setup() const
{
	union
	{
		uint8_t raw;
		NetworkSetupDatabase database;
	} _ = {.database = this->database};
	return (bool)(_.raw);
}

NetworkSetupDatabase NetworkSetup::get_network_setup() const
{
	return this->database;
}