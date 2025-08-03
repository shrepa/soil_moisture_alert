/* Get these three details from your Blynk account */
#define BLYNK_TEMPLATE_ID "TMPL3r-yqNHmP"
#define BLYNK_TEMPLATE_NAME "soil health"
#define BLYNK_AUTH_TOKEN "t3VCBy7A2x_EiEnAS7zTzYQVuetSMHj8"

#include "DHT.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <HTTPClient.h>

#define AOUT_PIN 36     // Soil sensor analog
#define UV_PIN   35     // UV sensor analog
#define DHT_PIN  25     // DHT11 digital
#define M_THRESHOLD 1488  // Moisture sensor threshold
#define T_THRESHOLD 28  // Temperature threshold
#define U_THRESHOLD 500 // UV threshold
#define DHTTYPE DHT11   //DHT11 type
DHT dht(DHT_PIN, DHTTYPE); 

char ssid[] = " "; //WiFi Name
char pass[] = " "; //WiFi Password

BlynkTimer timer;
HTTPClient http;

String apiKey = "7b8621f0beb7044a241fbeb01527bdb6";     //OpenWeatherMap API key  
String city = "Mumbai";         
String country = "IN";                       

void checkWeatherAndNotify() {
  if (WiFi.status() == WL_CONNECTED) {
    String endpoint = "http://api.openweathermap.org/data/2.5/forecast?q=" + city + "," + country + "&appid=" + apiKey + "&units=metric";

    http.begin(endpoint);
    int httpCode = http.GET();

    if (httpCode == 200) {
      String payload = http.getString();
      Serial.println("[✓] Weather data fetched");

      if (payload.indexOf("rain") != -1 || payload.indexOf("Rain") != -1) {
        Blynk.logEvent("Weather Alert", "Rain forecasted, do not water the plant.");
      } else {
        Blynk.logEvent("Weather Alert", "Plant needs to be watered NOW.");
      }
    } else {
      Serial.println("Weather fetch failed");
    }

    http.end();
  }
}

void sendData() {
  int soil_value = analogRead(AOUT_PIN);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int uv_data = analogRead(UV_PIN);

  Blynk.virtualWrite(V0, soil_value);
  Blynk.virtualWrite(V1, uv_data);
  Blynk.virtualWrite(V2, t);
  Blynk.virtualWrite(V3, h);

  Serial.print("Soil moisture reading: ");
  Serial.println(soil_value);
  Serial.print("UV reading: ");;
  Serial.println(uv_data);
  Serial.print("Humidity: ");
  Serial.println(h);
  Serial.print("Temperature: ");
  Serial.println(t);

  if (soil_value < M_THRESHOLD && t > T_THRESHOLD && uv_data > U_THRESHOLD) {
    checkWeatherAndNotify();
  }
}
void setup() {
  Serial.begin(115200);
  analogSetAttenuation(ADC_11db);
  dht.begin();
  pinMode(UV_PIN, INPUT);
  WiFi.begin(ssid, pass);
  Blynk.config(BLYNK_AUTH_TOKEN);

  Serial.print("Connecting to Wifi: ");
  Serial.println(ssid);
  
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
  Serial.println("WiFi connection failed. Restarting...");
  delay(5000);
  
}
  timer.setInterval(10000L, sendData);
}

void loop() {
  delay(2000);
  Blynk.run();
  timer.run();
}
