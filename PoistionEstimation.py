import numpy as np
from numpy.linalg import inv

class Position_estimation():
    def __init__(self, position, distance_matrix):
        # hypothesis is 3 * 3
        self.p = position
        self.d = distance_matrix

    def calculate_matrix(self):
        top = 2 * (self.p[1, 0:2] - self.p[0, 0:2])
        below = 2 * (self.p[2, 0:2] - self.p[0, 0:2])
        self.c_m = np.concatenate((top, below), axis=0).reshape(2, 2)

    def renew_data(self, position, distance_matrix):
        self.p = position
        self.d = distance_matrix
        return self.cal_answer()

    def renew_distance(self,distance):
        self.d = distance
        return self.cal_answer()

    def cal_right(self):
        power_p = np.power(self.p, 2)
        power_d = np.power(self.d, 2)
        h_cal = power_p[1, 0:2] - power_p[0, 0:2]
        t_cal = power_p[2, 0:2] - power_p[0, 0:2]
        h_sum = h_cal.sum() + power_d[0] - power_d[1]
        t_sum = t_cal.sum() + power_d[0] - power_d[2]
        self.a_l = np.array([[h_sum,t_sum]])

    def cal_answer(self):
        self.calculate_matrix()
        self.cal_right()
        x_y = np.dot(self.a_l, inv(self.c_m)).reshape(1,2)
        x_tmp = x_y[0,0] - self.p[0,0]
        y_tmp = x_y[0,1] - self.p[0,1]
        z = (self.d[0] ** 2 - x_tmp ** 2 - y_tmp ** 2) ** 0.5
        result = np.append(x_y, z)
        return result


def test():
    position = np.array([8.0622, 6.403, 5.745])
    print(position.shape)


def main():
    position = np.array([(0, 0, 0), (4, 0, 0), (0, 4, 0)])
    distance_matrix = np.array([8.0622, 6.403, 5.745])
    print(position)
    print(distance_matrix)
    p_e = Position_estimation(position, distance_matrix)
    print(p_e.cal_answer())

if __name__ == '__main__':
    # test()
    main()