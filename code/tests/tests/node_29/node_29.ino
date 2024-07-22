
#define BLYNK_TEMPLATE_ID           "TMPLHz-2F1jH"
#define BLYNK_DEVICE_NAME           "OtoPark"
#define BLYNK_AUTH_TOKEN            "****"
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include <SoftwareSerial.h>
#include <ArduinoJson.h>

SoftwareSerial nodemcu(D5, D6);

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "****";
char pass[] = "****";

BlynkTimer timer;

String IDX,ID1,ID2,ID3;
StaticJsonDocument<100> doc;
WidgetTerminal terminal(V0);
BLYNK_WRITE(V0)
{
 if (ID1 == param.asStr())
    terminal.println("1. Parkta, Degilsen lutfen degistir");
 else if (ID2 == param.asStr())
    terminal.println("2. Parkta, Degilsen lutfen degistir");
 else if (ID3 == param.asStr())
    terminal.println("3. Parkta, Degilsen lutfen degistir");
 else {
  IDX = param.asStr();
  doc["IDX"] = IDX;
  serializeJson(doc, nodemcu);
  Serial.print(IDX);
  }    
 terminal.flush();
}

void setup()
{
  Serial.begin(115200);
  
  Blynk.begin(auth, ssid, pass);
  terminal.clear();

  nodemcu.begin(115200);

  Serial.print ("hosgeldin");
}

void loop()
{
  Blynk.run();
  timer.run();
  
  DeserializationError error = deserializeJson(doc, nodemcu);
  
  if (!error){
  
  ID1 = doc["ID1"].as<String>();
  ID2 = doc["ID2"].as<String>();
  ID3 = doc["ID3"].as<String>();
  /*Serial.println("JSON Object Recieved");
  Serial.print("ID1:  ");
  Serial.println(ID1);
  Serial.print("ID2:  ");
  Serial.println(ID2);
  Serial.print("ID3:  ");
  Serial.println(ID3);
  Serial.println("-----------------------------------------");*/
  Blynk.virtualWrite(V1,ID1);
  Blynk.virtualWrite(V2,ID2);
  Blynk.virtualWrite(V3,ID3);
  }
}