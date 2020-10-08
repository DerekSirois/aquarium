#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <wire.h>
#include "SSD1306.h"
#include <OneWire.h>

SSD1306 display(0x3c, 21, 22); 

const char *ssid = "moreau";
const char *password = "maison123456789";

AsyncWebServer server(80);

OneWire  ds(19);  

byte i;
byte type_s;
byte data[12];
byte addr[8];

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
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "AQUA2000\nip : "+ WiFi.localIP().toString() + "\nTempérature : x degré\nTemp souhaité : x degré\nChauffe: non");
  display.display();
}

void OneWireScanner(){
  if ( !ds.search(addr)) {
    Serial.println("No more addresses.");
    Serial.println();
    ds.reset_search();
    return;
  }
  
  Serial.print("ROM = ");
  for( i = 0; i < 8; i++) {
    Serial.write(' ');
    Serial.print("0x");
    Serial.print(addr[i], HEX);
    if ( i != 7 ) {
      Serial.print(", ");
    }
  }
  
  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return;
  }
  Serial.println();
 
  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
      Serial.println("Device is not a DS18x20 family device.");
      return;
  } 
}

void loop() {
  OneWireScanner();
}