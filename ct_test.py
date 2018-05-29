import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
import pandas as pd

def fit_func(x, a):
    return a * x ** (-3)

def csv_read(filename):
    df = pd.read_csv(filename, sep=',', header=None)
    return df.as_matrix()


def signal_distance_extract(filename='./result/d2s.csv'):
    m = csv_read(filename)
    d_m, s_m = m[:,0], m[:,1]
    plt.plot(d_m, s_m,'b.')
    popt, pcov = curve_fit(fit_func, d_m, s_m)
    sort_d_m = np.sort(d_m)
    plt.plot(sort_d_m, fit_func(sort_d_m, *popt), 'r-', label="fit:the coefficient {}".format(tuple(popt)))

    plt.xlabel("Distance (mm)")
    plt.ylabel("Signal (uT)")
    plt.title("Relationship between distance & signal")
    plt.legend()
    plt.show()


def main():
    signal_distance_extract()



def test():
    df = pd.read_csv('./result/d2s.csv', sep=',', header=None)
    np = df.as_matrix()
    print(np)

if __name__ == '__main__':
    # test()
    main()