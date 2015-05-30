## Introduction ##

Our paper [Anatomy of High-Performance Many-Threaded Matrix Multiplication](http://www.cs.utexas.edu/users/flame/pubs/blis3_ipdps14.pdf), presented at IPDPS'14, identified 5 loops around the micro-kernel as opportunities for parallelization. Within BLIS, we have enabled parallelism for 4 of those loops and have extended it to the rest of the level-3 operations except for `trsm`.

## Enabling multithreading ##

Note that BLIS disables multithreading by default.

As of this writing, BLIS optionally supports multithreading via either OpenMP or POSIX threads.

To enable multithreading via OpenMP, place the following line in your [bli\_config.h](ConfigurationHowTo#bli_config.h.md):
```
#define BLIS_ENABLE_OPENMP
```
Likewise, to enable multithreading via POSIX threads (pthreads), define the following instead:
```
#define BLIS_ENABLE_PTHREADS
```


## Specifying multithreading ##

The below chart describes those loops. Right now the only way to control the amount of parallelism is with environment variables, but we hope to add a nice threading API in the future. The total number of threads is equal to the product of the number of threads used for each loop.

Note: Parallelization of the 4th loop is not currently enabled because each iteration of the loop updates the same part of the matrix C. Thus to parallelize it requires either a reduction or mutex locks when updating C.

| Loop around micro-kernel | Environment variable | Direction | Notes       |
|:-------------------------|:---------------------|:----------|:------------|
| 1st loop                 | `BLIS_IR_NT`         | M         |             |
| 2nd loop                 | `BLIS_JR_NT`         | N         |             |
| 3rd loop                 | `BLIS_IC_NT`         | M         |             |
| 4th loop                 | _N/A_                | K         | Not enabled |
| 5th loop                 | `BLIS_JC_NT`         | N         |             |

Parallelization in BLIS is hierarchical. So if we parallelize multiple loops, the total number of threads will be the product of the amount of parallelism for each loop. Thus the total number of threads used is `BLIS_IR_NT * BLIS_JR_NT * BLIS_IC_NT * BLIS_JC_NT`.

In general, the way to choose how to set these environment variables is as follows:
The amount of parallelism from the M and N dimensions should be roughly the same.
Thus `BLIS_IR_NT * BLIS_IC_NT` should be roughly equal to `BLIS_JR_NT * BLIS_JC_NT`.

Next, which combinations of loops to parallelize depends on which caches are shared.
Think about parallelizing the `JC` loop if threads have their own sockets or L3 caches.
Think about parallelizing the `IC` loop if threads each have their own L2 cache.
Think about parallelizing the `JR` and `IR` loops if threads share an L2 cache.

## BLIS memory manager and multithreading ##

BLIS uses static buffers to hold packed blocks of A and B. Because the number of packed blocks of A and B changes based on how multithreading is achieved, the maximum number of MC by KC blocks and the maximum number of NC by KC blocks must be specified in bli\_config.h.

The following two macros are defined by default:
```
#define BLIS_NUM_MC_X_KC_BLOCKS          BLIS_MAX_NUM_THREADS
#define BLIS_NUM_KC_X_NC_BLOCKS          1
```

In order to not run out of space for these packed blocks in the BLIS memory manager:
  * BLIS\_NUM\_MC\_X\_KC\_BLOCKS must be at least BLIS\_IC\_NT `*` BLIS\_JC\_NT.
  * BLIS\_NUM\_KC\_X\_NC\_BLOCKS must be at least BLIS\_JC\_NT.

If BLIS is called from a multithreaded context (e.g. multiple OpenMP threads each calling dgemm),
then the values of those macros must be multiplied by the number of threads calling BLIS functions.