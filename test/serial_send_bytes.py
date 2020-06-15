#!/usr/bin/env python3
import serial
import time
from os import getenv

PORT = getenv('PORT', '/dev/ttyUSB0')

s = serial.serial_for_url(PORT, baudrate=9600, bytesize=8 )
while True:
    for i in range(0xFF):
        print(hex(i))
        s.write(bytes([i]))
        time.sleep(0.1)
