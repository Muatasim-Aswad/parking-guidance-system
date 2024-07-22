
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

//Initialise Arduino to NodeMCU (5=Rx & 6=Tx)
//SoftwareSerial nodemcu(5, 6);

//SoftwareSerial nodemcu(5, 6);
//SoftwareSerial nodemcu(15,14);



String ID1, ID2, ID3, IDX;
int T1=0,T2=3,T3=7;

void setup() {
  Serial.begin(9600);
  //nodemcu.begin(115200);
  Serial2.begin(115200);
  delay(1000);

  Serial.println("Program started");
}

void loop() {

  StaticJsonDocument<100> doc;
  
  if (T3==100)
  {
    T1=0;T2=3;T3=7;
  }
  T1++;
  T2++;
  T3++;
  
  ID1="1";
  ID2="2";
  ID3="3";
 
  //Assign collected data to JSON Object
  doc["ID1"] = ID1;
  doc["ID2"] = ID2;
  doc["ID3"] = ID3;

  //Send data to NodeMCU
  serializeJson(doc, Serial2);
  delay(500);
  
  //StaticJsonDocument<100> cod;

  DeserializationError error= deserializeJson(doc, Serial2);
  delay(500);
  String IDX = doc["IDX"];
  //String IDX= Serial2.readString();

  Serial.print("IDX: ");
  Serial.println(IDX);
  Serial.println(ID1);
  Serial.println(ID2);
  Serial.println(ID3);
}
