
#define BLYNK_TEMPLATE_ID           "TMPLHz-2F1jH"
#define BLYNK_DEVICE_NAME           "OtoPark"
#define BLYNK_AUTH_TOKEN            "****"

#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "****";
char pass[] = "****";

BlynkTimer timer;

WidgetTerminal terminal(V0);
String IDX;
BLYNK_WRITE(V0)
{
 if (IDX == param.asStr()) {
    terminal.println("again") ;
    terminal.println("please'") ;
  }
 else 
    IDX = param.asStr(); 
 terminal.flush();
}

void setup()
{
  Serial.begin(115200);
  
  Blynk.begin(auth, ssid, pass);
  terminal.clear();

  Serial.print ("hosgeldin");
}

void loop()
{
  Blynk.run();
  timer.run();

  String ID1="1";
  String ID2=IDX;
  String ID3="500";
  
  Blynk.virtualWrite(V1,ID1);
  Blynk.virtualWrite(V2,ID2);
  Blynk.virtualWrite(V3,ID3);
}