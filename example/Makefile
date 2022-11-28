DUMPMACHINE := $(shell $(CC) -dumpmachine)

ifneq ($(SANITIZE_ADDRESS),1)
  XCFLAGS = -Ofast -DNDEBUG -s -static
ifneq ($(firstword $(filter x86_64-%,$(DUMPMACHINE))),)
  XCFLAGS += -march=x86-64 -mtune=nocona
else ifneq ($(firstword $(filter i686-%,$(DUMPMACHINE))),)
  XCFLAGS += -march=pentiumpro -mtune=intel
endif
else
  XCFLAGS = -fsanitize=address -static-libasan -g
endif

ifneq ($(firstword $(filter %-mingw32 %-cygwin,$(DUMPMACHINE))),)
  EXE_SUFFIX := .exe
ifneq ($(firstword $(filter i686-%,$(DUMPMACHINE))),)
  XCFLAGS += -Wl,--large-address-aware
endif
endif

CFLAGS = -std=c99 -D_DEFAULT_SOURCE -Wpedantic -I../libhashset/include $(XCFLAGS)

SRC_PATH := src
BIN_PATH := bin
ALL_PATH := $(SRC_PATH) $(BIN_PATH)

BIN_FILE := $(BIN_PATH)/hashset-example$(EXE_SUFFIX)
SRC_FILE := $(SRC_PATH)/main.c
LIB_FILE := ../libhashset/lib/libhashset-1.a

.PHONY: all build clean

all: clean build

build: $(ALL_PATH) $(BIN_FILE)

$(BIN_FILE): $(SRC_FILE) $(LIB_FILE)
	$(CC) $(CFLAGS) -o $@ $^

$(ALL_PATH):
	mkdir -p $@

clean:
	rm -f $(BIN_FILE)
