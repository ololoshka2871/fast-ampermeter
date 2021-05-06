#!/usr/bin/env python

import matplotlib.pyplot as plt
import csv
import argparse


x = []
y = []

parser = argparse.ArgumentParser()

parser.add_argument('File', type=str, help='Файл с данными')

parser.add_argument('-c', '--column', required=False, default=0, type=int, help='column to plot')
args = parser.parse_args()

with open(args.File, newline='') as csvfile:
    spamreader = csv.reader(csvfile, delimiter=';', quotechar='|')
    i = 0
    for row in spamreader:
        x.append(i)
        i += 1
        y.append(float(row[args.column]))

# plotting the points
plt.plot(x, y)

# naming the x axis
plt.xlabel('Time')
# naming the y axis
plt.ylabel(f'Column {args.column}')

# function to show the plot
plt.show()
