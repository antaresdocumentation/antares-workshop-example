#include "AntaresESPHTTP.h"
#include <ArduinoJson.h>
#include "DHT.h"

#define DHTPIN D2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

#define ACCESSKEY "e7e349fc2216941a:9d0cf82c25277bdd"
#define WIFISSID "mobile"
#define PASSWORD "1326060813"

#define URI_PROJECT_CONTROL "/antares-cse/cnt-48253522"

StaticJsonBuffer<10000> jsonBuffer;

String projectName = "WorkshopAntares";
String deviceNameStatus = "SmartFan_1_Status";
String deviceNameControl = "SmartFan_1_Control";
String Value,Unit,Label;

Antares antares(ACCESSKEY);

void setup() {
    Serial.begin(115200);
    antares.setDebug(true);
    antares.wifiConnection(WIFISSID,PASSWORD);
    pinMode(D5,OUTPUT);
    dht.begin();
}

unsigned long sendInterval=10000; // the time we need to wait
unsigned long previousMillis=0; 
int statusFan=0;

void loop() {
   unsigned long currentMillis = millis(); 
 
   if ((unsigned long)(currentMillis - previousMillis) >= sendInterval) {
      float humidity = dht.readHumidity();
      float temperature = dht.readTemperature();
      
      int sizeData = 3; 
      String dataName[sizeData],dataValue[sizeData];
      dataName[0]= "temperature";
      dataName[1]= "humidity";
      dataName[2]= "status";

      dataValue[0]= (String)humidity;
      dataValue[1]= (String)temperature;
      dataValue[2]= (String)statusFan;
     
      antares.storeData(projectName, deviceNameStatus, dataName, dataValue,sizeData);
      previousMillis = millis();
  }

  String dataFromAntares = antares.retrieveLatestData(projectName,deviceNameControl);

   if (dataFromAntares != "")
  { 
    dataFromAntares.replace(" ","");
    dataFromAntares.replace("\n","");
    dataFromAntares.replace("\r","");
    dataFromAntares.replace("\\\""," ");
    dataFromAntares.replace("[","");
    dataFromAntares.replace("]","");
    //Serial.println(dataFromAntares);
    JsonObject& root = jsonBuffer.parseObject(dataFromAntares);
    JsonObject& root4 = jsonBuffer.parseObject(root["m2m:cin"].as<String>());
    Label = root4["pi"].as<String>();
    
    String con = root4["con"].as<String>();
    con.replace(" ","\"");

    JsonObject& contentAntares = jsonBuffer.parseObject(con);
    String statusFans = contentAntares["status"].as<String>();
    
    if (Label == URI_PROJECT_CONTROL)
    {
      if (statusFans == "1") 
      {
        digitalWrite(D5,LOW);
        statusFan=0;
      }
      else 
      {
        digitalWrite(D5,HIGH);
        statusFan=1;
      }  
    }
    jsonBuffer.clear();
    dataFromAntares=""; 
  }
   delay(1000);
}

 
