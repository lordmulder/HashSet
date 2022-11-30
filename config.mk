DUMPMACHINE := $(shell $(CC) -dumpmachine)
ifeq ($(DUMPMACHINE),)
  $(error The C compiler could not be detected!)
endif

ifneq ($(DEBUG),)
  XCFLAGS = -Og -g
else
ifneq ($(ASAN),)
  XCFLAGS = -O1 -g -fsanitize=address -fno-omit-frame-pointer
  XLDFLAGS += -static-libasan
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
  XLDFLAGS += -s -static
endif
endif

ifneq ($(firstword $(filter %-mingw32 %-cygwin,$(DUMPMACHINE))),)
  EXE_SUFFIX := .exe
ifneq ($(firstword $(filter i686-%,$(DUMPMACHINE))),)
  XLDFLAGS += -Wl,--large-address-aware
endif
endif
