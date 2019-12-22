#include <Wire.h>

#define LEFT_MOTOR_SPEED_PIN 6
#define LEFT_MOTOR_DIRECTION_A_PIN 7
#define LEFT_MOTOR_DIRECTION_B_PIN 8
#define RIGHT_MOTOR_SPEED_PIN 11
#define RIGHT_MOTOR_DIRECTION_A_PIN 12
#define RIGHT_MOTOR_DIRECTION_B_PIN 13

unsigned long left_front_encoder_counter = 0;
unsigned long right_front_encoder_counter = 0;

unsigned int left_front_encoder_pin = 2;
unsigned int right_front_encoder_pin = 3;

void count_left_encoder() {
  Serial.println("Left tick");
  left_front_encoder_counter++;
} 

void count_right_encoder() {
  Serial.println("Right tick");
  right_front_encoder_counter++;
} 

void receiveEvent(int howMany) {
  while (1 < Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
//  Serial.print("Received: ");
//  Serial.println(x);         // print the integer
}

void setup ( ) {
  Wire.begin(8); 
  Wire.onReceive(receiveEvent);
  
  Serial.begin (9600); //Starting the serial communication at 9600 baud rate

  pinMode(LEFT_MOTOR_SPEED_PIN, OUTPUT);
  pinMode(LEFT_MOTOR_DIRECTION_A_PIN, OUTPUT);  
  pinMode(LEFT_MOTOR_DIRECTION_B_PIN, OUTPUT);
  pinMode(RIGHT_MOTOR_SPEED_PIN, OUTPUT);
  pinMode(RIGHT_MOTOR_DIRECTION_A_PIN, OUTPUT);  
  pinMode(RIGHT_MOTOR_DIRECTION_B_PIN, OUTPUT);

  pinMode(9, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(left_front_encoder_pin), count_left_encoder, RISING);
  attachInterrupt(digitalPinToInterrupt(right_front_encoder_pin), count_right_encoder, RISING);
}

void loop () {

    int bumper1 = digitalRead(9);
    if(bumper1 == 1) {
      Serial.println("Bumped");
    }
    
    digitalWrite(LEFT_MOTOR_DIRECTION_A_PIN, LOW);
    digitalWrite(LEFT_MOTOR_DIRECTION_B_PIN, HIGH);
    analogWrite(LEFT_MOTOR_SPEED_PIN, 150);

    digitalWrite(RIGHT_MOTOR_DIRECTION_A_PIN, LOW);
    digitalWrite(RIGHT_MOTOR_DIRECTION_B_PIN, HIGH);
    analogWrite(RIGHT_MOTOR_SPEED_PIN, 100);
}
