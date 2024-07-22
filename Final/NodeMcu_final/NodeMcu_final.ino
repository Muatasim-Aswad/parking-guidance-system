/*---------------------------------------------------------------------------------------
Tanimlamalar ve kutuphaneler*/

#define BLYNK_TEMPLATE_ID           "TMPLHz-2F1jH"  /*Blynk ile baglanmak icin tanimlamalar*/
#define BLYNK_DEVICE_NAME           "OtoPark"
#define BLYNK_AUTH_TOKEN            "WHScUPyfdXMfVvCYjf-MUeU65XglM9i_"
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h> /*Nodemcu wifi icin*/
#include <BlynkSimpleEsp8266.h>

#include <SoftwareSerial.h> /*arduino ile serial haberlesme kurmak icin*/
#include <ArduinoJson.h>  /*serial haberlesme degisik veri gondermek ve almak icin*/

SoftwareSerial nodemcu(D5, D6); /*Rx,Tx arduino ile ters baglanmali*/

char auth[] = BLYNK_AUTH_TOKEN; 
char ssid[] = "TurkTelekom_ZTK4SC_2.4GHz"; /*wifi adi ve sifresi*/
char pass[] = "eaEse47hz4Fd";

BlynkTimer timer;

String IDX,ID1,ID2,ID3; /*Node araciligiyla haberlesilecek degiskenler*/
StaticJsonDocument<1000> doc; /*json object tanimlama*/

/*---------------------------------------------------------------------------------------
hep aktif fonk*/

WidgetTerminal terminal(V0);  /*blynk tararfindan bir id girildiginde fonksiyon calisacak*/
BLYNK_WRITE(V0)
{
 /*hazir kullanilan bir id ise hangi parkta oldugunu ve kullanici yeni ise baska bir id iste*/
 if (ID1 == param.asStr())      
    terminal.println("1. Parkta, Araba girişte ise farklı bir ID sec");
 else if (ID2 == param.asStr())
    terminal.println("2. Parkta, Araba girişte ise farklı bir ID sec");
 else if (ID3 == param.asStr())
    terminal.println("3. Parkta, Araba girişte ise farklı bir ID sec");
 else { /*id arduinoya gonder*/
  IDX = param.asStr();
  doc["IDX"] = IDX;
  serializeJson(doc, nodemcu);
  Serial.print(IDX);
  terminal.println(IDX);
  terminal.println("ID otoparka gonderildi");
  }    
 terminal.flush();
}

/*---------------------------------------------------------------------------------------
ayarlama fonk*/

void setup()
{
  //Serial.begin(115200);
  
  Blynk.begin(auth, ssid, pass);
  terminal.clear();

  nodemcu.begin(115200); 
}

/*---------------------------------------------------------------------------------------
ana fonk*/

void loop()
{
  Blynk.run();
  timer.run();

  /*park idleri Arduinodan al*/
  DeserializationError error = deserializeJson(doc, nodemcu);
  
  if (!error){
  /*Json'dan IDleri al*/
  ID1 = doc["ID1"].as<String>();
  ID2 = doc["ID2"].as<String>();
  ID3 = doc["ID3"].as<String>();

  /*IDleri Blynke gonder*/
  Blynk.virtualWrite(V1,ID1);
  Blynk.virtualWrite(V2,ID2);
  Blynk.virtualWrite(V3,ID3);
  }
}