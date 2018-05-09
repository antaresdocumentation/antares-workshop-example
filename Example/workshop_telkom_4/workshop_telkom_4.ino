#include "AntaresESPHTTP.h"
#include <ArduinoJson.h>
#include "DHT.h"

#define DHTPIN D2
#define DHTTYPE DHT11
#define RELAY D7
#define LED_RED     D6
#define LED_BLUE    D0
#define LED_GREEN   D5

DHT dht(DHTPIN, DHTTYPE);

#define ACCESSKEY "e7e349fc2216941a:9d0cf82c25277bdd"
#define WIFISSID "workshopantares2"
#define PASSWORD "123456789"

#define URI_PROJECT_CONTROL "/antares-cse/cnt-218353752"

StaticJsonBuffer<10000> jsonBuffer;

String projectName = "WorkshopAntares"; //Application Name Antares
String deviceNameStatus = "SmartSwitch_1_Status"; //Untuk Hardware Update Status
String deviceNameControl = "SmartSwitch_1_Control"; //Untuk Apps/Web/Postman Kirim Data
String Value,Unit,Label;

unsigned long intervalSend=10000; // 10 detik update status ke antares
unsigned long previousMillis=0; 
int statusSwitch=0;

Antares antares(ACCESSKEY);

void ledBlueOn()
{
  digitalWrite(LED_GREEN,HIGH); //OFF
  digitalWrite(LED_BLUE,LOW); //ON
  digitalWrite(LED_RED,HIGH); //OFF
} 

//LED RGB COMMON ANODA (+) - = 0
//KATODA (-) =1
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
    Serial.begin(115200);//default 9600bps -> 115200 byte per second
    antares.setDebug(true);
    antares.wifiConnection(WIFISSID,PASSWORD);
    pinMode(RELAY,OUTPUT); // D7 => OUTPUT
    pinMode(LED_BLUE,OUTPUT);
    pinMode(LED_RED,OUTPUT);
    pinMode(LED_GREEN,OUTPUT); //pinMode (BUTTON,INPUT)
    dht.begin(); //Deklarasi sensor temperature,humidity
}



void loop() {
   unsigned long currentMillis = millis(); 
   if (antares.checkWifiConnection())
   {

    //UPDATE STATUS
     if ((unsigned long)(currentMillis - previousMillis) >= intervalSend) {
         float humidity = dht.readHumidity();
         float temperature = dht.readTemperature();
          
         int sizeData = 3; 
         String dataName[sizeData],dataValue[sizeData];
         dataName[0]= "temperature";
         dataName[1]= "humidity";
         dataName[2]= "status"; //Keadaan RELAY ON/OFF
    
         dataValue[0]= (String)temperature;
         dataValue[1]= (String)humidity;
         dataValue[2]= (String)statusSwitch;
        
        
         String responseCode = antares.storeData(projectName, deviceNameStatus, dataName, dataValue,sizeData);
         
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
         previousMillis = millis();
    }
  
    //Data Terakhir yang ada di 
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
      String statusSwitchs = contentAntares["status"].as<String>(); //1 Atau 0

      
      if (Label == URI_PROJECT_CONTROL)
      {
        if (statusSwitchs == "1") 
        {
          digitalWrite(RELAY,HIGH);
          statusSwitch=1;
        }
        else 
        {
          digitalWrite(RELAY,LOW);
          statusSwitch=0;
        }  
      }
      jsonBuffer.clear();
      dataFromAntares=""; 
    }
   }
   delay(1000);
}

 
