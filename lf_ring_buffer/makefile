MAKEFLAGS+=-Rr
SHELL:=/bin/bash
.ONESHELL:
.SECONDEXPANSION:
.NOTINTERMEDIATE: %.d %.h
.SECONDARY: logger.o lf_ring_buffer.o lf_ring_buffer_mpmc.t.o

srcdir:=..
vpath %.cpp $(srcdir) $(srcdir)/../logger

#asan_flags:=-fsanitize=address -fsanitize=pointer-compare -fsanitize=leak -fsanitize=undefined -fno-sanitize-recover
asan_flags:=-fsanitize=thread -fno-sanitize-recover
all_cppflags:=-I$(srcdir) -I$(srcdir)/../logger $(CPPFLAGS)
all_cxxflags:=-Wall -Wextra -Werror -Wfatal-errors -ggdb -O0 -m64 -std=c++17\
  $(asan_flags) $(CXXFLAGS)
all_ldflags:=$(asan_flags) $(LDFLAGS)

obj:=logger.o lf_ring_buffer.o lf_ring_buffer_mpmc.t.o
dfiles:=$(obj:.o=.d)

target:=lf_ring_buffer_mpmc.t.tsk
all: $(target)
$(target): $(obj)
	g++ -o $@ $^ $(all_ldflags) -lstdc++ -lpthread

%.o: %.cpp %.d $$(file <%.d)
	g++ $(all_cppflags) $(all_cxxflags) -MMD -MF $*.td -o $@ -c $<
	read obj src headers <$*.td; echo "$$headers" >$*.d
	touch -c $@

$(dfiles): ;
%.h:;

tarxf:
	cp $(srcdir)/../logger/*.{h,cpp} .
	tar cfz mpmc.tgz $(target) $(srcdir)/makefile $(srcdir)/*{h,.cpp} *.{h,cpp}
	-rm -f *.{h,cpp}

clean:
	rm -f $(target) $(obj) $(dfiles) $(obj:.o=.td)

makefile::;
