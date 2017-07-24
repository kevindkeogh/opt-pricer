CC=gcc
CFLAGS=-Wall -fPIC -O3 -ansi -pedantic-errors
LDFLAGS=-lm
PREFIX= /usr/local

opt-pricer : src/opt-pricer.c gbm.o black_scholes.o utils.o
	@$(CC) $(CFLAGS) $^ -o $@

gbm.o : src/gbm_mc.c
	@$(CC) $(CFLAGS) -c $^ -o $@

black_scholes.o : src/black_scholes.c
	@$(CC) $(CFLAGS) -c $^ -o $@

utils.o : src/utils.c
	@$(CC) $(CFLAGS) -c $^ -o $@

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
	@rm -f gbm.o black_scholes.o utils.o
