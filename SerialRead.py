import serial
import logging
import serial.tools.list_ports

def serial_connect(port, baudrate=19200):
    ser = serial.Serial()
    ser.baudrate = baudrate
    ser.port = port
    ser.open()
    return ser.isOpen()

def serial_read(port, baudrate=19200):
    Arduino = serial.Serial(port=port, baudrate=baudrate)
    Arduino.flush()
    Arduino.flushInput()
    while True:
        data = Arduino.readline()
        print(data.decode('UTF-8'))

def list_port():
    port = []
    arduino_ports = serial.tools.list_ports.comports()
    for item in arduino_ports:
        port.append(item[0])
    return port

def main():
    port = list_port()
    if not port:
        raise IOError("No Arduino found")
    elif len(port) > 1 :
        logging.warning("There are two or more port")
    else:
        pass
    use_port = port[0]
    if serial_connect(use_port):
        serial_read(use_port)


if __name__ == '__main__':
    main()