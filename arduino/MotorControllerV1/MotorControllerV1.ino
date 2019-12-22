#include <Wire.h>
#include "Queue.h"

#define DEBUG 0

#define MOTOR_I2C_ADDRESS 4

#define LEFT_MOTOR_SPEED_PIN 11
#define LEFT_MOTOR_DIRECTION_A_PIN 13
#define LEFT_MOTOR_DIRECTION_B_PIN 12
#define RIGHT_MOTOR_SPEED_PIN 8
#define RIGHT_MOTOR_DIRECTION_A_PIN 7
#define RIGHT_MOTOR_DIRECTION_B_PIN 6
#define LEFT_ENCODER_PIN 3
#define RIGHT_ENCODER_PIN 2

#define TICKS_PER_DEGREE 1


unsigned long left_front_encoder_counter = 0;
unsigned long right_front_encoder_counter = 0;
unsigned int left_encoder_target;
unsigned int right_encoder_target;

struct Command {
  int type;
  int distance;
  int speed;
  int turn_degrees; 
};

Queue<Command> command_queue = Queue<Command>(10);

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
  digitalWrite(LEFT_MOTOR_DIRECTION_A_PIN, HIGH);
  digitalWrite(LEFT_MOTOR_DIRECTION_B_PIN, LOW);
  analogWrite(LEFT_MOTOR_SPEED_PIN, 200);

  digitalWrite(RIGHT_MOTOR_DIRECTION_A_PIN, HIGH);
  digitalWrite(RIGHT_MOTOR_DIRECTION_B_PIN, LOW);
  analogWrite(RIGHT_MOTOR_SPEED_PIN, 200);
}

void go_backward() {
  digitalWrite(LEFT_MOTOR_DIRECTION_A_PIN, LOW);
  digitalWrite(LEFT_MOTOR_DIRECTION_B_PIN, HIGH);
  analogWrite(LEFT_MOTOR_SPEED_PIN, 200);

  digitalWrite(RIGHT_MOTOR_DIRECTION_A_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_DIRECTION_B_PIN, HIGH);
  analogWrite(RIGHT_MOTOR_SPEED_PIN, 200);
}

void stop_all_motors() {
  digitalWrite(LEFT_MOTOR_DIRECTION_A_PIN, LOW);
  digitalWrite(LEFT_MOTOR_DIRECTION_B_PIN, LOW);
  analogWrite(LEFT_MOTOR_SPEED_PIN, 0);

  digitalWrite(RIGHT_MOTOR_DIRECTION_A_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_DIRECTION_B_PIN, LOW);
  analogWrite(RIGHT_MOTOR_SPEED_PIN, 0);
}

void turn_in_place(int target_degrees) {
//  Serial.print("Right counter: ");
//  Serial.println(right_front_encoder_counter);
  right_encoder_target = right_front_encoder_counter + (int)(target_degrees * TICKS_PER_DEGREE);
  left_encoder_target = left_front_encoder_counter + (int)(target_degrees * TICKS_PER_DEGREE);

//  Serial.print("Right target: ");
//  Serial.println(right_encoder_target);
  
  digitalWrite(LEFT_MOTOR_DIRECTION_A_PIN, HIGH);
  digitalWrite(LEFT_MOTOR_DIRECTION_B_PIN, LOW);
  analogWrite(LEFT_MOTOR_SPEED_PIN, 150);

  digitalWrite(RIGHT_MOTOR_DIRECTION_A_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_DIRECTION_B_PIN, HIGH);
  analogWrite(RIGHT_MOTOR_SPEED_PIN, 150);
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


  process_command(Command { 1, 50 });
  process_command(Command { 2, 50 });
  process_command(Command { 3, 0, 0, 90 });
  process_command(Command { 1, 50 });
  process_command(Command { 2, 50 });
  process_command(Command { 3, 0, 0, 180 });
  process_command(Command { 1, 50 });
  process_command(Command { 2, 50 });
  process_command(Command { 3, 0, 0, 90 });
  process_command(Command { 1, 50 });
  process_command(Command { 2, 50 });
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
    turn_in_place(cmd.turn_degrees);
  } else if (cmd.type == 4) {
    run_diagnostics();
  } else {
    stop_all_motors();
  }
}

void receiveCommand(int howMany) {
    int c = 0;
    int cmd = 0;
    int param1 = 0;
    int param2 = 0;
    int param3 = 0;
    
    while (Wire.available()) { // loop through all but the last
      int val = Wire.read();
      if(c == 0) {
        cmd = val;
      } else if (c == 1) {
        param1 = val;
      } else if (c == 2) {
        param2 = val;
      } else if (c == 3) {
        param3 = val;
      }

      c++;
    }

    Serial.println("Received :");
    Serial.println(cmd);
    Serial.println(param1);
    Serial.println(param2);
    Serial.println(param3);

    if(c==4) {
      Command command = { cmd, param1, param2, param3 };
      process_command(command);
    }  
}

void setup() {
//  Wire.begin(MOTOR_I2C_ADDRESS); 
//  Wire.setClock(100000);
//  Wire.onReceive(receiveCommand);`
 
  Serial.begin(9600);
  
  attachInterrupt(digitalPinToInterrupt(LEFT_ENCODER_PIN), count_left_encoder, RISING);
  attachInterrupt(digitalPinToInterrupt(RIGHT_ENCODER_PIN), count_right_encoder, RISING);

  pinMode(LEFT_MOTOR_SPEED_PIN, OUTPUT);
  pinMode(LEFT_MOTOR_DIRECTION_A_PIN, OUTPUT);  
  pinMode(LEFT_MOTOR_DIRECTION_B_PIN, OUTPUT);
  pinMode(RIGHT_MOTOR_SPEED_PIN, OUTPUT);
  pinMode(RIGHT_MOTOR_DIRECTION_A_PIN, OUTPUT);  
  pinMode(RIGHT_MOTOR_DIRECTION_B_PIN, OUTPUT);

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
//    Serial.println("No active target or commands");
    stop_all_motors();
    // Put diagnostic commands on the queue
//    Serial.println("Running diagnostic commands");
//    run_diagnostics();
  }
}
