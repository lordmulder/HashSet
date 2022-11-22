SUBDIRS := libhashset hashset

.PHONY: all clean

all:
	for subdir in $(SUBDIRS); do $(MAKE) -C $$subdir; done

clean:
	for subdir in $(SUBDIRS); do $(MAKE) -C $$subdir clean; done
