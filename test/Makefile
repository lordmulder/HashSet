SUBDIRS := hash-set hash-map

BUILD_ALL := $(patsubst %,build\:%,$(SUBDIRS))
CLEAN_ALL := $(patsubst %,clean\:%,$(SUBDIRS))
TESTS_ALL := $(patsubst %,rtest\:%,$(SUBDIRS))

.PHONY: all clean test $(BUILD_ALL) $(CLEAN_ALL) $(TESTS_ALL)

all: $(BUILD_ALL)

clean: $(CLEAN_ALL)

test: $(TESTS_ALL)

$(BUILD_ALL):
	$(MAKE) -C $(patsubst build:%,%,$@)

$(CLEAN_ALL):
	$(MAKE) -C $(patsubst clean:%,%,$@) clean

$(TESTS_ALL):
	$(MAKE) -C $(patsubst rtest:%,%,$@) test
