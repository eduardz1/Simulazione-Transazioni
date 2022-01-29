CC=gcc
CFLAGS=-std=c89 -pedantic -D_GNU_SOURCE
DEBUG=-DDEBUG -g -Wall
# -fmove-loop-stores not recognized, and target machine does not have delayed branches
OPT=-fauto-inc-dec -fbranch-count-reg -fcombine-stack-adjustments -fcompare-elim \
-fcprop-registers -fdce -fdefer-pop -fdse -fforward-propagate -fguess-branch-probability \
-fif-conversion -fif-conversion2 -finline-functions-called-once -fipa-modref \
-fipa-profile -fipa-pure-const -fipa-reference -fipa-reference-addressable \
-fmerge-constants -fmove-loop-invariants -fomit-frame-pointer -freorder-blocks \
-fshrink-wrap -fshrink-wrap-separate -fsplit-wide-types -fssa-backprop -fssa-phiopt \
-ftree-bit-ccp -ftree-ccp -ftree-ch -ftree-coalesce-vars -ftree-copy-prop -ftree-dce \
-ftree-dominator-opts -ftree-dse -ftree-forwprop -ftree-fre -ftree-phiprop -ftree-pta \
-ftree-scev-cprop -ftree-sink -ftree-slsr -ftree-sra -ftree-ter -funit-at-a-time \
-falign-functions  -falign-jumps -falign-labels  -falign-loops -fcaller-saves \
-fcode-hoisting -fcrossjumping -fcse-follow-jumps  -fcse-skip-blocks \
-fdelete-null-pointer-checks -fdevirtualize  -fdevirtualize-speculatively \
-fexpensive-optimizations -ffinite-loops -fgcse  -fgcse-lm  -fhoist-adjacent-loads \
-finline-functions -finline-small-functions -findirect-inlining -fipa-bit-cp  \
-fipa-cp  -fipa-icf -fipa-ra  -fipa-sra  -fipa-vrp -fisolate-erroneous-paths-dereference \
-flra-remat -foptimize-sibling-calls -foptimize-strlen -fpartial-inlining \
-fpeephole2 -freorder-blocks-algorithm=stc -freorder-blocks-and-partition  \
-freorder-functions -frerun-cse-after-loop  -fschedule-insns  -fschedule-insns2 \
-fsched-interblock  -fsched-spec -fstore-merging -fstrict-aliasing -fthread-jumps \
-ftree-builtin-call-dce -ftree-loop-vectorize -ftree-pre -ftree-slp-vectorize \
-ftree-switch-conversion  -ftree-tail-merge -ftree-vrp -fvect-cost-model=very-cheap

# Dependencies
SHARED=src/include/common.h src/utils/*.h src/utils/*.c
OBJS=lklist.o msg.o pool.o debug.o sem.o
MASTER=src/master.c src/include/master.h src/print.c src/include/print.h src/parser.c src/include/parser.h
USER=src/users.c src/include/users.h
NODE=src/nodes.c src/include/nodes.h

all: master users nodes

master: $(OBJS) $(MASTER)
	$(CC) $(CFLAGS) -O1 src/master.c  src/print.c src/parser.c *.o -lm -o master

users: $(OBJS) $(USER)
	$(CC) $(CFLAGS) -O1 src/users.c src/print.c *.o -lm -o users

nodes: $(OBJS) $(NODE)
	$(CC) $(CFLAGS) -O1 src/nodes.c src/print.c *.o -lm -o nodes

%.o: src/utils/%.c $(SHARED)
	$(CC) -c $(CFLAGS) -O1 src/utils/*.c

debug:
	rm -f *.o master users nodes *~
	$(CC) -c $(CFLAGS) $(DEBUG) $(OPT) src/utils/*.c
	$(CC) $(CFLAGS) $(DEBUG) $(OPT) src/master.c  src/print.c src/parser.c *.o -lm -o master
	$(CC) $(CFLAGS) $(DEBUG) $(OPT) src/users.c src/print.c *.o -lm -o users
	$(CC) $(CFLAGS) $(DEBUG) $(OPT) src/nodes.c src/print.c *.o -lm -o nodes

clean:
	rm -f *.o master users nodes log.txt ledger.txt *~

run: all
	./master