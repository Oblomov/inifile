all: libs test

libs:
	$(MAKE) -f Makefile.lib libs

test: libs
	$(MAKE) -f Makefile.test test

clean:
	$(MAKE) -f Makefile.common common-clean
	$(MAKE) -f Makefile.lib clean
	$(MAKE) -f Makefile.test clean

.PHONY: all lib test
.PHONY: clean
