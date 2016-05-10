Fault resistant Bitsliced AES
-------------

Bitslicing is a technique to compute steps in an algorithm 1 bit at a time.  Each bit in a processor word would be a part
of a different data stream for that particular algorithm.  It is attractive because then it can run many different streams 
in parallel (depending on the word length).  E.g. a 32 bit word length can compute 32 different streams in parallel.

Because every stream is transposed, it becomes easy to enumerate them and decide which ones can act as redundant or
constant slices for detection of fault injections. 



Current results
-----------------


Performance measurements done for a fault resistant AES-CTR on a 64 bit 4 GHz Intel 4790 and compiled with GCC 4.8.4.
Overhead was calculated in reference to the [(unprotected) bitsliced implementation](https://github.com/conorpp/bitsliced-aes).

#### No fault detection (i.e. minimal redundant slices)

|                      |footprint       | overhead | throughput     | overhead |
|----------------------|----------------|----------|----------------|----------|
|Performance optimized |  14,309 bytes  |  17.8%   | 55 cycles/byte |   7.8%   |
|Footprint optimized   |  10,277 bytes  |   20%    | 85 cycles/byte |   4.9%   |


#### Maximum fault detection (i.e. half redundant slices, checking during every AES round)

|                      |footprint       | overhead | throughput      | overhead |
|----------------------|----------------|----------|-----------------|----------|
|Performance optimized |  14,309 bytes  |    17.8% | 116 cycles/byte |  127.5%  |
|Footprint optimized   |  10,277 bytes  |    20%   | 174 cycles/byte |  114.8%  |



The amount of redundant slices and number of rounds to check are decided during run time.  The idea is
to at first have a small fault detection at first to optimize performance but then increase redundancy
dynamically in the advent of a fault injection.



Performance could be improved by about 5-10x by writting in assembly and ensuring
more operations stay in registers rather then spill to memory.


Compiling
----------

Compile the benchmarking program by running:

```bash
make
```

Benchmark program requires OpenSSL.


Compile the test program by running:

```bash
make test
```

Compile it for measuring the AES implementation overhead:

```bash
make footprint
```

Change to the word length of your processor by editing the `WORD_SIZE` macro in bs.h.  Optimize for
footprint by using `-O2` instead of `-O3` in the Makefile and also deleting the `-DUNROLL_TRANSPOSE` 
flag.


