#include "AntaresESPHTTP.h"

#define ACCESSKEY "e7e349fc2216941a:9d0cf82c25277bdd" //sesuai account
#define WIFISSID "mobile"
#define PASSWORD "1326060813"

String projectName = "WorkshopAntares";
String deviceName = "SmartFan_1_Status";

#include "DHT.h"

#define DHTPIN D2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

Antares antares(ACCESSKEY);

void setup() {
    Serial.begin(115200);
    antares.setDebug(true);
    antares.wifiConnection(WIFISSID,PASSWORD);
    pinMode(D5,OUTPUT);
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
   dataValue[2]= digitalRead(D5);
   Serial.println(antares.storeData(projectName, deviceName, dataName, dataValue,sizeData));
   delay(10000);

   antares.retrieveLatestData("WorkshopAntares","SmartFan_1_Control");
   delay(1000);
}

 
