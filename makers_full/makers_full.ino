#include <DHT.h>
#include <Servo.h>
#define DHTTYPE DHT11

// Define pin number
const byte isCat = 12;
const byte enablePin = 11;
const byte in1Pin = 10;
const byte in2Pin = 9;
const byte DHTPIN = 7;  
const byte Ybutton = 2;
const byte Bbutton = 3;

// Variable for millis()
unsigned long time_prev = 0;

// Variables for DC Motor
boolean isDoor = false;
boolean bReverse = false;
int nspeed = 0;
// Variables for Servo Motor
int servoPos = 0;
int servoParam =0;
int captureCat = 0;
 
String sMeg = "";

// CREATE SENSOR OBJECT
DHT dht(DHTPIN, DHTTYPE);
Servo myservo;

void setup() {
  Serial.begin(9600);

  pinMode(enablePin, OUTPUT);
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);

  pinMode(Ybutton, INPUT);
  pinMode(Bbutton, INPUT);
  pinMode(isCat, INPUT);

  dht.begin();
  myservo.attach(8);
}

void loop() {
  float h = dht.readHumidity();// 습도를 측정합니다.
  float t = dht.readTemperature();// 온도를 측정합니다.

  // 값 읽기에 오류가 있으면 오류를 출력합니다.
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // 보정된 섭씨 값을 가져옵니다.
  float hic = dht.computeHeatIndex(t, h, false);
  Serial.println(sMeg + "TEMPERATURE:" + float(hic) + " bReverse: "+boolean(bReverse));
  
  // 기온이 29도 이하고 문이 열려있으면(isDoor==false)
  // DC모터의 속도를 255로 설정해 겨울용문을 닫음
 if(hic<31&&!isDoor)
  {
    isDoor = true;
    bReverse = true;
    nspeed = 255;
    time_prev = millis();
  }
  // 기온이 29도 이상이고 문이 닫혀있으면(isDoor==true)
  // DC모터의 방향을 바꾸고 속도를 255로 설정해 겨울용문을 엶
  else if(hic>31&&isDoor)
  {
    isDoor = false;
    bReverse = false;
    nspeed = 255;
    time_prev = millis();    
  }

  // 현재 시각이 DC모터 작동으로부터 10초가 흐른 후라면
  // DC모터의 속도를 0으로 만들어 동작을 멈춤
  if(millis()-time_prev>10000){
    nspeed = 0;
  }

  // 포획을 위한 버튼이 눌렸고(captureCat==1) 적외선 센서에 무언가 감지되면 
  // 서보모터를 돌려 문을 닫음
  if(captureCat==1 && digitalRead(isCat)==1){
    ServoON();
  }
  // 노란 버튼이 눌리면 서보모터를 돌려 문을 열고 포획모드를 해제함
  if(digitalRead(Ybutton)==1)
  {
    captureCat = 0;
    ServoOFF();
  } 
  // 파란 버튼이 눌리면 포획모드를 작동시킴
  if(digitalRead(Bbutton)==1){
    captureCat = 1;
  }

  // 모터의 속도와 방향을 지정해 작동
  setMotor(nspeed, bReverse);
  Serial.println(sMeg+ "Speed : " + int(nspeed));
}

void setMotor(int speed, boolean bReverse)
{
  analogWrite(enablePin,speed);
  digitalWrite(in1Pin, bReverse);
  digitalWrite(in2Pin, !bReverse);
}

void ServoON(){
  if(servoParam == 1){
    return;
  }
  else{
    for (int i = 50 ; i>=0 ; i-=10){
      myservo.write(i);
      delay(100);
    }
    servoParam = 1;
  }
  delay(1000);
}

void ServoOFF(){
  if(servoParam==0){
    return;
  }
  servoPos=90;
  myservo.write(servoPos);
  servoParam = 0;
  delay(5000);
}