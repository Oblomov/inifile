SRCDIR=src
INCDIR=include
OBJDIR=build
LIBNAME=inifile
ERRNAME=inierr

CPPFLAGS=-g -Wall -I$(INCDIR)

OBJS=$(addprefix $(OBJDIR)/, $(LIBNAME).o $(LIBNAME)_private.o $(ERRNAME).o)

OUTLIB=lib$(LIBNAME).a
SAMPLE=sample

all: $(OUTLIB)

vpath %.cc $(SRCDIR) test/
vpath %.h $(INCDIR) $(SRCDIR)

$(OBJDIR):
	mkdir -p $@

$(OBJDIR)/%.o: %.cc %.h
	$(COMPILE.cc) $(OUTPUT_OPTION) $<

# they all depend from inierr.h
$(OBJS): $(ERRNAME).h | $(OBJDIR)

$(OBJDIR)/$(LIBNAME).o: $(LIBNAME)_private.h

$(OUTLIB): $(OBJS)
	$(AR) cr $@ $^

$(SAMPLE): LDLIBS=-L. -lstdc++ -l$(LIBNAME)
$(SAMPLE): $(OUTLIB)

test: $(SAMPLE)
	./$(SAMPLE) test/sample.ini

clean:
	-rm -rf $(OBJDIR) $(OUTLIB) $(SAMPLE)

.PHONY: all clean test
