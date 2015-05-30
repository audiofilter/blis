### Introduction ###

BLIS is a software framework for instantiating high-performance BLAS-like dense linear algebra libraries. BLIS is written in [ISO C99](http://en.wikipedia.org/wiki/C99) and available under a [new/modified/3-clause BSD license](http://opensource.org/licenses/BSD-3-Clause). While BLIS exports a [new BLAS-like API](BLISAPIQuickReference.md), it also includes a BLAS compatibility layer which gives application developers access to BLIS implementations via traditional [BLAS routine calls](http://www.netlib.org/lapack/lug/node145.html).

For a thorough presentation of our framework, please read our recently accepted journal article, ["BLIS: A Framework for Rapidly Instantiating BLAS Functionality"](http://www.cs.utexas.edu/users/flame/pubs/BLISTOMSrev2.pdf). For those who just want an executive summary, please see the [next section](#Key_Features.md).

In a follow-up article, ["The BLIS Framework: Experiments in Portability"](http://www.cs.utexas.edu/users/flame/pubs/blis2_toms_rev2.pdf), we investigate using BLIS to instantiate level-3 BLAS implementations on a variety of general-purpose, low-power, and multicore architectures.

It is our belief that BLIS offers substantial benefits in productivity when compared to conventional approaches to developing BLAS libraries, as well as a much-needed refinement of the BLAS interface, and thus constitutes a major advance in dense linear algebra computation. While BLIS remains a work-in-progress, we are excited to continue its development and further cultivate its use within the community.


### Key Features ###

BLIS offers several advantages over traditional BLAS libraries:

  * **Portability that doesn't impede high performance.** Portability was a top priority of ours when creating BLIS. With zero additional effort on the part of the developer, BLIS is configurable as a fully-functional reference implementation. But more importantly, the framework identifies and isolates a key set of computational kernels which, when optimized, immediately and automatically optimize performance across virtually _all_ level-2 and level-3 BLIS operations. In this way, the framework acts as a productivity multiplier. And since the optimized (non-portable) code is compartmentalized within these few kernels, instantiating a high-performance BLIS library on a new architecture is a relatively straightforward endeavour.

  * **Generalized matrix storage.** The BLIS framework exports interfaces that allow one to specify both the row stride and column stride of a matrix. This allows one to compute with matrices stored in column-major order, row-major order, or by general stride. (This latter storage format is important for those seeking to implement tensor contractions on multidimensional arrays.) Furthermore, since BLIS tracks stride information for each matrix, operands of different storage formats can be used within the same operation invocation. By contrast, BLAS requires column-major storage. And while the CBLAS interface supports row-major storage, it does not allow mixing storage formats.

  * **Full support for the complex domain.** BLIS operations are developed and expressed in their most general form, which is typically in the complex domain. These formulations then simplify elegantly down to the real domain, with conjugations becoming _no-ops_. Unlike the BLAS, all input operands in BLIS that allow transposition and conjugate-transposition also support conjugation (without transposition), which obviates the need for thread-unsafe workarounds. Also, where applicable, both complex symmetric and complex Hermitian forms are supported. (BLAS omits some complex symmetric operations, such as `symv`, `syr`, and `syr2`.)

  * **Ease of use.** The BLIS framework, and the library of routines it generates, are easy to use for end users, experts, and vendors alike. An optional BLAS compatibility layer provides application developers with backwards compatibility to existing BLAS-dependent codes. Or, one may adjust or write their application to take advantage of new BLIS functionality (such as generalized storage formats or additional complex operations) by calling BLIS directly. BLIS's interfaces will feel familiar to many veterans of BLAS since BLIS exports APIs with BLAS-like calling sequences. And experts will find BLIS's internal object-based APIs a delight to use when customizing or writing their own BLIS operations. (Objects are relatively lightweight `structs` and passed by address, which helps tame function calling overhead.)

  * **Multilayered API and exposed kernels.** The BLIS framework exposes its implementations in various layers, allowing expert developers to access exactly the functionality desired. This layered interface includes that of the lowest-level kernels, for those who wish to bypass the bulk of the framework. Optimizations can occur at various levels, in part thanks to exposed packing and unpacking facilities, which by default are highly parameterized and flexible.

  * **Functionality that grows with the community's needs.** As its name suggests, the BLIS framework is not a single library or static API, but rather a nearly-complete template for instantiating high-performance BLAS-like libraries. Furthermore, the framework is extensible, allowing developers to leverage existing components to support new operations as they are identified. If such operations require new kernels for optimal efficiency, the framework and its APIs will be adjusted and extended accordingly.

  * **A foundation for mixed domain and/or mixed precision operations.** BLIS was designed from the ground up to allow computation on real and complex operands within the same operation. Similarly, one could mix operands' floating-point precisions, or both domain and precision. Unfortunately, this feature results in a significant amount of additional code, mostly in level-2 and lower operations; thus, it is disabled by default. However, mixing domains in level-3 operations is possible with almost no additional effort on the part of the library developer, and such operations remain capable of high performance. (Please note that this functionality is still highly experimental and should be thought of as a feature that will be more thoroughly implemented at some future date.)

  * **Code re-use.** Auto-generation approaches to achieving the aforementioned goals tend to quickly lead to code bloat due to the multiple dimensions of variation supported: operation (i.e. `gemm`, `herk`, `trmm`, etc.); parameter case (i.e. side, [conjugate-]transposition, upper/lower storage, unit/non-unit diagonal); datatype (i.e. single-/double-precision real/complex); matrix storage (i.e. row-major, column-major, generalized); and algorithm (i.e. partitioning path and kernel shape). These "brute force" approaches often consider and optimize each operation or case combination in isolation, which is less than ideal when the goal is to provide entire libraries. BLIS was designed to be a complete framework for implementing basic linear algebra operations, but supporting this vast amount of functionality in a manageable way required a holistic design that employed careful abstractions, layering, and recycling of generic (highly parameterized) codes, subject to the constraint that high performance remain attainable.


### Discussion ###

To participate in discussions related to BLIS, please check out one or more of the following mailing lists:
  * [blis-announce](https://groups.google.com/d/forum/blis-announce) - to receive announcements and other important messages.
  * [blis-discuss](https://groups.google.com/d/forum/blis-discuss) - for general discussions.
  * [blis-devel](https://groups.google.com/d/forum/blis-devel) - for development-related discussions and bug reports.
    * **Note:** Most of the interesting discussions happen here; don't be afraid to join!


### Getting Started ###

If you just want to browse a quick-reference guide on user-level BLIS interfaces, please read the [BLIS API quick reference](http://code.google.com/p/blis/wiki/BLISAPIQuickReference). There you will find a brief description of each operation as well as some more general information needed when developing an application with BLIS.

Have a quick question? You may find the answer in our list of [frequently-asked questions](FAQ.md).

Does BLIS contain kernels optimized for your favorite architecture? Please see our [Hardware Support](HardwareSupport.md) wiki for a full list of optimized kernels.

Ready to use BLIS? You may download the BLIS source code [here](http://code.google.com/p/blis/source/checkout) (also via the "Source" tab at the top of the page). BLIS is hosed via [git](http://git-scm.com/). If you are unfamiliar with `git`, don't panic; most users (i.e. application developers) can get by with just two commands: [git-clone](https://www.kernel.org/pub/software/scm/git/docs/git-clone.html) and [git-pull](https://www.kernel.org/pub/software/scm/git/docs/git-pull.html). These commands are documented for your convenience in the [BLIS git wiki](GitHowTo.md).

BLIS generally targets two audiences: end-users and developers.

**End-users.** If you are an end-user and simply want to call BLIS from your application, using either the reference implementations or one of the other kernel sets included with BLIS, then:
  1. Read the [BLIS build system guide](BuildSystem.md) to build a BLIS library (provided that this has not already been done for you by someone else). Make sure you know which configuration you are trying to build. This guide also shows a very basic example of how to link your application to the BLIS library.
  1. Read the [BLIS test suite](Testsuite.md) wiki for detailed instructions on running the BLIS test suite, located in the top-level directory `testsuite`.
  1. Refer to the [BLIS API quick reference](BLISAPIQuickReference.md) if you wish to utilize the native BLIS API. If you simply want to use BLIS as a [BLAS](http://www.netlib.org/blas/) library, you may use the traditional [BLAS interface](http://www.netlib.org/lapack/lug/node145.html).


**Developers.** If you are a developer and you want to write BLIS kernels (and perhaps even contribute those kernels back to the project), then:
  1. Read the [BLIS Kernel guide](KernelsHowTo.md). This wiki describes each of the BLIS kernel operations in detail and should provide you with most of the information needed to get started with writing and optimizing your own kernels.
  1. Read the [BLIS Configuration guide](ConfigurationHowTo.md). This wiki describes how to create a BLIS "configuration", which captures all of the details necessary to build BLIS for a specific hardware architecture. Configurations specify things like blocksizes, kernel names, and various optional configuration settings.
  1. Read the [BLIS build system guide](BuildSystem.md). This wiki provides step-by-step instructions for building a BLIS library. Be sure to specify the configuration you developed in Step 2 to the `configure` script.
  1. After you build your BLIS library, you should try running the [BLIS test suite](Testsuite.md). If your kernels have bugs, certain operation tests will probably fail (including the tests of the kernels themselves).
  1. Refer to [BLIS API quick reference](BLISAPIQuickReference.md) if you (or your users) wish to utilize the native BLIS API. If you wish to use the lower-level object-based API, please refer to the relevant BLIS framework code itself. You can always ask questions on the [blis-devel blis-devel](https://groups.google.com/d/forum/blis-devel) mailing list.





### Citations ###

For those of you looking for the appropriate article to cite regarding BLIS, we recommend citing our [first ACM TOMS journal paper](http://www.cs.utexas.edu/users/flame/pubs/BLISTOMSrev2.pdf):

```
@article{BLIS1,
   author      = {Field G. {V}an~{Z}ee and Robert A. {v}an~{d}e~{G}eijn},
   title       = {{BLIS}: A Framework for Rapidly Instantiating {BLAS} Functionality},
   journal     = {ACM Transactions on Mathematical Software},
   year        = 2013,
   note        = {Accepted},
}
```

You may also cite the [second ACM TOMS journal paper](http://www.cs.utexas.edu/users/flame/pubs/blis2_toms_rev3.pdf):

```
@article{BLIS2,
   author      = {Field G. {V}an~{Z}ee and Tyler Smith and Francisco D. Igual and
                  Mikhail Smelyanskiy and Xianyi Zhang and Michael Kistler and Vernon Austel and
                  John Gunnels and Tze Meng Low and Bryan Marker and Lee Killough and
                  Robert A. {v}an~{d}e~{G}eijn},
   title       = {The {BLIS} Framework: Experiments in Portability},
   journal     = {ACM Transactions on Mathematical Software},
   year        = 2015,
   note        = {Accepted pending minor modifications},
}
```

We also have a third paper, submitted to IPDPS 2014, on achieving
[multithreaded parallelism in BLIS](http://www.cs.utexas.edu/users/flame/pubs/blis3_ipdps14.pdf):

```
@inproceedings{BLIS3,
   author      = {Tyler M. Smith and Robert A. {v}an~{d}e~{G}eijn and Mikhail Smelyanskiy and
                  Jeff R. Hammond and Field G. {V}an~{Z}ee},
   title       = {Anatomy of High-Performance Many-Threaded Matrix Multiplication},
   booktitle   = {28th IEEE International Parallel \& Distributed Processing Symposium
                  (IPDPS 2014)},
   year        = 2014,
}
```

A fourth paper, submitted to ACM TOMS, also exists, which proposes an [analytical model](http://www.cs.utexas.edu/users/flame/pubs/TOMS-BLIS-Analytical.pdf) for determining blocksize parameters in BLIS:

```
@article{BLIS4,
   author      = {Tze Meng Low and Francisco D. Igual and Tyler M. Smith and Enrique S. Quintana-Ort\'{\i}},
   title       = {Analytical Models for the {BLIS} Framework},
   journal     = {ACM Transactions on Mathematical Software},
   year        = 2015,
   note        = {Pending},
}
```

### Funding ###

This project and its associated research was partially sponsored by a grant from [Microsoft](http://www.microsoft.com/) as well as grants from the [National Science Foundation](http://www.nsf.gov/) (Awards CCF-0917167 and ACI-1148125/1340293).

_Any opinions, findings and conclusions or recommendations expressed in this material are those of the author(s) and do not necessarily reflect the views of the National Science Foundation (NSF)._