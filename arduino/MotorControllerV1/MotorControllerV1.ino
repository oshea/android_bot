
#include <Wire.h>
#include "Queue.h"

#define DEBUG 0

#define MOTOR_I2C_ADDRESS 0x8

#define LEFT_MOTOR_SPEED_PIN 11
#define LEFT_MOTOR_DIRECTION_A_PIN 13
#define LEFT_MOTOR_DIRECTION_B_PIN 12
#define RIGHT_MOTOR_SPEED_PIN 8
#define RIGHT_MOTOR_DIRECTION_A_PIN 7
#define RIGHT_MOTOR_DIRECTION_B_PIN 6
#define LEFT_ENCODER_PIN 3
#define RIGHT_ENCODER_PIN 2

#define TICKS_PER_DEGREE 1

#define CMD_STOP 0

#define CMD_DRIVE_FORWARD 1
#define CMD_DRIVE_BACKWARD 2
#define CMD_STOP_AND_TURN_RIGHT 3
#define CMD_STOP_AND_TURN_LEFT 4

#define CMD_DRIVE_FORWARD_DISTANCE 11
#define CMD_DRIVE_BACKWARD_DISTANCE 12
#define CMD_DRIVE_FORWARD_SPEED 13
#define CMD_DRIVE_BACKWARD_SPEED 14
#define CMD_STOP_AND_TURN_DEGREES 15

#define CMD_RUN_DIAGNOSTICS 21

struct Command {
  byte type;
  int distance;
  int speed;
  int turn_degrees; 
};

union UnsignedLongBuffer {
  unsigned long number;
  byte bytes[4];
};

union IntBuffer {
  int number;
  byte bytes[2];
};

volatile unsigned long left_front_encoder_counter = 0;
volatile unsigned long right_front_encoder_counter = 0;
unsigned int left_encoder_target;
unsigned int right_encoder_target;

String last_status;
bool perform_stop;

volatile Queue<Command> command_queue = Queue<Command>(10);

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
      left_encoder_target = 0;
      right_encoder_target = 0; 
      perform_stop = true; 
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

void turn_degrees(int target_degrees) {
  // TODO: clean up this method
//  Serial.print("Right counter: ");
//  Serial.println(right_front_encoder_counter);
  bool negative_degrees = false;
  if(target_degrees < 0) {
    negative_degrees = true;
    target_degrees = target_degrees * -1;
  }

  right_encoder_target = right_front_encoder_counter + (int)(target_degrees * TICKS_PER_DEGREE);
  left_encoder_target = left_front_encoder_counter + (int)(target_degrees * TICKS_PER_DEGREE);

//  Serial.print("Right target: ");
//  Serial.println(right_encoder_target);
  
  if (negative_degrees) {
    digitalWrite(LEFT_MOTOR_DIRECTION_A_PIN, LOW);
    digitalWrite(LEFT_MOTOR_DIRECTION_B_PIN, HIGH);
    analogWrite(LEFT_MOTOR_SPEED_PIN, 150);

    digitalWrite(RIGHT_MOTOR_DIRECTION_A_PIN, HIGH);
    digitalWrite(RIGHT_MOTOR_DIRECTION_B_PIN, LOW);
    analogWrite(RIGHT_MOTOR_SPEED_PIN, 150);
  } else {
    digitalWrite(LEFT_MOTOR_DIRECTION_A_PIN, HIGH);
    digitalWrite(LEFT_MOTOR_DIRECTION_B_PIN, LOW);
    analogWrite(LEFT_MOTOR_SPEED_PIN, 150);

    digitalWrite(RIGHT_MOTOR_DIRECTION_A_PIN, LOW);
    digitalWrite(RIGHT_MOTOR_DIRECTION_B_PIN, HIGH);
    analogWrite(RIGHT_MOTOR_SPEED_PIN, 150);
  }
}

void turn_left() {
  digitalWrite(LEFT_MOTOR_DIRECTION_A_PIN, LOW);
  digitalWrite(LEFT_MOTOR_DIRECTION_B_PIN, HIGH);
  analogWrite(LEFT_MOTOR_SPEED_PIN, 150);

  digitalWrite(RIGHT_MOTOR_DIRECTION_A_PIN, HIGH);
  digitalWrite(RIGHT_MOTOR_DIRECTION_B_PIN, LOW);
  analogWrite(RIGHT_MOTOR_SPEED_PIN, 150);
}

void turn_right() {
  digitalWrite(LEFT_MOTOR_DIRECTION_A_PIN, HIGH);
  digitalWrite(LEFT_MOTOR_DIRECTION_B_PIN, LOW);
  analogWrite(LEFT_MOTOR_SPEED_PIN, 150);

  digitalWrite(RIGHT_MOTOR_DIRECTION_A_PIN, LOW);
  digitalWrite(RIGHT_MOTOR_DIRECTION_B_PIN, HIGH);
  analogWrite(RIGHT_MOTOR_SPEED_PIN, 150);
}

void run_diagnostics() {
  process_command(Command { CMD_DRIVE_FORWARD}, false);
  delay(100);

  process_command(Command { CMD_DRIVE_BACKWARD}, false);
  delay(100);
  
  process_command(Command { CMD_DRIVE_FORWARD_DISTANCE, 50 }, false);
  process_command(Command { CMD_DRIVE_BACKWARD_DISTANCE, 50 }, false);
  process_command(Command { CMD_STOP_AND_TURN_DEGREES, 0, 0, 90 }, false);
  process_command(Command { CMD_DRIVE_FORWARD_DISTANCE, 50 }, false);
  process_command(Command { CMD_DRIVE_BACKWARD_DISTANCE, 50 }, false);
  process_command(Command { CMD_STOP_AND_TURN_DEGREES, 0, 0, 180 }, false);
  process_command(Command { CMD_DRIVE_FORWARD_DISTANCE, 50 }, false);
  process_command(Command { CMD_DRIVE_BACKWARD_DISTANCE, 50 }, false);
  process_command(Command { CMD_STOP_AND_TURN_DEGREES, 0, 0, 90 }, false);
  process_command(Command { CMD_DRIVE_FORWARD_DISTANCE, 50 }, false);
  process_command(Command { CMD_DRIVE_BACKWARD_DISTANCE, 50 }, false);
}

void print_status(String s) {
  if(s == last_status) {
    return;
  }

  Serial.println(s);
  last_status = s;
}

void process_command(Command cmd, bool force_queue) {
  if(cmd.type == CMD_STOP) {
    perform_stop = true;
    command_queue.clear();
    return;
  } else if(has_active_target() || force_queue) {
    // Do nothing and put cmd in the queue
    command_queue.push(cmd);
    Serial.print("Queuing command: ");
    Serial.println(cmd.type);
    return;
  }

  Serial.print("Running command:");
  Serial.println(cmd.type);

  if (cmd.type == CMD_STOP) {
    stop_all_motors();
  } else if (cmd.type == CMD_DRIVE_FORWARD) {
    go_forward();
  } else if (cmd.type == CMD_DRIVE_FORWARD_DISTANCE) { 
    right_encoder_target = right_front_encoder_counter + cmd.distance;
    left_encoder_target = left_front_encoder_counter + cmd.distance;
    go_forward();
  } else if (cmd.type == CMD_DRIVE_BACKWARD) {
    go_backward();
  } else if (cmd.type == CMD_DRIVE_BACKWARD_DISTANCE) {
    right_encoder_target = right_front_encoder_counter + cmd.distance;
    left_encoder_target = left_front_encoder_counter + cmd.distance;
    go_backward();  
  } else if (cmd.type == CMD_STOP_AND_TURN_LEFT) {
    stop_all_motors();
    turn_left();
  } else if (cmd.type == CMD_STOP_AND_TURN_RIGHT) {
    stop_all_motors();
    turn_right();
  } else if (cmd.type == CMD_STOP_AND_TURN_DEGREES) {
    stop_all_motors();
    turn_degrees(cmd.turn_degrees);
  } else if (cmd.type == CMD_RUN_DIAGNOSTICS) {
    run_diagnostics();
  } else {
    stop_all_motors();
  }
}

void receiveCommand(int byte_count) {
    int counter = 0;
    
    byte cmd_type = Wire.read();

    byte cmd_data[byte_count - 1];

    Serial.print("Received command: ");
    Serial.println(cmd_type);

    Command command = { cmd_type };

    while (Wire.available()) {
      byte val = Wire.read();
      cmd_data[counter] = val;
      counter++;
    }

    Serial.print("Received bytes:");
    Serial.println(counter);

    if (cmd_type == CMD_DRIVE_FORWARD_DISTANCE || cmd_type == CMD_DRIVE_BACKWARD_DISTANCE) {
      UnsignedLongBuffer distance;
      distance.number = 0;

      // Take every byte up to 4 and put in the array
      for (size_t i = 0; i < 4; i++) {
        if(i < byte_count) {
          Serial.println(i);
          Serial.println(cmd_data[i]);
          distance.bytes[i] = cmd_data[i];
        }
      }
      command.distance = (unsigned long)distance.number;

      Serial.print("Received distance: ");
      Serial.println(distance.number);
      Serial.println(command.distance);
    } else if (cmd_type == CMD_STOP_AND_TURN_DEGREES) {
      IntBuffer degrees;
      degrees.number = 0;

      for (size_t i = 0; i < 2; i++) {
        if(i < byte_count) {
          Serial.println(i);
          Serial.println(cmd_data[i]);
          degrees.bytes[i] = cmd_data[i];
        }
      }

      command.turn_degrees = (int)degrees.number;

      Serial.print("Received degrees: ");
      Serial.println(degrees.number);
      Serial.println(command.turn_degrees);
    }

    process_command(command, true);
}

void setup() {
  Wire.begin(MOTOR_I2C_ADDRESS); 
  Wire.setClock(100000);
  Wire.onReceive(receiveCommand);
 
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

  // run_diagnostics();

  perform_stop = false;
}

void loop() {
  if(perform_stop) {
    print_status("Stopping");
    stop_all_motors();
    perform_stop = false;
  } else if(has_active_target()) {
    // String target_string = "active target: ";
    // String current_string = " current count: ";
    // print_status(target_string + left_encoder_target + current_string + left_front_encoder_counter);
  } else if(command_queue.count() > 0) {
    print_status("Pulling command off queue");
    process_command(command_queue.pop(), false);
  } else {
    print_status("No active targets");
  }
}
