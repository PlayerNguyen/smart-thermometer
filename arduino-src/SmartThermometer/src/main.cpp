/**
 
 * POWER_LED_OUTPUT
 * If power is flashing, means no connection.
 * Otherwise, is connected
 * 
 * STATUS_OUTPUT
 * If status led is turn on -> transmitting
 * If slowly flashing -> retrieve code 200 after post
 * If quickly flashing -> retrieve other code
 * If turn of -> not transmitting
 * 
 */
#include <Arduino.h>
#include <OneWire.h> 
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const int POWER_LED_PIN = D0;
const int THERMOMETER_PIN = D1; 
const int STATUS_PIN = D2;

const String WIFI_SSID = "";
const String WIFI_PASSWORD = "";
const String API_URL = "";

OneWire oneWire(THERMOMETER_PIN); 
DallasTemperature sensors(&oneWire);


void initializeWiFi() {
  // Connect to the server
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
}

void setup() {
  // Serialization
  Serial.begin(9600);
  sensors.begin();

  // Power LED
  pinMode(POWER_LED_PIN, OUTPUT);
  digitalWrite(POWER_LED_PIN, HIGH);
  pinMode(STATUS_PIN, OUTPUT);

  // WiFi connect
  initializeWiFi();
}


void sendTemperature() {
  digitalWrite(STATUS_PIN, HIGH);
  WiFiClient wifiClient;
  HTTPClient client;
  client.begin(wifiClient, API_URL);
  client.addHeader("Content-Type", "application/json");

  int httpCode = client.POST("{\"temperature\": " + String(sensors.getTempCByIndex(0)) + "}");
  if (httpCode > 0) {
      client.writeToStream(&Serial);

      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] ... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        String payload = client.getString();
        Serial.println(payload);

        // Slowly flashing
        for (int i = 0; i < 3; i++) {
          digitalWrite(STATUS_PIN, LOW);
          delay(500);
          digitalWrite(STATUS_PIN, HIGH);
        }
        
      }
    } else {
      Serial.printf("[HTTP] ... failed, error: %s\n", client.errorToString(httpCode).c_str());
      for (int i = 0; i < 3; i++) {
        digitalWrite(STATUS_PIN, LOW);
        delay(100);
        digitalWrite(STATUS_PIN, HIGH);
      }
    }
  client.end();
  digitalWrite(STATUS_PIN, LOW);
  delay(1000);
}

void sendWiFiInfo() {
  Serial.println("WiFi ~ ");

  Serial.print("Local address: "); 
  Serial.println(WiFi.localIP());

  Serial.print("Connection status: "); 
  Serial.println(WiFi.status());
}

void loopTemperature() {
  Serial.print(" Requesting temperatures..."); 
  sensors.requestTemperatures();
  Serial.println("DONE"); 

  Serial.print("Temperature is: "); 
  Serial.print(sensors.getTempCByIndex(0));
 
  delay(200);
}

void loop() {
  while (!WiFi.isConnected()) {
    Serial.println("No connection ~ Retrying...");
    digitalWrite(POWER_LED_PIN, LOW);
    delay(300);
    digitalWrite(POWER_LED_PIN, HIGH);
    delay(300);
  }

  sendWiFiInfo();
  sendTemperature();
  loopTemperature();
}