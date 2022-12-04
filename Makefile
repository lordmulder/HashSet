SUBDIRS := libhashset example test

.PHONY: all clean $(SUBDIRS)

all clean: $(SUBDIRS)

example test: libhashset

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)
