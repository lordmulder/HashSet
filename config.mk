DUMPMACHINE := $(strip $(shell $(CC) -dumpmachine))
ifeq ($(DUMPMACHINE),)
  $(error C compiler could not be detected!)
endif

ifneq ($(DEBUG),)
  XCFLAGS = -Og -g
else
ifneq ($(ASAN),)
  XCFLAGS = -O1 -g -fsanitize=address -fno-omit-frame-pointer
  XLDFLAGS += -static-libasan
else
  XCFLAGS = -Ofast -DNDEBUG
endif
endif

ifneq ($(firstword $(filter x86_64-%,$(DUMPMACHINE))),)
  XCFLAGS += -march=x86-64 -mtune=nocona
else ifneq ($(firstword $(filter i686-%,$(DUMPMACHINE))),)
  XCFLAGS += -march=pentiumpro -mtune=intel
endif

ifeq ($(firstword $(filter %-mingw32 %-windows-gnu %-cygwin,$(DUMPMACHINE))),)
  DLL_LDFLAGS = -shared
  DLL_SUFFIX := .so
else
  DLL_LDFLAGS = -shared -Wl,--out-implib,$@.a
  EXE_SUFFIX := .exe
  DLL_SUFFIX := .dll
ifneq ($(firstword $(filter i686-%,$(DUMPMACHINE))),)
  XLDFLAGS += -Wl,--large-address-aware
endif
endif

ifneq ($(STATIC),)
  XLDFLAGS += -static
endif

ifneq ($(FLTO),)
  XCFLAGS += -flto
endif

ifneq ($(STRIP),)
  XLDFLAGS += -Wl,--strip-all
  DLL_LDFLAGS += -Wl,--strip-all
endif
