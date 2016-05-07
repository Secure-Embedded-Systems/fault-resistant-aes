src = $(wildcard *.c)
obj = $(src:.c=.o)

LDFLAGS =
CFLAGS = -O2

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
