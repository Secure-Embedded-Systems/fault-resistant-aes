src = $(wildcard *.c)
obj = $(src:.c=.o)

LDFLAGS = -Wl,--gc-sections
CFLAGS = -O2 -fdata-sections -ffunction-sections

name = bitslice

$(name): $(obj)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDFLAGS)

test: _test $(obj)
	$(CC) $(LDFLAGS) -o $(name) $(obj) $(LDFLAGS)


_test: tests/tests.c
	$(eval obj+=$@.o)
	$(eval CFLAGS+= -DRUN_TESTS=1)
	$(CC) -c $(CFLAGS) -o $@.o $^

clean:
	rm -f $(obj) _tests.o $(name)
