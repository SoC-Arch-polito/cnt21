#!/usr/bin/python3.9

from os import times
from sys import argv
from datetime import datetime
import time
import socket


if __name__ == "__main__":

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(("127.0.0.1", 1234))
    s.settimeout(1)

    s.send("\r".encode('utf-8'))
    time.sleep(1)
    s.recv(1024)

    s.send("get\r".encode('utf-8'))
    time.sleep(1)
    s.recv(5)

    time.sleep(1)
    while True:
        try:
            entry = s.recv(6)
            if b"\nthor@" == entry:
                break
        except:
            exit(0)

        timestamp = int.from_bytes(entry[0:4], "little")
        cnt = int.from_bytes(entry[4:6], "little")

        print(f"{datetime.utcfromtimestamp(timestamp).strftime('%d/%m/%Y %H/%M/%S')} -- {cnt}")

