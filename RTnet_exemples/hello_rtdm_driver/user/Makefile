# Allow overriding xeno-config on make command line
XENO_CONFIG=xeno-config

prefix := $(shell $(XENO_CONFIG) --prefix)

ifeq ($(prefix),)
$(error Please add <xenomai-install-path>/bin to your PATH variable)
endif

CC := $(shell $(XENO_CONFIG) --skin=posix --cc)
STD_CFLAGS  := $(shell $(XENO_CONFIG) --skin=posix --cflags) -g
STD_LDFLAGS := $(shell $(XENO_CONFIG) --skin=posix --ldflags) -g -lrtdm

STD_TARGETS := hello_rtdm_user

all: $(STD_TARGETS)

$(STD_TARGETS): $(STD_TARGETS:%=%.c)
	$(CC) -o $@ $< $(STD_CFLAGS) $(STD_LDFLAGS)

clean:
	$(RM) -f *.o *~ $(STD_TARGETS) 
