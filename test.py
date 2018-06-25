from main import signal2distance
import time
from SerialRead import connect_ardunio, get_signal
from GPIB import GPIB_Manager
import visa
import numpy as np
import logging
import retrying

def test_GPIB():
    rm = visa.ResourceManager()
    eq = rm.list_resources()
    print(eq)

def distance_test():
    arduno_port = connect_ardunio()
    sampling_n = 1
    voltage = 12.00  # (v)
    current = voltage / 470  # (i)
    channel = 1
    with GPIB_Manager(1) as gb1:
        while True:
            reference = get_signal(arduno_port, sampling_n)
            gb1.write_data('v', voltage, channel)
            gb1.write_data('i', current, channel)
            # print(current)
            gb1.turn_on(channel)
            time.sleep(channel)
            signal = get_signal(arduno_port, sampling_n)
            print("The signal is : {}".format(signal))
            print("The reference is : {}".format(reference))
            gb1.turn_off(channel)
            print(signal2distance(np.abs(signal - reference)))


def distance_error():
    # logging.basicConfig(level=logging.DEBUG)
    arduno_port = connect_ardunio()
    try_time = 10
    logs = np.zeros(try_time)
    sampling_n = 1
    voltage = 12.00  # (v)
    current = voltage / 470  # (i)
    channel = 2
    with GPIB_Manager(0) as gb1:
        for i in range(try_time):
            print("times : {}".format(i))
            reference = get_signal(arduno_port, sampling_n)
            gb1.write_data('v', voltage, channel)
            gb1.write_data('i', current, channel)
            # print(current)
            gb1.turn_on(channel)
            time.sleep(1)
            signal = get_signal(arduno_port, sampling_n)
            print("The signal is : {}".format(signal))
            print("The reference is : {}".format(reference))
            gb1.turn_off(channel)
            distance = signal2distance(np.abs(signal - reference))
            logging.debug("distance is {}".format(distance))
            logs[i] = distance
    print("The average of distance: {}".format(logs.mean()))
    print("The std of distance: {}".format(logs.std()))

if __name__ == '__main__':
    # test_GPIB()
    # distance_test()
    distance_error()