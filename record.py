from datetime import datetime
import argparse
import serial
import os
import re

# record.py
# 
# Will also record the data from the arduino
# Will also save to a .csv file
# 

parser = argparse.ArgumentParser(prog="Data recorder",
                                 description="This script uses Serial to record datalogged from our custom flight computer")

parser.add_argument('device', help="The Serial Device")

parser.add_argument('--baud', default=115200,
                    help='The baud rate for the Serial Connection')

packetSize = 2
baud = 0
device = ""
conn = None

fileFormat = r"data/%m-%Y-%d-%H-%M-data.csv"
searchPattern = r"(-?\d+(\.\d+)?)" + (r"(,(-?\d+(\.\d+)?))" * (packetSize-1))
print(searchPattern)
loggedData = []

def processData():
    file = open(datetime.now().strftime(fileFormat), "w")
    file.write("Time,Pitch,Roll,Yaw\n")

    for datapoint in loggedData:
        datapoint = datapoint.replace("  ", " ")
        match = re.match(searchPattern, datapoint)

        if match:
            file.write(datapoint[:-2] + "\n")

if __name__ == "__main__":
    args = parser.parse_args()

    device = args.device
    baud = args.baud

    conn = serial.Serial(port=device, baudrate=baud)

    if not os.path.exists("data/"):
        os.makedirs("data/")

    recording = False
    print("Serial Connection Open")
    while True:
        try:
            if conn.in_waiting:
                data = conn.readline().decode()
                print(data, end="")
                
                # Non blocking input
                if not recording:
                    cmd = input("Command (e, p, d)")
                    if cmd == "e":
                        pass
                    elif cmd == "p":
                        recording = True
                    
                    recording = True
                    conn.write(cmd.encode())
                
                else: # If we are recording
                    if "DATA DONE" in data: break
                    
                loggedData.append(data)

        except KeyboardInterrupt:
            print("Quiting")
            break
    
    print("Done Processing Data")
    processData()