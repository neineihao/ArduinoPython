from main import signal2distance
import numpy as np
import matplotlib
import matplotlib.pyplot as plt

STD = 0.21


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

def distance_error(position, point):
    before = position2distance(position, point)
    after = position_variety(before)
    diff = np.abs(after - before)
    return diff.sum()

def position2distance(position, point):
    row, col = point.shape
    distance = np.ones(row)
    for i in range(row):
        distance[i] = p2distance(point[i,:], position)
    return distance
    # distance size : (3,)

def p2distance(a,b):
    return np.power(a - b, 2).sum() ** (1 / 2)

def position_variety(distance_matrix):
    number = distance_matrix.size
    result = np.ones(number)
    for i in range(number):
        result[i] = distance_variety(distance_matrix[i])
    return result

def plane_simulation(point):
    x, y = 20, 20
    place = np.zeros((y,x))
    z = 5
    for x in range(20):
        for y in range(20):
            p = np.array([x-10, y-10, z])
            place[y,x] = distance_error(p, point)
    return place

if __name__ == '__main__':
    position = np.array([5,8,9])
    point = np.array([(5, 0, 0), (-5, 0, 0), (0, -5, 0)])
    print("distance error is : {}".format(distance_error(position, point)))
    z = 3
    heat_map = plane_simulation(point)
    plt.imshow(heat_map, interpolation='nearest')
    plt.show()
