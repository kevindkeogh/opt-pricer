CC=gcc
CFLAGS=-Wall -fPIC -O3 -ansi -pedantic-errors
LDFLAGS=-lgsl -lcblas -lm
PREFIX= /usr/local

optpricer : src/optpricer.c gbm.o black_scholes.o
	@$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

gbm.o : src/gbm_mc.c
	@$(CC) $(CFLAGS) -c $^ $(LDFLAGS) -o $@

black_scholes.o : src/black_scholes.c
	@$(CC) $(CFLAGS) -c $^ $(LDFLAGS) -o $@

.PHONY: install
install : optpricer
	@mkdir -p $(DESTDIR)$(PREFIX)/bin
	@cp $< $(DESTDIR)$(PREFIX)/bin/optpricer
	@rm -f $<

.PHONY: uninstall
uninstall :
	@rm -f $(DESTDIR)$(PREFIX)/bin/optpricer

.PHONY: clean
clean :
	@rm -f gbm.o black_scholes.o
