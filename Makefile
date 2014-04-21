SRCDIR=src
OBJDIR=build
LIBNAME=inifile

CPPFLAGS=-g -Wall -I$(SRCDIR)

OBJS=$(addprefix $(OBJDIR)/, $(LIBNAME).o notfound.o)

OUTLIB=lib$(LIBNAME).a
SAMPLE=sample

all: $(OUTLIB)

vpath %.cc $(SRCDIR) test/
vpath %.h $(SRCDIR)

$(OBJDIR):
	mkdir -p $@

$(OBJDIR)/%.o: %.cc %.h
	$(COMPILE.cc) $(OUTPUT_OPTION) $<

# they all depend from notfound.h
$(OBJS): notfound.h | $(OBJDIR)

$(OUTLIB): $(OBJS)
	$(AR) cr $@ $^

$(SAMPLE): LDLIBS=-L. -lstdc++ -l$(LIBNAME)
$(SAMPLE): $(OUTLIB)

test: $(SAMPLE)
	./$(SAMPLE) test/sample.ini

clean:
	-rm -rf $(OBJDIR) $(OUTLIB) $(SAMPLE)

.PHONY: all clean test
