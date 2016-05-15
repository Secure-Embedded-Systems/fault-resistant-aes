Fault resistant Bitsliced AES
-------------

Bitslicing is a technique to compute steps in an algorithm 1 bit at a time.  Each bit in a processor word would be a part
of a different data stream for that particular algorithm.  It is attractive because then it can run many different streams 
in parallel (depending on the word length).  E.g. a 32 bit word length can compute 32 different streams in parallel.

Because every stream is transposed, it becomes easy to enumerate them and decide which ones can act as redundant or
constant slices for detection of fault injections. 



Current results
-----------------

## For (1) countermeasure

Performance measurements done for a fault resistant AES-CTR on a 64 bit 4 GHz Intel 4790 and compiled with GCC 4.8.4.
Overhead was calculated in reference to the [(unprotected) bitsliced implementation](https://github.com/conorpp/bitsliced-aes).

#### lightweight fault detection (i.e. minimal redundant slices)

|                      |footprint       | overhead | throughput     | overhead |
|----------------------|----------------|----------|----------------|----------|
|Performance optimized |  12,708 bytes  |  4.6%    |54.4 cycles/byte|   6.7%   |
|Footprint optimized   |  9,428 bytes   |   10.6%  | 83 cycles/byte |   2.5%   |


#### Maximum fault detection (i.e. half redundant slices, checking during every AES round)

|                      |footprint       | overhead | throughput      | overhead |
|----------------------|----------------|----------|-----------------|----------|
|Performance optimized |  12,708 bytes  |    4.6%  |111.8 cycles/byte|  119.2%  |
|Footprint optimized   |  9,428 bytes   |    10.6% |166.9 cycles/byte|  106.0%  |

The amount of redundant slices and number of rounds to check are decided during run time.  The idea is
to at first have a small fault detection at first to optimize performance but then increase redundancy
dynamically in the advent of a fault injection.


## For (2) countermeasure

Performance measurements were done on 64 bit intel 4790 but used a 32 bit word length.

|                      |footprint       | overhead | throughput     | overhead |
|----------------------|----------------|----------|----------------|----------|
|Performance optimized |  19,189 bytes  |  57.9%    | 223 cycles/byte |   218.5%   |
|Footprint optimized   |  8,157 bytes   |   -4.3% <sup id="a1">[1](#f1)</sup>  | 373 cycles/byte |   432.9%   |

<b id="f1">1</b> Footprint is actually smaller because the transpose is simplified [↩](#a1)


Performance could be improved by about 5-10x by writing in assembly and ensuring
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


