SRCDIR=src
OBJDIR=build
LIBNAME=inifile

CPPFLAGS=-g -Wall -I$(SRCDIR)

OBJS=$(LIBNAME).o notfound.o

OUTLIB=lib$(LIBNAME).a
SAMPLE=sample

all: $(OUTLIB)

vpath %.cc src/ test/
vpath %.h src/
vpath %.o $(OBJDIR)

$(OBJDIR):
	mkdir -p $@

$(OBJDIR)/%.o: %.cc | $(OBJDIR)
	$(COMPILE.cc) $(OUTPUT_OPTION) $<

# each object file depends from the corresponding .h
$(OBJDIR)/%.o: %.h

# they all depend from notfound.h
$(OBJDIR)/%.o: notfound.h

$(OBJDIR)/$(SAMPLE).o: $(LIBNAME).h

$(OUTLIB): $(addprefix $(OBJDIR)/, $(OBJS))
	$(AR) cr $@ $^

$(SAMPLE): LDLIBS=-L. -lstdc++ -l$(LIBNAME)
$(SAMPLE): $(OUTLIB)

test: $(SAMPLE)
	./$(SAMPLE) test/sample.ini

clean:
	-rm -rf $(OBJDIR) $(OUTLIB) $(SAMPLE)

.PHONY: all clean test
