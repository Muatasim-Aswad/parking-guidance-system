/*---------------------------------------------------------------------------------------
Definitions and Libraries*/

#define BLYNK_TEMPLATE_ID           "TMPLHz-2F1jH"  /* Definitions for connecting with Blynk */
#define BLYNK_DEVICE_NAME           "OtoPark"
#define BLYNK_AUTH_TOKEN            "****"
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h> /* For NodeMCU WiFi */
#include <BlynkSimpleEsp8266.h>

#include <SoftwareSerial.h> /* For serial communication with Arduino */
#include <ArduinoJson.h>    /* For sending and receiving data in JSON format */

SoftwareSerial nodemcu(D5, D6); /* RX, TX must be connected inversely with Arduino */

char auth[] = BLYNK_AUTH_TOKEN; 
char ssid[] = "****"; /* WiFi name and password */
char pass[] = "****";

BlynkTimer timer;

String IDX, ID1, ID2, ID3; /* Variables for communication via NodeMCU */
StaticJsonDocument<1000> doc; /* Define JSON object */

/*---------------------------------------------------------------------------------------
Always Active Function*/

WidgetTerminal terminal(V0);  /* Function that will be triggered when an ID is entered in Blynk */
BLYNK_WRITE(V0)
{
 /* If the ID is already used, specify the parking spot; if the ID is new, request a different ID */
 if (ID1 == param.asStr())      
    terminal.println("In Parking Spot 1, select a different ID if the car is entering");
 else if (ID2 == param.asStr())
    terminal.println("In Parking Spot 2, select a different ID if the car is entering");
 else if (ID3 == param.asStr())
    terminal.println("In Parking Spot 3, select a different ID if the car is entering");
 else { /* Send the ID to Arduino */
  IDX = param.asStr();
  doc["IDX"] = IDX;
  serializeJson(doc, nodemcu);
  Serial.print(IDX);
  terminal.println(IDX);
  terminal.println("ID sent to parking system");
  }    
 terminal.flush();
}

/*---------------------------------------------------------------------------------------
Setup Function*/

void setup()
{
  //Serial.begin(115200);
  
  Blynk.begin(auth, ssid, pass);
  terminal.clear();

  nodemcu.begin(115200); 
}

/*---------------------------------------------------------------------------------------
Main Function*/

void loop()
{
  Blynk.run();
  timer.run();

  /* Get parking spot IDs from Arduino */
  DeserializationError error = deserializeJson(doc, nodemcu);
  
  if (!error){
  /* Get IDs from JSON */
  ID1 = doc["ID1"].as<String>();
  ID2 = doc["ID2"].as<String>();
  ID3 = doc["ID3"].as<String>();

  /* Send IDs to Blynk */
  Blynk.virtualWrite(V1, ID1);
  Blynk.virtualWrite(V2, ID2);
  Blynk.virtualWrite(V3, ID3);
  }
}
