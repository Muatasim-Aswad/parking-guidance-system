//---------------------------------------------------------------------------------------
//ilk olarak kutuphane ve genel degiskenleri tanimlariz.

#include <LiquidCrystal_I2C.h> //LCD ve I2C haberlesme kutuphanesi
#include <Servo.h>             // Servo motor kutuphanesi
#include <ArduinoJson.h>       // degiskenlerin serial haberlesmesi icin


//ultrasonik ve ledlerin pinleri tanimlama
const int gi_green = 24, gi_red = 25, gi_trig = 26, gi_echo = 27;
const int p1_green = 28, p1_red = 29, p1_trig = 30, p1_echo = 31;
const int p2_green = 34, p2_red = 35, p2_trig = 36, p2_echo = 37;
const int p3_green = 40, p3_red = 41, p3_trig = 42, p3_echo = 43;

LiquidCrystal_I2C lcd(0x27, 20, 21); //I2c adresi,SDA ve SCL pinleri

Servo myservo; //motor adini tanimlama, pin: servo 9 veya herhangi pwm



int pos = 0; //servo motorun konum degiskeni
int distance;

StaticJsonDocument<500> doc; //serial haberlesme icin object

String idx, oldidx; //gecici id icin, IoT den veya arduinoden
String  idh; //simdiki arabanin idisi
int FirstRun = 1; //baslangictaki kod icin

int old[3], p[3]; //park noktalarin durumlarin dizileri
String place [3]; //idleri ve parklari eslestiren dizi




//---------------------------------------------------------------------------------------
//Ayarlama fonksiyonu

void setup() {
  pinMode(gi_trig, OUTPUT);//pinlerin ayarlari her led ve ultrasonik icin
  pinMode(gi_echo, INPUT); //giris
  pinMode(gi_red, OUTPUT);
  pinMode(gi_green, OUTPUT);



  pinMode(p1_trig, OUTPUT); //Park1
  pinMode(p1_echo, INPUT);
  pinMode(p1_green, OUTPUT);
  pinMode(p1_red, OUTPUT);

  pinMode(p2_trig, OUTPUT); //Park2
  pinMode(p2_echo, INPUT);
  pinMode(p2_green, OUTPUT);
  pinMode(p2_red, OUTPUT);


  pinMode(p3_trig, OUTPUT); //Park3
  pinMode(p3_echo, INPUT);
  pinMode(p3_green, OUTPUT);
  pinMode(p3_red, OUTPUT);


  myservo.attach(9); //servo motorun pini
  myservo.write(0); //sifir konuma hareket


  Serial2.begin(115200); //serial haberlesme arduino kanali

  lcd.init(); //lcd ayarlama ve baslatma
  lcd.backlight(); //lcd arka isigi
  lcd.print("Otopark Sistemi"); //lcdye yaz
  delay(2000);
}


//---------------------------------------------------------------------------------------
//hep tekrar edilecek ana fonksiyon

void loop() {

  //eski park durumu kaydetme
  for (byte i = 0; i < 3; i++)
  {
    old[i] = p[i];
  }

  //simdiki park durumu guncelleme, dolu ise 1
  p[0] = IsItFull(p1_trig, p1_echo, p1_green, p1_red);
  p[1] = IsItFull(p2_trig, p2_echo, p2_green, p2_red);
  p[2] = IsItFull(p3_trig, p3_echo, p3_green, p3_red);

  int toplam = p[0] + p[1] + p[2]; //dolularin toplami


  //baslangicta gerekirse idler tanimlama
  if (FirstRun)
  {
    for (byte i = 0; i < 3; i++)
    {
      if (p[i] == 1)
        place[i] = String(i + 1);
    }
    FirstRun = 0;
  }

  //park noktalarindaki durum degisimi icin
  Park_Situation_Change();

  //ID ve park durumu Json objectine aktarip Nodemcuye gonderme
  doc["ID1"] = place[0];
  doc["ID2"] = place[1];
  doc["ID3"] = place[2];

  serializeJson(doc, Serial2);
  delay(500);

  //girisi kontrol etme, lcd, servo, led, ultrasonik
  Entrance(toplam);
}

//---------------------------------------------------------------------------------------
//fonksiyon park noktalarin bosluk durumlarini tespit edip ledleri kontrol eder

int IsItFull(int a, int b, int GreenLed, int RedLed)
{
  distance = DistanceUltrasonic (a, b);
  if (distance > 9)
  {
    digitalWrite(GreenLed, HIGH);
    digitalWrite(RedLed, LOW);
  }
  else
  {
    digitalWrite(GreenLed, LOW);
    digitalWrite(RedLed, HIGH);
  }
  int x = digitalRead(RedLed);
  return x;
}

//---------------------------------------------------------------------------------------
//fonksiyon ultrasonik sensoru kontrol ederek mesafe verir

int DistanceUltrasonic (int Trig, int Echo)
{
  digitalWrite(Trig, LOW); //emin olmak icin
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);  //isaret gonderme
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);
  long duration = pulseIn(Echo, HIGH); //alici isaret alana kadar sure
  return duration * (0.034 / 2);  //mesafe denklemi
}

//---------------------------------------------------------------------------------------
//girisin butun aygitlari kontrol eden fonksiyon

void Entrance(int toplam)
{
  if (toplam == 3) //otopark dolu ise
  {
    digitalWrite(gi_red, HIGH);
    digitalWrite(gi_green, LOW);
    lcd.clear();
    lcd.print("otopark dolu");
    lcd.setCursor(0, 1);
    lcd.print("   0 dolu 0   ");
    delay(500);
  }
  else  //otoparkta bosluk var ise
  {
    digitalWrite(gi_red, LOW);
    digitalWrite(gi_green, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(3-toplam);
    lcd.print(" otopark var");

    lcd.setCursor(0, 1);  //bos parklari lcdye yazma
    for (byte i = 0; i < 3; i++)
    {
      if (p[i] == 0) {
        lcd.print("  P0");
        lcd.print(i + 1);
      }
      else
        lcd.print("     ");
    }
    delay(500);
    CheckEntrance(gi_trig, gi_echo); //giriste araba var ise
  }
}

//---------------------------------------------------------------------------------------
//giriste araba var mi kontrol edip gerekeni gerceklestiren fonk

void CheckEntrance(int a, int b)
{
  distance = DistanceUltrasonic (a, b);
  if (distance < 6) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Lutfen bekle");
    lcd.setCursor(0, 1);
    lcd.print("Blynk'ta ID girin");
    RecieveID (a, b); //Nodemcu den kullanicinin IDsi al
    if (distance < 6)
    { DefineID (); //ID tanimlama
      lcd.clear();
      lcd.print(idh);
      lcd.setCursor(0, 1);
      lcd.print( " girebilirsiniz");
      OpenEntrance (); //giris kapisi ac
      while (distance < 8) {  //araba hareket edene kadar bekle
        distance = DistanceUltrasonic (a, b);
      }
      delay(1000);
      CloseEntrance ();  //giris kapisi kapat

    }
  }
  else if (pos != 0) //giris kapisi kapali degil ise kapat
    CloseEntrance ();
}

//---------------------------------------------------------------------------------------
//Nodemcu den kullanicinin sitede girdigi IDisi alma

void RecieveID (int a, int b)
{

  for (byte i = 0; i < 20; i++) //yaklasik 20 saniye bekleme saglar
  {
    delay (1000);
    DeserializationError error = deserializeJson(doc, Serial2);

    oldidx = idx;
    if (!error && oldidx != doc["IDX"]) //idx al Nodemcu dan
    {
      idx = doc["IDX"].as<String>();
      delay (500);
    }
    distance = DistanceUltrasonic (a, b);
    if (distance > 8)
      break;
    if (idx != oldidx) //id alındıyse donguden cik
      break;
  }
}

//---------------------------------------------------------------------------------------
//Bu fonksiyonda ID tanimlanir, kullanicidan yok ise otomatik olarak
//verilen ID benzer olmamasi ve bir bosluk olmasini kontrol eder

void DefineID()
{
  int y = 0;
  if (idx == oldidx)
    idx = "0";

  while (idx == "0") //create an unique ID
  {
    y++;
    String x = String(y);
    for (byte i = 0; i < 3; i++)
    {
      if (place[i] == x)
      {
        idx = "0";
        break;
      }
      else
      {
        idx = x;
      }
    }
  }
  idh = idx;
}

//---------------------------------------------------------------------------------------
//to open or close the gate

void OpenEntrance ()
{
  for (pos ; pos <= 90; pos += 1) {
    myservo.write(pos);
    delay(15);
  }
}

void CloseEntrance ()
{
  for (pos ; pos >= 0; pos -= 1) {
    myservo.write(pos);
    delay(15);
  }
}

//---------------------------------------------------------------------------------------
//park noktasi durumu degisince id ve park dizilerinde gereken degisimleri yapmak

void Park_Situation_Change()
{
  for (byte i = 0; i < 3; i++)
  {
    if (p[i] != old[i])
    {
      if (p[i] == 1)
      { place [i] = idh;
        idh = "tanimsiz";
      }
      else
        place [i] = "0";

    }
  }
}