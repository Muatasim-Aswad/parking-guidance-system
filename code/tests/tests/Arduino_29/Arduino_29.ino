#include <LiquidCrystal.h>
#include <Servo.h>
#include <ArduinoJson.h>
StaticJsonDocument<100> doc;


Servo myservo;
int pos = 0;
                                 
String  idh;
String  id1="0", id2="0", id3="0";
String id []= {id1,id2,id3};


int old[3],p[3];
String place [3];

//servo 9
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int gi_trig=24, gi_echo=25, gi_green=26, gi_red=27;
const int p1_trig=30, p1_echo=31, p1_green=32, p1_red=33;
const int p2_trig=36, p2_echo=37, p2_green=38, p2_red=39;
const int p3_trig=42, p3_echo=43, p3_green=44, p3_red=45;

String idx,oldidx;


void setup() {
  pinMode(gi_trig, OUTPUT);
  pinMode(gi_echo, INPUT);
  pinMode(gi_red, OUTPUT);

  pinMode(p1_trig, OUTPUT);
  pinMode(p1_echo, INPUT);
  pinMode(p1_green, OUTPUT);

  pinMode(p2_trig, OUTPUT);
  pinMode(p2_echo, INPUT);
  pinMode(p2_green, OUTPUT);

  pinMode(p3_trig, OUTPUT);
  pinMode(p3_echo, INPUT);
  pinMode(p3_green, OUTPUT);

  myservo.attach(9);

  Serial.begin(9600);
  Serial2.begin(115200);

  lcd.begin(16, 2);
  lcd.print("Otopark Sistemi");
  delay(1000);
  Serial.println("otopark sistemine hos geldiniz");
  
  lcd.clear();
  Serial.print(id [0]);
  Serial.print(id [1]);
  Serial.print(id [2]);
  Serial.println(id [3]);
  delay(3000);



}

void loop() {
  
  for (byte i = 0; i < 3; i++)
  {old[i]= p[i];}

  p[0] = IsItEmpty(p1_trig, p1_echo, p1_green, p1_red);
  p[1] = IsItEmpty(p2_trig, p2_echo, p2_green, p2_red);
  p[2] = IsItEmpty(p3_trig, p3_echo, p3_green, p3_red);

  int toplam = p[0]+p[1]+p[2];

  Entrance(toplam);
  
  Park_Situation_Change();
  
  lcd.clear();
  Serial.print(place [0]);
  Serial.print(place [1]);
  Serial.println(place [2]);
  delay(1000);

  int FirstRun=1;
  
  /*if (FirstRun)
  {
    for (byte i = 0; i < 3; i++)
    {
    if (p[i]==0)
      id[i]=place[i]=String(i+1);
    }
    FirstRun=0;
  }*/

  doc["ID1"] = place[0];
  doc["ID2"] = place[1];
  doc["ID3"] = place[2];

  //Send data to NodeMCU
  serializeJson(doc, Serial2);
  delay(500);
  
  /*DeserializationError error= deserializeJson(doc, Serial2);
  
  oldidx=idx;
  if(!error && oldidx != doc["IDX"])
  {
    idx = doc["IDX"].as<String>();
    Serial.print("IDX:");
    Serial.println(idx);
    delay (500);
  }*/

}

int IsItEmpty(int a, int b, int GreenLed,int RedLed)
{
  int distance = DistanceUltrasonic (a,b);
  if (distance > 6)
  {
    digitalWrite(GreenLed, HIGH);
    digitalWrite(RedLed, LOW);
  }
  else
  {
    digitalWrite(GreenLed, LOW);
    digitalWrite(RedLed, HIGH);
  }
  int x = digitalRead(GreenLed);
  return x;
}

int DistanceUltrasonic (int Trig, int Echo) 
{
  digitalWrite(Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);
  long duration = pulseIn(Echo, HIGH);
  return duration * (0.034 / 2);
}


void Entrance(int toplam)
{
  if (toplam == 0)
  {
    digitalWrite(gi_red, HIGH);
    digitalWrite(gi_green,LOW);
    lcd.clear();
    lcd.print("otopark dolu");
    lcd.setCursor(0, 1);
    lcd.print("   0 dolu 0   ");
    delay(3000);
  }  
  else
  {
    digitalWrite(gi_red, LOW);
    digitalWrite(gi_green, HIGH);
    CheckEntrance(gi_trig, gi_echo);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(toplam);
    lcd.print(" otopark var");
  
    lcd.setCursor(0, 1);
    for (byte i = 0; i < 3; i++)
    {
      if (p[i]==1) {
        lcd.print("  P0");
        lcd.print(i+1);}
      else 
        lcd.print("     ");
    }
    delay(1000);
  }
}


void CheckEntrance(int a, int b)
{
  int distance = DistanceUltrasonic (a,b);
  if (distance < 6) { 
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Lutfen bekle");
    Serial.print("Lutfen bekle");
    lcd.setCursor(0, 1);
    lcd.print("id numarasi: ");
    Serial.println("id numarasi: ");
    RecieveID ();
    idh = DefineID ();
    lcd.clear();
    lcd.print(idh); 
    Serial.println(idh);
    lcd.setCursor(0, 1);
    lcd.print(idh);
    Serial.println(idh);
    Serial.println(" girebilirsiniz");
    lcd.print( " girebilirsiniz");
    OpenEntrance ();
    while (distance < 4){
      distance = DistanceUltrasonic (a,b);}
    }
 
  if (pos != 0 && distance > 4)
  CloseEntrance ();
} 

void RecieveID ()
{
  for (byte i = 0; i < 10; i++)
    {
      delay (1000);
      DeserializationError error= deserializeJson(doc, Serial2);
  
      oldidx=idx;
      if(!error && oldidx != doc["IDX"])
      {
        idx = doc["IDX"].as<String>();
        Serial.print("IDX:");
        Serial.println(idx);
        delay (500);
      }
      if (idx != oldidx)
      break;
      Serial.println("id gir sitede");
      delay(1000);
    }
}
String DefineID()
{
  int j;
  
  int y=0;
  if (idx == oldidx)
  idx="0";

  while (idx == "0") //check for similar id
  {
  y++;
  String x= String(y);
  for (byte i = 0; i < 3; i++)
  {
   if (id[i]==x)
    {
    idx="0";
    break;
    }
   else
   {idx = x;

    }
  }
  }
  for (byte i = 0; i < 3; i++) //check for empty
  {
   if (id[i]=="0")
   {
     id[i]=idx;
     idx="0";
     j=i;
     break;
   }
   else if (i==2)
   return "bos yok";
  }
  lcd.clear();
  lcd.print (id [0]);
  lcd.print (id [1]);
  lcd.print (id [2]);
  lcd.print (id [3]);
  delay(3000);
  return id[j];
}

void OpenEntrance ()
{
  for (pos=0; pos <= 90; pos += 1) { 
    // in steps of 1 degree
    myservo.write(pos);              
    delay(20);                       
  }
}

void CloseEntrance ()
{   
  for (pos=90; pos >= 0; pos -= 1) { 
    myservo.write(pos);              
    delay(15);                       
  }
}


void Park_Situation_Change()
{
  for (byte i = 0; i < 3; i++)
  {
    if (p[i]!=old[i])
    {
      if (p[i]==0)
      place [i]= idh;
      else {
        DeleteID (place [i]);
        place [i]= "0";}
    }
  }
}


void DeleteID (String x)
{
  for (byte i = 0; i < 3; i++)
  {
    if (id[i]==x){
      id[i]="0";
      break; }
  }
}