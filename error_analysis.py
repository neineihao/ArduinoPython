from main import signal2distance
import numpy as np
import matplotlib
import matplotlib.pyplot as plt

STD = 0.21
# STD = 2.31
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

def heatmap(data, row_labels, col_labels, ax=None,
            cbar_kw={}, cbarlabel="", **kwargs):
    """
    Create a heatmap from a numpy array and two lists of labels.

    Arguments:
        data       : A 2D numpy array of shape (N,M)
        row_labels : A list or array of length N with the labels
                     for the rows
        col_labels : A list or array of length M with the labels
                     for the columns
    Optional arguments:
        ax         : A matplotlib.axes.Axes instance to which the heatmap
                     is plotted. If not provided, use current axes or
                     create a new one.
        cbar_kw    : A dictionary with arguments to
                     :meth:`matplotlib.Figure.colorbar`.
        cbarlabel  : The label for the colorbar
    All other arguments are directly passed on to the imshow call.
    """

    if not ax:
        ax = plt.gca()

    # Plot the heatmap
    im = ax.imshow(data, **kwargs)

    # Create colorbar
    cbar = ax.figure.colorbar(im, ax=ax, **cbar_kw)
    cbar.ax.set_ylabel(cbarlabel, rotation=-90, va="bottom")

    # We want to show all ticks...
    ax.set_xticks(np.arange(data.shape[1]))
    ax.set_yticks(np.arange(data.shape[0]))
    # ... and label them with the respective list entries.
    ax.set_xticklabels(col_labels)
    ax.set_yticklabels(row_labels)

    # Let the horizontal axes labeling appear on top.
    ax.tick_params(top=True, bottom=False,
                   labeltop=True, labelbottom=False)

    # Rotate the tick labels and set their alignment.
    plt.setp(ax.get_xticklabels(), rotation=-30, ha="right",
             rotation_mode="anchor")

    # Turn spines off and create white grid.
    for edge, spine in ax.spines.items():
        spine.set_visible(False)

    ax.set_xticks(np.arange(data.shape[1]+1)-.5, minor=True)
    ax.set_yticks(np.arange(data.shape[0]+1)-.5, minor=True)
    ax.grid(which="minor", color="w", linestyle='-', linewidth=3)
    ax.tick_params(which="minor", bottom=False, left=False)

    return im, cbar


def annotate_heatmap(im, data=None, valfmt="{x:.2f}",
                     textcolors=["black", "white"],
                     threshold=None, **textkw):
    """
    A function to annotate a heatmap.

    Arguments:
        im         : The AxesImage to be labeled.
    Optional arguments:
        data       : Data used to annotate. If None, the image's data is used.
        valfmt     : The format of the annotations inside the heatmap.
                     This should either use the string format method, e.g.
                     "$ {x:.2f}", or be a :class:`matplotlib.ticker.Formatter`.
        textcolors : A list or array of two color specifications. The first is
                     used for values below a threshold, the second for those
                     above.
        threshold  : Value in data units according to which the colors from
                     textcolors are applied. If None (the default) uses the
                     middle of the colormap as separation.

    Further arguments are passed on to the created text labels.
    """

    if not isinstance(data, (list, np.ndarray)):
        data = im.get_array()

    # Normalize the threshold to the images color range.
    if threshold is not None:
        threshold = im.norm(threshold)
    else:
        threshold = im.norm(data.max())/2.

    # Set default alignment to center, but allow it to be
    # overwritten by textkw.
    kw = dict(horizontalalignment="center",
              verticalalignment="center")
    kw.update(textkw)

    # Get the formatter in case a string is supplied
    if isinstance(valfmt, str):
        valfmt = matplotlib.ticker.StrMethodFormatter(valfmt)

    # Loop over the data and create a `Text` for each "pixel".
    # Change the text's color depending on the data.
    texts = []
    for i in range(data.shape[0]):
        for j in range(data.shape[1]):
            kw.update(color=textcolors[im.norm(data[i, j]) > threshold])
            text = im.axes.text(j, i, valfmt(data[i, j], None), **kw)
            texts.append(text)

    return texts


if __name__ == '__main__':
    z = 10
    test_time = 10
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
    # heat_map(place, xlabel, ylabel)
    fig, ax = plt.subplots()
    im, cbar = heatmap(place, ylabel, xlabel, ax=ax,
                       cmap="YlGn", cbarlabel="error distance [cm]")
    texts = annotate_heatmap(im, valfmt="{x:.1f}")

    fig.tight_layout()
    # ax.set_title("The error of the distance when in the plane of z = {}".format(z))
    plt.show()
