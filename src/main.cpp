#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <wire.h>
#include "SSD1306.h"

//Adafruit_SSD1306 display(-1);
SSD1306 display(0x3c, 21, 22); 

const char *ssid = "ecoleduweb";
const char *password = "EcoleDuWeb";

AsyncWebServer server(80);

void setup() {
  //serial
  Serial.begin(115200);
  display.init();
  //GPIO
    
  //SPIFFS
  if(!SPIFFS.begin())
  {
    Serial.println("Erreur SPIFFS");
    return;
  }

  File root = SPIFFS.open("/");
  File file = root.openNextFile();

  while(file){
    Serial.print("File: ");
    Serial.println(file.name());
    file.close();
    file = root.openNextFile();
  }
  //WIFI
  WiFi.begin(ssid, password);
	Serial.print("Tentative de connexion...");
	
	while(WiFi.status() != WL_CONNECTED)
	{
		Serial.print(".");
		delay(100);
	}
	
	Serial.println("\n");
	Serial.println("Connexion etablie!");
	Serial.print("Adresse IP: ");
	Serial.println(WiFi.localIP());
  //SERVER
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/w3.css", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/w3.css", "text/css");
  });

  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/script.js", "text/javascript");
  });
  server.on("/lireDegre", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("lire degre");
  });
  server.on("/rechauffe", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("rechauffe");
  });
  server.on("/pompe", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("pompe");
  });

  server.begin();
  Serial.println("Serveur actif!");

  display.drawString(10, 10, "ip : "+ WiFi.localIP().toString() + "\nTempérature : x degré");
  display.display();
}

void loop() {
  
}