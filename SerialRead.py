import time
import serial
import logging
import retrying
import numpy as np
from functools import reduce
import serial.tools.list_ports

def serial_connect(port, baudrate=19200):
    ser = serial.Serial()
    ser.baudrate = baudrate
    ser.port = port
    ser.open()
    return ser.isOpen()

def dic_init():
    d = {}
    d['X'] = []
    d['Y'] = []
    d['Z'] = []
    return d

def list_port():
    port = []
    arduino_ports = serial.tools.list_ports.comports()
    for item in arduino_ports:
        port.append(item[0])
    return port

def data_process(serial_str):
    serial_str = serial_str.strip()
    data = serial_str.split(',')
    if data:
        return data
    else:
        print("No data")

def list_average(int_list):
    return reduce(lambda x, y: x+y, int_list) / len(int_list)

def amplify_cal(d_data):
    list_data = []
    for key, value in d_data.items():
        list_data.append(np.asarray(value))
    return reduce(lambda x,y: np.power(np.add(np.power(x,2), np.power(y,2)), 0.5),list_data)

def connect_ardunio():
    port = list_port()
    if not port:
        raise IOError("No Arduino found")
    elif len(port) > 1:
        logging.warning("There are two or more port\n")
        use_port = port[1]
    else:
        use_port = port[0]
    return use_port

def separate_data(ser_obj, data_number):
    data_dict = ardunio_read(ser_obj, data_number)
    sig_list = amplify_cal(data_dict)
    return sig_list

def get_signal(ser_obj, data_number):
    return_d = {}
    sig_list = separate_data(ser_obj, data_number)
    return_d['mean'] = sig_list.mean()
    return_d['std'] = sig_list.std()
    return return_d

@retrying.retry(stop_max_attempt_number=3)
def ardunio_read(ser_obj, line_count):
    d = dic_init()
    for i in range(line_count):

        ser_obj.flush()
        ser_obj.flushInput()
        line = ser_obj.readline().decode('UTF-8')  # read a '\n' terminated line
        line = line.strip()
        if line:
            content = line.split(',')
            x, y, z = None, None, None
            try:
                x, y, z =float(content[0]), float(content[1]), float(content[2])
            except:
                raise ValueError("Something wrong")
        else:
            continue
        d['X'].append(x)
        d['Y'].append(y)
        d['Z'].append(z)
        logging.info("X, Y, Z : {}, {}, {}".format(x, y, z))
    return d

def test():
    port = connect_ardunio()
    print(port)
    with serial.Serial(port, 19200, timeout=1) as ser:
        ardunio_read(ser, 1)
        print(get_signal(ser, 5))

def time_test():
    port = connect_ardunio()
    time_prob = time.time()
    with serial.Serial(port, 19200, timeout=1) as ser:
        ser.flush()
        ser.flushInput()
        ardunio_read(ser, 1)
        ardunio_read(ser, 1)
        print("cost time: {}".format(time.time() - time_prob))
        time_prob = time.time()
        ardunio_read(ser, 1)
        print("cost time: {}".format(time.time() - time_prob))



if __name__ == '__main__':
    list_port()
    port = connect_ardunio()
    print(port)
    with serial.Serial(port, 9600, timeout=1) as ser:
        ser.flush()
        ser.flushInput()
        while(True):
            line = ser.readline().decode('UTF-8')  # read a '\n' terminated line
            if line:
                line = line.strip()
                print(line)

            else:
                print("No data")
        # line = line.strip()
        # print(line)