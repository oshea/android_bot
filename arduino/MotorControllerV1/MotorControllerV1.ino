#include <Wire.h>
#include <Adafruit_MotorShield.h>

#include "Queue.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61); with a different I2C address (say for stacking)

Adafruit_DCMotor *left_front_motor = AFMS.getMotor(1);
Adafruit_DCMotor *right_front_motor = AFMS.getMotor(2);
Adafruit_DCMotor *left_back_motor = AFMS.getMotor(3);
Adafruit_DCMotor *right_back_motor = AFMS.getMotor(4);

unsigned int left_front_encoder_counter = 0;
unsigned int right_front_encoder_counter = 0;

unsigned int left_front_encoder_pin = 2;
unsigned int right_front_encoder_pin = 3;

struct Command {
  byte type;
  word distance;
  int speed;
  int turn_degrees; 
};

unsigned int left_encoder_target;
unsigned int right_encoder_target;

//QueueList <Command> command_queue;
Queue<Command> command_queue = Queue<Command>(10);

void init_motor(Adafruit_DCMotor *motor) {
  motor->setSpeed(150);
  motor->run(FORWARD);
  motor->setSpeed(0);
  motor->run(RELEASE);  
}

void count_left_encoder() {
//  Serial.println("Left tick");
  left_front_encoder_counter++;
  check_encoder_targets();
} 

void count_right_encoder() {
//  Serial.println("Right tick");
  right_front_encoder_counter++;
  check_encoder_targets();
} 


void check_encoder_targets() {
  if(has_active_target()) {
    if(left_encoder_target <= left_front_encoder_counter && right_encoder_target <= right_front_encoder_counter) {
      Serial.println("Encoder targets reached");
//      stop_all_motors();
      left_encoder_target = 0;
      right_encoder_target = 0;  
    }
  }
}

bool has_active_target() {
  return left_encoder_target > 0 || right_encoder_target > 0;
}

void go_forward() {
  left_front_motor->run(FORWARD);
  left_front_motor->setSpeed(150);

  right_front_motor->run(FORWARD);
  right_front_motor->setSpeed(150);

  left_back_motor->run(FORWARD);
  left_back_motor->setSpeed(150);

  right_back_motor->run(FORWARD);
  right_back_motor->setSpeed(150);
}

void go_backward() {
  left_front_motor->run(BACKWARD);
  left_front_motor->setSpeed(150);

  right_front_motor->run(BACKWARD);
  right_front_motor->setSpeed(150);

  left_back_motor->run(BACKWARD);
  left_back_motor->setSpeed(150);

  right_back_motor->run(BACKWARD);
  right_back_motor->setSpeed(150);  
}

void stop_all_motors() {
  left_front_motor->setSpeed(0);
  left_front_motor->run(RELEASE);

  right_front_motor->setSpeed(0);
  right_front_motor->run(RELEASE);

  left_back_motor->setSpeed(0);
  left_back_motor->run(RELEASE);

  right_back_motor->setSpeed(0);
  right_back_motor->run(RELEASE);
}

void turn_in_place() {
  left_front_motor->run(FORWARD);
  left_front_motor->setSpeed(50);

  right_front_motor->run(BACKWARD);
  right_front_motor->setSpeed(50);

  left_back_motor->run(FORWARD);
  left_back_motor->setSpeed(50);

  right_back_motor->run(BACKWARD);
  right_back_motor->setSpeed(50);  
}

void run_diagnostics() {
//  process_command(Command { 1 });
//  delay(1000);
//  
//  process_command(Command { 0 });
//  delay(1000);
//  
//  process_command(Command { 2 });
//  delay(1000);
//
//  process_command(Command { 3 });
//  delay(1000);  
// 
//  process_command(Command { 0 });
//  delay(1000);


  process_command(Command { 1, 100 });
  process_command(Command { 2, 100 });
  process_command(Command { 1, 300 });
  process_command(Command { 2, 300 });
}

void process_command(Command cmd) {
  if(has_active_target()) {
    // Do nothing and put cmd in the queue
    command_queue.push(cmd);
    Serial.print("Queuing command: ");
    Serial.println(cmd.type);
    return;
  }

  Serial.print("Running command:");
  Serial.println(cmd.type);

  if (cmd.type == 0 ) {
    // Stop
    stop_all_motors();
  } else if(cmd.type == 1 && cmd.distance > 0) { 
    //Forward with distance
    //Set motor and monitor ticks
    right_encoder_target = right_front_encoder_counter + cmd.distance;
    left_encoder_target = left_front_encoder_counter + cmd.distance;
    go_forward();
  } else if (cmd.type == 1) {
    //Forward with no distance
    //Set motor clear current command
    go_forward();
  } else if (cmd.type == 2 && cmd.distance > 0) {
    //Backword with distance
    //Set motor and monitor ticks
    right_encoder_target = right_front_encoder_counter + cmd.distance;
    left_encoder_target = left_front_encoder_counter + cmd.distance;
    go_backward();  
  } else if (cmd.type == 2) {
    //Backward with no distance
    go_backward();
  } else if (cmd.type == 3) {
    stop_all_motors();
    turn_in_place();
  } else if (cmd.type == 4) {
    run_diagnostics();
  } else {
    stop_all_motors();
  }
}

void setup() {
  Serial.begin(9600);

  AFMS.begin();  // create with the default frequency 1.6KHz
  
  attachInterrupt(digitalPinToInterrupt(left_front_encoder_pin), count_left_encoder, RISING);
  attachInterrupt(digitalPinToInterrupt(right_front_encoder_pin), count_right_encoder, RISING);

  init_motor(left_front_motor);
  init_motor(right_front_motor);
  init_motor(left_back_motor);
  init_motor(right_back_motor);

  left_encoder_target = 0;
  right_encoder_target = 0;

  run_diagnostics();
}

void loop() {
  if(has_active_target()) {
    // Do nothing
//    Serial.print("active target ");
//    Serial.print(left_encoder_target);
//    Serial.print(" current count ");
//    Serial.println(left_front_encoder_counter);
  } else if(command_queue.count() > 0) {
    Serial.println("Pulling command off queue");
    process_command(command_queue.pop());
  } else {
    Serial.println("No active target or commands");
    stop_all_motors();
    // Put diagnostic commands on the queue
//    Serial.println("Running diagnostic commands");
//    run_diagnostics();
  }
}
