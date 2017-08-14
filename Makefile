CC=gcc
CFLAGS=-Wall -g -fPIC -O3 -pthread
CPPFLAGS=-pedantic -std=c++11
LDFLAGS=-lm -lstdc++
PREFIX=/usr/local
THIS_DIR=$(dir $(abspath $(lastword $(MAKEFILE_LIST))))

WINDOWS_CC=x86_64-w64-mingw32-gcc
WINDOWS_CFLAGS= -Wall -O3 -ansi -pedantic-errors -pthread
WINDOWS_PLATFORM= windows

.DEFAULT_GOAL := build/opt-pricer

build/opt-pricer : src/opt-pricer.c build/depends/linux/gbm.o build/depends/linux/black_scholes.o build/depends/linux/utils.o
	@$(MAKE) -s build/libs/libstats.so
	@$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@ -Wl,-rpath,'$$ORIGIN/libs' -Lbuild/libs -lstats

build/depends/%/gbm.o : src/gbm_mc.c | folders
	@$(CC) $(CFLAGS) -c $^ $(LDFLAGS) -o $@

build/depends/%/black_scholes.o : src/black_scholes.c | folders
	@$(CC) $(CFLAGS) -c $^ $(LDFLAGS) -o $@

build/depends/%/utils.o : src/utils.c | folders
	@$(CC) $(CFLAGS) -c $^ $(LDFLAGS) -o $@

build/depends/%/strptime.o : src/strptime.c | folders
	@$(CC) $(CFLAGS) -c $^ $(LDFLAGS) -o $@

build/depends/%/asa241.o : src/asa241.c | folders
	@$(CC) $(CFLAGS) -c $^ $(LDFLAGS) -o $@

build/depends/%/sobol.o : src/sobol.cpp | folders
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $^ $(LDFLAGS) -o $@

build/libs/libstats.so : build/depends/linux/sobol.o build/depends/linux/asa241.o | folders
	@$(CC) -fPIC -shared -o $@ $^ -lstdc++

folders:
	@mkdir -p build
	@mkdir -p build/libs
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
	@ln -sf $(THIS_DIR)$< $(DESTDIR)$(PREFIX)/bin/opt-pricer

.PHONY: uninstall
uninstall :
	@rm -f $(DESTDIR)$(PREFIX)/bin/opt-pricer

.PHONY: clean
clean :
	@rm -rf build
