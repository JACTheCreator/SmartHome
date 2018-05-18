#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h> 
#include <ESP8266HTTPClient.h>

#include <ArduinoJson.h>
 
ESP8266WebServer server(80);
 
const char* ap_ssid = "SmartBulb";
const char* ap_password = "";

char* wifi_ssid = "";
char* wifi_password = "";

bool isAccessPointRunning = false;

enum wifi_states{CONNECTED_NO_INTERNET, CONNECTED_INTERNET, NO_INTERNET, ATTEMPT_TO_CONNECT} wifi_state;

void setup() 
{
  Serial.begin(115200);
  
  wifi_state = NOT_CONNECTED;
}
 
void loop() 
{
	switch(wifi_state)
	{
		case NOT_CONNECTED:
			if (!isAccessPointRunning)
				hostAccessPoint();
			wifi_state = ATTEMPT_TO_CONNECT;
		break;

		case ATTEMPT_TO_CONNECT:
			handleBody();
			connectToWiFi()
		break;

		case CONNECTED_INTERNET:

		case CONNECTED_NO_INTERNET:
			stopAccessPoint();
		break;
	}
  
}
 
void handleBody() 
{ 
  if (server.hasArg("plain") == false)
  {
    server.send(200, "text/plain", "Body not received");
    return;
  }
  
  String message = server.arg("plain");
  message += "\n";

  server.send(200, "text/plain", message);
  
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(message);

  wifi_ssid = root["SSID"].as<String>().toCharArray();
  wifi_password = root["password"].as<String>().toCharArray();
  
  Serial.print("SSID: ");
  Serial.print(wifi_ssid);
  Serial.println();
  
  Serial.println("password");
  Serial.println(wifi_password);
  Serial.println();
}

void hostAccessPoint()
{
	WiFi.softAP(ap_ssid, ap_password);  
  server.on("/wifi_credentials", handleBody); 
  server.begin();
  isAccessPointRunning = true;
}

void stopAccessPoint()
{
	WiFi.softAPdisconnect();
	isAccessPointRunning = false;
}

void connectToWiFi()
{
  WiFi.begin(ssid, password);
	if (WiFi.status() == WL_CONNECTED) 
	{
			Serial.println("Connected");
			if(isConnectedToInternet())
			{
				wifi_state = CONNECTED_INTERNET;
			}
			else
			{
				wifi_state = CONNECTED_NO_INTERNET;
			}
	}
	else
	{
		wifi_state = NOT_CONNECTED;
	}
}

void disconnectToWiFi()
{
  WiFi.disconnect();
}

bool isConnectedToInternet()
{
	HTTPClient http;

	http.begin("www.google.com");
  int httpCode = http.GET();            
  String payload = http.getString();
  http.end();

  if (httpCode == 200)
		return true;
	return false;
}