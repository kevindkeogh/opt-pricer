CC=gcc
WINDOWS_CC=x86_64-w64-mingw32-gcc
CFLAGS=-Wall -fPIC -O3 -ansi -pedantic-errors
WINDOWS_CFLAGS= -Wall -O3 -ansi -pedantic-errors
LDFLAGS=-lm
PREFIX= /usr/local

opt-pricer : src/opt-pricer.c gbm.o black_scholes.o utils.o
	@$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

gbm.o : src/gbm_mc.c
	@$(CC) $(CFLAGS) -c $^ $(LDFLAGS) -o $@

black_scholes.o : src/black_scholes.c
	@$(CC) $(CFLAGS) -c $^ $(LDFLAGS) -o $@

utils.o : src/utils.c
	@$(CC) $(CFLAGS) -c $^ $(LDFLAGS) -o $@

strptime.o : src/strptime.c
	@$(CC) $(CFLAGS) -c $^ $(LDFLAGS) -o $@

opt-pricer.exe : CC=$(WINDOWS_CC)
opt-pricer.exe : CFLAGS=$(WINDOWS_CFLAGS)
opt-pricer.exe : src/opt-pricer.c gbm.o black_scholes.o utils.o strptime.o
	@$(WINDOWS_CC) $(WINDOWS_CFLAGS) $^ $(LDFLAGS) -o $@

.PHONY: windows
windows: opt-pricer.exe

.PHONY: install
install : opt-pricer
	@mkdir -p $(DESTDIR)$(PREFIX)/bin
	@cp $< $(DESTDIR)$(PREFIX)/bin/opt-pricer
	@rm -f $<

.PHONY: uninstall
uninstall :
	@rm -f $(DESTDIR)$(PREFIX)/bin/opt-pricer

.PHONY: clean
clean :
	@rm -f gbm.o black_scholes.o utils.o strptime.o
