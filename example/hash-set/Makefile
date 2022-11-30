include ../../config.mk

CFLAGS = -std=c99 -D_DEFAULT_SOURCE -Wpedantic -I../../libhashset/include $(XCFLAGS)
LDFLAGS = -L../../libhashset/lib -lhashset-1 $(XLDFLAGS)

SRC_PATH := src
BIN_PATH := bin
ALL_PATH := $(SRC_PATH) $(BIN_PATH)

BIN_FILE := $(BIN_PATH)/example-hash-set$(EXE_SUFFIX)
SRC_FILE := $(wildcard $(SRC_PATH)/*.c)

.PHONY: all build clean

all: clean build

build: $(ALL_PATH) $(BIN_FILE)

$(BIN_FILE): $(SRC_FILE)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(ALL_PATH):
	mkdir -p $@

clean:
	rm -vf $(BIN_FILE)
