src = $(wildcard *.c)
obj = $(src:.c=.o)

LDFLAGS = -Wl,--gc-sections -lcrypto
CFLAGS = -O3 -fdata-sections -ffunction-sections -DUNROLL_TRANSPOSE

name = bitslice

$(name): $(obj)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDFLAGS)

test: _test $(obj)
	$(CC) $(LDFLAGS) -o $(name) $(obj) $(LDFLAGS)

footprint: _footprint $(obj)
	$(CC) $(LDFLAGS) -o $(name) $(obj) $(LDFLAGS)


_test: tests/tests.c
	$(eval obj+=$@.o)
	$(eval CFLAGS+= -DRUN_TESTS=1)
	$(CC) -c $(CFLAGS) -o $@.o $^

_footprint: tests/tests.c
	$(eval obj+=$@.o)
	$(eval CFLAGS+= -DRUN_TESTS=1 -DTEST_FOOTPRINT=1)
	$(CC) -c $(CFLAGS) -o $@.o $^


clean:
	rm -f $(obj) _tests.o $(name)
