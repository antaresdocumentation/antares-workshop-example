#include "AntaresESPHTTP.h"
#include <PubSubClient.h>

#define ACCESSKEY "your-access-key"
#define WIFISSID "your-wifi-ssid"
#define PASSWORD "your-wifi-password"

#define URI_PROJECT_CONTROL "your-Non-hierarchical URI"
#define RELAY D7
#define LED_RED     D6
#define LED_BLUE    D0
#define LED_GREEN   D5

String projectName = "WorkshopAntares";
String deviceName = "SmartSwitch_1";
const char* MQTTServer = "platform.antares.id";
char payload[1024];
char topicPublish[250];
char topicSubscribe[250];
char clientID[30];
String dataFromAntares = "";

String Value,Unit,Label;
int statusSwitch=0;

Antares antares(ACCESSKEY);
StaticJsonBuffer<10000> jsonBuffer;

WiFiClient espWifiClient;
PubSubClient client(espWifiClient);

void reconnect() {
  digitalWrite(LED_BUILTIN,LOW);
  while (!client.connected()) {
    Serial.print("Connecting MQTT Server ...");
    String tempClientID = "ANTARES_"+projectName+"_"+ (String)random(0,100000);
    tempClientID.toCharArray(clientID, tempClientID.length()+1);

    if (client.connect(clientID)) {
      Serial.println("connected");
      digitalWrite(LED_BUILTIN,HIGH);
      client.subscribe(topicSubscribe,MQTTQOS0);
    } else {
      Serial.print("Failed Connect, rc=");
      Serial.print(client.state());

      for (int i=0; i<30; i++)
      {
        digitalWrite(LED_BUILTIN,digitalRead(LED_BUILTIN)^1);
        delay(100);
      }
      digitalWrite(LED_BUILTIN,LOW);

    }
  }
  
}

void setup() {
  Serial.begin(115200); 
  antares.setDebug(true);
  antares.wifiConnection(WIFISSID,PASSWORD);
  pinMode(RELAY,OUTPUT);
  pinMode(LED_BLUE,OUTPUT);
  pinMode(LED_RED,OUTPUT);
  pinMode(LED_GREEN,OUTPUT);
  pinMode(LED_BUILTIN,OUTPUT);
  client.setServer(MQTTServer, 1883);
  client.setCallback(callback);
  String tempTopic = "/oneM2M/req/"+(String)ACCESSKEY+"-"+projectName+"/antares-cse/json";
  tempTopic.toCharArray(topicPublish, tempTopic.length()+1);
  Serial.println("Topic Publish : " + tempTopic);
  tempTopic = "/oneM2M/resp/antares-cse/"+(String)ACCESSKEY+"-"+projectName+"/json";
  tempTopic.toCharArray(topicSubscribe, tempTopic.length()+1);
  Serial.println("Topic Subscribe : " + tempTopic);
  reconnect();
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] \n\n");
  for (int i = 0; i < length; i++) {
    dataFromAntares += (char)payload[i];
  }
}

String getPayload(String Token, String Project, String Device)
{
  String result = "{\"m2m:rqp\": {\"fr\": \""+Token+"\",\"to\": \"/antares-cse/antares-id/"+Project+"/"+Device+"_Status\",\"op\": 1,\"rqi\": 123456,\"pc\": {\"m2m:cin\": {\"cnf\": \"message\",\"con\": \"{"; 
  result +="\"temperature\" : \""+(String)random(0,10)+"\"/>";
  result +="\"humidity\" : \""+(String)random(0,10)+"\"/>";
  result +="}\" }},\"ty\": 4 }}"; 
  return result;
}

void sendDataToAntares()
{
  String tempPayload = getPayload(ACCESSKEY,projectName,deviceName);
  Serial.println("Send data ...");
  tempPayload.toCharArray(payload, tempPayload.length()+1);
  Serial.println(payload);
  client.publish(topicPublish, payload);      
}

void loop() {
  if (antares.checkWifiConnection())
   {
    if (dataFromAntares != "")
      { 
        dataFromAntares.replace(" ","");
        dataFromAntares.replace("\n","");
        dataFromAntares.replace("\r","");
        dataFromAntares.replace("\\\""," ");
        dataFromAntares.replace("[","");
        dataFromAntares.replace("]","");
        JsonObject& root = jsonBuffer.parseObject(dataFromAntares);
        JsonObject& root4 = jsonBuffer.parseObject(root["m2m:cin"].as<String>());
        Label = root4["pi"].as<String>();
        
        String con = root4["con"].as<String>();
        con.replace(" ","\"");
    
        JsonObject& contentAntares = jsonBuffer.parseObject(con);
        String statusSwitchs = contentAntares["status"].as<String>();
        
        if (Label == URI_PROJECT_CONTROL)
        {
          if (statusSwitchs == "1") 
          {
            digitalWrite(RELAY,LOW);
            statusSwitch=0;
          }
          else 
          {
            digitalWrite(RELAY,HIGH);
            statusSwitch=1;
          }  
        }
        jsonBuffer.clear();
        dataFromAntares=""; 
      }
  
      if (!client.connected())reconnect();
     
      client.loop();
   }
}
