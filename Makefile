DUMPMACHINE := $(shell $(CC) -dumpmachine)

ifneq ($(firstword $(filter x86_64-%,$(DUMPMACHINE))),)
  XCFLAGS = -march=x86-64 -mtune=nocona
else ifneq ($(firstword $(filter i686-%,$(DUMPMACHINE))),)
  XCFLAGS = -march=pentiumpro -mtune=intel
endif

CFLAGS = -O3 -DNDEBUG -Ilibhashset/include $(XCFLAGS)

BASE_DIR := libhashset

SRC_PATH := $(BASE_DIR)/src
OBJ_PATH := $(BASE_DIR)/obj
LIB_PATH := $(BASE_DIR)/lib

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
