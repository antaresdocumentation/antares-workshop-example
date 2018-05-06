#include "DHT.h"

#define DHTPIN D2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
 
void setup() {
  Serial.begin(115200);
  dht.begin();
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Gagal Membaca DHT sensor!");
    return;
  }

  Serial.println("Humidity: " + (String)humidity + " %");
  Serial.println("Temperature: " + (String)temperature + " *C");

  delay(1000);
}
