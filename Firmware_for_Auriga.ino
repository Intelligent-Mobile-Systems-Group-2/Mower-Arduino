#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <MeAuriga.h>
#include <math.h>       




MeGyro gyro_0(0, 0x69);
MeEncoderOnBoard Encoder_1(SLOT1);
MeEncoderOnBoard Encoder_2(SLOT2);
MeLineFollower linefollower_9(9);
MeUltrasonicSensor ultrasonic_10(10);
MeLightSensor lightsensor_12(12);

void isr_process_encoder1(void)
{
  if(digitalRead(Encoder_1.getPortB()) == 0){
    Encoder_1.pulsePosMinus();
  }else{
    Encoder_1.pulsePosPlus();
  }
}
void isr_process_encoder2(void)
{
  if(digitalRead(Encoder_2.getPortB()) == 0){
    Encoder_2.pulsePosMinus();
  }else{
    Encoder_2.pulsePosPlus();
  }
}
void move(int direction, int speed)
{
  int leftSpeed = 0;
  int rightSpeed = 0;
  if(direction == 1){
    leftSpeed = -speed;
    rightSpeed = speed;
  }else if(direction == 2){
    leftSpeed = speed;
    rightSpeed = -speed;
  }else if(direction == 3){
    leftSpeed = -speed;
    rightSpeed = -speed;
  }else if(direction == 4){
    leftSpeed = speed;
    rightSpeed = speed;
  }
  Encoder_1.setTarPWM(leftSpeed);
  Encoder_2.setTarPWM(rightSpeed);
}

void _delay(float seconds) {
  if(seconds < 0.0){
    seconds = 0.0;
  }
  long endTime = millis() + seconds * 1000;
  while(millis() < endTime) _loop();
}

void setup() {
  Serial.begin(9600);
  gyro_0.begin();

  TCCR1A = _BV(WGM10);
  TCCR1B = _BV(CS11) | _BV(WGM12);
  TCCR2A = _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS21);
  attachInterrupt(Encoder_1.getIntNum(), isr_process_encoder1, RISING);
  attachInterrupt(Encoder_2.getIntNum(), isr_process_encoder2, RISING);
  randomSeed((unsigned long)(lightsensor_12.read() * 123456));
  
  String data = "";
  String Gx;
  String Gy;
  String Gz;
  String RPM;
  String velocity;

  String x;
  String y;
  
  float currentRPM;
  float diameter;
  float pi;
  float currentVelocityInInch;
  float currentVelocityInCm;
  int GyX, GyY, GyZ; 
  float Cx =0;
  float Cy =0;
  int lenght;
  
  while(1) {
   
    GyX = gyro_0.getAngle(1); 
    GyY = gyro_0.getAngle(2); 
    GyZ = gyro_0.getAngle(3);
    pi = 3.1415926535897;

    currentRPM = Encoder_1.getCurrentSpeed(); // hatighet när den kör höger vänster if satser
    diameter = 2.36220472;
    Gx = String(GyX);
    Gy = String(GyY);
    Gz = String(GyZ);
    RPM = String(currentRPM);
    currentVelocityInInch = (currentRPM * ( diameter*pi))/60; //inch/sec
    currentVelocityInCm = currentVelocityInInch * (-2.54);  //cm/sec
    velocity = String(currentVelocityInCm);
    
    lenght = currentVelocityInCm/4;   // s = v/t
    
    //Cx = sin(GyZ * pi / 180) * lenght;
    //Cy = cos(GyZ * pi / 180) * lenght;
    Cx =  Cx + currentVelocityInCm * cos(GyZ*0.0174532925);
    Cy =  Cy + currentVelocityInCm * sin(GyZ*0.0174532925);
    
    x = String(Cx / 100);
    y = String(Cy / 100);
    
    if (Serial.available() > 0) {
      data = Serial.readStringUntil('\n');
      //Serial.println(data);
    }
      Serial.print("X value :"+ x + "X speed = "+ (currentVelocityInCm * cos(GyZ*0.0174532925)) / 100);
      Serial.print(",");
      Serial.print("Y value :" + y + "Y speed = "+ (currentVelocityInCm * sin(GyZ*0.0174532925)) / 100);
      Serial.println();
      delay(300);

    
    //if ( data == "goForward" ){
      move(1, 15 / 100.0 * 255);
    //}
    if((0?(3==0?linefollower_9.readSensors()==0:(linefollower_9.readSensors() & 3)==3):(3==0?linefollower_9.readSensors()==3:(linefollower_9.readSensors() & 3)==0))){
          move(2, 30 / 100.0 * 255);
          _delay(1);
          move(2, 0);

          move(4, 15 / 100.0 * 255);
          _delay(1);
          move(4, 0);

      }
      if( ultrasonic_10.distanceCm() <= 20 && ultrasonic_10.distanceCm() > 11){
          Serial.print("takePic");
          Serial.print(",");
          Serial.print(Gz);
          Serial.print(",");
          Serial.print(velocity);

          Serial.println();
          delay(300);
      }
      if(ultrasonic_10.distanceCm() < 10){          

          move(2, 30 / 100.0 * 255);
          _delay(1);
          move(2, 0);

          move(4, 15 / 100.0 * 255);
          _delay(1);
          move(4, 0);

      }

      _loop();
  }

}

void _loop() {
  gyro_0.update();
  Encoder_1.loop();
  Encoder_2.loop();
}

void loop() {
  _loop();
}
