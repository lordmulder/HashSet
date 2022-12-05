include ../../config.mk

CFLAGS = -std=c99 -D_DEFAULT_SOURCE -Wall -Wpedantic -I../../libhashset/include -I../shared/include $(XCFLAGS)
LDFLAGS = -L../../libhashset/lib -lhashset-1 $(XLDFLAGS)

SRC_PATH := src
BIN_PATH := bin
ALL_PATH := $(SRC_PATH) $(BIN_PATH)

BIN_FILE := $(BIN_PATH)/test-hash-map$(EXE_SUFFIX)
SRC_FILE := $(wildcard $(SRC_PATH)/*.c) $(wildcard ../shared/src/*.c)

.PHONY: all clean test

all: clean $(ALL_PATH) $(BIN_FILE)

$(BIN_FILE): $(SRC_FILE)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(ALL_PATH):
	mkdir -p $@

test: all
	env $(ENV_LDPATH)="$(realpath .):$(realpath ../../libhashset/lib)$(if $($(ENV_LDPATH)),:$($(ENV_LDPATH)))" ./$(BIN_FILE)

clean:
	rm -vf $(BIN_FILE)
