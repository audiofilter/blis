## Introduction ##

This wiki describes how to create, edit, and manage BLIS framework configurations. The target audience is primarily BLIS developers who write (or tinker with) BLIS kernels.

The [wiki](BuildSystem.md) for the BLIS build system introduces the concept of a BLIS [configuration](BuildSystem#Step_1:_Choose_a_framework_configuration.md). To reiterate, a BLIS configuration (or, "a configuration" for short) generally consists of:

  * two key header files with important definitions: `bli_config.h` and `bli_kernel.h`
  * a file containing makefile definitions, such as your choice of compiler and compiler flags: `make_defs.mk`
  * optimized kernels, usually specified via a symbolic link to a subdirectory of the top-level `kernels` directory, though the kernel source may, if desired, exist in an actual subdirectory within the configuration directory.

The configuration files live inside a subdirectory of the top-level `config` directory, where the subdirectory is named after the configuration. Here are the current (as of this writing) contents of the the BLIS framework's `config` directory:

```
  $ cd config
  $ ls
  bgq         loongson3a  piledriver  reference    template
  dunnington  mic         power7      sandybridge
```

Different configurations were authored by different developers. To look up who contributed each BLIS configuration (and/or its associated kernels) please see the `CREDITS` file in the top-level directory. You may contact these and other developers via the [blis-devel](http://groups.google.com/group/blis-devel) mailing list. Although you may also contact these individuals directly if you have their contact info, we encourage you to use the mailing list to foster discussion and keep everyone in the loop.

## Creating a framework configuration ##

Creating a basic BLIS configuration is easy: simply make a copy of an existing configuration. Which directory you make a copy of depends on what kind of configuration you want to create:
  * If you want to tweak or experiment with an existing optimized configuration (for example, the `sandybridge` configuration), start with a copy of that configuration.
  * If you want to create a new configuration, we recommend starting with a copy of the `template` configuration.

For the remainder of this document, we will assume you are creating a new configuration, which will allow us the opportunity to explain all aspects of the configuration:

```
  $ cp -r template mynewconfig
  $ ls
  bgq         loongson3a  mynewconfig  power7     sandybridge
  dunnington  mic         piledriver   reference  template
```

So, we now have a new configuration directory, `mynewconfig`. Let's inspect the contents:

```
  $ cd mynewconfig
  $ ls
  bli_config.h  bli_kernel.h  kernels  make_defs.mk
```

### `template` configuration and kernels ###

Before we continue, let us briefly discuss the `template` configuration. The `template` configuration is similar to the `reference` configuration in that it contains basic default settings that should work on most systems. However, unlike the `reference` configuration, the `template` configuration contains a subdirectory of ready-to-edit kernel implementations (located in `kernels`, shown above), written portably in C99. For each kernel operation, one datatype flavor (usually double-precision real or complex) is explicitly implemented to serve as a basic example of how to write the kernel in question. (The other datatype flavors simply call the corresponding reference kernel.) Also, for your convenience, each of the chosen explicitly coded kernel functions contains extensive in-code comments and documentation. However, for the most up-to-date documentation, please refer to the [BLIS Kernels guide](KernelsHowTo.md).

The idea now is to edit each of the three files shown above (`bli_config.h`, `bli_kernel.h`, and `make_defs.mk`) so that they encode the desired characteristics of our BLIS library.

## Configuration components ##

We will not walk through each of the three configuration files to discuss its contents and role in defining the BLIS configuration.

### `bli_config.h` ###

This file is where you, the library developer, specify various C preprocessor macros that encode general parameters of your BLIS configuration, including (but not limited to) properties of the memory allocator. The file itself contains comments that describe each configurable option. We will describe some of the more important components here.

> _**BLIS integer properties.**_ BLIS uses signed integers to facilitate better interoperability with Fortran. However, the developer can specify the size of its integers:
```
    #define BLIS_INT_TYPE_SIZE               32
```
> Values of 32 and 64 force integers to be 32 or 64 bits in length. Any other value results in use of the C language type `signed long int`. **Note:** This does _not_ affect the size of integers in the BLAS compatibility layer interfaces, which is set by a separate value, `BLIS_BLAS2BLIS_INT_TYPE_SIZE`, which is described later on in this section.

> _**BLIS floating-point properties.**_ Most developers want to provide functionality for all four IEEE floating-point types---single-precision real, double-precision real, single-precision complex, and double-precision complex---and thus should leave the following settings unmodified:
```
    #define BLIS_NUM_FP_TYPES                4
    #define BLIS_MAX_TYPE_SIZE               sizeof(dcomplex)
```
> Even if you wish to only optimize one or two of these four datatypes, you should probably configure for all four types unless you have a really good reason (Hint: I haven't tested BLIS with anything but the above settings.)

> Some users may wish to utilize the complex types and arithmetic defined by the C99 support in their preferred compiler. If so, make sure `BLIS_ENABLE_C99_COMPLEX` is uncommented:
```
     #define BLIS_ENABLE_C99_COMPLEX
```
> Otherwise, leaving it commented will cause BLIS to implement its complex arithmetic in terms of real arithmetic on the fields of `struct`s, which are `typedef`'ed to the abstract BLIS complex types `scomplex` and `dcomplex`.

> _**Multithreading.**_ BLIS presently only supports internal multithreading via OpenMP. (POSIX threads support is planned for the future.) In order to enable multithreading **within** BLIS, make sure you define the following two macro constants:
```
     #define BLIS_ENABLE_MULTITHREADING
     #define BLIS_ENABLE_OPENMP
```
> Also, you must set `BLIS_MAX_NUM_THREADS` to the maximum number of threads you will be using. For example:
```
     #define BLIS_MAX_NUM_THREADS 16
```
> This is needed so that the memory allocator is initialized with enough memory for packing buffers.
> If you do **not** want to build BLIS with multithreading enabled, you may still need BLIS to be thread-safe (perhaps because you are implementing threading within your application). If so, please be aware of the following:
    1. The critical section (marked with comments) should be made into a critical section using your preferred threading environment. For example, if you are using OpenMP, enclose the code with a `#pragma omp critical` block.
    1. Set `BLIS_MAX_NUM_THREADS` to the maximum number of internal BLIS threads you will be instantiating (if experimenting with multithreading within BLIS) or the maximum number of application-level threads that will be calling BLIS. Also, make sure that `BLIS_NUM_MC_X_KC_BLOCKS`, `BLIS_NUM_KC_X_NC_BLOCKS`, and `BLIS_NUM_MC_X_NC_BLOCKS` (which are described below) are all defined to be `BLIS_MAX_NUM_THREADS`.
> For more information on controlling multithreading within BLIS, please see the multithreading [wiki](Multithreading.md).

> _**Memory allocator.**_ In BLIS, we go out of our way to ensure that internal "packing" buffers are physically contiguous in memory. The only portable way to achieve this is to allocate memory statically and then manage this memory at run-time. We call this subsystem the _contiguous memory allocator_, or, the memory allocator. The memory allocator maintains three separate "pools" of blocks that it manages, one for blocks of size _MC x KC_, one for blocks of size _KC x NC_, and one for blocks of size _MC x NC_ (which usually correspond to blocks of A, B, and C, respectively). Also note that if you are planning to [extract multithreaded parallelism](Multithreading.md) within BLIS within the `IC` or `JC` loops, you **may** need to set these values higher.
```
    #define BLIS_NUM_MC_X_KC_BLOCKS          1
    #define BLIS_NUM_KC_X_NC_BLOCKS          1
    #define BLIS_NUM_MC_X_NC_BLOCKS          0
```
> For single-threaded BLIS, usually only one block is needed in the pools for A and B, and blocks for C are not needed. These blocks of A and B (and C) are padded with extra space, according to `BLIS_MAX_PRELOAD_BYTE_OFFSET`
```
    #define BLIS_MAX_PRELOAD_BYTE_OFFSET     128
```
> This extra space is needed when the micro-kernel, in its loop body (along the _k_ dimension), "pre-loads" elements of a micro-panel of A or B from some future iteration. The problem occurs when computing at the end of a micro-panel; if this micro-panel is at the end of a contiguous block, the pre-loading may result in illegally memory accesses.

> _**Memory alignment.**_ BLIS provides several types of memory alignment, each targeting a specific situation. These alignment values are often bound to properties of the hardware or operating system, which we explicitly define as follows:
```
    #define BLIS_CACHE_LINE_SIZE             64
    #define BLIS_PAGE_SIZE                   4096
    #define BLIS_SIMD_ALIGN_SIZE             16
```
> The values `BLIS_CACHE_LINE_SIZE` and `BLIS_PAGE_SIZE` are the sizes, in bytes, of a hardware cache line and virtual memory page, respectively. The `BLIS_SIMD_ALIGN_SIZE` value represents the alignment preferred by the underlying SIMD vector instructions (if applicable). Note that these values are _not_ required to accurate, per se, as they are not referenced by source code outside of `bli_config.h`; rather, they merely help us reason about and express the subsequent alignment values below:
```
    #define BLIS_STACK_BUF_ALIGN_SIZE        BLIS_SIMD_ALIGN_SIZE
    #define BLIS_HEAP_ADDR_ALIGN_SIZE        BLIS_SIMD_ALIGN_SIZE
    #define BLIS_HEAP_STRIDE_ALIGN_SIZE      BLIS_CACHE_LINE_SIZE
    #define BLIS_CONTIG_ADDR_ALIGN_SIZE      BLIS_PAGE_SIZE
```
> The value `BLIS_STACK_BUF_ALIGN_SIZE` is used for temporary internal buffers, such as for output matrices to the micro-kernel when computing edge cases. (See [implementation notes](KernelsHowTo#Implementation_Notes_for_gemm.md) for the `gemm` micro-kernel for details.) This value, strictly speaking, should only require the most basic alignment (such as the 8-byte default system alignment of many modern systems). Aligning to the SIMD value may speed up access for some micro-kernels, if those kernels take advantage of aligned contiguous (row- or column-major) storage of the temporary edge-case buffer.

> The value `BLIS_HEAP_ADDR_ALIGN_SIZE` defines the alignment used when allocating memory on the heap, usually via `posix_memalign()`. Setting this value to the SIMD alignment size may speed up certain level-1v and -1f kernels. Setting this value to `1` will disable use of `posix_memalign()`, in which case `malloc()` is used instead.

> The value `BLIS_HEAP_STRIDE_ALIGN_SIZE` defines the alignment used for so-called "leading dimensions" (i.e. column strides for column-stored matrices, and row strides for row-stored matrices) when creating BLIS matrices via the object-based API. While setting `BLIS_HEAP_ADDR_ALIGN_SIZE` guarantees alignment for the first column (or row), creating a matrix with certain dimension values (_m_ and _n_) may cause subsequent columns (or rows) to be mis-aligned. Setting this value to the size of a cache line may be desirable. Additional alignment may or may not be beneficial.

> The value `BLIS_CONTIG_ADDR_ALIGN_SIZE` defines the alignment used when allocating blocks in the contiguous memory allocator. Any block of memory returned by the memory allocator is guaranteed to be aligned to this value. Aligning these blocks to the virtual memory page size (usually 4096 bytes) is standard practice. There is also the separate but related issue of micro-panel alignment, which allows subsequent micro-panels (other than the first) within a block of memory to also begin on aligned addresses. This topic is discussed [below](ConfigurationHowTo#bli_kernel.h.md).

> _**Mixed datatype support.**_ In theory, BLIS can support operations whose operands are from different domains, or are stored in different precisions. However, this feature is still very experimental! **Do not expect this feature to work. You've been warned.**

> _**BLAS compatibility layer.**_ BLIS provides an optional interface that allows legacy codes to use BLIS via calls to traditional BLAS routines.
```
    #define BLIS_ENABLE_BLAS2BLIS
```
> To disable the interface, un-define or comment out the `BLIS_ENABLE_BLAS2BLIS` macro. Also related to the compatibility layer, we define a macro to specify the size of integers in the compatibility interface.
```
    #define BLIS_BLAS2BLIS_INT_TYPE_SIZE     32
```
> Its semantics are similar to that of `BLIS_INT_TYPE_SIZE`, except it only affects the size of integers in the BLAS interface (not internally). If there is a size difference between the two, typecasting is used to translate between the two. Finally, we have a series of Fortran name-mangling macros:
```
    #define PASTEF770(name)                        name ## _
    #define PASTEF77(ch1,name)       ch1        ## name ## _
    #define PASTEF772(ch1,ch2,name)  ch1 ## ch2 ## name ## _
```
> These macros append (or prepend) underscores to BLAS routine names, as defined in C, so that the BLAS compatibility layer's routine names match those expected by the end user's Fortran-compiled application. By default, we append a single underscore to BLAS symbol names. For example, given the definitions shown above, the `dgemm` interface provided by the BLAS compatibility layer would be called from C code with a single underscore as:
```
    dgemm_( ... );
```
> but in Fortran, it would be called without an underscore:
```
    call dgemm( ... )
```
> Notice that BLIS does not presently support mangling to uppercase symbol names.

> _**CBLAS compatibility layer.**_ BLIS also provides an optional CBLAS compatibility layer which allows BLIS to serve as a drop-in replacement for CBLAS.
```
    #define BLIS_ENABLE_CBLAS
```
> To disable the interface, un-define or comment out the `BLIS_ENABLE_CBLAS` macro.
> The CBLAS layer in BLIS will define its integer size to be the same as that assumed by the BLAS compatibility layer, as specified by `BLIS_BLAS2BLIS_INT_TYPE_SIZE`. Also, enabling the CBLAS layer in BLIS will implicitly enable the BLAS compatibility layer, regardless of whether `BLIS_ENABLE_BLAS2BLIS` was defined in `bli_config.h`. This is because the CBLAS layer first calls BLAS, which will then call the corresponding native functionality in BLIS.
> Note that the application may access CBLAS prototypes and definitions at compile-time indirectly via `blis.h`, or directly via the `cblas.h` header included with BLIS.


### `bli_kernel.h` ###

This file defines C preprocessor macros associated with the various kernels and micro-kernels. Specifically, the primary function of this file is to define:
  * the cache and register blocksizes used to partition (block) through and pack (reformat) the matrix operands of level-3 operations;
  * the names of the kernel functions.

**IMPORTANT**: Any definition that is not `#define`d in `bli_kernel.h` will be set to a default value by the framework. For example, if you look at the `bli_kernel.h` in the `reference` configuration, the file is empty because that configuration uses only default values.

The `template` configuration defines its kernels in terms of `template` kernel implementations, most of which simply call built-in reference kernels (while the rest are defined as reference-like implementations that serve as examples). If you end up writing your own optimized kernels for some operation(s), be sure to enable their use by specifying their names within the appropriate section of the `bli_kernel.h` file.

As with the previous section on `bli_config.h`, we will describe the main sections here.

> _**Level-3 register blocksizes.**_ The `bli_kernel.h` header file defines a pair of register blocksizes, _MR_ and _NR_, for each datatype. For example, the register blocksizes for double-precision real computation may be defined as:
```
    #define BLIS_DEFAULT_MR_D              8
    #define BLIS_DEFAULT_NR_D              4
```
> These values are specific to the level-3 micro-kernels that are used by the configuration and are chosen when the micro-kernels are written. The header file also defines _packing_ register blocksize:
```
    #define BLIS_PACKDIM_MR_D              8
    #define BLIS_PACKDIM_NR_D              4
```
> These values determine _PACKMR_ and _PACKNR_, which serve as "leading dimensions" of the packed micro-panels that are passed into the micro-kernel. Oftentimes, _PACKMR = MR_ and _PACKNR = NR_, and thus the developer does not typically need to set these values manually. (See the [implementation notes for gemm](KernelsHowTo#Implementation_Notes_for_gemm.md) in the BLIS Kernel guide for more details on these topics.)

> A third value, _KR_, is also defined for each datatype:
```
    #define BLIS_DEFAULT_KR_D              1
```
> _KR_ is used by BLIS only when determining how much zero-padded space is needed in the _k_ dimension when packing blocks of matrices `A` and `B`. If _KR_ > 1, then the packing function will insert _k % KR_ columns or rows of zeros in these packed matrices. When _KR = 1_, as is typically the case, no zero-padding is inserted in the _k_ dimension. The reason zero-padding is usually not needed in the _k_ dimension is that most `gemm` micro-kernels handle micro-panels of arbitrary length. If instead the micro-kernel were written such that it could only be used with micro-panels whose lengths were multiples of, say, the micro-kernel's loop unrolling factor _u_, then the developer would need to set _KR = u_. This tells the packing function to insert zeros at the far edges of the micro-panels when _k_ is not a whole multiple of _u_.


> _**Level-3 cache blocksizes.**_ The configuration's cache blocksizes, _MC_, _NC_, and _KC_, are defined for each datatype. For example, the cache blocksizes for double-precision real computation may be defined as:
```
    #define BLIS_DEFAULT_MC_D              64
    #define BLIS_DEFAULT_KC_D              128
    #define BLIS_DEFAULT_NC_D              4096
```
> Each cache blocksize has a corresponding maximum value:
```
    #define BLIS_MAXIMUM_MC_D              (BLIS_DEFAULT_MC_D + BLIS_DEFAULT_MC_D/4)
    #define BLIS_MAXIMUM_KC_D              (BLIS_DEFAULT_KC_D + BLIS_DEFAULT_KC_D/4)
    #define BLIS_MAXIMUM_NC_D              (BLIS_DEFAULT_NC_D + BLIS_DEFAULT_NC_D/4)
```
> The maximum cache blocksizes are a convenient and portable way of smoothing performance of the level-3 operations when computing with a matrix operand that is just slightly larger than a multiple of the default cache blocksize in that dimension. For example, suppose we are going to execute BLIS's `gemm` operation (in double-precision real arithmetic) using the cache blocksizes shown above, and suppose that the _m_ dimension is 200. The optimal blocksize of 64 would be used three times, leaving a very small remainder (edge case) of 8. Since 8 is much smaller than the optimal value of 64, we may observe a degradation in performance for problem sizes of _m = 200_. We can solve this problem by merging the "edge case" iteration into the second-to-last iteration, such that the cache blocksizes used are 64, 64, and finally 72. The maximum cache blocksizes allow the developer to specify the _maximum_ size of the edge case such that it may still be merged with the neighboring iteration; if the edge case exceeds this maximum, then the edge case is _not_ merged and instead it is computed upon in separate iteration. In the code we show above, we have commented out expressions that would allow up to a 25% increase in all of the default cache blocksizes.

> _**Micro-panel alignment.**_ Recall that `BLIS_CONTIG_ADDR_ALIGN_SIZE`, defined in `bli_config.h`, allows us to guarantee that whole memory blocks begin at addresses with a specified alignment, such as the page size. There is also a separate family of macro constants to define the alignment of micro-panels _within_ a block. This is similar to the idea that, with conventional column-stored matrices, you may wish to align the leading dimension (column stride) in addition to the starting address so that you can guarantee that each column begins at an address with a certain alignment. The available macros are:
```
    #define BLIS_UPANEL_A_ALIGN_SIZE_S     4
    #define BLIS_UPANEL_A_ALIGN_SIZE_D     8
    #define BLIS_UPANEL_A_ALIGN_SIZE_C     8
    #define BLIS_UPANEL_A_ALIGN_SIZE_Z     16

    #define BLIS_UPANEL_B_ALIGN_SIZE_S     4
    #define BLIS_UPANEL_B_ALIGN_SIZE_D     8
    #define BLIS_UPANEL_B_ALIGN_SIZE_C     8
    #define BLIS_UPANEL_B_ALIGN_SIZE_Z     16
```
> Notice that we allow separate alignments for micro-panels of packed blocks of matrix A and packed row-panels of matrix B. The above definitions show the default values of each macro, which are simply the size of the datatype. Using the default values effectively disables micro-panel alignment, since aligning to the size of the datatype will never change the distance between micro-panels.

> _**Level-2 cache blocksizes.**_ Similar to the level-3 cache blocksizes, BLIS also allows the developer to specify cache blocksizes for level-2 operations. For example, the cache blocksizes for double-precision real computation may be defined as:
```
    #define BLIS_DEFAULT_L2_MC_D           1000
    #define BLIS_DEFAULT_L2_NC_D           1000
```
> These values induce matrix and vector blocking for level-2 operations when the problem size exceeds the specified certain value(s).


> _**Level-1f fusing factors.**_ [Several level-2 operations](KernelsHowTo#Level-1v/-1f_Dependencies_for_Level-2_operations.md) may be implemented in terms of level-1f (fused) operations. These fused operations are characterized by sequences of level-1v operations that are fused at the register-level to avoid certain redundant memory operations. For three of the level-1f kernels--`axpyf`, `dotxf`, and `dotxaxpyf`--the number of level-1v operations fused together is implementation-dependent, and called the _fusing factor_. The fusing factors are usually directly related to the number of registers available, and specified on a per datatype basis.
```
    #define BLIS_L1F_FUSE_FAC_D             4
    #define BLIS_DAXPYF_FUSE_FAC_D          BLIS_L1F_FUSE_FAC_D
    #define BLIS_DDOTXF_FUSE_FAC_D          BLIS_L1F_FUSE_FAC_D
    #define BLIS_DDOTXAXPYF_FUSE_FAC_D      BLIS_L1F_FUSE_FAC_D
```
> As shown above, we usually define a value `BLIS_L1F_FUSE_FAC_?`, which we then use to define the fusing factors for the individual double-precision real level-1f kernels.


> _**Level-3 micro-kernel definitions.**_ There are five sets of function names for the level-3 micro-kernels, one for each type of kernel. Here, we show the double-precision real flavor of each kernel being set to optimized function names.
```
    #define BLIS_DGEMM_UKERNEL         bli_dgemm_opt_mxn
    #define BLIS_DGEMMTRSM_L_UKERNEL   bli_dgemmtrsm_l_opt_mxn
    #define BLIS_DGEMMTRSM_U_UKERNEL   bli_dgemmtrsm_u_opt_mxn
    #define BLIS_DTRSM_L_UKERNEL       bli_dtrsm_l_opt_mxn
    #define BLIS_DTRSM_U_UKERNEL       bli_dtrsm_u_opt_mxn
```

> _Note:_ As discussed previously in the [template configuration and kernels](ConfigurationHowTo#template_configuration_and_kernels.md) section, the `template` configuration defines all kernels to use template kernel implementations. Despite the "`opt`" substring in their names, these kernel implementations simply call built-in reference (or reference-like) kernels. As you gradually transform these template implementations into optimized codes, you may decide to change their default function names from `<operation>_opt_mxn` to something else. If you do decide to use a different name for one or more of your micro-kernel functions, don't forget to change the corresponding definitions above (e.g. `BLIS_DGEMM_UKERNEL`) to enable use of your optimized micro-kernels.

> _Note:_ Some developers name micro-kernel functions according to their corresponding values of _MR_ and _NR_ so they easily remember which register blocksizes are assumed by each micro-kernel:
```
    void bli_sgemm_opt_8x4( ... );
    void bli_dgemm_opt_4x4( ... );
    void bli_cgemm_opt_4x2( ... );
    void bli_zgemm_opt_2x2( ... );
```

> The framework assumes by default that the `gemm` micro-kernels prefer to access elements of matrix C by contiguous columns. That is to say, the framework assumes that the `gemm` micro-kernels are able to more efficiently update C when the matrix is stored such that column elements are contiguous. If any or all of the `gemm` micro-kernels perform better when C is stored by contiguous rows, you can set a macro to indiciate this preference. For example, if you have a double-precision real `gemm` micro-kernel with a contiguous row preference, you would simply add the line:
```
    #define BLIS_DGEMM_UKERNEL_PREFERS_CONTIG_ROWS
```
> If, on the other hand, your `gemm` micro-kernel prefers contiguous columns, no additional macro is needed (since this preference is assumed by default). By informing the framework of your micro-kernels' preferred storage format of C, it can perform a minor optimization at run-time that will ensure the micro-kernel preference is honored, if at all possible.

> _Note:_ Currently, the framework only allows the kernel developer to signal a preference (row or column) for `gemm` micro-kernels. The preference of the `gemmtrsm` and `trsm` micro-kernels may not be indicated at this time.

> _**Level-1m kernel definitions.**_ In BLIS, the `packm` operation packs a submatrix to contiguous storage in preparation for computation with the micro-kernel. The BLIS framework isolates kernels to `packm` operation, such that kernel is called on time for each micro-panel that is packed as part of a larger packed matrix. The function names for these micro-kernels are defined in this section.
```
    #define BLIS_DPACKM_2XK_KERNEL     bli_dpackm_ref_2xk
    #define BLIS_DPACKM_4XK_KERNEL     bli_dpackm_ref_4xk
    #define BLIS_DPACKM_6XK_KERNEL     bli_dpackm_ref_6xk
    #define BLIS_DPACKM_8XK_KERNEL     bli_dpackm_ref_8xk
    ...
```
> Most developers will not need to provide custom definitions for packm micro-kernels, even when optimizing level-3 micro-kernels for high performance.


> _**Level-1f kernel definitions.**_ Similar to the section on level-3 micro-kernel definitions discussed previously, this section defines function names for the level-1f fused kernels:
```
    #define BLIS_DAXPY2V_KERNEL        bli_daxpy2v_opt
    #define BLIS_DDOTAXPYV_KERNEL      bli_ddotaxpyv_opt
    #define BLIS_DAXPYF_KERNEL         bli_daxpyf_opt
    #define BLIS_DDOTXF_KERNEL         bli_ddotxf_opt
    #define BLIS_DDOTXAXPYF_KERNEL     bli_ddotxaxpyf_opt
```
> The `template` configuration's level-1f kernels are each implemented either as simple C99, or as a call to the reference implementation (which is also implemented in C99). If you choose to optimize `axpyf`, `dotxf`, and/or `dotxaxpyf`, don't forget to specify their fusing factors (as previously discussed).

> _**Level-1v kernel definitions.**_ While they are often not utilized, BLIS also defines kernels for level-1v operations. The base names for such kernels are defined in this section:
```
    #define BLIS_DADDV_KERNEL          bli_daddv_opt
    #define BLIS_DAXPYV_KERNEL         bli_daxpyv_opt
    #define BLIS_DCOPYV_KERNEL         bli_dcopyv_opt
    #define BLIS_DDOTV_KERNEL          bli_ddotv_opt
    #define BLIS_DDOTXV_KERNEL         bli_ddotxv_opt
    ...
```
> Most developers leave these definitions undefined; the only exception would perhaps be `axpyv` and `dotv`/`dotxv`, since they are used in [some level-2 operations](KernelsHowTo#Level-1v/-1f_Dependencies_for_Level-2_operations.md) and thus their optimization may be desirable to some (though definitely not all) users.


### `make_defs.h` ###

This file contains general `make` definitions. To specify things such as your C compiler and which compiler options it should use when compiling BLIS, edit this file.

The format of the file is mostly self-explanatory. However, we will expound on some of the contents here.

> _**Build definitions.**_ This section contains three `make` variables:
```
    BLIS_ENABLE_VERBOSE_MAKE_OUTPUT := no
    BLIS_ENABLE_STATIC_BUILD        := yes
    BLIS_ENABLE_DYNAMIC_BUILD       := no
```
> The first option above allows the developer to request that the build system output the full compiler command lines as they are executed, rather than abbreviated output that includes only the name of the file being compiled. This option is useful to verify that certain compiler flags are (or aren't) being passed to the compiler. The second and third options specify whether to build a static and/or dynamic (shared) library.

> _**Utility program definitions.**_ This section defines some ordinary shell utilities, such as `mkdir` and `find`, that may be used by the build system. In most environments, these definitions should not be changed.

> _**Development tools definitions.**_ This section defines the C compiler as well as its compiler flags. It also defines the static library archiver (usually `ar`) and the linker to be used by the test drivers and test suite.
```
    CC             := gcc
    CPPROCFLAGS    := -D_POSIX_C_SOURCE=200112L   # This is often needed to enable posix_memalign().
    CMISCFLAGS     := -std=c99                    # Enable C99 language.
    CPICFLAGS      := -fPIC
    CDBGFLAGS      := -g
    CWARNFLAGS     := -Wall
    COPTFLAGS      := -O2
    CKOPTFLAGS     := $(COPTFLAGS)
    CVECFLAGS      :=
```
> The `make` variables above allow the developer the opportunity to specify C preprocessor flags, miscellaneous flags, debugging flags, warning flags, general optimization flags, kernel optimization flags, and SIMD/vector flags. Here, we show that the kernel optimization flags are the same as the general optimization flags, though they could be set differently.

> The values above are used to construct full sets of compiler flags for three different types of source code files: general framework, kernel, and optimization-sensitive:
```
    CFLAGS_NOOPT   := $(CDBGFLAGS) $(CWARNFLAGS) $(CPICFLAGS) $(CMISCFLAGS) $(CPPROCFLAGS)
    CFLAGS         := $(COPTFLAGS)  $(CVECFLAGS) $(CFLAGS_NOOPT)
    CFLAGS_KERNELS := $(CKOPTFLAGS) $(CVECFLAGS) $(CFLAGS_NOOPT)
```
> As you can see above, the kernel optimization flags are used instead of the general optimization flags when setting the value of `CFLAGS_KERNELS`. This feature is useful on some systems where you wish to use certain optimization settings on the kernels, but not the rest of the framework. Similarly, `CFLAGS_NOOPT` should contain no optimization flags; this set of flags is used to compile C translations to the `slamch()` and `dlamch()` routines in [LAPACK](http://www.netlib.org/lapack/). These routines allow computational codes to query machine constants such as epsilon (machine precision), but since they determine their values empirically, we must use no optimization to ensure proper execution.

If you use the standalone test drivers in the top-level `test` directory, some of the definitions defined in `make_defs.mk` (i.e., those used for compiling the BLIS library) will also be used when building the test drivers. If you want to use a different compiler and/or flags, simply edit `test/Makefile` so that the desired definitions are used. The universal test suite, located in the top-level `testsuite` directory, similarly includes definitions from the current configuration's `make_defs.mk` file, which may be similarly overridden in `testsuite/Makefile`.


### `kernels` (directory) ###

As mentioned in the [Introduction](ConfigurationHowTo#Introduction.md), any configuration that uses a non-reference kernel must include either (1) a symbolic link to a directory in which the kernels are located, or (2) an actual directory containing the kernels. The only kind of configuration that does not need to specify kernels is the `reference` configuration, or a configuration based on the `reference` configuration.

As you optimize your kernels, you are welcome to keep them within the configuration directory. However, at some point, we encourage you to move your kernels to an appropriately named subdirectory of the top-level `kernels` directory and use a symbolic link to point to those kernels. (Almost all optimized configurations abide by this convention.) This gives you the option of creating additional (perhaps slightly modified or experimental) configurations that use the same kernel set.



### Configuration checklist ###

Before running the `configure` script with your new configuration, make sure that the following tasks have been completed:
  * `config/<configname>` exists and is a directory, where `<configname>` is the name of the configuration you wish to use when building a BLIS library.
  * `config/<configname>/bli_config.h` exists and contains the appropriate definitions.
  * `config/<configname>/bli_kernel.h` exists and contains the appropriate kernel definitions, including cache and register blocksizes.
  * `config/<configname>/make_defs.mk` exists and contains your desired build system definitions, such as your choice of compiler and compiler flags.
  * `config/<configname>/<kernel_dir>` exists and is either a symbolic link to a directory **or** an actual directory of kernels and kernel headers. If you are just compiling a reference implementation of BLIS, this symbolic link is not needed.

Once the configuration sub-directory exists and is complete, you are ready to run the `configure` script, as described in [Step 2 of the BLIS build system wiki](http://code.google.com/p/blis/wiki/BuildSystem#Step_2:_make_configuration).


## Further Development Topics ##

### Switching configurations ###

The BLIS build system was designed to allow a developer to work with multiple framework configurations simultaneously. For example, the build system allows you to create a configuration that is oriented towards debugging and then another similar configuration that requests various kinds of compiler optimizations. Or, you might have yet another configuration to test out a new kernel that you just wrote. All of these configurations can co-exist together in the `config` directory. (The BLIS build system will **never** delete any of these directories, not even when executing a full clean via `make distclean`.) And switching between these configurations is easy; simply run `./configure <configname>`, where `<configname>` is the configuration you want to switch to. The script will prepare the build system for compiling for that configuration. If you are ever unsure which configuration is "active", simply run:

```
  $ make showconfig
  Current configuration is 'reference', located in './config/reference'
```

This will tell you the current configuration, which is stored in the `config.mk` file.

Another benefit to configuration switching is that a previous configuration's object files are saved from the previous build. For example, suppose you configure a configuration named `debug`. You run 'make' followed by `make install`. You use that debug-enabled library for a while and then you create a configuration named `opt`, which is the same configuration in optimized form. You compile, install, and test that library. But then you want to return to developing with the `debug` configuration. Simply run `./configure debug` again to switch to the `debug` configuration. If you haven't run any of the `clean` targets in the interim, the previously created object files for the `debug` configuration will still be there (inside `./obj/debug`). If you change a `.c` source file (not a header file; see [header dependencies](ConfigurationHowTo#Header_dependencies.md) section below), you won't have to recompile the whole library. Rather, only the source files that changed since the previous configuration and compilation will need to be recompiled.

### Header dependencies ###

Due to the way the BLIS framework handles header files, **any** change to **any** header file will result in the entire library being rebuilt. This policy is in place mostly out of an abundance of caution. If two or more files use definitions in a header that is modified, and one or more of those files somehow does not get recompiled to reflect the updated definitions, you could end up sinking hours of time trying to track down a bug that didn't ever need to be an issue to begin with. Thus, to prevent developers (including the framework developer(s)) from shooting themselves in the foot with this problem, the BLIS build system recompiles **all** object files if any header file is touched. We apologize for the inconvenience this may cause.

### Still have questions? ###

If you have further questions about BLIS configurations, please do not hesitate to contact the BLIS developer community. To do so, simply join and post to the [blis-devel](http://groups.google.com/group/blis-devel) mailing list.