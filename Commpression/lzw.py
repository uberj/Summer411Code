import sys
import os
from operator import itemgetter


def usage():
    print "<usage>: lzw.py <datafile> <initial_dictionary>"
if len(sys.argv) < 3:
    usage()
    exit()

# Get data file
fp = open(sys.argv[1],"r")
# Get dictionary file
d_fp = open(sys.argv[2],"r")

symbols = {}
for line in d_fp.readlines():
    symbols[line.strip()] = 1

print "Dictionary..."
print symbols

omega = ""
for line in fp.readlines():
    if line[0] == ">":
        print line
        continue
    for char in line.strip():
        omega = omega+char
        if omega not in symbols:
            # New symbol
            print "w == "+omega
            symbols[omega] = 1
            # Start omega over
            omega = omega[-1:]
            continue
        else:
            symbols[omega] = symbols[omega] + 1

print "Dictionary..."
popular = sorted(symbols.items(), key=itemgetter(1), reverse=True)
for i in range(10):
    print popular[i]

