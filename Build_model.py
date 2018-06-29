from SerialRead import connect_ardunio, get_signal, ardunio_read
from GPIB import GPIB_Manager
import serial
import numpy as np
import time
import pandas as pd
from SerialRead import serial_connect
from PDF import PDF_data
voltage = 18.00  # (v)
current = voltage / 470  # (i)
z = 3.4

def PDF_draw(times, filename='distance_10.csv'):
    port = connect_ardunio()
    result = {}
    result['distance'] = []
    result['signal'] = []
    head = time.time()
    with serial.Serial(port, 19200, timeout=1) as ser:
        with GPIB_Manager(0) as g_m:
            while True:
                o_d = get_signal(ser, 10)
                while o_d['std'] > 1:
                    print("sampling fail")
                    o_d = get_signal(ser, 10)
                ref = o_d['mean']
                print("The ref signal is {}".format(ref))
                print("The std of ref is {}".format(o_d['std']))
                g_m.write_data('v', voltage, 1)
                g_m.write_data('i', current, 1)
                g_m.turn_on(1)
                time.sleep(1)
                r_d = get_signal(ser, 10)
                while r_d['std'] > 1:
                    print("sampling fail")
                    r_d = get_signal(ser, 10)
                signal = r_d['mean']
                print("The signal is {}".format(signal))
                print("The std of signal is {}".format(r_d['std']))
                g_m.turn_off(1)
                result['signal'].append(abs(signal - ref))
                if len(result['signal']) > times:
                    break
    end = time.time()
    print("cost time: {}".format(end - head))
    data = np.asarray(result['signal'])
    np.savetxt(filename, data, delimiter=',')
    PDF_data(filename)

def build_experiment(filename='distance_10.csv', result={}):
    port = connect_ardunio()
    result['distance'] = []
    result['signal'] = []
    result['ref_std'] = []
    result['ref_mean'] = []
    result['mean'] = []
    result['std'] = []
    with serial.Serial(port, 19200, timeout=1) as ser:
        with GPIB_Manager(0) as g_m:
            while True:
                content = input("Please enter the x, y :")
                if content == 's':
                    break
                position = content.split(',')
                if len(position) < 2:
                    print("Wrong key, try again")
                    continue
                x, y = float(position[0]), float(position[1])
                distance = (x ** 2 + y ** 2 + z ** 2) ** (1 / 2)
                result['distance'].append(distance)
                ref_d = get_signal(ser, 10)
                while ref_d['std'] > 1:
                    print("sampling failed")
                    ref_d = get_signal(ser, 10)
                print("ref : {}".format(ref_d['mean']))
                result['ref_mean'].append(ref_d['mean'])
                result['ref_std'].append(ref_d['std'])
                g_m.write_data('v', voltage, 1)
                g_m.write_data('i', current, 1)
                g_m.turn_on(1)
                time.sleep(1)
                sig_d = get_signal(ser, 10)
                while sig_d['std'] > 1:
                    print("sampling failed")
                    sig_d = get_signal(ser, 10)
                print("sig : {}".format(sig_d['mean']))
                result['mean'].append(sig_d['mean'])
                result['std'].append(sig_d['std'])
                result['signal'].append(abs(sig_d['mean'] - ref_d['mean']))
                g_m.turn_off(1)
                k = input("With position x, y : {}, {}\nsave data ? ".format(x, y))
                if len(k) > 0:
                    print("give up this data")
                    continue
    for i, j in result.items():
        print("{} content is len of {}".format(i, len(j)))
    df = pd.DataFrame.from_dict(result)
    df.to_csv(filename)


if __name__ == '__main__':
    build_experiment('model.csv')
    # PDF_draw(100, 'test_10.csv')

