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

ALGO =["Pthread Barrier", "Spin Barrier"]
csvs=[ 
    [196310, 142061, 93786, 69304, 63484, 51694, 43394, 43235, 52490, 47221, 46926, 47110, 47571, 45304, 43390, 46337, 47880],
    [200660, 139201, 90322, 66228, 57849, 48388, 41344, 35873, 78155, 93301, 82631, 85395, 67025, 71582, 75365, 92530, 93788]
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


