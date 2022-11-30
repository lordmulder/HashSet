SUBDIRS := libhashset example test

BUILD_ALL := $(patsubst %,build\:%,$(SUBDIRS))
CLEAN_ALL := $(patsubst %,clean\:%,$(SUBDIRS))

.PHONY: all test clean $(BUILD_ALL) $(CLEAN_ALL)

all: $(BUILD_ALL)

clean: $(CLEAN_ALL)

$(BUILD_ALL):
	$(MAKE) -C $(patsubst build:%,%,$@)

$(CLEAN_ALL):
	$(MAKE) -C $(patsubst clean:%,%,$@) clean

test: $(BUILD_ALL)
	$(MAKE) -C test test
