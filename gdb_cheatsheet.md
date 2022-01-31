layout next
refresh
(arrows up/down to scroll)

set print pretty on

set follow-fork-mode child/parent
set follow-exec-mode new/old

run (start to run with b on main)
n, next
s, step
c, continue
b, break

p var, print var
w var, watch var

(to print an array of 10 initialized as pointer write something like *arr@10)

bt, backtrace full (when SEGFAULT or something similar)

q, quit

## To analize valgrind output via gdb:

valgrind --log-file="val.txt" --trace-children=yes --leak-check=full -s --vgdb=yes --vgdb-error=0 --track-origins=yes ./master