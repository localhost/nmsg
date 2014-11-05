CFLAGS ?= -I/opt/local/include -g
LDFLAGS ?= -L/opt/local/lib -lnanomsg

all: nmsg

clean:
	@cd ext/nmsg && make distclean || true

nmsg: ext/nmsg/extconf.rb ext/nmsg/rubyext.c
	@echo Building Ruby extension 'nmsg'...
	@cd ext/nmsg && CFLAGS="${CFLAGS}" LDFLAGS="${LDFLAGS}" ruby extconf.rb && make

.PHONY: all clean nmsg
