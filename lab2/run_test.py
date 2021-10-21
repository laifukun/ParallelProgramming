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
#nThread = 17
#THREADS = [2*i for i in range(0, nThread)]
#LOOPS = [100000]
INPUTS = ["random-n2048-d16-c16.txt"]
#INPUTS = ["test_16.txt"]

#csvs = []
for inp in INPUTS:
#    for loop in LOOPS:
#        csv = ["{}/{}".format(inp, loop)]

    cmd = "./bin/kmeans -k 16 -d 16 -i tests/random-n2048-d16-c16.txt -m 150 -t 1E-6 -c -a seq -s 8675309".format(
                inp)
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
#ax.set_xticklabels(THREADS)
plt.ylabel('Running Time(ms) (log scale)')
plt.xlabel('Number of Threads')
plt.legend()
plt.show()


