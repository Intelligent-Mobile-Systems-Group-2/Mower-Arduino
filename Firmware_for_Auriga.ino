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
  String Gz, RPM, velocity, x, y ;

  float currentRPM1, currentRPM2, currentRPM;
  float radius; 
  float angularSpeed, mowerSpeed;
  float GyZ; 
  float Cx =0;
  float Cy =0;

  
  while(1) {
    // GyZ is the degree of Z angle from Gyro 
    GyZ = gyro_0.getAngle(3);
    
    // Get the average of the speed of both motors 
    currentRPM1 = Encoder_1.getCurrentSpeed(); 
    currentRPM2 = -Encoder_2.getCurrentSpeed();
    currentRPM = (currentRPM1 + currentRPM2) / 2;

    // the radius of the wheel in cm
    radius = 2;

    // convert the Z angle and the current speed to strings 
    Gz = String(GyZ);
    RPM = String(currentRPM);

    // calculate the angular speed 
    angularSpeed = currentRPM * 2 * PI/60; 
    //angularSpeed = (2*PI*(radius/2)) / (currentRPM/60);

    //calculate the speed v=rω   cm/s
    mowerSpeed = radius * angularSpeed;
    


    // convert the velocity of the mower to string
    //velocity = String(mowerSpeed);
    
    //lenght = angularSpeed * 1;   // s = v*t

    // calculate X and Y coordinates
    //Cx = Cx + cos(GyZ * PI / 180) * lenght;
    //Cy = Cy + sin(GyZ * PI / 180) * lenght;
    Cx =  Cx + mowerSpeed * cos(GyZ * PI / 180);
    Cy =  Cy + mowerSpeed * sin(GyZ * PI / 180);

    //convert X and Y to strings
    x = String(Cx);
    y = String(Cy);
    
    if (Serial.available() > 0) {
      data = Serial.readStringUntil('\n');
      //Serial.println(data);
    }

      Serial.print("X value :" + x);
      Serial.print(",");
      Serial.print("Y value :" + y);
      Serial.println();
      delay(300);
      
    char *cstr = &data[0];
   
      switch(*cstr){
        case 'F':
           move(1, 30 / 100.0 * 255);
           break;
        case 'B':
          move(2, 30 / 100.0 * 255);
          break;
        case 'R':
          move(4, 40 / 100.0 * 255);
          break;     
        case 'L':
          move(3, 40 / 100.0 * 255);
          break;     
        case 'G':
              //if ( data == "goForward" ){
            move(1, 40 / 100.0 * 255);
          //}
          if((0?(3==0?linefollower_9.readSensors()==0:(linefollower_9.readSensors() & 3)==3):(3==0?linefollower_9.readSensors()==3:(linefollower_9.readSensors() & 3)==0))){
              Serial.print("lineDetected");
              Serial.print(",");
              Serial.print(x);
              Serial.print(",");
              Serial.print(y);
              Serial.println();
              delay(300);
              move(2, 40 / 100.0 * 255);
              _delay(1);
              move(2, 0);
              if(random(1, 2 +1) == 2.000000){
                  move(4, 40 / 100.0 * 255);
                  _delay(1);
                  move(4, 0);
              }else{
                  move(3, 40 / 100.0 * 255);
                  _delay(1);
                  move(3, 0);
              }
              move(1, 40 / 100.0 * 255);
      
            }
            if( ultrasonic_10.distanceCm() <= 20 && ultrasonic_10.distanceCm() > 11){
                Serial.print("objectDetected");
                Serial.print(",");
                Serial.print(x);
                Serial.print(",");
                Serial.print(y);
                Serial.println();
                delay(300);
            }
            if(ultrasonic_10.distanceCm() < 10){          

              move(2, 40 / 100.0 * 255);
              _delay(1);
              move(2, 0);
              if(random(1, 2 +1) == 1){
                  move(4, 40 / 100.0 * 255);
                  _delay(1);
                  move(4, 0);
              }else{
                  move(3, 40 / 100.0 * 255);
                  _delay(1);
                  move(3, 0);
              }
              move(1, 40 / 100.0 * 255);
            }
        break;
        case 'T':
          if(Cx < 100 && Cy < 100){
             move(1,40 / 100.0 * 255);
            }
           else{
             move(3,40 / 100.0 * 255);
           }
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
