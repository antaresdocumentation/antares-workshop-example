#include "AntaresESPHTTP.h"

#define ACCESSKEY "your-accesskey" //sesuai account
#define WIFISSID "your-wifi-ssid"
#define PASSWORD "your-wifi-pass"

String projectName = "WorkshopAntares";
String deviceName = "SmartSwitch_1_Status";

#include "DHT.h"

#define DHTPIN D2
#define DHTTYPE DHT11
#define RELAY D7
#define LED_RED     D6
#define LED_BLUE    D0
#define LED_GREEN   D5

DHT dht(DHTPIN, DHTTYPE);

Antares antares(ACCESSKEY);

int intervalSend=10;

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

void setup() {
    Serial.begin(115200);
    antares.setDebug(true);
    antares.wifiConnection(WIFISSID,PASSWORD);
    pinMode(RELAY,OUTPUT);
    pinMode(LED_BLUE,OUTPUT);
    pinMode(LED_RED,OUTPUT);
    pinMode(LED_GREEN,OUTPUT);
    dht.begin();
}

void loop() {
   int sizeData = 3; 
   String dataName[sizeData],dataValue[sizeData];
   dataName[0]= "temperature";
   dataName[1]= "humidity";
   dataName[2]= "status";

   dataValue[0]= (String)dht.readTemperature();
   dataValue[1]= (String)dht.readHumidity();
   dataValue[2]= digitalRead(RELAY);

   if (antares.checkWifiConnection())
   {
     String responseCode = antares.storeData(projectName, deviceName, dataName, dataValue,sizeData);
     
     if (responseCode=="201")
     {
        Serial.println("SENDING SUCCESS");
        ledGreenOn();
        delay(1000);
        ledGreenOff();
     }
     else
     {
        Serial.println("SENDING FAILED");
        ledRedOn();
        delay(1000);
        ledRedOff();
     }
   }

   for (int i=0;i<intervalSend;i++)
   {
      ledBlueOn();
      delay(500);
      ledBlueOff();
      delay(500);
   }
}

 
