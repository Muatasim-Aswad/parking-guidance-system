//https://wokwi.com/projects/329965885225697876

#include <LiquidCrystal.h>
#include <Servo.h>
Servo myservo;
int pos = 0;
                                 
int  idh;
int  id1=0, id2=0, id3=0;
int id []= {id1,id2,id3};

int p1,p2,p3;
int place [3];

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int gi_trig=25, gi_echo=27, gi_red=22;
const int p1_trig=31, p1_echo=33, p1_green=28;
const int p2_trig=37, p2_echo=39, p2_green=34;
const int p3_trig=43, p3_echo=45, p3_green=40;


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

  lcd.begin(16, 2);
  lcd.print("Otopark Sistemi");
  delay(1000);
  
  lcd.clear();
  lcd.print (id [0]);
  lcd.print (id [1]);
  lcd.print (id [2]);
  lcd.print (id [3]);
    delay(3000);

}

void loop() {
  
  int old[]= {p1,p2,p3};

  p1 = IsItEmpty(p1_trig, p1_echo, p1_green);
  p2 = IsItEmpty(p2_trig, p2_echo, p2_green);
  p3 = IsItEmpty(p3_trig, p3_echo, p3_green);
 
  int toplam = p1+p2+p3;

  if (toplam == 0)
  {
    digitalWrite(gi_red, HIGH);
    lcd.clear();
    lcd.print("otopark dolu");
    lcd.setCursor(0, 1);
    lcd.print("   0 dolu 0   ");
    delay(1000);
  }  
  else
  {
    digitalWrite(gi_red, LOW);
    CheckEntrance(gi_trig, gi_echo);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(toplam);
    lcd.print(" otopark var");
  
    lcd.setCursor(0, 1);
    int p[]= {p1,p2,p3};
    for (byte i = 0; i < 3; i++)
    {
      if (p[i]==1) {
        lcd.print("  P0");
        lcd.print(i+1);}
      else 
        lcd.print("     ");
  
      if (p[i]!=old[i])
      {
        if (p[i]==0)
        place [i]= idh;
        else {
          DeleteID (place [i]);
          place [i]= 0;}
      }
    }
      delay(1000);
  }
  
  lcd.clear();
  lcd.print (place [0]);
  lcd.print (place [1]);
  lcd.print (place [2]);
  delay(3000);

}

void OpenEntrance ()
{
  for (pos=0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(20);                       // waits 15ms for the servo to reach the position
  }
}

void CloseEntrance ()
{   
  for (pos=90; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}

int IsItEmpty(int a, int b, int Led)
{
  int distance = DistanceUltrasonic (a,b);
  if (distance > 100) 
    digitalWrite(Led, HIGH);
  else 
    digitalWrite(Led, LOW);
  int x = digitalRead(Led);
  return x;
}

void CheckEntrance(int a, int b)
{
  int distance = DistanceUltrasonic (a,b);
  if (distance < 100) { 
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Lutfen bekle");
    lcd.setCursor(0, 1);
    lcd.print("id numarasi: ");
    delay (5000);
    idh = DefineID ();
    lcd.clear();
    lcd.print(idh); 
    lcd.setCursor(0, 1);
    lcd.print(idh);
    lcd.print( " girebilirsiniz");
    OpenEntrance ();
    while (distance < 100){
      distance = DistanceUltrasonic (a,b);}
    }
 
  if (pos != 0 && distance > 100)
  CloseEntrance ();
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

int DefineID()
{
  int j;
  int idx=0;
  int x=0;
  while (idx == 0) //check for similar id
  {
  x++;
  for (byte i = 0; i < 3; i++)
  {
   if (id[i]==x)
    {
    idx=0;
    break;
    }
   else
   {idx = x;

    }
  }
  }
  for (byte i = 0; i < 3; i++) //check for empty
  {
   if (id[i]==0)
   {
     id[i]=idx;
     idx=0;
     j=i;
     break;
   }
   else if (i==2)
   return 0;
  }
  lcd.clear();
  lcd.print (id [0]);
  lcd.print (id [1]);
  lcd.print (id [2]);
  lcd.print (id [3]);
  delay(3000);
  return id[j];
}

void DeleteID (int x)
{
  for (byte i = 0; i < 3; i++)
  {
    if (id[i]==x){
      id[i]=0;
      break; }
  }
}