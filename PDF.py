from SerialRead import connect_ardunio, get_signal,separate_data
from numpy import genfromtxt
import matplotlib.pyplot as plt
import numpy as np

def get_data():
    data_number = 1000
    arduno_port = connect_ardunio()
    data = np.asarray(separate_data(arduno_port, data_number))
    np.savetxt('out.csv', data, delimiter=',')

def read_csv(file='./out.csv'):
    my_data = genfromtxt(file, delimiter=',')
    return my_data

def PDF_data(interval_number):
    static_data = np.zeros(interval_number)
    data = read_csv()
    # upper = data.max()
    # lower = data.min()
    # interval = np.linspace(lower, upper, interval_number, endpoint=False)
    # for item in data:
    #     position = int((item - lower) / (interval[1]-interval[0])) -1
    #     static_data[position] += (1 / interval_number)
    # print(static_data)
    plt.hist(data,range=(data.min(), data.max()),density=True, rwidth=3)
    plt.show()



if __name__ == '__main__':
    # get_data()
    # read_csv()
    PDF_data(10)