MAKEFLAGS+=-r
SHELL:=/bin/bash
.ONESHELL:
.SECONDEXPANSION:

srcdir:=..
vpath %.cpp $(srcdir) $(srcdir)/../logger

asan_flags:=-fsanitize=address -fsanitize=pointer-compare -fsanitize=leak -fsanitize=undefined
#asan_flags:=-fsanitize=thread
all_cppflags:=-I$(srcdir) -I$(srcdir)/../logger $(CPPFLAGS)
all_cxxflags:=-Wall -Wextra -Werror -Wfatal-errors -ggdb -O0 -m64 -std=c++17\
  $(asan_flags) $(CXXFLAGS)
all_ldflags:=$(asan_flags) $(LDFLAGS)

obj:=logger.o lf_circular_buffer.o lf_circular_buffer.t.o
dfiles:=$(obj:.o=.d)
.SECONDARY: $(obj)

target:=lf_circular_buffer.t.tsk
all: $(target)
$(target): $(obj)
	$(CXX) -o $@ $^ $(all_ldflags) -lubsan -lstdc++ -lpthread

%.o: %.cpp %.d $$(file <%.d)
	$(CXX) $(all_cppflags) $(all_cxxflags) -MMD -MF $*.td -o $@ -c $<
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
