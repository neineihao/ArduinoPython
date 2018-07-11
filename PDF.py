from SerialRead import connect_ardunio, get_signal,separate_data, ardunio_read
import matplotlib.mlab as mlab
from numpy import genfromtxt
import matplotlib.pyplot as plt
import numpy as np
import serial
import time

def get_data(filename):
    data_number = 10
    port = connect_ardunio()
    with serial.Serial(port, 19200, timeout=1) as ser:
            ardunio_read(ser, 1)
            head = time.time()
            data = np.asarray(separate_data(ser, data_number))
            end = time.time()
    print("The measure time : {}".format(end - head))

    # data = np.asarray(separate_data(arduno_port, data_number))
    np.savetxt(filename, data, delimiter=',')

def read_csv(file='./out.csv'):
    my_data = genfromtxt(file, delimiter=',')
    return my_data

def PDF_data(filename):
    data = read_csv(filename)
    mu = data.mean()
    sigma = data.std()
    print("means : {}, with std: {}".format(mu, sigma))
    x = np.linspace(mu - 3 * sigma, mu + 3 * sigma, 100)
    plt.plot(x, mlab.normpdf(x, mu, sigma))
    plt.hist(data,range=(data.min(), data.max()), density=True, rwidth=3, bins=15)
    plt.show()

def PDF_get(filename):
    get_data(filename)
    PDF_data(filename)

if __name__ == '__main__':
    # get_data()
    # read_csv()
    # PDF_data(10)

    filename = './data/{}'.format('100btimes.csv')
    PDF_get(filename)
    # PDF_data(filename)
