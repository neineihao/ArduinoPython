import serial
import logging
import serial.tools.list_ports
import pandas as pd

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
            data = arduino.readline().decode('UTF-8')
            list_data = data_process(data)
            print(list_data)
            if list_data[0] in result_d.keys():
                count = count+1
                result_d[list_data[0]].append(list_data[1])
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


def main(file_name):
    port = list_port()
    if not port:
        raise IOError("No Arduino found")
    elif len(port) > 1:
        logging.warning("There are two or more port")
    else:
        pass
    use_port = port[0]
    if serial_connect(use_port):
        dict_data = serial_read(use_port, 50)
    df = pd.DataFrame(dict_data, columns=['X', 'Y', 'Z'])
    df.to_csv('./result/{}.csv'.format(file_name))

def test():
    s = 'BM1422AGMV_WIA Register Value = 0x41'

if __name__ == '__main__':
    main('test')
    # test()