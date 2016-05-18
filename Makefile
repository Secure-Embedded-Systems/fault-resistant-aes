src = $(wildcard *.c)
obj = $(src:.c=.o)

LDFLAGS = -Wl,--gc-sections
#CFLAGS = -O3 -fdata-sections -ffunction-sections -DUNROLL_TRANSPOSE
CFLAGS = -O2 -fdata-sections -ffunction-sections  #-DUNROLL_TRANSPOSE


#CC=gcc
CC=sparc-elf-gcc

name = bitslice

$(name):  _testbench $(obj)
	$(CC) $(LDFLAGS) -o $@ $(obj) $(LDFLAGS)
	sparc-elf-objdump -D $@ > $@.lst


test: _test $(obj)
	$(CC) $(LDFLAGS) -o $(name) $(obj) $(LDFLAGS)

footprint: _footprint $(obj)
	$(CC) $(LDFLAGS) -o $(name) $(obj) $(LDFLAGS)


fault: _fault $(obj)
	$(CC) $(LDFLAGS) -o $(name) $(obj) $(LDFLAGS)
	sparc-elf-objdump -D $(name) > $(name).lst



_test: tests/tests.c
	$(eval obj+=$@.o)
	$(eval CFLAGS+= -DRUN_TESTS=1)
	$(CC) -c $(CFLAGS) -o $@.o $^

_footprint: tests/tests.c
	$(eval obj+=$@.o)
	$(eval CFLAGS+= -DRUN_TESTS=1 -DTEST_FOOTPRINT=1)
	$(CC) -c $(CFLAGS) -o $@.o $^

_testbench: testbench/app.c
	$(eval obj+=_testbench.o)
	$(CC) -c $(CFLAGS) -o $@.o $^

_fault: testbench/leon-app.c
	$(eval obj+=_fault.o)
	$(CC) -c $(CFLAGS) -o $@.o $^



clean:
	rm -f $(obj) _test.o _footprint.o _testbench.o _fault.o $(name)
