from GPIB import GPIB_Manager
import numpy as np
from SerialRead import connect_ardunio, get_signal
from PoistionEstimation import Position_estimation
import time
import logging

def signal2distance(signal_np):
    coefficient = 3026.88441
    distance_np = np.power(np.divide(signal_np, coefficient), -1/3)
    print("The distance matrix return: {} ".format(distance_np))
    return distance_np


def main():
    # parameter setting
    arduno_port = connect_ardunio()
    sender_position = np.array(([(-5, 0, 0), (5, 0, 0), (0, -5, 0)]))
    sender_n = sender_position.shape[0]
    # print(sender_n)
    distance = np.zeros(3)
    amplitude = np.zeros(sender_n)
    p_e = Position_estimation(sender_position, distance)
    sampling_n = 100
    voltage = 12.00  # (v)
    current = voltage / 470 #(i)
    #starting
    with GPIB_Manager(0) as gb1:
        with GPIB_Manager(1) as gb2:
            while True:
                # Get reference data
                reference = get_signal(arduno_port, sampling_n)
                print("The reference signal is {}".format(reference))
                # Get the data when sender gibe the signal
                for i in range(sender_n):
                    channel = i+1
                    if channel > 2:
                        gb = gb2
                        channel = channel - 2
                    else:
                        gb = gb1
                    gb.write_data('v', voltage, channel)
                    gb.write_data('i', current, channel)
                    # print(current)
                    gb.turn_on(channel)
                    time.sleep(0.1)
                    signal = get_signal(arduno_port, sampling_n)
                    print("channel : {}, signal get : {}".format(i, signal))

                    amplitude[i] = signal
                    gb.turn_off(channel)
                    time.sleep(0.1)
                    print("Change channel")

                amplitude = np.subtract(amplitude, reference)
                amplitude = np.absolute(amplitude)
                #change the signal to distance
                distance = signal2distance(amplitude)
                position = p_e.renew_distance(distance)
                print("The position now : {}".format(position))

if __name__ == '__main__':
    main()
