CC=gcc
CFLAGS=-Wall -fPIC -O3 -ansi -pedantic-errors
LDFLAGS=-lcblas -lm
PREFIX= /usr/local

opt-pricer : src/opt-pricer.c gbm.o black_scholes.o utils.o
	@$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

gbm.o : src/gbm_mc.c
	@$(CC) $(CFLAGS) -c $^ $(LDFLAGS) -o $@

black_scholes.o : src/black_scholes.c
	@$(CC) $(CFLAGS) -c $^ $(LDFLAGS) -o $@

utils.o : src/utils.c
	@$(CC) $(CFLAGS) -c $^ $(LDFLAGS) -o $@

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
