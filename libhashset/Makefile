include ../config.mk

CFLAGS = -std=c99 -D_DEFAULT_SOURCE -Wall -Wpedantic -Iinclude -fPIC $(XCFLAGS)

SRC_PATH := src
OBJ_PATH := obj
LIB_PATH := lib
ALL_PATH := $(SRC_PATH) $(OBJ_PATH) $(LIB_PATH)

SRC_FILE := $(wildcard $(SRC_PATH)/*.c)
OBJ_FILE := $(addprefix $(OBJ_PATH)/,$(patsubst %.c,%.o,$(notdir $(SRC_FILE))))
LIB_FILE := $(LIB_PATH)/libhashset-1.a
DLL_FILE := $(LIB_PATH)/libhashset-1$(DLL_SUFFIX)

.PHONY: all build clean test

all: clean build

test: build

build: $(ALL_PATH) $(LIB_FILE) $(DLL_FILE)

$(LIB_FILE): $(OBJ_FILE)
	$(AR) rcs $@ $(filter-out $(OBJ_PATH)/dll%,$^)

$(DLL_FILE): $(OBJ_FILE)
	$(CC) $(CFLAGS) $(DLL_LDFLAGS) -o $@ $^

$(OBJ_FILE):
	$(CC) $(CFLAGS) -c $(SRC_PATH)/$(patsubst %.o,%.c,$(notdir $@)) -o $@

$(ALL_PATH):
	mkdir -p $@

clean:
	rm -vf $(LIB_FILE) $(DLL_FILE) $(DLL_FILE).a $(OBJ_PATH)/*.o
