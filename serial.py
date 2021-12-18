#!/usr/bin/python3.9

import serial
from sys import argv

if __name__ == "__main__":
    ser = serial.Serial(port= argv[1], baudrate=argv[2])
    print(ser.name)

    if (argv[3] == "SET"):
        # ser.write(bytearray([0x2, 0x0A, 0x4, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3]))
        ser.write(bytearray([0x2]))
        ser.write(bytearray([0x0A]))
        ser.write(bytearray([0x4]))
        ser.write(bytearray([0x0]))
        ser.write(bytearray([0x0]))
        ser.write(bytearray([0x0]))
        ser.write(bytearray([0x0]))
        ser.write(bytearray([0x0]))
        ser.write(bytearray([0x0]))
        ser.write(bytearray([0x0]))
        ser.write(bytearray([0x3]))
    elif (argv[3] == "GET"):
        # ser.write(bytearray([0x2, 0xA0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3]))
        ser.write(bytearray([0x2]))
        ser.write(bytearray([0xA0]))
        ser.write(bytearray([0x0]))
        ser.write(bytearray([0x0]))
        ser.write(bytearray([0x0]))
        ser.write(bytearray([0x0]))
        ser.write(bytearray([0x0]))
        ser.write(bytearray([0x0]))
        ser.write(bytearray([0x0]))
        ser.write(bytearray([0x0]))
        ser.write(bytearray([0x3]))

    print("waiting for ack")
    (start, cmd, end) = ser.read(3)
    print(f"recv {start} {cmd} {end}")
    if (start == 0x2 and cmd == 0xdd and end == 0x3):
        print("ack recv!")

    if (argv[3] == "GET"):
        print("waiting for rdata")
        (start, cmd, len0, len1, len2, len3) = ser.read(6)
        length = len3 << 24 | len2 << 16 | len1 << 8 | len0;
        print(f"recv {start} {cmd} {length}")

        if (start == 0x2 and cmd == 0xaa):
            print(f"rdata: {ser.read(size=length)}")

        ser.read() # ETX