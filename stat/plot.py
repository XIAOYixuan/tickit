import numpy as np
import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator
from collections import namedtuple
def max_ntask(epic_tasks):
    ret = 0
    for k in epic_tasks:
        ret = max(ret, len(epic_tasks[k]))
    return ret

def size_to_label(epic_tasks):
    pass

def plot_week_report(epics, epic_dict, epic_tasks):
    fig, ax1 = plt.subplots()
    fig.canvas.manager.set_window_title("Weekly report")

    pos = np.arange(len(epics))

    rects = ax1.barh(pos, [len(epic_tasks[k]) for k in epics], 
        align = 'center', 
        height = 0.5, 
        tick_label=epics)

    max_n = max_ntask(epic_tasks)
    ax1.set_xlim(0, max_n + 1)
    ax1.xaxis.set_major_locator(MaxNLocator(11))
    ax1.xaxis.grid(True, linestyle='--', which='major',
                   color='grey', alpha=.25)

    ax1.axvline(50, color='grey', alpha=0.25)

    split_width = max_n/2;
    rect_labels = []
    for i in range(len(rects)):
        rect = rects[i]
        epicid = epics[i]
        width = int(rect.get_width())
        xloc = None
        epic = epic_dict[epicid] + ' ' + str(width)
        align = None
        clr = None
        if width < split_width:
            xloc = width
            align = 'left'
            clr = 'black'
        else:
            xloc = 0.98 * width
            align = 'right'
            clr = 'white'
        yloc = rect.get_y() + rect.get_height()/2.0
        label = ax1.text(xloc, yloc, epic,
            horizontalalignment=align, 
            verticalalignment='center',
            color = clr,
            clip_on=True)
        rect_labels.append(label)


    
    plt.show()
    
def read_tasks():
    epics = []
    epic_tasks = {}
    epic_dict = {}
    with open("stat/epic.tmp") as fin:
        for line in fin:
            line = line.strip().split()
            epics.append(int(line[0]))
            epic_dict[epics[-1]] = ' '.join(line[1:-1])
            num_tasks = int(line[-1])
            tasks = []
            for i in range(num_tasks):
                task = fin.readline().strip()
                tasks.append(task)
            epic_tasks[epics[-1]] = tasks
    return epics, epic_dict, epic_tasks
                


if __name__ == '__main__':
    epics, epic_dict, epic_tasks = read_tasks()
    plot_week_report(epics, epic_dict, epic_tasks)
