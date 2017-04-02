CC=gcc
CFLAGS=-Wall -fPIC -O3 -ansi -pedantic-errors
LDFLAGS=-lgsl -lcblas -lm

pricer : src/optpricer.c gbm.o black_scholes.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

gbm.o : src/gbm_mc.c
	$(CC) $(CFLAGS) -c $^ $(LDFLAGS) -o $@

black_scholes.o : src/black_scholes.c
	$(CC) $(CFLAGS) -c $^ $(LDFLAGS) -o $@

.PHONY: clean
clean:
	rm -f gbm.o black_scholes.o
