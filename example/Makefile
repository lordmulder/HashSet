SUBDIRS := hash-set hash-map

.PHONY: all clean test $(SUBDIRS)

all clean test: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)
