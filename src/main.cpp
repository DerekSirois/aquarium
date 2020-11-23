#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <wire.h>
#include "SSD1306.h"
#include <OneWire.h>
#include <DallasTemperature.h>


SSD1306 display(0x3c, 21, 22); 

const char *ssid = "moreau";
const char *password = "maison123456789";

AsyncWebServer server(80);

// le pin pour le onewire
const int oneWireBus = 19;     

// Setup du onewire
OneWire oneWire(oneWireBus);

DallasTemperature sensors(&oneWire);

int tempMin = 20;

int tempMax = 25;

int tempsEntre = 10;

int tempsFiltre = 10;

bool chauffe = false;

unsigned long previousMillis = millis();

bool filtrage = false;

bool filtrageEnCours = false;

void setup() {
  //serial
  Serial.begin(115200);
  display.init();
  sensors.begin();
  //GPIO
    
    pinMode(18, OUTPUT);
    pinMode(27, OUTPUT);
    digitalWrite(27, HIGH);
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
  server.on("/index_en.html", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/index_en.html", "text/html");
  });

  server.on("/w3.css", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/w3.css", "text/css");
  });

  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/script.js", "text/javascript");
  });
  server.on("/jquery-3.5.1.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/jquery-3.5.1.min.js", "text/javascript");
  });
  server.on("/lireDegre", HTTP_GET, [](AsyncWebServerRequest *request){
    sensors.requestTemperatures(); 
    int val = int(sensors.getTempCByIndex(0));
    String luminosite = String(val);
    request->send(200, "text/plain", luminosite);
    
  });
  server.on("/getTempMin", HTTP_GET, [](AsyncWebServerRequest *request){
    String tempMinStr = String(tempMin);
    request->send(200, "text/plain", tempMinStr);
    
  });
  server.on("/getTempMax", HTTP_GET, [](AsyncWebServerRequest *request){
    String tempMaxStr = String(tempMax);
    request->send(200, "text/plain", tempMaxStr);
    
  });

  server.on("/getTempsEntre", HTTP_GET, [](AsyncWebServerRequest *request){
    String tempsEntreStr = String(tempsEntre);
    request->send(200, "text/plain", tempsEntreStr);
    
  });
  server.on("/getTempsFiltre", HTTP_GET, [](AsyncWebServerRequest *request){
    String tempsFiltreStr = String(tempsFiltre);
    request->send(200, "text/plain", tempsFiltreStr);
    
  });
  server.on("/setTempsMin", HTTP_POST, [](AsyncWebServerRequest *request){
    if(request->hasParam("tempMin", true))
    {
      String message;
      message = request->getParam("tempMin", true)->value();
      tempMin = message.toInt();
    }
    Serial.println(tempMin);
    request->send(204);
  });
  server.on("/setTempsMax", HTTP_POST, [](AsyncWebServerRequest *request){
    if(request->hasParam("tempMax", true))
    {
      String message;
      message = request->getParam("tempMax", true)->value();
      tempMax = message.toInt();
    }
    Serial.println(tempMax);
    request->send(204);
  });

  server.on("/setTempsEntre", HTTP_POST, [](AsyncWebServerRequest *request){
    if(request->hasParam("tempsEntre", true))
    {
      String message;
      message = request->getParam("tempsEntre", true)->value();
      tempsEntre = message.toInt();
    }
    Serial.println(tempsEntre);
    request->send(204);
  });
  server.on("/setTempsFiltre", HTTP_POST, [](AsyncWebServerRequest *request){
    if(request->hasParam("tempsFiltre", true))
    {
      String message;
      message = request->getParam("tempsFiltre", true)->value();
      tempsFiltre = message.toInt();
    }
    Serial.println(tempsFiltre);
    request->send(204);
  });
  

  server.begin();
  Serial.println("Serveur actif!");
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "AQUA2000\nip : "+ WiFi.localIP().toString() + "\nTempérature : x degré\nTemp minimum : "+tempMin+" degré\nChauffe: non");
  display.display();
}

void verifierTemp(){
    display.clear();
    sensors.requestTemperatures(); 
    int tempActuel = int(sensors.getTempCByIndex(0));
    //Serial.println(tempActuel);
    if(tempActuel < tempMin){
      chauffe = true;
      digitalWrite(18, HIGH);
      display.setFont(ArialMT_Plain_10);
      display.drawString(0, 0, "AQUA2000\nip : "+ WiFi.localIP().toString() + "\nTempérature : "+tempActuel+" degré\nTemp minimum : "+tempMin+" degré\nChauffe: oui");
      display.display();
    }
    else if(chauffe){
      if(tempActuel > (tempMin + ((tempMax-tempMin)/2))){
        chauffe = false;
        digitalWrite(18, LOW);
      }
      display.setFont(ArialMT_Plain_10);
      display.drawString(0, 0, "AQUA2000\nip : "+ WiFi.localIP().toString() + "\nTempérature : "+tempActuel+" degré\nTemp minimum : "+tempMin+" degré\nChauffe: oui");
      display.display();
    }
    else{
      display.setFont(ArialMT_Plain_10);
      display.drawString(0, 0, "AQUA2000\nip : "+ WiFi.localIP().toString() + "\nTempérature : "+tempActuel+" degré\nTemp minimum : "+tempMin+" degré\nChauffe: non");
      display.display();
    }
}

void filtragePompe(){
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= tempsFiltre * 1000){
    previousMillis = currentMillis;
    if(!filtrage){
      filtrage = true;
      digitalWrite(27, HIGH);
      delay(tempsEntre * 1000);
    }
    else{
      filtrage = false;
      digitalWrite(27, LOW);
    }
  }
}

void loop() {
    verifierTemp();
    filtragePompe();
    /*delay(3000);
    digitalWrite(17, LOW);
    delay(3000);*/
}
