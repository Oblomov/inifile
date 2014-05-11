MKCOM=Makefile.common
MKLIB=Makefile.lib
MKTST=Makefile.test


all: libs test

libs: $(MKLIB)
	$(MAKE) -f $< $@

test: $(MKTST) libs
	$(MAKE) -f $< $@

clean: $(MKCOM) $(MKLIB) $(MKTST)
	$(MAKE) -f $(MKCOM) common-$@
	$(MAKE) -f $(MKLIB) $@
	$(MAKE) -f $(MKTST) $@

.PHONY: all lib test
.PHONY: clean
