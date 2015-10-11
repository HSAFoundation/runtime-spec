#!/usr/bin/env python
from collections import defaultdict

import fileinput

import itertools
import re
import sys

d = defaultdict(list)
curr_func = None
src_file = None

locexpr = re.compile(r'firstline=(\d+), lastline=(\d+)]{(.*?)}')
funexpr = re.compile(r'strcmp{#2}{(.*?)}=0')
fun_replace_expr = re.compile(r'\\lstinputfunlisting\[(\d+)\]{(.*?)}')

inbody = False
example_lines = list()
used_src = None
# replace certains lines in the input in place
for line in fileinput.input(inplace=True):
    if "\\lstinputlisting" in line and not inbody:
        # populate map {function name -> (starting line, end line)}
        if not line.startswith("\lstinputlisting"):
            curr_func = funexpr.findall(line)[0]
        first, last, src = locexpr.findall(line)[0]
        # we accept a single source file
        assert used_src is None or src == used_src
        if not example_lines:
            used_src = src
            with open(src, "rt") as source:
                example_lines = source.readlines()
        d[curr_func].append((first, last))
        print line,
    elif "\\begin{document}" in line:
        inbody = True
        print line,
    elif  "\\lstinputfunlisting" in line and inbody:
        # use map to replace references to a function with the source itself
        idx, func_name = fun_replace_expr.findall(line)[0]
        first, last = d[func_name][int(idx)]
        print "\\begin{lstlisting}\n",
        # line numbers start at one in lstlisting, so we adjust them
        for src_l in example_lines[int(first) - 1: int(last)]:
            print src_l,
        print "\\end{lstlisting}\n",
    else:
        print line,
