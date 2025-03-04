#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from matplotlib import pyplot as plt
import matplotlib as mpl

fig, ax = plt.subplots()
fig.set_size_inches(25, 15)
font = {"size": 20, "family": "sans-serif"}
mpl.rc("font", **font)
plt.rc("text", usetex=True)
ax.set_xlabel(r"$z$")
ax.set_ylabel(r"$v$")

with open("portrait.txt", "r") as f:
    z = []
    v = []
    for line in f.readlines():
        if line.startswith("#"):
            continue
        sl = line.split()
        z.append(float(sl[1]))
        v.append(float(sl[2]))
    ax.plot(z, v, "o", markersize=0.8)

plt.show()
