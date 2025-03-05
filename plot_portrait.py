#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from matplotlib import pyplot as plt
import matplotlib as mpl

fig, ax = plt.subplots()
font = {"size": 20, "family": "sans-serif"}
mpl.rc("font", **font)
plt.rc("text", usetex=True)
ax.set_xlabel(r"$z$")
ax.set_ylabel(r"$v$")

ax.set_ylim(-2, 2)
ax.set_xlim(-2.5, 2.5)

with open("portrait.txt", "r") as f:
    z = []
    v = []
    for line in f.readlines():
        if line.startswith("#"):
            continue
        sl = line.split()
        z.append(float(sl[1]))
        v.append(float(sl[2]))
    ax.plot(z, v, "o", markersize=0.4)

plt.show()
