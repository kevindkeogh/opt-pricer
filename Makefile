CC=gcc
CFLAGS=-Wall -fPIC -O3 -ansi -pedantic-errors
LDFLAGS=-lm
PREFIX= /usr/local

WINDOWS_CC=x86_64-w64-mingw32-gcc
WINDOWS_CFLAGS= -Wall -O3 -ansi -pedantic-errors
WINDOWS_PLATFORM= windows

.DEFAULT_GOAL := build/opt-pricer

build/opt-pricer : src/opt-pricer.c build/depends/linux/gbm.o build/depends/linux/black_scholes.o build/depends/linux/utils.o
	@$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

build/depends/%/gbm.o : src/gbm_mc.c | folders
	@$(CC) $(CFLAGS) -c $^ $(LDFLAGS) -o $@

build/depends/%/black_scholes.o : src/black_scholes.c | folders
	@$(CC) $(CFLAGS) -c $^ $(LDFLAGS) -o $@

build/depends/%/utils.o : src/utils.c | folders
	@$(CC) $(CFLAGS) -c $^ $(LDFLAGS) -o $@

build/depends/%/strptime.o : src/strptime.c | folders
	@$(CC) $(CFLAGS) -c $^ $(LDFLAGS) -o $@

folders:
	@mkdir -p build
	@mkdir -p build/depends
	@mkdir -p build/depends/linux
	@mkdir -p build/depends/windows


build/opt-pricer.exe : CC=$(WINDOWS_CC)
build/opt-pricer.exe : CFLAGS=$(WINDOWS_CFLAGS)
build/opt-pricer.exe : src/opt-pricer.c build/depends/windows/gbm.o build/depends/windows/black_scholes.o build/depends/windows/utils.o build/depends/windows/strptime.o
	@$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@


.PHONY: windows
windows: build/opt-pricer.exe

.PHONY: install
install : build/opt-pricer
	@mkdir -p $(DESTDIR)$(PREFIX)/bin
	@cp $< $(DESTDIR)$(PREFIX)/bin/opt-pricer
	@rm -f $<

.PHONY: uninstall
uninstall :
	@rm -f $(DESTDIR)$(PREFIX)/bin/opt-pricer

.PHONY: clean
clean :
	@rm -rf build
	@mkdir build
	@mkdir build/depends
