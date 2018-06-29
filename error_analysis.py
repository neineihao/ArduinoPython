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

def plane_simulation(point,size,z):
    x_i, y_i = size, size
    place = np.zeros((y_i,x_i))
    for x in range(x_i):
        for y in range(y_i):
            x_c = int(x - x_i / 2)
            y_c = int(y - y_i / 2)
            p = np.array([x_c, y_c, z])
            place[y,x] = distance_error(p, point)
    return place

def heat_map(x, xlabel, ylabel):
    if x.shape[1] != len(xlabel) or x.shape[0] != len(ylabel):
        raise ValueError("The size is not match")
    fig, ax = plt.subplots()
    im = ax.imshow(x)
    # We want to show all ticks...
    ax.set_xticks(np.arange(len(xlabel)))
    ax.set_yticks(np.arange(len(ylabel)))
    # ... and label them with the respective list entries
    ax.set_xticklabels(xlabel)
    ax.set_yticklabels(ylabel)

    # Rotate the tick labels and set their alignment.
    plt.setp(ax.get_xticklabels(), rotation=45, ha="right",
             rotation_mode="anchor")

    # Loop over data dimensions and create text annotations.
    for i in range(len(ylabel)):
        for j in range(len(xlabel)):
            text = ax.text(j, i, round(x[i, j], 2),
                           ha="center", va="center", color="w")

    ax.set_title("The heatmap in the plane when z = {}".format(z))
    fig.tight_layout()
    plt.show()


if __name__ == '__main__':
    z = 5
    test_time = 50
    np_size = 11
    point = np.array([(5, 0, 0), (-5, 0, 0), (0, -5, 0)])
    place = np.zeros((np_size,np_size))
    for i in range(test_time):
        place += plane_simulation(point, np_size, z)
    place = place / test_time
    xlabel = [x-5 for x in range(np_size)]
    ylabel = [y-5 for y in range(np_size)]
    # print("The size of xlabel: {}".format(len(xlabel)))
    # print("The size of ylabel: {}".format(len(ylabel)))
    # print("The size of data: {}".format(place.shape[1]))
    heat_map(place, xlabel, ylabel)