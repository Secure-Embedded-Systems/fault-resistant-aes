src = $(wildcard *.c)
obj = $(src:.c=.o)

LDFLAGS = 

name = bitslice

$(name): $(obj)
	$(CC) -O3 -Wall -Werror -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(obj) $(name)
