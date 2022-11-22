DUMPMACHINE := $(shell $(CC) -dumpmachine)

ifneq ($(firstword $(filter x86_64-%,$(DUMPMACHINE))),)
  XCFLAGS = -march=x86-64 -mtune=nocona
else ifneq ($(firstword $(filter i686-%,$(DUMPMACHINE))),)
  XCFLAGS = -march=pentiumpro -mtune=intel
endif

CFLAGS = -O3 -DNDEBUG -Iinclude $(XCFLAGS)

SRC_PATH := src
OBJ_PATH := obj
LIB_PATH := lib

LIB_FILE := $(LIB_PATH)/libhashset-1.a
OBJ_FILE := $(OBJ_PATH)/hash_set.o

.PHONY: all clean

all: $(LIB_FILE)

$(LIB_FILE): $(OBJ_FILE) $(LIB_PATH)
	$(AR) rcs $@ $<

$(OBJ_FILE): $(SRC_PATH) $(OBJ_PATH)
	$(CC) $(CFLAGS) -c $(SRC_PATH)/$(patsubst %.o,%.c,$(notdir $@)) -o $@

$(SRC_PATH) $(OBJ_PATH) $(LIB_PATH):
	mkdir -p $@

clean:
	rm -f $(LIB_FILE) $(OBJ_PATH)/*.o
