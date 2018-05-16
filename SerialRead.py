import serial
import logging
import serial.tools.list_ports
import pandas as pd
from functools import reduce
import time

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

def serial_read(port,save_c, baudrate=19200):
    count = 0
    result_d = dic_init()
    arduino = serial.Serial(port=port, baudrate=baudrate)
    arduino.flush()
    arduino.flushInput()

    while True:
        if count < save_c * 3:
            start_time = time.time()
            data = arduino.readline().decode('UTF-8')
            end_time = time.time()
            print("The time of measurement: {}".format(end_time - start_time))
            list_data = data_process(data)
            print(list_data)
            if list_data[0] in result_d.keys():
                count = count+1
                result_d[list_data[0]].append(float(list_data[1]))
        else:
            break
    # print("size of X : {}".format(len(result_d['X'])))
    # print("size of Y : {}".format(len(result_d['Y'])))
    # print("size of Z : {}".format(len(result_d['Z'])))
    return result_d

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
        # print("The key, value: {}, {}".format(data[0], data[1]))
        return data
    else:
        print("No data")

def list_average(int_list):
    return reduce(lambda x, y: x+y, int_list) / len(int_list)

def distance_from_avg(int_list):
    average = list_average(int_list)
    bigger = []
    smaller = []
    for item in int_list:
        if item > average:
            bigger.append(item)
        elif item < average:
            smaller.append(item)
        else:
            logging.error("There exists errors in the average")
    before = list_average(bigger)
    after = list_average(smaller)
    return (after - before) ** 2

def diff_cal(d_data):
    return reduce(lambda x, y: x + y, map(distance_from_avg, d_data.values())) ** 0.5

def read_all_data():
    pass



def connect_ardunio():
    port = list_port()
    if not port:
        raise IOError("No Arduino found")
    elif len(port) > 1:
        logging.warning("There are two or more port\n")
        print(port)
        port_select = input("Please select which port to use :")
        if len(port_select) == 0:
            port_select = 1
        else:
            port_select = len(port_select)
        use_port = port[port_select]
    else:
        use_port = port[0]
    return use_port

def main(file_name):
    use_port = connect_ardunio()
    result_dict = {}
    result_dict['distance'] = []
    result_dict['var'] = []
    result_dict['X'] = []
    result_dict['Y'] = []
    result_dict['Z'] = []
    if serial_connect(use_port):
        # Enter and calculate the distance
        while True:
            key_input = input("Enter the position information of the sensor:")
            if key_input.lower() == 'save':
                break
            elif len(key_input) == 0:
                continue
            item_key_input = list(map(int, key_input.split(',')))
            x, y, z = item_key_input[0], item_key_input[1], item_key_input[2]
            distance = (x ** 2 + y ** 2 + z ** 2) ** 0.5
            # Read the data from serial and do some process
            dict_data = serial_read(use_port, 50)
            var = diff_cal(dict_data)
            key_input = input("Valid Value or Not ? [y/n]")
            if key_input != 'y' or 'Y':
                continue
            # Save the data into dictionary
            result_dict['distance'].append(distance)
            result_dict['var'].append(var)
            result_dict['X'].append(x)
            result_dict['Y'].append(y)
            result_dict['Z'].append(z)
        df = pd.DataFrame(result_dict, columns=['X', 'Y', 'Z', 'distance', 'var'])
        df.to_csv('./result/{}.csv'.format(file_name))
    # df = pd.DataFrame(dict_data, columns=['X', 'Y', 'Z'])
    # df.to_csv('./result/{}.csv'.format(file_name))

def test():
    #s = 'BM1422AGMV_WIA Register Value = 0x41'
    # file_path = "./result/5_90.csv"
    # read_data = pd.read_csv(file_path, index_col=0)
    # read_dict = read_data.to_dict("list")
    # print(diff_cal(read_dict))
    position = input("Enter the position information of the sensor:")
    x, y = int(position.split(',')[0]), int(position.split(',')[1])
    print("X, Y: {}, {}".format(x, y))
    distance = (x ** 2 + y ** 2) ** 0.5
    print(distance)
    data = input("Enter the word")
    print(data.lower())
if __name__ == '__main__':
    main('test')
    # test()