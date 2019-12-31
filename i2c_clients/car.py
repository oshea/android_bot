import smbus
import time
import sys

from getkey import getkey, keys

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

def uint32_to_bytes(val):
    bin_str = "{:032b}".format(val)
    byte_strs = list(map(''.join, zip(*[iter(bin_str)] * 8)))
    byte_list = [int(b, 2) for b in byte_strs]
    byte_list.reverse()
    return byte_list

def int16_to_bytes(val):
    if val < 1:
        val = val * -1
        val = (val ^ 0xFFFF) + 1
    
    bin_str = "{:016b}".format(val)
    byte_strs = list(map(''.join, zip(*[iter(bin_str)] * 8)))
    byte_list = [int(b, 2) for b in byte_strs]
    byte_list.reverse()
    return byte_list


def drive_forward():
    bus.write_i2c_block_data(MOTOR_ADDRESS, CMD_DRIVE_FORWARD, [])
    time.sleep(CMD_DELAY_PER_BYTE)


def drive_forward_distance(distance):
    distance_bytes = uint32_to_bytes(distance)
    bus.write_i2c_block_data(MOTOR_ADDRESS, CMD_DRIVE_FORWARD_DISTANCE, distance_bytes)
    time.sleep(CMD_DELAY_PER_BYTE * 5)


def drive_backward():
    bus.write_i2c_block_data(MOTOR_ADDRESS, CMD_DRIVE_BACKWARD, [])
    time.sleep(CMD_DELAY_PER_BYTE)


def drive_backward_distance(distance):
    distance_bytes = uint32_to_bytes(distance)
    bus.write_i2c_block_data(MOTOR_ADDRESS, CMD_DRIVE_BACKWARD_DISTANCE, distance_bytes)
    time.sleep(CMD_DELAY_PER_BYTE * 5)


def turn_degrees(degree):
    degree_bytes = int16_to_bytes(degree)
    bus.write_i2c_block_data(MOTOR_ADDRESS, CMD_STOP_AND_TURN_DEGREES, degree_bytes)
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


def interactive_loop():
    print('Interactive mode')
    last_key = None
    while True:
        key = getkey()
        if key == keys.UP:
            drive_forward()
        elif key == keys.DOWN:
            drive_backward()
        elif key == keys.LEFT:
            turn_left()
        elif key == keys.RIGHT:
            turn_right()
        elif key == keys.SPACE:
            stop()
        elif key == 'q':
            print()
            break
        else:
            pass

        last_key = key

    
def command_loop():
    while True:
        cmd = raw_input("Enter command > ")
        parts = cmd.split(':')
        
        cmd_type = parts[0]
        cmd_type = cmd_type.lower()
        
        param1 = None
        if len(parts) >= 2: param1 = parts[1]

        if cmd_type == 'i':
            interactive_loop()
        elif cmd_type == 'f':
            if param1:
                drive_forward_distance(int(param1))
            else:
                drive_forward()
        elif cmd_type == 'b':
            if param1:
                drive_backward_distance(int(param1))
            else:
                drive_backward()
        elif cmd_type == 'l':
            if param1:
                turn_degrees(-int(param1))
            else:
                turn_left()
        elif cmd_type == 'r':
            if param1:
                turn_degrees(int(param1))
            else:
                turn_right()
        elif cmd_type == 's':
            stop()


if __name__ == "__main__":
    command_loop()
    #print(int_to_bytes(100))
    #print(int_to_bytes(-100))



