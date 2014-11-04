CFLAGS ?= -I/opt/local/include -g
LDFLAGS ?= -L/opt/local/lib -lnanomsg

all: rnmsg test

clean:
	@cd ext/ruby && make distclean || true

test.o: ext/ruby/test.c
	cc -c ${CFLAGS} ${INCS} -o $@ $<

test: ext/ruby/test.o
	cc -o $@ ${LDFLAGS} $<


rnmsg: ext/ruby/extconf.rb ext/ruby/rubyext.c
	@echo Building Ruby extension 'nnmsg'...
	@cd ext/ruby && CFLAGS="${CFLAGS}" LDFLAGS="${LDFLAGS}" ruby extconf.rb && make

.PHONY: all clean rnmsg
