SRCDIR=src
INCDIR=include
OBJDIR=build
LIBNAME=inifile
ERRNAME=$(LIBNAME)_err

OBJS=$(addprefix $(OBJDIR)/, $(LIBNAME).o $(LIBNAME)_private.o $(ERRNAME).o)

VERSION_MAJOR=1
VERSION_MINOR=0
VERSION=$(VERSION_MAJOR).$(VERSION_MINOR)

ALIB=lib$(LIBNAME).a

SOBASENAME=lib$(LIBNAME).so
SOLIBNAME=lib$(LIBNAME).so.$(VERSION_MAJOR)
SOLIB=$(SOBASENAME).$(VERSION)

SOFILES=$(SOLIB) $(SOLIBNAME) $(SOBASENAME)

CPPFLAGS=-g -Wall -I$(INCDIR)
CXXFLAGS=-fPIC -fvisibility=hidden

SAMPLE=sample

all: libs
	
solib: $(SOLIB) $(SOLIBAME) $(SOBASENAME)

libs: $(ALIB) solib

vpath %.cc $(SRCDIR) test/
vpath %.h $(INCDIR) $(SRCDIR)

$(OBJDIR):
	mkdir -p $@

$(OBJDIR)/%.o: %.cc %.h
	$(COMPILE.cc) $(OUTPUT_OPTION) $<

# they all depend from $(ERRNAME).h
$(OBJS): $(ERRNAME).h | $(OBJDIR)

$(OBJDIR)/$(LIBNAME).o: $(LIBNAME)_private.h

$(ALIB): $(OBJS)
	$(AR) cr $@ $^

$(SOLIB): LDFLAGS=-shared -Wl,-soname=$(SOLIBNAME)
$(SOLIB): $(OBJS)
	$(CXX) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

$(SOLIBNAME): $(SOLIB)
	ln -sf $< $@

$(SOBASENAME): $(SOLIBNAME)
	ln -sf $< $@

$(SAMPLE): LDLIBS=-L. -l$(LIBNAME)

test: $(SAMPLE)
	LD_LIBRARY_PATH=$${LDLIBRARY_PATH}:. ./$(SAMPLE) test/sample.ini

clean:
	-rm -rf $(OBJDIR) $(ALIB) $(SOFILES) $(SAMPLE)

.PHONY: all libs solib
.PHONY: clean test
