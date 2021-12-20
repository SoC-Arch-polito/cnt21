#!/usr/bin/python3.9

from os import times
import serial
from sys import argv
from datetime import datetime
import time

if __name__ == "__main__":
    ser = serial.Serial(port= argv[1], baudrate=argv[2])
    print(ser.name)

    ser.write(bytearray("get\r".encode()))
    entry = ser.read(5)

    totb = 0
    time.sleep(1)
    while ser.inWaiting() > 5:
        entry = ser.read(6)
        totb += 6
        # print(entry.hex())
        timestamp = int.from_bytes(entry[0:4], "little")
        cnt = int.from_bytes(entry[4:6], "little")

        print(f"{datetime.utcfromtimestamp(timestamp).strftime('%d/%m/%Y %H/%M/%S')} -- {cnt} -- tot: {totb}")

