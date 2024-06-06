import serial
import time
import matplotlib.pyplot as plt
from math import sin, cos, radians

class RadarData:
    def __init__(self, degrees=180):
        self.angles = [i for i in range(0, degrees)]  # Convert angles to radians
        self.distance = [0 for _ in range(0, degrees+1)]
        plt.ion()  # Turn on interactive mode

    def compute_points(self):
        self.xs = [cos(angle/57.29) * self.distance[i] for i, angle in enumerate(self.angles)]
        self.ys = [sin(angle/57.29) * self.distance[i] for i, angle in enumerate(self.angles)]

    def plot(self):
        plt.cla()
        plt.scatter(self.xs, self.ys, s = 3)
        '''plt.xlim(-200, 200)
        plt.ylim(0, 200)'''
        plt.draw()
        plt.pause(0.001)

radar = RadarData()

def read_serial(port, baud_rate=9600, timeout=1):
    try:
        ser = serial.Serial(port, baud_rate, timeout=timeout)
        print(f"Connected to {port} at {baud_rate} baud.")
        
        while True:
            if ser.in_waiting > 0:
                line = ser.readline().decode('utf-8').rstrip().strip(' ').split(',')
                print(f"Received: {line}")
                if len(line) == 2:
                    angle = int(line[0])
                    distance = int(line[1])
                    radar.distance[angle] = distance
                    radar.compute_points()

                    radar.plot()

    except serial.SerialException as e:
        print(f"Error: {e}")
    except KeyboardInterrupt:
        print("Serial reading stopped.")
    finally:
        if ser.is_open:
            ser.close()
            print(f"Closed connection to {port}.")

if __name__ == "__main__":
    # Change 'COM6' to the appropriate serial port for your setup
    read_serial(port='COM6')
