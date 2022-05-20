#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <MeAuriga.h>
#include <math.h>       

//The value represents how many pulses the motor performs per centimeter.
#define ENCODER_PULSE_PER_CM 0.033719174  // 134 cm / 3974 pulses

String data = "";
// x and y as strings to send them as strings to the backend
String x, y;
float currentPulse1, currentPulse2, averageCurrentPulse;
float GyZ; 
int xCoordinates = 0;
int yCoordinates = 0;
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

// The function makes the mower move right or left randomly.
void randomMoving(){
     if(random(1, 2 +1) == 1){
        move(3, 40 / 100.0 * 255);
        _delay(1);
        move(4, 0);
    }else{
        move(4, 40 / 100.0 * 255);
        _delay(1);
        move(3, 0);
    }
}

void moveForward(){
    move(1, 40 / 100.0 * 255);
}

void moveBackward(){
    move(2, 40 / 100.0 * 255);
}

void moveRight(){
  move(4, 40 / 100.0 * 255);
}

void moveLeft(){
  move(3, 40 / 100.0 * 255);
}

void moveBackwardFor2Sec(){
  move(2, 40 / 100.0 * 255);
  _delay(1);
  move(2, 0);
}

//The function reset the value of the pulse
void resetEncoderPulse(){
  Encoder_1.setPulsePos(0);
  Encoder_2.setPulsePos(0);   
}

void stopMoving(){
  Encoder_1.setTarPWM(0);
  Encoder_2.setTarPWM(0);
  _delay(0.5);
}

//The function calculates the average value of the motors pulse, the value of X and Y, and resets the pulse of the motor after each calculation.
void calculateXY(){
    // GyZ is the degree of Z angle from Gyro sensor
    GyZ = gyro_0.getAngle(3);
    // Get the average pulse of both motors 
    currentPulse1 = -Encoder_1.getPulsePos(); 
    currentPulse2 = Encoder_2.getPulsePos();
    averageCurrentPulse= (currentPulse1 + currentPulse2) / 2;
    xCoordinates = xCoordinates + averageCurrentPulse* ENCODER_PULSE_PER_CM * cos(GyZ * PI / 180);
    yCoordinates = yCoordinates + averageCurrentPulse* ENCODER_PULSE_PER_CM * sin(GyZ * PI / 180);
    x = String(xCoordinates);
    y = String(yCoordinates);
    resetEncoderPulse();
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
  while(1) { 
    if (Serial.available() > 0) {
      data = Serial.readStringUntil('\n');
    }
    //The char is the first character of the RPI command.
      char *cstr = &data[0];
      switch(*cstr){
        case 'F':
           moveForward();
           break;
        case 'B':
          moveBackward();
          break;
        case 'R':
          moveRight();
          break;     
        case 'L':
          moveLeft();
          break;  
        // The G case is the GoRandom case. It sends the coordinates when there is a boundary or collision
        case 'G':
          moveForward();
          if(linefollower_9.readSensors() == 0){
              calculateXY();
              moveBackwardFor2Sec();
              randomMoving();
              moveForward();
              Serial.print("lineDetected");
              Serial.print(",");
              Serial.print(x);
              Serial.print(",");
              Serial.print(y);
              Serial.println();
            }
           else if( ultrasonic_10.distanceCm() <= 12 && ultrasonic_10.distanceCm() > 11){
                calculateXY();
                Serial.print("objectDetected");
                Serial.print(",");
                Serial.print(x);
                Serial.print(",");
                Serial.print(y);
                Serial.println();
                delay(200);
            }
            if(ultrasonic_10.distanceCm() < 10){          
              moveBackwardFor2Sec();
              randomMoving();
              moveForward();
            }
        break;
       case 'S':
        stopMoving();
        break;
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
