#include "ESP8266WiFi.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>

String accessKey = "e7e349fc2216941a:9d0cf82c25277bdd";
String projectID = "WorkshopAntares";
String projectName = "SmartFan_1";
const char* MQTTServer = "platform.antares.id";
char payload[1024];
char topicPublish[250];
char topicSubscribe[250];
char clientID[30];
String dataFromAntares = "";


const char* ssid = "mobile";
const char* password = "1326060813";

String Value,Unit,Label;

int statusFan;

#define URI_PROJECT_CONTROL "/antares-cse/cnt-48253522"


StaticJsonBuffer<10000> jsonBuffer;

WiFiClient espWifiClient;
PubSubClient client(espWifiClient);

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  for (int i=0; i<60; i++) {
    delay(500);
    digitalWrite(D4,digitalRead(D4)^1);
    Serial.print(".");
    if (WiFi.status() == WL_CONNECTED) i=61;
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

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
  // put your setup code here, to run once:
  Serial.begin(115200); 
  setup_wifi();
  pinMode(D5,OUTPUT);
  pinMode(LED_BUILTIN,OUTPUT);
  client.setServer(MQTTServer, 1883);
  client.setCallback(callback);
  String tempTopic = "/oneM2M/req/"+accessKey+"-"+projectName+"/antares-cse/json";
  tempTopic.toCharArray(topicPublish, tempTopic.length()+1);
  Serial.println("Topic Publish : " + tempTopic);
  tempTopic = "/oneM2M/resp/antares-cse/"+accessKey+"-"+projectName+"/json";
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
  String result = "{\"m2m:rqp\": {\"fr\": \""+Token+"\",\"to\": \"/antares-cse/antares-id/"+Project+"/"+Device+"_Status\",\"op\": 1,\"rqi\": 123456,\"pc\": {\"m2m:cin\": {\"cnf\": \"message\",\"con\": \"<obj>"; 
  result +="<int name=\\\"temperature\\\" val=\\\""+(String)random(0,10)+"\\\"/>";
  result +="<int name=\\\"humidity\\\" val=\\\""+(String)random(0,10)+"\\\"/>";
  result +="</obj>\" }},\"ty\": 4 }}"; 
  return result;
}

void sendDataToAntares()
{
  String tempPayload = getPayload(accessKey,projectID,projectName);
  Serial.println("Send data ...");
  tempPayload.toCharArray(payload, tempPayload.length()+1);
  Serial.println(payload);
  client.publish(topicPublish, payload);      
}

void loop() {
  if (dataFromAntares != "")
  { 
    dataFromAntares.replace(" ","");
    dataFromAntares.replace("\n","");
    dataFromAntares.replace("\r","");
    dataFromAntares.replace("\\\""," ");
    dataFromAntares.replace("[","");
    dataFromAntares.replace("]","");
    JsonObject& root = jsonBuffer.parseObject(dataFromAntares);
    JsonObject& root2 = jsonBuffer.parseObject(root["m2m:rsp"].as<String>());
    JsonObject& root3 = jsonBuffer.parseObject(root2["pc"].as<String>());
    JsonObject& root4 = jsonBuffer.parseObject(root3["m2m:cin"].as<String>());
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
        Serial.println("FAN ON");
      }
      else 
      {
        digitalWrite(D5,HIGH);
        Serial.println("FAN OFF");
        statusFan=1;
      }  
    }

    jsonBuffer.clear();
    dataFromAntares=""; 
  }

  if (!client.connected()) {
      reconnect();
    }
      client.loop();
}
