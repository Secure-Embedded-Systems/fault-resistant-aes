src = $(wildcard *.c)
src += $(wildcard ../bs.c)
obj = $(src:.c=.o)

LDFLAGS = 
CFLAGS = -O3 -I..

name = test

$(name): $(obj)
	$(CC) -Wall -Werror -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(obj) $(name)
