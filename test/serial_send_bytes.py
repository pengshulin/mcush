#!/usr/bin/env python3
import serial
import time
from os import getenv

PORT = getenv('PORT', '/dev/ttyUSB0')
BAUDRATE = 9600

s = serial.serial_for_url(PORT, baudrate=BAUDRATE )
while True:
    for i in range(256):
        print(hex(i))
        s.write(bytes([i]))
        time.sleep(0.1)
