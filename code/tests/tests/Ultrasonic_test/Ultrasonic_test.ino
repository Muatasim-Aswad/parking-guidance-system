/*

*/
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
  
  Serial.begin(9600);
}

void loop() {
  
  int dg= DistanceUltrasonic (gi_trig, gi_echo);
  int d1= DistanceUltrasonic (p1_trig, p1_echo);
  int d2= DistanceUltrasonic (p2_trig, p2_echo);
  int d3= DistanceUltrasonic (p3_trig, p3_echo);
  Serial.print("giris:");
  Serial.println(dg);
  Serial.print("d1:");
  Serial.println(d1);
  Serial.print("d2:");
  Serial.println(d2);
  Serial.print("d3:");
  Serial.println(d3);
  delay(1000);
  
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