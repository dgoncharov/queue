MAKEFLAGS+=-Rr
SHELL:=/bin/bash
.ONESHELL:
.SECONDEXPANSION:
.NOTINTERMEDIATE: %.d %.h
.SECONDARY: posixqueue.o posixmpmc.t.o

srcdir:=..
vpath %.cpp $(srcdir)
vpath %.h $(srcdir)

all_cppflags:=-I$(srcdir) $(CPPFLAGS)
all_cflags:=-Wall -Wextra -Werror -ggdb -O0 -m64 -std=c++17 $(CFLAGS)

obj:=posixqueue.o posixmpmc.t.o
dfiles:=$(obj:.o=.d)

target:=posixmpmc.t.tsk
all: $(target)
$(target): $(obj)
	gcc -o $@ $^ -lstdc++ -lpthread

%.o: %.cpp %.d $$(file <%.d)
	g++ $(all_cppflags) $(all_cflags) -MMD -MF $*.td -o $@ -c $<
	read obj src headers <$*.td; echo "$$headers" >$*.d
	touch -c $@

$(dfiles): ;
%.h:;

clean:
	rm -f $(target) $(obj) $(dfiles) $(obj:.o=.td)

makefile::;
