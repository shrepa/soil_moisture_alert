#include "DHT.h"
#define DHTTYPE DHT11
#define AOUT_PIN 36 // ESP32 pin GPIO36 (ADC0) that connects to AOUT pin of moisture sensor
#define THRESHOLD 1488 // CHANGE YOUR THRESHOLD HERE
#define DHT_PIN 39
#define UV_PIN 1
DHT dht(DHT_PIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  // set the ADC attenuation to 11 dB (up to ~3.3V input)
  analogSetAttenuation(ADC_11db);
  //dht.begin();
  pinMode(UV_PIN, INPUT);
}

void loop() {
  // moisture detection
  int value = analogRead(AOUT_PIN); 
  if (value > THRESHOLD)
    Serial.print("The soil is DRY (");
  else
    Serial.print("The soil is WET (");
  Serial.print(value);
  Serial.println(")");

  //temperature detection
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));

  uv detection
  int uv_data = analogRead(UV_PIN);
  Serial.println(uv_data);

  delay(2000);
}
