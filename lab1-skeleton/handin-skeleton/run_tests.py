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
THREADS = [2*i for i in range(1, nThread)]
LOOPS = [100000]
INPUTS = ["1k.txt", "8k.txt", "16k.txt"]
#INPUTS = ["test_16.txt"]

csvs = []
for inp in INPUTS:
    for loop in LOOPS:
        csv = ["{}/{}".format(inp, loop)]
        for thr in THREADS:
            cmd = "./bin/prefix_scan -o temp.txt -n {} -i tests/{} -l {}".format(
                thr, inp, loop)
            out = check_output(cmd, shell=True).decode("ascii")
            m = re.search("time: (.*)", out)
            if m is not None:
                time = m.group(1)
                csv.append(time)

        csvs.append(csv)
        sleep(0.5)

header = ["microseconds"] + [str(x) for x in THREADS]

print("\n")
print(", ".join(header))
for csv in csvs:
    print (", ".join(csv))

x = np.arange(len(INPUTS))+2
bar_wth = 0.15
#fig = plt.figure()
#ax = fig.add_axes(THREADS)
plt.yscale('log')

for i in range(len(csvs)):
    y = [int(i) for i in csvs[i][1:]]
    plt.plot(THREADS, y, marker='o', label=INPUTS[i])
#for i in range(len(THREADS)):
    #time=[0]*len(INPUTS)
    #for j in range(len(INPUTS)):
    #    time[j] = csvs[j][i]
    #rects = ax.bar(x + bar_wth*bar_pos[i], time, bar_wth, label=str(THREADS[i]))
#for csv in csvs:
#    rects = ax.bar(x - bar_wth/2, csv, bar_wth)
#ax.set_xticks(x)
#ax.set_xticklabels(INPUTS)
#ax.set_ylabel('Running Time(ms)')
plt.legend()
plt.show()


