import smbus
import time

bus = smbus.SMBus(1)

motor_address = 8

while True:
	cmd = input("Enter Command ID: ")
	if not cmd: continue
	
	param1 = input("Enter param 1: ")
	param2 = input("Enter param 2: ")
	param3 = input("Enter param 3: ")
	bus.write_i2c_block_data(motor_address, cmd, [param1, param2, param3])
    

	
	
	
