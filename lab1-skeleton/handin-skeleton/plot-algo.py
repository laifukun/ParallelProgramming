#!/usr/bin/env python3
import os
from subprocess import check_output
import re
from time import sleep
import matplotlib.pyplot as plt
import numpy as np
#
#  Feel free (a.k.a. you have to) to modify this to instrument your code
#
nThread = 17
THREADS = [2*i for i in range(0, nThread)]
LOOPS = [100000]
INPUTS = ["1k.txt", "8k.txt", "16k.txt"]
#INPUTS = ["test_16.txt"]

ALGO =["HS algorithm","Two-Level algorithm", "Blelloch algorithm"]
csvs=[ [3148685, 20673822, 10609510, 7264144, 5546271, 4495322, 3762094, 3307725, 3094040, 3740319, 3512333, 3292545, 3171233, 3146872, 3137473, 3172823, 3100537],
    [3149547, 2103475, 1299975, 948547, 770933, 638516, 540767, 466731, 468156, 573370, 529293, 477673, 515269, 509925, 487391, 502313, 502950],
    [3122223, 3171043, 1685801, 1176382, 904363, 740030, 632555, 557342, 533817, 635088, 600975, 595340, 570027, 561560, 552358, 575093, 633484]
]


x = np.arange(len(INPUTS))+2
bar_wth = 0.15
#fig = plt.figure()
#ax = fig.add_axes(THREADS)
plt.yscale('log')

for i in range(len(csvs)):
    y = [int(i) for i in csvs[i][0:]]
    plt.plot(THREADS, y, marker='o', label=ALGO[i])
#for i in range(len(THREADS)):
    #time=[0]*len(INPUTS)
    #for j in range(len(INPUTS)):
    #    time[j] = csvs[j][i]
    #rects = ax.bar(x + bar_wth*bar_pos[i], time, bar_wth, label=str(THREADS[i]))
#for csv in csvs:
#    rects = ax.bar(x - bar_wth/2, csv, bar_wth)
#ax.set_xticks(x)
#ax.set_xticklabels(THREADS)
plt.ylabel('Running Time(ms) (log scale)')
plt.xlabel('Number of Threads')
plt.legend()
plt.show()


