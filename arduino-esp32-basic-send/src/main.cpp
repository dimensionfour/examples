#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include <secrets.h>

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 0;

const char *SSID = WIFI_SSID_SECRET;
const char *WIFIPASS = WIFI_PASSWORD_SECRET;

const char *MQTT_BROKER_ADDRESS = MQTT_BROKER_ADDRESS_SECRET;
const char *MQTT_BROKER_USERNAME = MQTT_BROKER_USERNAME_SECRET;
 const char *MQTT_BROKER_PASSWORD = MQTT_BROKER_PASSWORD_SECRET;
const int MQTT_BROKER_PORT = MQTT_BROKER_PORT_SECRET;

const char *DEVICE_ID = DEVICE_ID_SECRET;
const char *TENANT_ID = TENANT_ID_SECRET;
const char *TENANT_KEY = TENANT_KEY_SECRET;

struct tm timeinfo;

WiFiClient wifiClient;
PubSubClient psClient(wifiClient);


void sendData() {
  Serial.println("Sending data to DimensionFour.io:");

  StaticJsonDocument<200> doc;
  doc["deviceId"] = DEVICE_ID;
  doc["tenantId"] = TENANT_ID;
  doc["tenantKey"] = TENANT_KEY;

  JsonArray jar = doc.createNestedArray("signals");  
  JsonObject sig1 = jar.createNestedObject();

  float measuredtemp = random(1800,2200) / 100.0;
  char temp[32];
  dtostrf(measuredtemp, 0, 2, temp);
  
  sig1["value"] = temp;
  sig1["unit"] = "CELSIUS_DEGREES";
  sig1["type"] = "Temperature";
  
  getLocalTime(&timeinfo);
  char time[100];
  strftime(time, 100, "%FT%T+00:00", &timeinfo);
  sig1["timestamp"] = time;

  serializeJsonPretty(doc, Serial);
  
  char payload[200];
  serializeJson(doc, payload);
  psClient.publish("POINT/PUSH", payload);
}


void reconnectMQTT()
{
  while (!psClient.connected())
  {
    Serial.print("Attempting MQTT connection...");
 
    String clientId = "D4_ESP32_";
    clientId += String(random(0xffff), HEX);
 
    if (psClient.connect(clientId.c_str(), MQTT_BROKER_USERNAME, MQTT_BROKER_PASSWORD))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(psClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.print("Connecting to Wifi: ");
  WiFi.begin(SSID, WIFIPASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("Success!");

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  if(!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time!");
  }

  Serial.print("Current time: ");
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  psClient.setServer(MQTT_BROKER_ADDRESS, MQTT_BROKER_PORT);
}

void loop() {
  sendData();
  reconnectMQTT();
  delay(1000);
}