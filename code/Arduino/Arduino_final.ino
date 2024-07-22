//---------------------------------------------------------------------------------------
// First, we define libraries and global variables.

#include <LiquidCrystal_I2C.h> // LCD and I2C communication library
#include <Servo.h>             // Servo motor library
#include <ArduinoJson.h>       // Library for serial communication with JSON variables

// Define pins for ultrasonic sensors and LEDs
const int gi_green = 24, gi_red = 25, gi_trig = 26, gi_echo = 27;
const int p1_green = 28, p1_red = 29, p1_trig = 30, p1_echo = 31;
const int p2_green = 34, p2_red = 35, p2_trig = 36, p2_echo = 37;
const int p3_green = 40, p3_red = 41, p3_trig = 42, p3_echo = 43;

LiquidCrystal_I2C lcd(0x27, 20, 21); // I2C address, SDA, and SCL pins

Servo myServo; // Define servo motor, pin: servo 9 or any PWM pin

int pos = 0; // Variable for servo motor position
int distance;

StaticJsonDocument<500> doc; // JSON object for serial communication

String idx, oldIDX; // Temporary ID for IoT or Arduino
String idh;         // ID of the current car
int FirstRun = 1;   // For initial setup

int old[3], p[3]; // Arrays for parking spot statuses
String place[3];  // Array to match IDs and parking spots

//---------------------------------------------------------------------------------------
// Setup function

void setup()
{
  pinMode(gi_trig, OUTPUT); // Set pin modes for each LED and ultrasonic sensor
  pinMode(gi_echo, INPUT);  // Input
  pinMode(gi_red, OUTPUT);
  pinMode(gi_green, OUTPUT);

  pinMode(p1_trig, OUTPUT); // Parking spot 1
  pinMode(p1_echo, INPUT);
  pinMode(p1_green, OUTPUT);
  pinMode(p1_red, OUTPUT);

  pinMode(p2_trig, OUTPUT); // Parking spot 2
  pinMode(p2_echo, INPUT);
  pinMode(p2_green, OUTPUT);
  pinMode(p2_red, OUTPUT);

  pinMode(p3_trig, OUTPUT); // Parking spot 3
  pinMode(p3_echo, INPUT);
  pinMode(p3_green, OUTPUT);
  pinMode(p3_red, OUTPUT);

  myServo.attach(9); // Pin for the servo motor
  myServo.write(0);  // Move to zero position

  Serial2.begin(115200); // Serial communication for Arduino

  lcd.init();                  // Initialize and start LCD
  lcd.backlight();             // LCD backlight
  lcd.print("Parking System"); // Display message on LCD
  delay(2000);
}

//---------------------------------------------------------------------------------------
// Main loop function

void loop()
{

  // Save old parking spot statuses
  for (byte i = 0; i < 3; i++)
  {
    old[i] = p[i];
  }

  // Update current parking spot statuses, 1 if full
  p[0] = IsItFull(p1_trig, p1_echo, p1_green, p1_red);
  p[1] = IsItFull(p2_trig, p2_echo, p2_green, p2_red);
  p[2] = IsItFull(p3_trig, p3_echo, p3_green, p3_red);

  int total = p[0] + p[1] + p[2]; // Sum of occupied spots

  // Define IDs if it's the first run
  if (FirstRun)
  {
    for (byte i = 0; i < 3; i++)
    {
      if (p[i] == 1)
        place[i] = String(i + 1);
    }
    FirstRun = 0;
  }

  // Check for changes in parking spot statuses
  Park_Situation_Change();

  // Send ID and parking status as JSON object to NodeMCU
  doc["ID1"] = place[0];
  doc["ID2"] = place[1];
  doc["ID3"] = place[2];

  serializeJson(doc, Serial2);
  delay(500);

  // Check entrance, update LCD, servo, LEDs, and ultrasonic sensor
  Entrance(total);
}

//---------------------------------------------------------------------------------------
// Function to detect parking spot status and control LEDs

int IsItFull(int trigPin, int echoPin, int greenLed, int redLed)
{
  distance = DistanceUltrasonic(trigPin, echoPin);
  if (distance > 9)
  {
    digitalWrite(greenLed, HIGH);
    digitalWrite(redLed, LOW);
  }
  else
  {
    digitalWrite(greenLed, LOW);
    digitalWrite(redLed, HIGH);
  }
  int status = digitalRead(redLed);
  return status;
}

//---------------------------------------------------------------------------------------
// Function to get distance using ultrasonic sensor

int DistanceUltrasonic(int trigPin, int echoPin)
{
  digitalWrite(trigPin, LOW); // Ensure a clean low signal
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); // Send trigger pulse
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH); // Measure pulse duration
  return duration * (0.034 / 2);          // Calculate distance
}

//---------------------------------------------------------------------------------------
// Function to check the entrance and control various devices

void Entrance(int total)
{
  if (total == 3)
  { // Parking is full
    digitalWrite(gi_red, HIGH);
    digitalWrite(gi_green, LOW);
    lcd.clear();
    lcd.print("Parking Full");
    lcd.setCursor(0, 1);
    lcd.print("   0 full 0   ");
    delay(500);
  }
  else
  { // Parking has space
    digitalWrite(gi_red, LOW);
    digitalWrite(gi_green, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(3 - total);
    lcd.print(" spots available");

    lcd.setCursor(0, 1); // Display empty parking spots on LCD
    for (byte i = 0; i < 3; i++)
    {
      if (p[i] == 0)
      {
        lcd.print("  P0");
        lcd.print(i + 1);
      }
      else
      {
        lcd.print("     ");
      }
    }
    delay(500);
    CheckEntrance(gi_trig, gi_echo); // Check if a car is at the entrance
  }
}

//---------------------------------------------------------------------------------------
// Function to check if a car is at the entrance and perform necessary actions

void CheckEntrance(int trigPin, int echoPin)
{
  distance = DistanceUltrasonic(trigPin, echoPin);
  if (distance < 6)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Please wait");
    lcd.setCursor(0, 1);
    lcd.print("Enter ID in Blynk");
    RecieveID(trigPin, echoPin); // Get the user's ID from NodeMCU
    if (distance < 6)
    {
      DefineID(); // Define the ID
      lcd.clear();
      lcd.print(idh);
      lcd.setCursor(0, 1);
      lcd.print(" can enter");
      OpenEntrance(); // Open the entrance gate
      while (distance < 8)
      { // Wait until the car moves
        distance = DistanceUltrasonic(trigPin, echoPin);
      }
      delay(1000);
      CloseEntrance(); // Close the entrance gate
    }
  }
  else if (pos != 0)
  { // If the entrance gate is not closed, close it
    CloseEntrance();
  }
}

//---------------------------------------------------------------------------------------
// Function to receive the user's ID from NodeMCU

void RecieveID(int trigPin, int echoPin)
{
  for (byte i = 0; i < 20; i++)
  { // Wait approximately 20 seconds
    delay(1000);
    DeserializationError error = deserializeJson(doc, Serial2);

    oldIDX = idx;
    if (!error && oldIDX != doc["IDX"])
    { // Get IDX from NodeMCU
      idx = doc["IDX"].as<String>();
      delay(500);
    }
    distance = DistanceUltrasonic(trigPin, echoPin);
    if (distance > 8)
      break;
    if (idx != oldIDX) // Exit loop if ID is received
      break;
  }
}

//---------------------------------------------------------------------------------------
// Function to define the ID, automatically if not provided by the user
// Ensures the ID is unique and a parking spot is available

void DefineID()
{
  int y = 0;
  if (idx == oldIDX)
    idx = "0";

  while (idx == "0")
  { // Create a unique ID
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
// Function to open the gate

void OpenEntrance()
{
  for (pos; pos <= 90; pos += 1)
  {
    myServo.write(pos);
    delay(15);
  }
}

//---------------------------------------------------------------------------------------
// Function to close the gate

void CloseEntrance()
{
  for (pos; pos >= 0; pos -= 1)
  {
    myServo.write(pos);
    delay(15);
  }
}

//---------------------------------------------------------------------------------------
// Function to update the ID and parking spot arrays when parking spot status changes

void Park_Situation_Change()
{
  for (byte i = 0; i < 3; i++)
  {
    if (p[i] != old[i])
    {
      if (p[i] == 1)
      {
        place[i] = idh;
        idh = "undefined";
      }
      else
      {
        place[i] = "0";
      }
    }
  }
}
