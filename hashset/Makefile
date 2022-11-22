DUMPMACHINE := $(shell $(CC) -dumpmachine)

ifneq ($(firstword $(filter x86_64-%,$(DUMPMACHINE))),)
  XCFLAGS = -march=x86-64 -mtune=nocona -s
else ifneq ($(firstword $(filter i686-%,$(DUMPMACHINE))),)
  XCFLAGS = -march=pentiumpro -mtune=intel -s
endif

ifeq ($(OS),Windows_NT)
ifneq ($(firstword $(filter i686-%,$(DUMPMACHINE))),)
  XCFLAGS += -Wl,--large-address-aware
endif
  EXE_SUFFIX := .exe
endif

CFLAGS = -O3 -DNDEBUG -I../libhashset/include $(XCFLAGS)

SRC_PATH := src
BIN_PATH := bin

BIN_FILE := $(BIN_PATH)/hashset$(EXE_SUFFIX)
SRC_FILE := $(SRC_PATH)/main.c
LIB_FILE := ../libhashset/lib/libhashset-1.a

.PHONY: all clean

all: $(BIN_FILE)

$(BIN_FILE): $(SRC_FILE) $(LIB_FILE) $(BIN_PATH)
	$(CC) $(CFLAGS) $< -o $@ $(LIB_FILE)

$(SRC_PATH) $(BIN_PATH):
	mkdir -p $@

clean:
	rm -f $(BIN_FILE)
