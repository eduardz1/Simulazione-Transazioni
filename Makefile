CC := clang
override CFLAGS := -std=c99 -pedantic -Wall -Wextra -Werror -D_GNU_SOURCE $(CFLAGS)

# Directories ------------------------------------------------------------------

BIN := bin
SRC := src
OBJ := obj
$(info $(shell mkdir -p $(BIN) $(OBJ)))

# Master -----------------------------------------------------------------------

MASTER := $(SRC)
MASTER_SRCS := $(wildcard $(MASTER)/*.c)
MASTER_OBJS := $(patsubst $(MASTER)/%.c, $(OBJ)/%.o, $(MASTER_SRCS))
MASTER_HDRS := $(wildcard $(MASTER)/include/*.h)

# Boat -------------------------------------------------------------------------

NODE := $(SRC)/nodes
NODE_SRCS := $(wildcard $(NODE)/*.c)
NODE_OBJS := $(patsubst $(NODE)/%.c, $(OBJ)/%.o, $(NODE_SRCS))
NODE_HDRS := $(wildcard $(NODE)/include/*.h)

# Pier -------------------------------------------------------------------------

USER := $(SRC)/users
USER_SRCS := $(wildcard $(USER)/*.c)
USER_OBJS := $(patsubst $(USER)/%.c, $(OBJ)/%.o, $(USER_SRCS))
USER_HDRS := $(wildcard $(USER)/include/*.h)

# Utils ------------------------------------------------------------------------

UTILS := $(SRC)/utils
UTILS_SRCS := $(wildcard $(UTILS)/*.c)
UTILS_OBJS := $(patsubst $(UTILS)/%.c, $(OBJ)/%.o, $(UTILS_SRCS))
UTILS_HDRS := $(wildcard $(UTILS)/*.h)

.PHONY: run all clean master users nodes

all: master users nodes

run: all
	@$(BIN)/master

clean:
	@rm -f -r $(BIN) $(OBJ) *~

master: $(BIN)/master
$(BIN)/master: $(MASTER_OBJS) $(UTILS_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -pthread

users: $(BIN)/users
$(BIN)/users: $(USER_OBJS) $(UTILS_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lm

nodes: $(BIN)/nodes
$(BIN)/nodes: $(NODE_OBJS) $(UTILS_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Objs -------------------------------------------------------------------------

$(OBJ)/%.o: $(MASTER)/%.c $(MASTER_HDRS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ)/%.o: $(NODE)/%.c $(NODE_HDRS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ)/%.o: $(USER)/%.c $(USER_HDRS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ)/%.o: $(UTILS)/%.c $(UTILS_HDRS)
	$(CC) $(CFLAGS) -c -o $@ $<
