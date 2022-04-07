#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <MeAuriga.h>

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
  TCCR1A = _BV(WGM10);
  TCCR1B = _BV(CS11) | _BV(WGM12);
  TCCR2A = _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS21);
  attachInterrupt(Encoder_1.getIntNum(), isr_process_encoder1, RISING);
  attachInterrupt(Encoder_2.getIntNum(), isr_process_encoder2, RISING);
  randomSeed((unsigned long)(lightsensor_12.read() * 123456));
  while(1) {
    String data = "";
    if (Serial.available() > 0) {
      data = Serial.readStringUntil('\n');
      Serial.println(data);
    }
    if ( data == "goForward" ){
      move(1, 50 / 100.0 * 255);
    }
      if((0?(3==0?linefollower_9.readSensors()==0:(linefollower_9.readSensors() & 3)==3):(3==0?linefollower_9.readSensors()==3:(linefollower_9.readSensors() & 3)==0))){

          move(2, 50 / 100.0 * 255);
          _delay(1);
          move(2, 0);

          move(4, 50 / 100.0 * 255);
          _delay(2);
          move(4, 0);

      }
      if(ultrasonic_10.distanceCm() < 10){
          move(2, 50 / 100.0 * 255);
          _delay(1);
          move(2, 0);

          move(4, 50 / 100.0 * 255);
          _delay(2);
          move(4, 0);

      }

      _loop();
  }

}

void _loop() {
  Encoder_1.loop();
  Encoder_2.loop();
}

void loop() {
  _loop();
}
