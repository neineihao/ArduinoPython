from main import signal2distance
import numpy as np
import matplotlib
STD = 0.106233928

def distance2signal(d):
    co = 3026.88441
    signal = co * d ** (-3)
    return signal

def signal_variety(mu):
    off_set = np.random.normal(0, STD, 1)
    signal = np.random.normal(mu, STD, 1)
    signal = np.abs(signal - off_set)
    return signal

def distance_variety(d):
    signal = distance2signal(d)
    v_signal = signal_variety(signal)
    r_d = signal2distance(v_signal)
    return r_d

def error_function(test, num):
    data = np.zeros(test)
    for i in range(test):
        data[i] = distance_variety(num)
    return data.mean()

def accumulate_np(size):
    m = np.zeros(size)
    for i in range(size):
        m[i] = i

if __name__ == '__main__':
    test = 10
    print(np.arange())
