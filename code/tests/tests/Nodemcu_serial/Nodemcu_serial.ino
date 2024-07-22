
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

//D6 = Rx & D5 = Tx
SoftwareSerial nodemcu(D5, D6);


//SoftwareSerial nodemcu(D6, D5);
//SoftwareSerial nodemcu(D7, D8);
String IDX;
int x=2;  

void setup() {
  // Initialize Serial port
  Serial.begin(9600);
  nodemcu.begin(115200);

  while (!Serial) continue;
  Serial.println("hosgeldiniz");
  
}

void loop() {

  IDX="2";
  Serial.print("IDX:  ");
  Serial.println(IDX);

 
  //nodemcu.print(IDX);
  
  delay (500);
  StaticJsonDocument<100> doc;
  DeserializationError error = 
  deserializeJson(doc, nodemcu);

  // Test parsing
  //while (error) {
   // delay (5000);
    //Serial.println("Invalid JSON Object");
  //  delay(500);
   // DeserializationError error = deserializeJson(doc, nodemcu);
  //}
if (!error){
  Serial.println("JSON Object Recieved");
  Serial.print("ID1:  ");
  String ID1 = doc["ID1"];
  Serial.println(ID1);
  Serial.print("ID2:  ");
  String ID2 = doc["ID2"];
  Serial.println(ID2);
  Serial.print("ID3:  ");
  String ID3 = doc["ID3"];
  Serial.println(ID3);
  Serial.println("-----------------------------------------");}

   //StaticJsonDocument<100> cod;
  doc["IDX"] = IDX;
  serializeJson(doc, nodemcu);
  delay(500);
  
}