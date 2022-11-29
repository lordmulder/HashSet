DUMPMACHINE := $(shell $(CC) -dumpmachine)

ifneq ($(DEBUG),)
  XCFLAGS = -Og -g
else
ifneq ($(ASAN),)
  XCFLAGS = -O1 -g -fsanitize=address -fno-omit-frame-pointer -static-libasan
else
  XCFLAGS = -Ofast -DNDEBUG
ifneq ($(firstword $(filter x86_64-%,$(DUMPMACHINE))),)
  XCFLAGS += -march=x86-64 -mtune=nocona
else ifneq ($(firstword $(filter i686-%,$(DUMPMACHINE))),)
  XCFLAGS += -march=pentiumpro -mtune=intel
endif
ifneq ($(FLTO),)
  XCFLAGS += -flto
endif
endif
endif

CFLAGS = -std=c99 -D_DEFAULT_SOURCE -Wpedantic -Iinclude $(XCFLAGS)

SRC_PATH := src
OBJ_PATH := obj
LIB_PATH := lib
ALL_PATH := $(SRC_PATH) $(OBJ_PATH) $(LIB_PATH)

SRC_FILE := $(wildcard $(SRC_PATH)/*.c)
OBJ_FILE := $(addprefix $(OBJ_PATH)/,$(patsubst %.c,%.o,$(notdir $(SRC_FILE))))
LIB_FILE := $(LIB_PATH)/libhashset-1.a

.PHONY: all build clean

all: clean build

build: $(ALL_PATH) $(LIB_FILE)

$(LIB_FILE): $(OBJ_FILE)
	$(AR) rcs $@ $(OBJ_FILE)

$(OBJ_FILE):
	$(CC) $(CFLAGS) -c $(SRC_PATH)/$(patsubst %.o,%.c,$(notdir $@)) -o $@

$(ALL_PATH):
	mkdir -p $@

clean:
	rm -vf $(LIB_FILE) $(OBJ_PATH)/*.o
