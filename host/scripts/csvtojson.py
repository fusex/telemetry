#!/usr/bin/env python3

import imufusion
import numpy
import csv
import json
import socket
import time
import sys

ip = "127.0.0.1"
port = 54321
sample_rate = 10 # 10 Hz

offset = imufusion.Offset(sample_rate)
ahrs = imufusion.Ahrs()
ahrs.settings = imufusion.Settings(imufusion.CONVENTION_NWU,  # convention
                                   0.5,  # gain
                                   10,  # acceleration rejection
                                   20,  # magnetic rejection
                                   5 * sample_rate)  # rejection timeout = 5 seconds

if len(sys.argv) < 2:
    print("Usage: {} imu-file.csv".format(sys.argv[0]))
    exit(1)

csvfilename = sys.argv[1]
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

with open(csvfilename, encoding = 'utf-8') as csvfile:
    csv_reader = csv.DictReader(csvfile)
    i = 0
    for rows in csv_reader:
        i += 1
        if i < 1200: continue
        data = {}
        data["id"] = i
        data["accelx"] = rows['Accelerometer X (g)']
        data["accely"] = rows['Accelerometer Y (g)']
        data["accelz"] = rows['Accelerometer Z (g)']
        data["gyrox"]  = rows['Gyroscope X (deg/s)']
        data["gyroy"]  = rows['Gyroscope Y (deg/s)']
        data["gyroz"]  = rows['Gyroscope Z (deg/s)']
        data["magnx"]  = rows['Magnetometer X (uT)']
        data["magny"]  = rows['Magnetometer Y (uT)']
        data["magnz"]  = rows['Magnetometer Z (uT)']
        gyroscope     = numpy.array([data["gyrox"], data["gyroy"], data["gyroz"]]).astype(numpy.float)
        accelerometer = numpy.array([data["accelx"], data["accely"], data["accelz"]]).astype(numpy.float)
        magnetometer = numpy.array([data["magnx"], data["magny"], data["magnz"]]).astype(numpy.float)
        ahrs.update(gyroscope, accelerometer, magnetometer, 0.01)
        euler = ahrs.quaternion.to_euler()
        data["roll"] = str(euler[0])
        data["pitch"] = str(euler[1])
        data["yaw"] = str(euler[2])
        sock.sendto(json.dumps(data).encode(), (ip, port))
        time.sleep(1/sample_rate)

