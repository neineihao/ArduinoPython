from GPIB import GPIB_Manager
import numpy as np
from SerialRead import connect_ardunio,get_signal
from PoistionEstimation import Position_estimation

def signal2distance(signal_np):
    coefficient = 3026.88441
    distance_np = np.power(np.divide(signal_np, coefficient), -1/3)
    return distance_np


def main():
    # parameter setting
    arduno_port = connect_ardunio()
    sender_position = np.array(([(0,0,0),(4,0,0),(0,4,0)]))
    sender_n = sender_position.shape[0]
    distance = np.zeros(3)
    amplitude = np.zeros(sender_n)
    p_e = Position_estimation(sender_position, distance)
    sampling_n = 1
    voltage = 12.00 #(v)
    current = voltage / 470000 #(i)
    #starting
    with GPIB_Manager() as gb:
        while True:
            # Get reference data
            reference = get_signal(arduno_port, sampling_n)
            # Get the data when sender gibe the signal
            for i in range(sender_n):
                gb.write_data('v', voltage, i)
                gb.write_data('i', current, i)
                gb.turn_on(i)
                amplitude[i] = get_signal(arduno_port, sampling_n)
                gb.turn_off(i)
            amplitude = np.subtract(amplitude, reference)
            amplitude = np.absolute(amplitude)
            #change the signal to distance
            distance = signal2distance(amplitude)
            position = p_e.renew_distance(distance)
            print("The position now : {}".format(position))

if __name__ == '__main__':
    main()
