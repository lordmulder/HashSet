SUBDIRS := libhashset example test

.PHONY: all clean test $(SUBDIRS)

all clean test: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)
