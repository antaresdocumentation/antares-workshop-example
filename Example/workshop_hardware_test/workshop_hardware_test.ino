#include "DHT.h"

#define DHTTYPE     DHT11
#define SENSOR_DHT  D2
#define LED_RED     D6
#define LED_BLUE    D0
#define LED_GREEN   D5
#define RELAY       D7

DHT dht(SENSOR_DHT, DHTTYPE);

void ledBlueOn()
{
  digitalWrite(LED_GREEN,HIGH);
  digitalWrite(LED_BLUE,LOW);
  digitalWrite(LED_RED,HIGH);
} 

void ledBlueOff()
{
  digitalWrite(LED_BLUE,HIGH);
} 

void ledRedOn()
{
  digitalWrite(LED_GREEN,HIGH);
  digitalWrite(LED_BLUE,HIGH);
  digitalWrite(LED_RED,LOW);
} 

void ledRedOff()
{
  digitalWrite(LED_RED,HIGH);
} 

void ledGreenOn()
{
  digitalWrite(LED_BLUE,HIGH);
  digitalWrite(LED_GREEN,LOW);
  digitalWrite(LED_RED,HIGH);
} 

void ledGreenOff()
{
  digitalWrite(LED_GREEN,HIGH);
} 

void relayOn()
{
  digitalWrite(RELAY,HIGH);
}

void relayOff()
{
  digitalWrite(RELAY,LOW);
}

float getTemperature()
{
  float t = dht.readTemperature();
  if (isnan(t)) return 0;
  return t;
}

float getHumidity()
{
  float h = dht.readHumidity();
  if (isnan(h)) return 0;
  return h;
}


void setup() {
  Serial.begin(115200);
  pinMode(LED_RED,OUTPUT);
  pinMode(LED_GREEN,OUTPUT);
  pinMode(LED_BLUE,OUTPUT);

  digitalWrite(LED_RED,HIGH);
  digitalWrite(LED_GREEN,HIGH);
  digitalWrite(LED_BLUE,HIGH);
  
  pinMode(RELAY,OUTPUT);

  dht.begin();

  Serial.println("Test Hardware Workshop Antares ...");
}

void loop() {
  Serial.println("==================TESTING=================");
  Serial.println("Temperature : " + (String)getTemperature()+" *C");
  Serial.println("Humidity    : " + (String)getHumidity()+" %");
  Serial.println("LED BLUE    : ON");
  ledBlueOn();
  delay(1000);
  Serial.println("LED BLUE    : OFF");
  ledBlueOff();
  delay(1000);

  Serial.println("LED GREEN   : ON");
  ledGreenOn();
  delay(1000);
  Serial.println("LED GREEN   : OFF");
  ledGreenOff();

  delay(1000);
  Serial.println("LED RED     : ON");
  ledRedOn();
  delay(1000);
  Serial.println("LED RED     : OFF");
  ledRedOff();

  Serial.println("RELAY       : ON");
  relayOn();
  delay(1000);
  Serial.println("RELAY       : OFF\n");
  relayOff();
  delay(2000);
}
