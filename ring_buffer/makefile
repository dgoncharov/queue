MAKEFLAGS+=-Rr
SHELL:=/bin/bash
.ONESHELL:
.SECONDEXPANSION:
.NOTINTERMEDIATE: %.d %.h

srcdir:=..
vpath %.cpp $(srcdir)

asan_flags:=-fsanitize=address -fsanitize=pointer-compare -fsanitize=leak -fsanitize=undefined
#asan_flags:=-fsanitize=thread
all_cppflags:=-I$(srcdir) $(CPPFLAGS)
all_cflags:=-Wall -Wextra -Werror -Wfatal-errors -ggdb -O0 -m64 -std=c++17\
  $(asan_flags) $(CFLAGS)
all_ldflags:=$(asan_flags) $(LDFLAGS)

obj:=ring_buffer.t.o
dfiles:=$(obj:.o=.d)
.SECONDARY: $(obj)


target:=ring_buffer.t.tsk
all: $(target)
$(target): $(obj)
	gcc -o $@ $^ $(all_ldflags) -lstdc++ -lpthread

%.o: %.cpp %.d $$(file <%.d)
	g++ $(all_cppflags) $(all_cflags) -MMD -MF $*.td -o $@ -c $<
	read obj src headers <$*.td; echo "$$headers" >$*.d
	touch -c $@

$(dfiles): ;
%.h:;

clean:
	rm -f $(target) $(obj) $(dfiles) $(obj:.o=.td)

makefile::;

# Copyright (c) 2021 Dmitry Goncharov
#
# Distributed under the BSD License.
# (See accompanying file COPYING).
