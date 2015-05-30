## Introduction ##

This wiki describes the computational kernels used by the BLIS framework.

One of the primary features of BLIS is that it provides a large set of dense linear algebra functionality while simultaneously minimizing the amount of kernel code that must be optimized for a given architecture. BLIS does this by isolating a handful of kernels which, when implemented, facilitate functionality and performance of several of the higher-level operations.

Presently, BLIS supports several groups of operations:
  * **[Level-1v](BLISAPIQuickReference#Level-1v_operations.md)**: Operations on vectors:
    * [addv](BLISAPIQuickReference#addv.md), [axpyv](BLISAPIQuickReference#axpyv.md), [copyv](BLISAPIQuickReference#copyv.md), [dotv](BLISAPIQuickReference#dotv.md), [dotxv](BLISAPIQuickReference#dotxv.md), [fnormv](BLISAPIQuickReference#fnormv.md), [invertv](BLISAPIQuickReference#invertv.md), [scal2v](BLISAPIQuickReference#scal2v.md), [scalv](BLISAPIQuickReference#scalv.md), [setv](BLISAPIQuickReference#setv.md), [subv](BLISAPIQuickReference#subv.md), [swapv](BLISAPIQuickReference#swapv.md)
  * **[Level-1m](BLISAPIQuickReference#Level-1m_operations.md)**: Element-wise operations on matrices:
    * [addm](BLISAPIQuickReference#addm.md), [axpym](BLISAPIQuickReference#axpym.md), [copym](BLISAPIQuickReference#copym.md), [fnormm](BLISAPIQuickReference#fnormm.md), [scal2m](BLISAPIQuickReference#scal2m.md), [scalm](BLISAPIQuickReference#scalm.md), [setm](BLISAPIQuickReference#setm.md), [subm](BLISAPIQuickReference#subm.md)
  * **[Level-1d](BLISAPIQuickReference#Level-1d_operations.md)**: Element-wise operations on matrix diagonals:
    * [addd](BLISAPIQuickReference#addd.md), [axpyd](BLISAPIQuickReference#axpyd.md), [copyd](BLISAPIQuickReference#copyd.md), [invertd](BLISAPIQuickReference#invertd.md), [scal2d](BLISAPIQuickReference#scal2d.md), [scald](BLISAPIQuickReference#scald.md), [setd](BLISAPIQuickReference#setd.md), [subd](BLISAPIQuickReference#subd.md)
  * **[Level-2](BLISAPIQuickReference#Level-2_operations.md)**: Operations with at least one matrix and one vector operand:
    * [gemv](BLISAPIQuickReference#gemv.md), [ger](BLISAPIQuickReference#ger.md), [hemv](BLISAPIQuickReference#hemv.md), [her](BLISAPIQuickReference#her.md), [her2](BLISAPIQuickReference#her2.md), [symv](BLISAPIQuickReference#symv.md), [syr](BLISAPIQuickReference#syr.md), [syr2](BLISAPIQuickReference#syr2.md), [trmv](BLISAPIQuickReference#trmv.md), [trsv](BLISAPIQuickReference#trsv.md)
  * **[Level-3](BLISAPIQuickReference#Level-3_operations.md)**: Operations with matrices that are multiplication-like:
    * [gemm](BLISAPIQuickReference#gemm.md), [hemm](BLISAPIQuickReference#hemm.md), [herk](BLISAPIQuickReference#herk.md), [her2k](BLISAPIQuickReference#her2k.md), [symm](BLISAPIQuickReference#symm.md), [syrk](BLISAPIQuickReference#syrk.md), [syr2k](BLISAPIQuickReference#syr2k.md), [trmm](BLISAPIQuickReference#trmm.md), [trsm](BLISAPIQuickReference#trsm.md)
  * **[Utility](BLISAPIQuickReference#Utility_operations.md)**: Miscellaneous operations:
    * [abmaxv](BLISAPIQuickReference#abmaxv.md), [absumv](BLISAPIQuickReference#absumv.md), [absumm](BLISAPIQuickReference#absumm.md), [mkherm](BLISAPIQuickReference#mkherm.md), [mksymm](BLISAPIQuickReference#mksymm.md), [mktrim](BLISAPIQuickReference#mktrim.md), [fprintv](BLISAPIQuickReference#fprintv.md), [fprintm](BLISAPIQuickReference#fprintm.md),[printv](BLISAPIQuickReference#printv.md), [printm](BLISAPIQuickReference#printm.md), [randv](BLISAPIQuickReference#randv.md), [randm](BLISAPIQuickReference#randm.md), [sumsqv](BLISAPIQuickReference#sumsqv.md)

Most of the interest with BLAS libraries centers around level-3 operations because they exhibit favorable ratios of floating-point operations (flops) to memory operations (memops), which allows high performance. Some applications also require level-2 computation; however, these operations are at an inherent disadvantage on modern architectures due to their less favorable flop-to-memop ratio. The BLIS framework allows developers to quickly and easily build high performance level-3 operations, as well as relatively well-performing level-2 operations, simply by optimizing a small set of kernels. These kernels, and their relationship to the other higher-level operations supported by BLIS, are the subject of this wiki.

Some level-1v, level-1m, and level-1d operations may also be accelerated, but since they are memory-bound, optimization typically yields minor performance improvement.


---


## BLIS kernels summary ##

This section lists and briefly describes each of the main computational kernels supported by the BLIS framework. (Other kernels are supported, but they are not of interest to most developers.)

### Level-3 ###

BLIS supports the following three level-3 micro-kernels. These micro-kernels are used to implement optimized level-3 operations.
  * **gemm**: The `gemm` micro-kernel performs a small matrix multiplication and is used by every level-3 operation.
  * **trsm**: The `trsm` micro-kernel performs a small triangular solve with multiple right-hand sides. It is not required for optimal performance and in fact is only needed when the developer opts to not implement the fused `gemmtrsm` kernel.
  * **gemmtrsm**: The `gemmtrsm` micro-kernel implements a fused operation whereby a `gemm` and a `trsm` subproblem are fused together in a single routine. This avoids redundant memory operations that would otherwise be incurred if the operations were executed separately.

The following shows the steps one would take to optimize, to varying degrees, the level-3 operations supported by BLIS:
  1. By implementing and optimizing the `gemm` micro-kernel, **all** level-3 operations **except** `trsm` are fully optimized. In this scenario, the `trsm` operation may achieve 60-90% of attainable peak performance, depending on the architecture and problem size.
  1. If one goes further and implements and optimizes the `trsm` micro-kernel, this kernel, when paired with an optimized `gemm` micro-kernel, results in a `trsm` implementation that is accelerated (but not optimized).
  1. Alternatively, if one implements and optimizes the fused `gemmtrsm` micro-kernel, this kernel, when paired with an optimized `gemm` micro-kernel, enables a fully optimized `trsm` implementation.

### Level-1f ###

BLIS supports the following five level-1f (fused) kernels. These kernels are used to implement optimized level-2 operations.
  * **axpy2v**: Performs and fuses two [axpyv](BLISAPIQuickReference#axpyv.md) operations, accumulating to the same output vector.
  * **dotaxpyv**: Performs and fuses a [dotv](BLISAPIQuickReference#dotv.md) followed by an [axpyv](BLISAPIQuickReference#axpyv.md) operation with x.
  * **axpyf**: Performs and fuses some implementation-dependent number of [axpyv](BLISAPIQuickReference#axpyv.md) operations, accumulating to the same output vector. Can also be expressed as a [gemv](BLISAPIQuickReference#gemv.md) operation where matrix A is _m x nf_, where nf is the number of fused operations (fusing factor).
  * **dotxf**: Performs and fuses some implementation-dependent number of [dotxv](BLISAPIQuickReference#dotxv.md) operations, reusing the `y` vector for each [dotxv](BLISAPIQuickReference#dotxv.md).
  * **dotxaxpyf**: Performs and fuses a [dotxf](BLISAPIQuickReference#dotxf.md) and [axpyf](BLISAPIQuickReference#axpyf.md) in which the matrix operand is reused.


### Level-1v ###

BLIS supports kernels for the following level-1 operations. Aside from their self-similar operations (ie: the use of an `axpyv` kernel to implement the `axpyv` operation), these kernels are used only to implement level-2 operations, and only when the developer decides to forgo more optimized approaches that involve level-1f kernels (where applicable).
  * **axpyv**: Performs a [scale-and-accumulate vector](BLISAPIQuickReference#axpyv.md) operation.
  * **dotv**: Performs a [dot product](BLISAPIQuickReference#dotv.md) where the output scalar is overwritten.
  * **dotxv**: Performs an [extended dot product](BLISAPIQuickReference#dotxv.md) operation where the dot product is first scaled and then accumulated into a scaled output scalar.

There are other level-1v kernels that may be optimized, such as [addv](BLISAPIQuickReference#addv.md), [subv](BLISAPIQuickReference#subv.md), and [scalv](BLISAPIQuickReference#scalv.md), but their use is less common and therefore of much less importance to most users and developers.


### Level-1v/-1f Dependencies for Level-2 operations ###

The table below shows dependencies between level-2 operations and each of the level-1v and level-1f kernels.

Kernels marked with a "1" for a given level-2 operation are preferred for optimization because they facilitate an optimal implementation on most architectures. Kernels marked with a "2", "3", or "4" denote those which need to be optimized for alternative implementations that would typically be second, third, or fourth choices, respectively, if the preferred kernels are not optimized.

| operation / kernel | effective storage   | `axpyv` | `dotxv` | `axpy2v` | `dotaxpyv` | `axpyf` | `dotxf` | `dotxaxpyf` |
|:-------------------|:--------------------|:--------|:--------|:---------|:-----------|:--------|:--------|:------------|
| `gemv, trmv, trsv` | row-wise            |         |   2     |          |            |         |   1     |             |
|                    | column-wise         |   2     |         |          |            |   1     |         |             |
| `hemv, symv`       | row- or column-wise |   4     |   4     |          |    3       |   2     |   2     |     1       |
| `ger, her, syr`    | row- or column-wise |   1     |         |          |            |         |         |             |
| `her2, syr2`       | row- or column-wise |   2     |         |    1     |            |         |         |             |

**Note:** The "effective storage" column reflects the orientation of the matrix operand **after** transposition via the corresponding `trans_t` parameter (if applicable). For example, calling `gemv` with a column-stored matrix `A` and the `transa` parameter equal to `BLIS_TRANSPOSE` would be effectively equivalent to row-wise storage.


---


## BLIS kernels reference ##

This section seeks to provide developers with a complete reference for each of the following BLIS kernels, including function prototypes, parameter descriptions, implementation notes, and diagrams:
  * [Level-3 micro-kernels](KernelsHowTo#Level-3_micro-kernels.md)
    * [gemm](KernelsHowTo#gemm_micro-kernel.md)
    * [trsm](KernelsHowTo#trsm_micro-kernels.md)
    * [gemmtrsm](KernelsHowTo#gemmtrsm_micro-kernels.md)
  * [Level-1f kernels](KernelsHowTo#Level-1f_kernels.md)
    * axpy2v
    * dotaxpyv
    * axpyf
    * dotxf
    * dotxaxpyf
  * [Level-1v kernels](KernelsHowTo#Level-1v_kernels.md)
    * axpyv
    * dotv
    * dotxv

The function prototypes in this section follow the same guidelines as those listed in the [BLIS API Quick Reference](BLISAPIQuickReference#Notes_for_using_this_reference.md). Namely:
  * Any occurrence of `?` should be replaced with `s`, `d`, `c`, or `z` to form an actual function name.
  * Any occurrence of `ctype` should be replaced with the actual C type corresponding to the datatype instance in question.
  * Some matrix arguments have associated row and column strides arguments that proceed them, typically listed as `rsX` and `csX` for a given matrix `X`. Row strides are always listed first, and column strides are always listed second. The semantic meaning of a row stride is "the distance, in units of elements, from any given element to the corresponding element (within the same column) of the next row," and the meaning of a column stride is "the distance, in units of elements, from any given element to the corresponding element (within the same row) of the next column." Thus, unit row stride implies column-major storage and unit column stride implies row-major storage.
  * All occurrences of `alpha` and `beta` parameters are scalars.



### Level-3 micro-kernels ###

This section describes in detail the various level-3 micro-kernels supported by BLIS:
  * [gemm](KernelsHowTo#gemm_micro-kernel.md)
  * [trsm](KernelsHowTo#trsm_micro-kernels.md)
  * [gemmtrsm](KernelsHowTo#gemmtrsm_micro-kernels.md)


#### gemm micro-kernel ####

```
void bli_?gemm_<suffix>(  dim_t           k,
                          ctype* restrict alpha,
                          ctype* restrict a1,
                          ctype* restrict b1,
                          ctype* restrict beta,
                          ctype* restrict c11, inc_t rsc, inc_t csc,
                          auxinfo_t*      data );
```

where `<suffix>` is implementation-dependent. The following (more portable) wrapper is also defined:

```
void bli_?gemm_ukernel_void(  dim_t      k,
                              void*      alpha,
                              void*      a1,
                              void*      b1,
                              void*      beta,
                              void*      c11, inc_t rsc, inc_t csc,
                              auxinfo_t* data );
```

The `gemm` micro-kernel, sometimes simply referred to as "the BLIS micro-kernel" or "the micro-kernel", performs the following operation:

```
  C11 := beta * C11 + A1 * B1
```

where `A1` is an _MR x k_ "micro-panel" matrix stored in packed (column-wise) format, `B1` is a _k x NR_ "micro-panel" matrix stored in packed (row-wise) format, `C11` is an _MR x NR_ general matrix stored according to its row and column strides `rsc` and `csc`, and `alpha` and beta are scalars.

_MR_ and _NR_ are the register blocksizes associated with the micro-kernel. They are chosen by the developer when the micro-kernel is written and then encoded into a BLIS configuration, which will reference the micro-kernel when the BLIS framework is instantiated into a library. For more information on setting register blocksizes and related constants, please see the [BLIS developer configuration guide](ConfigurationHowTo.md).

Parameters:

  * `k`:      The number of columns of `A1` and rows of `B1`.
  * `alpha`:  The address of a scalar to the `A1 * B1` product.
  * `a1`:     The address of a micro-panel of matrix `A` of dimension _MR x k_, stored by columns with leading dimension _PACKMR_, where typically _PACKMR_ = _MR_. (See [Implementation Notes for gemm](KernelsHowTo#Implementation_Notes_for_gemm.md) for a discussion of _PACKMR_.)
  * `b1`:     The address of a micro-panel of matrix `B` of dimension _k x NR_, stored by rows with leading dimension _PACKNR_, where typically _PACKNR_ = _NR_. (See [Implementation Notes for gemm](KernelsHowTo#Implementation_Notes_for_gemm.md) for a discussion of _PACKNR_.)
  * `beta`:   The address of a scalar to the input value of matrix `C11`.
  * `c11`:    The address of a matrix `C11` of dimension _MR x NR_, stored according to `rsc` and `csc`.
  * `rsc`:    The row stride of matrix `C11` (ie: the distance to the next row, in units of matrix elements).
  * `csc`:    The column stride of matrix `C11` (ie: the distance to the next column, in units of matrix elements).
  * `data`:   The address of an `auxinfo_t` object that contains auxiliary information that may be useful when optimizing the `gemm` micro-kernel implementation. (See [Using the auxinfo\_t object](KernelsHowTo#Using_the_auxinfo_t_object.md) for a discussion of the kinds of values available via `auxinfo_t`.)

#### Diagram for gemm ####

The diagram below shows the packed micro-panel operands and how elements of each would be stored when _MR_ = _NR_ = 4. The hex digits indicate the layout and order (but NOT the numeric contents) of the elements in memory. Note that the storage of `C11` is not shown since it is determined by the row and column strides of `C11`.

```
         c11:           a1:                        b1:                   
         _______        ______________________     _______              
        |       |      |0 4 8 C               |   |0 1 2 3|             
    MR  |       |      |1 5 9 D . . .         |   |4 5 6 7|             
        |       |  +=  |2 6 A E               |   |8 9 A B|             
        |_______|      |3_7_B_F_______________|   |C D E F|             
                                                  |   .   |             
            NR                    k               |   .   | k           
                                                  |   .   |             
                                                  |       |             
                                                  |       |             
                                                  |_______|             
                                                                        
                                                      NR                
```

#### Implementation Notes for gemm ####

  * **Register blocksizes.** The C preprocessor macros `bli_?mr` and `bli_?nr` evaluate to the _MR_ and _NR_ register blocksizes for the datatype corresponding to the '?' character. These values are abbreviations of the macro constants `BLIS_DEFAULT_MR_?` and `BLIS_DEFAULT_NR_?`, which are defined in the `bli_kernel.h` header file of the BLIS configuration.
  * **Leading dimensions of `a1` and `b1`: _PACKMR_ and _PACKNR_.** The packed micro-panels `a1` and `b1` are simply stored in column-major and row-major order, respectively. Usually, the width of either micro-panel (ie: the number of rows of `A1`, or _MR_, and the number of columns of `B1`, or _NR_) is equal to that micro-panel's so-called "leading dimension." Sometimes, it may be beneficial to specify a leading dimension that is larger than the panel width. This may be desirable because it allows each column of `A1` or row of `B1` to maintain a certain alignment in memory that would not otherwise be maintained by _MR_ and/or _NR_. In this case, you should index through `a1` and `b1` using the values _PACKMR_ and _PACKNR_, respectively, as defined by `bli_?packmr` and `bli_?packnr`. These values are defined as `BLIS_PACKDIM_MR_?` and `BLIS_PACKDIM_NR_?`, respectively, in the `bli_kernel.h` header file of the BLIS configuration.
  * **Storage preference of `c11`.** Sometimes, an optimized `gemm` micro-kernel will have a "preferred" storage format for `C11`--typically either contiguous row-storage (i.e. `cs_c` = 1) or contiguous column-storage (i.e. `rs_c` = 1). This preference comes from how the micro-kernel is most efficiently able to load/store elements of `C11` from/to memory. Most micro-kernels use vector instructions to access contiguous columns (or column segments) of `C11`. However, the developer may decide that accessing contiguous rows (or row segments) is more desirable. If this is the case, this preference should be noted in `bli_kernel.h` by defining the macro `BLIS_?GEMM_UKERNEL_PREFERS_CONTIG_ROWS`. Leaving the macro undefined leaves the default assumption (contiguous column preference) in place. Setting this macro allows the framework to perform a minor optimization at run-time that will ensure the micro-kernel preference is honored, if at all possible.
  * **Edge cases in _MR_, _NR_ dimensions.** Sometimes the micro-kernel will be called with micro-panels `a1` and `b1` that correspond to edge cases, where only partial results are needed. Zero-padding is handled automatically by the packing function to facilitate reuse of the same micro-kernel. Similarly, the logic for computing to temporary storage and then saving only the elements that correspond to elements of `C11` that exist (at the edges) is handled automatically within the macro-kernel.
  * **Alignment of `a1` and `b1`.** By default, the alignment of addresses `a1` and `b1` are aligned only to `sizeof(type)`. If `BLIS_CONTIG_ADDR_ALIGN_SIZE` is set to some larger multiple of `sizeof(type)`, such as the page size, then `a1` and `b1` will be aligned to `PACKMR * sizeof(type)` and `PACKNR * sizeof(type)`, respectively. Alignment of `a1` and `b1` is also affected by `BLIS_UPANEL_A_ALIGN_SIZE_?` and `BLIS_UPANEL_B_ALIGN_SIZE_?`, which align the distance (stride) between subsequent micro-panels. (By default, those values are simply `sizeof(type)`, in which case they have no effect.)
  * **Unrolling loops.** As a general rule of thumb, the loop over _k_ is sometimes moderately unrolled; for example, in our experience, an unrolling factor of _u_ = 4 is fairly common. If unrolling is applied in the _k_ dimension, edge cases must be handled to support values of _k_ that are not multiples of _u_. It is nearly universally true that there should be no loops in the _MR_ or _NR_ directions; in other words, iteration over these dimensions should always be fully unrolled (within the loop over _k_).
  * **Zero `beta`.** If `beta` = 0.0 (or 0.0 + 0.0i for complex datatypes), then the micro-kernel should NOT use it explicitly, as `C11` may contain uninitialized memory (including `NaN`s). This case should be detected and handled separately, preferably by simply overwriting `C11` with the `alpha * A1 * B1` product. An example of how to perform this "beta equals zero" handling is included in the `gemm` micro-kernel associated with the `template` configuration.

#### Using the auxinfo\_t object ####

Each micro-kernel ([gemm](KernelsHowTo#gemm_micro-kernel.md), [trsm](KernelsHowTo#trsm_micro-kernels.md), and [gemmtrsm](KernelsHowTo#gemmtrsm_micro-kernels.md)) takes as its last argument a pointer of type `auxinfo_t`. This BLIS-defined type is defined as a `struct` whose fields contain auxiliary values that may be useful to some micro-kernel authors, particularly when implementing certain optimization techniques. BLIS provides kernel authors access to the fields of the `auxinfo_t` object via the following function-like preprocessor macros. Each macro takes a single argument, the `auxinfo_t` pointer, and returns one of the values stored within the object.

  * `bli_auxinfo_next_a()`. Returns the address (`void*`) of the micro-panel of `A` that will be used the next time the micro-kernel will be called.
  * `bli_auxinfo_next_b()`. Returns the address (`void*`) of the micro-panel of `B` that will be used the next time the micro-kernel will be called.
  * `bli_auxinfo_ps_a()`. Returns the panel stride (`inc_t`) of the current micro-panel of `A`.
  * `bli_auxinfo_ps_b()`. Returns the panel stride (`inc_t`) of the current micro-panel of `B`.

The addresses of the next micro-panels of `A` and `B` may be used by the micro-kernel to perform prefetching, if prefetching is supported by the architecture. Similarly, it may be useful to know the precise distance in memory to the next micro-panel. (Note that sometimes the next micro-panel to be used is **not** the same as the next micro-panel in memory.)

Any and all of these values may be safely ignored; they are completely optional. However, BLIS guarantees that all values accessed via the macros listed above will **always** be initialized and meaningful, for every invocation of each micro-kernel (`gemm`, `trsm`, and `gemmtrsm`).


#### Example code for gemm ####

An example implementation of the `gemm` micro-kernel may be found in the `template` configuration directory in:

  * [config/template/kernels/3/bli\_gemm\_opt\_mxn.c](http://code.google.com/p/blis/source/browse/config/template/kernels/3/bli_gemm_opt_mxn.c).

Note that this implementation is coded in C99 and lacks several kinds of optimization that are typical of real-world optimized micro-kernels, such as vector instructions (or intrinsics) and loop unrolling in _MR_ or _NR_. It is meant to serve only as a starting point for a micro-kernel developer.




---


#### trsm micro-kernels ####

```
void bli_?trsm_l_<suffix>( ctype* restrict a11,
                           ctype* restrict b11,
                           ctype* restrict c11, inc_t rsc, inc_t csc,
                           auxinfo_t*      data );

void bli_?trsm_u_<suffix>( ctype* restrict a11,
                           ctype* restrict b11,
                           ctype* restrict c11, inc_t rsc, inc_t csc,
                           auxinfo_t*      data );
```

where `<suffix>` is implementation-dependent. The following (more portable) wrappers are also defined:

```
void bli_?trsm_l_ukernel_void( void*      a11,
                               void*      b11,
                               void*      c11, inc_t rsc, inc_t csc,
                               auxinfo_t* data );

void bli_?trsm_u_ukernel_void( void*      a11,
                               void*      b11,
                               void*      c11, inc_t rsc, inc_t csc,
                               auxinfo_t* data );
```

The `trsm_l` and `trsm_u` micro-kernels perform the following operation:

```
  C11 := inv(A11) * B11
```

where `A11` is _MR x MR_ and lower (`trsm_l`) or upper (`trsm_u`) triangular, `B11` is _MR x NR_, and `C11` is _MR x NR_.

_MR_ and _NR_ are the register blocksizes associated with the micro-kernel. They are chosen by the developer when the micro-kernel is written and then encoded into a BLIS configuration, which will reference the micro-kernel when the BLIS framework is instantiated into a library. For more information on setting register blocksizes and related constants, please see the [BLIS developer configuration guide](ConfigurationHowTo.md).

Parameters:

  * `a11`:    The address of `A11`, which is the _MR x MR_ lower (`trsm_l`) or upper (`trsm_u`) triangular submatrix within the packed micro-panel of matrix `A`. `A11` is stored by columns with leading dimension _PACKMR_, where typically _PACKMR_ = _MR_. (See [Implementation Notes for gemm](KernelsHowTo#Implementation_Notes_for_gemm.md) for a discussion of _PACKMR_.) Note that `A11` contains elements in both triangles, though elements in the unstored triangle are not guaranteed to be zero and thus should not be referenced.
  * `b11`:    The address of `B11`, which is an _MR x NR_ submatrix of the packed micro-panel of `B`. `B11` is stored by rows with leading dimension _PACKNR_, where typically _PACKNR_ = _NR_. (See [Implementation Notes for gemm](KernelsHowTo#Implementation_Notes_for_gemm.md) for a discussion of _PACKNR_.)
  * `c11`:    The address of `C11`, which is an _MR x NR_ submatrix of matrix `C`, stored according to `rsc` and `csc`. `C11` is the submatrix within `C` that corresponds to the elements which were packed into `B11`. Thus, `C` is the original input matrix `B` to the overall `trsm` operation.
  * `rsc`:    The row stride of matrix `C11` (ie: the distance to the next row, in units of matrix elements).
  * `csc`:    The column stride of matrix `C11` (ie: the distance to the next column, in units of matrix elements).
  * `data`:   The address of an `auxinfo_t` object that contains auxiliary information that may be useful when optimizing the `trsm` micro-kernel implementation. (See [Using the auxinfo\_t object](KernelsHowTo#Using_the_auxinfo_t_object.md) for a discussion of the kinds of values available via `auxinfo_t`, and also [Implementation Notes for trsm](KernelsHowTo#Implementation_Notes_for_trsm.md) for caveats.)

#### Diagrams for trsm ####

Please see the diagram for [gemmtrsm\_l](KernelsHowTo#Diagram_for_gemmtrsm_l.md) and [gemmtrsm\_u](KernelsHowTo#Diagram_for_gemmtrsm_u.md) to see depictions of the `trsm_l` and `trsm_u` micro-kernel operations and where they fit in with their preceding `gemm` subproblems.

#### Implementation Notes for trsm ####

  * **Register blocksizes.** See [Implementation Notes for gemm](KernelsHowTo#Implementation_Notes_for_gemm.md).
  * **Leading dimensions of `a11` and `b11`: _PACKMR_ and _PACKNR_.** See [Implementation Notes for gemm](KernelsHowTo#Implementation_Notes_for_gemm.md).
  * **Edge cases in _MR_, _NR_ dimensions.** See [Implementation Notes for gemm](KernelsHowTo#Implementation_Notes_for_gemm.md).
  * **Alignment of `a11` and `b11`.** The addresses `a11` and `b11` are aligned according to `PACKMR * sizeof(type)` and `PACKNR * sizeof(type)`, respectively.
  * **Unrolling loops.** Most optimized implementations should unroll all three loops within the `trsm` micro-kernel.
  * **Prefetching next micro-panels of `A` and `B`.** We advise against using the `bli_auxinfo_next_a()` and `bli_auxinfo_next_b()` macros from within the `trsm_l` and `trsm_u` micro-kernels, since the values returned usually only make sense in the context of the overall `gemmtrsm` subproblem.
  * **Diagonal elements of `A11`.** At the time this micro-kernel is called, the diagonal entries of triangular matrix `A11` contain the **_inverse_** of the original elements. This inversion is done during packing so that we can avoid expensive division instructions within the micro-kernel itself. If the `diag` parameter to the higher level `trsm` operation was equal to `BLIS_UNIT_DIAG`, the diagonal elements will be explicitly unit.
  * **Zero elements of `A11`.** Since `A11` is lower triangular (for `trsm_l`), the strictly upper triangle implicitly contains zeros. Similarly, the strictly lower triangle of `A11` implicitly contains zeros when `A11` is upper triangular (for `trsm_u`). However, the packing function may or may not actually write zeros to this region. Thus, the implementation should not reference these elements.
  * **Output.** This micro-kernel must write its result to two places: the submatrix `B11` of the current packed micro-panel of `B` _and_ the submatrix `C11` of the output matrix `C`.

#### Example code for trsm ####

Example implementations of the `trsm` micro-kernels may be found in the `template` configuration directory in:
  * [config/template/kernels/3/bli\_trsm\_l\_opt\_mxn.c](http://code.google.com/p/blis/source/browse/config/template/kernels/3/bli_trsm_l_opt_mxn.c)
  * [config/template/kernels/3/bli\_trsm\_u\_opt\_mxn.c](http://code.google.com/p/blis/source/browse/config/template/kernels/3/bli_trsm_u_opt_mxn.c)

Note that these implementations are coded in C99 and lack several kinds of optimization that are typical of real-world optimized micro-kernels, such as vector instructions (or intrinsics) and loop unrolling in _MR_ or _NR_. They are meant to serve only as a starting point for a micro-kernel developer.


---


#### gemmtrsm micro-kernels ####

```
void bli_?gemmtrsm_l_<suffix>( dim_t           k,
                               ctype* restrict alpha,
                               ctype* restrict a10,
                               ctype* restrict a11,
                               ctype* restrict b01,
                               ctype* restrict b11,
                               ctype* restrict c11, inc_t rsc, inc_t csc,
                               auxinfo_t*      data );

void bli_?gemmtrsm_u_<suffix>( dim_t           k,
                               ctype* restrict alpha,
                               ctype* restrict a12,
                               ctype* restrict a11,
                               ctype* restrict b21,
                               ctype* restrict b11,
                               ctype* restrict c11, inc_t rsc, inc_t csc,
                               auxinfo_t*      data );
```

where `<suffix>` is implementation-dependent. The following (more portable) wrappers are also defined:

```
void bli_?gemmtrsm_l_ukernel_void( dim_t      k,
                                   void*      alpha,
                                   void*      a10,
                                   void*      a11,
                                   void*      b01,
                                   void*      b11,
                                   void*      c11, inc_t rsc, inc_t csc,
                                   auxinfo_t* data );

void bli_?gemmtrsm_u_ukernel_void( dim_t      k,
                                   void*      alpha,
                                   void*      a12,
                                   void*      a11,
                                   void*      b21,
                                   void*      b11,
                                   void*      c11, inc_t rsc, inc_t csc,
                                   auxinfo_t* data );
```

The `gemmtrsm_l` micro-kernel performs the following compound operation:

```
  B11 := alpha * B11 - A10 * B01
  B11 := inv(A11) * B11
  C11 := B11
```

where `A11` is _MR_ x _MR_ and lower triangular, `A10` is _MR_ x _k_, and `B01` is _k_ x _NR_.
The `gemmtrsm_u` micro-kernel performs:

```
  B11 := alpha * B11 - A12 * B21
  B11 := inv(A11) * B11
  C11 := B11
```

where `A11` is _MR_ x _MR_ and upper triangular, `A12` is _MR_ x _k_, and `B21` is _k_ x _NR_.
In both cases, `B11` is _MR_ x _NR_ and `alpha` is a scalar. Here, `inv()` denotes matrix inverse.

_MR_ and _NR_ are the register blocksizes associated with the micro-kernel. They are chosen by the developer when the micro-kernel is written and then encoded into a BLIS configuration, which will reference the micro-kernel when the BLIS framework is instantiated into a library. For more information on setting register blocksizes and related constants, please see the [BLIS developer configuration guide](ConfigurationHowTo.md).

Parameters:

  * `k`:      The number of columns of `A10` and rows of `B01` (`trsm_l`); the number of columns of `A12` and rows of `B21` (`trsm_u`).
  * `alpha`:  The address of a scalar to be applied to `B11`.
  * `a10`, `a12`:    The address of `A10` or `A12`, which is the _MR x k_ submatrix of the packed micro-panel of `A` that is situated to the left (`trsm_l`) or right (`trsm_u`) of the _MR x MR_ triangular submatrix `A11`. `A10` and `A12` are stored by columns with leading dimension _PACKMR_, where typically _PACKMR_ = _MR_. (See [Implementation Notes for gemm](KernelsHowTo#Implementation_Notes_for_gemm.md) for a discussion of _PACKMR_.)
  * `a11`:    The address of `A11`, which is the _MR x MR_ lower (`trsm_l`) or upper (`trsm_u`) triangular submatrix within the packed micro-panel of matrix `A` that is situated to the right of `A10` (`trsm_l`) or the left of `A12` (`trsm_u`). `A11` is stored by columns with leading dimension _PACKMR_, where typically _PACKMR_ = _MR_. (See [Implementation Notes for gemm](KernelsHowTo#Implementation_Notes_for_gemm.md) for a discussion of _PACKMR_.) Note that `A11` contains elements in both triangles, though elements in the unstored triangle are not guaranteed to be zero and thus should not be referenced.
  * `b01`, `b21`:   The address of `B01` and `B21`, which is the _k x NR_ submatrix of the packed micro-panel of `B` that is situated above (`trsm_l`) or below (`trsm_u`) the _MR x NR_ block `B11`. `B01` and `B21` are stored by rows with leading dimension _PACKNR_, where typically _PACKNR_ = _NR_. (See [Implementation Notes for gemm](KernelsHowTo#Implementation_Notes_for_gemm.md) for a discussion of _PACKNR_.)
  * `b11`:    The address of `B11`, which is the _MR x NR_ submatrix of the packed micro-panel of `B`, situated below `B01` (`trsm_l`) or above `B21` (`trsm_u`). `B11` is stored by rows with leading dimension _PACKNR_, where typically _PACKNR_ = _NR_. (See [Implementation Notes for gemm](KernelsHowTo#Implementation_Notes_for_gemm.md) for a discussion of _PACKNR_.)
  * `c11`:    The address of `C11`, which is an _MR x NR_ submatrix of matrix `C`, stored according to `rsc` and `csc`. `C11` is the submatrix within `C` that corresponds to the elements which were packed into `B11`. Thus, `C` is the original input matrix `B` to the overall `trsm` operation.
  * `rsc`:    The row stride of matrix `C11` (ie: the distance to the next row, in units of matrix elements).
  * `csc`:    The column stride of matrix `C11` (ie: the distance to the next column, in units of matrix elements).
  * `data`:   The address of an `auxinfo_t` object that contains auxiliary information that may be useful when optimizing the `gemmtrsm` micro-kernel implementation. (See [Using the auxinfo\_t object](KernelsHowTo#Using_the_auxinfo_t_object.md) for a discussion of the kinds of values available via `auxinfo_t`, and also [Implementation Notes for gemmtrsm](KernelsHowTo#Implementation_Notes_for_gemmtrsm.md) for caveats.)

#### Diagram for gemmtrsm\_l ####

The diagram below shows the packed micro-panel operands for `trsm_l` and how elements of each would be stored when _MR_ = _NR_ = 4. (The hex digits indicate the layout and order (but NOT the numeric contents) in memory. Here, matrix `A11` (referenced by `a11`) is **lower triangular**. Matrix `A11` **does contain** elements corresponding to the strictly upper triangle, however, they are not guaranteed to contain zeros and thus these elements should not be referenced.

```
                                              NR    
                                            _______ 
                                       b01:|0 1 2 3|
                                           |4 5 6 7|
                                           |8 9 A B|
                                           |C D E F|
                                         k |   .   |
                                           |   .   |
       a10:                a11:            |   .   |
       ___________________  _______        |_______|
      |0 4 8 C            |`.      |   b11:|       |
  MR  |1 5 9 D . . .      |  `.    |       |       |
      |2 6 A E            |    `.  |    MR |       |
      |3_7_B_F____________|______`.|       |_______|
                                                    
                k             MR                    
```


#### Diagram for gemmtrsm\_u ####

The diagram below shows the packed micro-panel operands for `trsm_u` and how elements of each would be stored when _MR_ = _NR_ = 4. (The hex digits indicate the layout and order (but NOT the numeric contents) in memory. Here, matrix `A11` (referenced by `a11`) is **upper triangular**. Matrix `A11` **does contain** elements corresponding to the strictly lower triangle, however, they are not guaranteed to contain zeros and thus these elements should not be referenced.

```
       a11:     a12:                          NR    
       ________ ___________________         _______ 
      |`.      |0 4 8              |   b11:|0 1 2 3|
  MR  |  `.    |1 5 9 . . .        |       |4 5 6 7|
      |    `.  |2 6 A              |    MR |8 9 A B|
      |______`.|3_7_B______________|       |___.___|
                                       b21:|   .   |
          MR             k                 |   .   |
                                           |       |
                                           |       |
     NOTE: Storage digits are shown      k |       |
     starting with a12 to avoid            |       |
     obscuring triangular structure        |       |
     of a11.                               |_______|
                                                                            
```


#### Implementation Notes for gemmtrsm ####

  * **Register blocksizes.** See [Implementation Notes for gemm](KernelsHowTo#Implementation_Notes_for_gemm.md).
  * **Leading dimensions of `a1` and `b1`: _PACKMR_ and _PACKNR_.** See [Implementation Notes for gemm](KernelsHowTo#Implementation_Notes_for_gemm.md).
  * **Edge cases in _MR_, _NR_ dimensions.** See [Implementation Notes for gemm](KernelsHowTo#Implementation_Notes_for_gemm.md).
  * **Alignment of `a1` and `b1`.** See [Implementation Notes for gemm](KernelsHowTo#Implementation_Notes_for_gemm.md).
  * **Unrolling loops.** Most optimized implementations should unroll all three loops within the `trsm` subproblem of `gemmtrsm`. See [Implementation Notes for gemm](KernelsHowTo#Implementation_Notes_for_gemm.md) for remarks on unrolling the `gemm` subproblem.
  * **Prefetching next micro-panels of `A` and `B`.** When invoked from within a `gemmtrsm_l` micro-kernel, the addresses accessible via `bli_auxinfo_next_a()` and `bli_auxinfo_next_b()` refer to the next invocation's `a10` and `b01`, respectively, while in `gemmtrsm_u`, the `_next_a()` and `_next_b()` macros return the addresses of the next invocation's `a11` and `b11` (since those submatrices precede `a12` and `b21`).
  * **Zero `alpha`.** The micro-kernel can safely assume that `alpha` is non-zero; "alpha equals zero" handling is performed at a much higher level, which means that, in such a scenario, the micro-kernel will never get called.
  * **Diagonal elements of `A11`.** See [Implementation Notes for trsm](KernelsHowTo#Implementation_Notes_for_trsm.md).
  * **Zero elements of `A11`.** See [Implementation Notes for trsm](KernelsHowTo#Implementation_Notes_for_trsm.md).
  * **Output.** See [Implementation Notes for trsm](KernelsHowTo#Implementation_Notes_for_trsm.md).
  * **Optimization.** Let's assume that the [gemm micro-kernel](KernelsHowTo#gemm_micro-kernel.md) has already been optimized. You have two options with regard to optimizing the fused `gemmtrsm` micro-kernels:
    1. Optimize only the [trsm micro-kernels](KernelsHowTo#trsm_micro-kernels.md). This will result in the `gemm` and `trsm_l` micro-kernels being called in sequence. (Likewise for `gemm` and `trsm_u`.)
    1. Fuse the implementation of the `gemm` micro-kernel with that of the `trsm` micro-kernels by inlining both into the `gemmtrsm_l` and `gemmtrsm_u` micro-kernel definitions. This option is more labor-intensive, but also more likely to yield higher performance because it avoids redundant memory operations on the packed _MR x NR_ submatrix `B11`.


#### Example code for gemmtrsm ####

Example implementations of the `gemmtrsm` micro-kernels may be found in the `template` configuration directory in:
  * [config/template/kernels/3/bli\_gemmtrsm\_l\_opt\_mxn.c](http://code.google.com/p/blis/source/browse/config/template/kernels/3/bli_gemmtrsm_l_opt_mxn.c)
  * [config/template/kernels/3/bli\_gemmtrsm\_u\_opt\_mxn.c](http://code.google.com/p/blis/source/browse/config/template/kernels/3/bli_gemmtrsm_u_opt_mxn.c)

Note that these implementations are coded in C99 and lack several kinds of optimization that are typical of real-world optimized micro-kernels, such as vector instructions (or intrinsics) and loop unrolling in _MR_ or _NR_. They are meant to serve only as a starting point for a micro-kernel developer.




### Level-1f kernels ###

_This section has yet to be written._

### Level-1v kernels ###

_This section has yet to be written._