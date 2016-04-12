src = $(wildcard *.c)
obj = $(src:.c=.o)

LDFLAGS = 
CFLAGS = -O3

name = bitslice

$(name): $(obj)
	$(CC) -Wall -Werror -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(obj) $(name)
