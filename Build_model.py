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

if __name__ == '__main__':
    port = connect_ardunio()
    result = {}
    result['distance'] = []
    result['signal'] = []
    head = time.time()
    with serial.Serial(port, 19200, timeout=1) as ser:
        with GPIB_Manager(0) as g_m:
            while True:
                # content = input("Please enter the x, y :")
                # if content == 's':
                #     break
                # position = content.split(',')
                # if len(position) < 2:
                #     print("Wrong key, try again")
                #     continue
                #
                # x, y = float(position[0]), float(position[1])
                # z = 3.4
                # distance = (x ** 2 + y ** 2 + z ** 2) ** (1/2)
                # result['distance'].append(distance)
                ref = get_signal(ser, 10)
                g_m.write_data('v', voltage, 1)
                g_m.write_data('i', current , 1)
                g_m.turn_on(1)
                time.sleep(1)
                signal = get_signal(ser, 10)
                g_m.turn_off(1)
                result['signal'].append(abs(signal - ref))
                if len(result['signal']) > 200:
                    break
    end = time.time()
    print("cost time: {}".format(end - head))
    data = np.asarray(result['signal'])
    filename = 'distance_10.csv'
    np.savetxt(filename, data, delimiter=',')
    PDF_data(filename)
    # df = pd.DataFrame.from_dict(result)
    # df.to_csv("data.csv")

