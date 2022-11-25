SUBDIRS := libhashset hashset

BUILD_ALL := $(patsubst %,build_rule\:%,$(SUBDIRS))
CLEAN_ALL := $(patsubst %,clean_rule\:%,$(SUBDIRS))

.PHONY: all clean $(BUILD_ALL) $(CLEAN_ALL)

all: $(BUILD_ALL)

clean: $(CLEAN_ALL)

$(BUILD_ALL):
	$(MAKE) -C $(patsubst build_rule:%,%,$@)

$(CLEAN_ALL):
	$(MAKE) -C $(patsubst clean_rule:%,%,$@) clean
