import sys, math, time, datetime
import socket, struct, threading
import serial


if __name__ == "__main__":
# Arduino initialization
    ser = serial.Serial('COM3',57600)  # THIS PORT IS SUBJECT TO CHANGE
    now = datetime.datetime.now();   
    file_name_arduino = "_".join(map(str,['arduino_data_waveguide_20psi_1sec',now.year,now.month,now.day,"_",now.hour,now.minute]))

    # Arduino file data
    f_a = open(file_name_arduino + ".txt",'w')
    print("Initialized Arduino File...")

    try:
        # DON'T # Wait for first data to come in
        while ser.readline() is None: pass

        while True:
            # ARDUINO DATA
            arduino_out = ser.readline()
            f_a.write(str(time.time()))
            f_a.write(", ")
            f_a.write(arduino_out)
            #f_a.write("\n")
            
            time.sleep(0.0005)
    except KeyboardInterrupt:

        f_a.close()
        pass