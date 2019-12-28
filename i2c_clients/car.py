import smbus
import time
import curses
import sys

bus = smbus.SMBus(1)

MOTOR_ADDRESS = 8

CMD_STOP = 0

CMD_DRIVE_FORWARD = 1
CMD_DRIVE_BACKWARD = 2
CMD_STOP_AND_TURN_RIGHT = 3
CMD_STOP_AND_TURN_LEFT = 4

CMD_DRIVE_FORWARD_DISTANCE = 11
CMD_DRIVE_BACKWARD_DISTANCE = 12
CMD_DRIVE_FORWARD_SPEED = 13
CMD_DRIVE_BACKWARD_SPEED = 14
CMD_STOP_AND_TURN_DEGREES = 15

CMD_DELAY_PER_BYTE = 0.1

#def int_to_bytes(val, num_bytes):
    # Big-endian
    #return [(val & (0xff << pos*8)) >> pos*8 for pos in reversed(range(num_bytes))]

    #Little-endian
    #return [(val & (0xff << pos*8)) >> pos*8 for pos in range(num_bytes)]

def long_to_bytes(val):
    bin_str = "{:032b}".format(val)
    byte_strs = list(map(''.join, zip(*[iter(bin_str)] * 8)))
    # print(byte_strs)
    byte_list = [int(b, 2) for b in byte_strs]
    byte_list.reverse()
    return byte_list

def drive_forward():
    bus.write_i2c_block_data(MOTOR_ADDRESS, CMD_DRIVE_FORWARD, [])
    time.sleep(CMD_DELAY_PER_BYTE)


def drive_forward_distance(distance):
    distance_bytes = long_to_bytes(distance)
    bus.write_i2c_block_data(MOTOR_ADDRESS, CMD_DRIVE_FORWARD_DISTANCE, distance_bytes)
    time.sleep(CMD_DELAY_PER_BYTE * 5)

def drive_backward():
    bus.write_i2c_block_data(MOTOR_ADDRESS, CMD_DRIVE_BACKWARD, [])
    time.sleep(CMD_DELAY_PER_BYTE)


def drive_backward_distance(distance):
    distance_bytes = long_to_bytes(distance)
    bus.write_i2c_block_data(MOTOR_ADDRESS, CMD_DRIVE_BACKWARD_DISTANCE, distance_bytes)
    time.sleep(CMD_DELAY_PER_BYTE * 5)

def turn_degrees(angle):
    bus.write_i2c_block_data(MOTOR_ADDRESS, CMD_STOP_AND_TURN_DEGREES, [angle])
    time.sleep(CMD_DELAY_PER_BYTE * 3)


def turn_left():
    bus.write_i2c_block_data(MOTOR_ADDRESS, CMD_STOP_AND_TURN_LEFT, [])
    time.sleep(CMD_DELAY_PER_BYTE)


def turn_right():
    bus.write_i2c_block_data(MOTOR_ADDRESS, CMD_STOP_AND_TURN_RIGHT, [])
    time.sleep(CMD_DELAY_PER_BYTE)


def stop():
    bus.write_i2c_block_data(MOTOR_ADDRESS, CMD_STOP, [])
    time.sleep(CMD_DELAY_PER_BYTE)


#drive_forward()
#time.sleep(2)
#drive_backward()
#time.sleep(2)
#turn_left()
#time.sleep(5)
#turn_right()
#time.sleep(5)
#stop()

drive_forward_distance(1000);
#time.sleep(2);
#drive_forward_distance(1000);
#time.sleep(2);
drive_backward_distance(1000);

drive_forward();
drive_backward();
stop();
#drive_forward_distance(1000);
#drive_forward_distance(10000);
#drive_forward_distance(100000);







