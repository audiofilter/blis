## Introduction ##

Here we attempt to provide some frequently-asked questions about the BLIS framework project, as well as those we think a new user or developer might ask. If you don't see the answer to your question here, please join and post your question to the [blis-discuss](https://groups.google.com/d/forum/blis-discuss) or [blis-devel](https://groups.google.com/d/forum/blis-devel) mailing lists.

## Contents ##

  * [Why did you create BLIS?](FAQ#Why_did_you_create_BLIS?.md)
  * [Why should I use BLIS instead of GotoBLAS / OpenBLAS / ATLAS / MKL / ESSL / ACML / Accelerate / OtherBLAS(TM)?](FAQ#Why_should_I_use_BLIS_instead_of_GotoBL.md)
  * [How is BLIS related to FLAME / libflame?](FAQ#How_is_BLIS_related_to_FLAME_/_libflame_?.md)
  * [Does BLIS automatically detect my hardware?](FAQ#Does_BLIS_automatically_detect_my_hardware?.md)
  * [So for now, BLIS is mostly a developer's tool?](FAQ#So_for_now,_BLIS_is_mostly_a_developer%27s_tool?.md)
  * [Must I use git? Can I download a tarball?](FAQ#Must_I_use_git?_Can_I_download_a_tarball?.md)
  * [What is a micro-kernel?](FAQ#What_is_a_micro-kernel?.md)
  * [What is a macro-kernel?](FAQ#What_is_a_macro-kernel?.md)
  * [I'm used to thinking in terms of column-major/row-major storage and leading dimensions. What is a "row stride" / "column stride"?](FAQ#I%27m_used_to_thinking_in_terms_of_column-major/row-major_sto.md)
  * [What does it mean when a matrix with general stride is column-tilted or row-tilted?](FAQ#What_does_it_mean_when_a_matrix_with_general_stride_is_column-ti.md)
  * [I'm not really interested in all of these newfangled features in BLIS. Can I just use BLIS as a BLAS library?](FAQ#I%27m_not_really_interested_in_all_of_these_newfangled_featur.md)
  * [What about CBLAS?](FAQ#What_about_CBLAS?.md)
  * [Can I call the native BLIS API from Fortran-77/90/95/2000/C++/Python?](FAQ#Can_I_call_the_native_BLIS_API_from_Fortran-77/90/95/2000/C++/Py.md)
  * [Does BLIS support multithreading?](FAQ#Does_BLIS_support_multithreading?.md)
  * [Does BLIS work with GPUs?](FAQ#Does_BLIS_work_with_GPUs?.md)
  * [Have you tested BLIS on modern architectures?](FAQ#Have_you_tested_BLIS_on_modern_architectures?.md)
  * [Does BLIS work on <some architecture>?](FAQ#Does_BLIS_work_on_%3Csome_architecture%3E_?.md)
  * [What about distributed-memory parallelism?](FAQ#What_about_distributed-memory_parallelism?.md)
  * [Can I build BLIS on Windows / Mac OS X?](FAQ#Can_I_build_BLIS_on_Windows_/_Mac_OS_X?.md)
  * [Can I build BLIS as a shared library?](FAQ#Can_I_build_BLIS_as_a_shared_library?.md)
  * [Can I use the mixed domain / mixed precision support in BLIS?](FAQ#Can_I_use_the_mixed_domain_/_mixed_precision_support_in_BLIS?.md)
  * [Who is involved in the project?](FAQ#Who_is_involved_in_the_project?.md)
  * [Who funded BLIS's development?](FAQ#Who_funded_BLIS%27s_development?.md)
  * [I found a bug. How do I report it?](FAQ#I_found_a_bug._How_do_I_report_it?.md)
  * [How do I request a new feature?](FAQ#How_do_I_request_a_new_feature?.md)



### Why did you create BLIS? ###

Initially, BLIS was conceived as simply "BLAS with a more flexible interface". The original BLIS was written as a wrapper layer around BLAS that allowed generalized matrix storage. We also took the opportunity to implement some complex domain features that were missing from the BLAS (mostly related to conjugating input operands). This "proto-BLIS" was deployed in [libflame](http://www.cs.utexas.edu/~flame/web/libFLAME.html) to facilitate cleaner implementations of some LAPACK-level operations.

Over time, we wanted more than just a more flexible interface; we wanted an entire framework from which we could build operations in the BLAS as well as those not present within the BLAS. After this new BLIS framework was implemented, it turned out that the interface improvements were much less interesting (and consequential) than the fact that BLIS allowed developers to rapidly instantiate new BLAS libraries by optimizing only a small amount of code.

### Why should I use BLIS instead of GotoBLAS / OpenBLAS / ATLAS / MKL / ESSL / ACML / Accelerate / OtherBLAS(TM)? ###

BLIS has numerous advantages to existing BLAS implementations. Many of these advantages are summarized [here](http://code.google.com/p/blis/#Key_Features). But here are a few reasons one might choose BLIS over some other implementation of BLAS:
  * BLIS facilitates high performance while remaining very portable. BLIS isolates performance-sensitive code to a micro-kernel which contains only one loop and which, when optimized, accelerates virtually all level-3 operations. Thus, BLIS serves as a powerful tool for quickly instantiating BLAS on new or experimental hardware architectures, as well as a flexible "laboratory" in which to conduct research and experiments.
  * BLIS supports a superset of BLAS functionality, providing operations omitted from the BLAS as well as some complex domain support that is missing in BLAS operations. BLIS is especially useful to researchers who need to develop and prototype new BLAS-like operations that do not exist in the BLAS.
  * BLIS is backwards compatible with BLAS. BLIS contains a BLAS compatibility layer that allows an application to treat BLIS as if it were a traditional BLAS library.
  * BLIS supports generalized matrix storage, which can be used to express column-major, row-major, and general stride storage.
  * BLIS is free software, available under a [new/modified/3-clause BSD license](http://opensource.org/licenses/BSD-3-Clause).

### How is BLIS related to FLAME / `libflame`? ###

As explained [above](FAQ#Why_did_you_create_BLIS?.md), BLIS was initially a layer within `libflame` that allowed more convenient interfacing to the BLAS. So in some ways, BLIS is a spin-off project. Prior to developing BLIS, [its author](http://www.cs.utexas.edu/users/field/) worked as the primary maintainer of `libflame`. If you look closely, you can also see that BLIS's design was influenced by some of the more useful and innovative aspects of `libflame`, such as internal object abstractions and control trees. Also, various members of the [FLAME research group](http://www.cs.utexas.edu/~flame/web/team.html) routinely provide insight, feedback, and also contribute code (especially kernels) to the BLIS project.

### Does BLIS automatically detect my hardware? ###

Not yet. For now, BLIS requires the user/developer to [manually specify](BuildSystem#Step_1:_Choose_a_framework_configuration.md) an existing configuration that corresponds to the hardware for which to build a BLIS library.

### So for now, BLIS is mostly a developer's tool? ###

Yes. In order to achieve high performance, BLIS requires that hand-coded kernels and micro-kernels be written and referenced in a valid [BLIS configuration](ConfigurationHowTo.md). These components are usually written by developers and then included within BLIS for use by others.

If high performance is not important, then you can always build the reference implementation on any hardware platform. The reference implementation does not contain any machine-specific code and thus should be very portable.

### Must I use git? Can I download a tarball? ###

We **strongly encourage** you to obtain the BLIS source code by cloning a `git` repository (via the [git clone](http://code.google.com/p/blis/wiki/GitHowTo#clone) command). The reason for this is that it will allow you to easily update your local copy of BLIS by executing [git pull](http://code.google.com/p/blis/wiki/GitHowTo#pull).

An alternative way of downloading BLIS is by clicking on the ["Source"](http://code.google.com/p/blis/source/checkout/) tab, and then ["Browse"](http://code.google.com/p/blis/source/browse/), and then clicking on the link labeled "Download zip". This link allows you to download a `.zip` archive of the master branch. However, this is **not recommended** for the aforementioned reasons, and also because it confuses the BLIS `configure` script, which was written to obtain version information via `git` and then use this version info to name the build products.

### What is a micro-kernel? ###

The micro-kernel (usually short for "`gemm` micro-kernel") is the basic unit of level-3 (matrix-matrix) computation within BLIS. It consists of one loop, where each iteration performs a very small outer product to update a very small matrix. The micro-kernel is typically the only piece of code that must be carefully optimized (via vector intrinsics or assembly code) to enable high performance in most of the level-3 operations such as `gemm`, `hemm`, `herk`, and `trmm`.

For a more thorough explanation of the micro-kernel and its role in the overall level-3 computations, please read our [ACM TOMS papers](http://code.google.com/p/blis/#Citations). For API and technical reference, please see the [gemm micro-kernel section](KernelsHowTo#gemm_micro-kernel.md) of the [BLIS Kernels guide](KernelsHowTo.md).

### What is a macro-kernel? ###

The macro-kernels are portable codes within the BLIS framework that implement relatively small subproblems within an overall level-3 operation. The overall problem (say, general matrix-matrix multiplication, or `gemm`) is partitioned down, according to cache blocksizes, such that its operands are (1) a suitable size and (2) stored in a special packed format. At that time, the macro-kernel is called. The macro-kernel is implemented as two loops around the micro-kernel.

The macro-kernels in BLIS correspond to the so-called "inner kernels" that formed the fundamental unit of computation in Kazushige Goto's GotoBLAS (and now in the successor library, OpenBLAS).

For more information on macro-kernels, please read our [ACM TOMS papers](http://code.google.com/p/blis/#Citations).

### I'm used to thinking in terms of column-major/row-major storage and leading dimensions. What is a "row stride" / "column stride"? ###

Traditional BLAS assumes that matrices are stored in column-major order, where a leading dimension measures the distance from one element to the next element in the same row. But column-major order is really just a special case of BLIS's more generalized storage scheme.

In generalized storage, we have a row stride and a column stride. The row stride measures the distance in memory between rows (within a single column) while the column stride measures the distance between columns (within a single row). Column-major storage corresponds to the situation where the row stride equals 1. Since the row stride is unit, you only have to track the column stride (i.e., the leading dimension). Similarly, in row-major order, the column stride is equal to 1 and only the row stride must be tracked.

BLIS also supports situations where both the row stride and column stride are non-unit. We call this situation "general stride".

### What does it mean when a matrix with general stride is column-tilted or row-tilted? ###

When a matrix is stored with general stride, both the row stride and column stride (let's call them `rs` and `cs`) are non-unit. When `rs` < `cs`, we call the general stride matrix "column-tilted" because it is "closer" to being column-stored (than row-stored). Similarly, when `rs` > `cs`, the matrix is "row-tilted" because it is closer to being row-stored.

### I'm not really interested in all of these newfangled features in BLIS. Can I just use BLIS as a BLAS library? ###

Absolutely. Just link your application to BLIS the same way you would link to a BLAS library. For a simple linking example, see the [Linking to BLIS](BuildSystem#Linking_to_BLIS.md) section of the [BLIS Build System guide](BuildSystem.md).

### What about CBLAS? ###

BLIS also contains an optional CBLAS compatibility layer, which leverages the BLAS compatibility layer to help map CBLAS function calls to the corresponding functionality in BLIS. Once BLIS is built with CBLAS support, your application can access CBLAS prototypes via either `cblas.h` or `blis.h`.

### Can I call the native BLIS API from Fortran-77/90/95/2000/C++/Python? ###

In principle, BLIS's [native BLAS-like API](BLISAPIQuickReference.md) can be called from Fortran. However, you must ensure that the size of the integer in BLIS is equal to the size of integer used by your Fortran program/compiler/environment. The size of BLIS integers is set in `bli_config.h`. Please see the [bli\_config.h](ConfigurationHowTo#bli_config.h.md) section of the [BLIS Configuration guide](ConfigurationHowTo.md) for more details.

As for bindings to other languages, please contact the [blis-devel](http://groups.google.com/group/blis-devel) mailing list.

### Does BLIS support multithreading? ###

Yes! BLIS supports multithreading via OpenMP for all of its level-3 operations. (POSIX threads support is planned for the future.) For more information on enabling and controlling multithreading, please see the wiki on [Multithreading](Multithreading.md).

BLIS can also very easily be made thread-safe so that you can call BLIS from threads within a multithreaded library or application. For more information on making BLIS thread-safe, see the "Multithreading" subsection of the [bli\_config.h](ConfigurationHowTo#bli_config.h.md) header file section in the [BLIS Configuration guide](ConfigurationHowTo.md).

### Does BLIS work with GPUs? ###

BLIS does not currently support graphical processing units (GPUs).

### Have you tested BLIS on modern architectures? ###

Yes. For example, a high performance BLIS implementation of `dgemm()` has been developed for both the Intel Xeon Phi and IBM Blue Gene/Q, and experimental multicore implementations on both architectures exhibit impressive scalability (utilizing 240 hardware threads across 60 cores of the Xeon Phi, and 64 threads across 16 cores of the Blue Gene/Q). Please see the latest draft of our second ACM TOMS journal paper, ["The BLIS Framework: Experiments in Portability"](http://code.google.com/p/blis/#Citations), for a study on how BLIS ported to numerous conventional, low-power, and many-core architectures.

### Does BLIS work on _<some architecture>_? ###

Please see the [BLIS Hardware Support](HardwareSupport.md) wiki for a full list of supported architectures. If your favorite hardware is not listed and you have the expertise, please consider developing your own kernels and sharing them with the project! We will, of course, gratefully credit your contribution.

### What about distributed-memory parallelism? ###

No. BLIS is a framework for sequential and shared-memory/multicore implementations of BLAS-like operations. If you need distributed-memory dense linear algebra implementations, we recommend the [Elemental library](https://code.google.com/p/elemental/).

### Can I build BLIS on Windows / Mac OS X? ###

BLIS was designed for use in a GNU/Linux environment, however, it should work on other UNIX-like systems as well, such as OS X. System software requirements for UNIX-like systems are discussed in the [BLIS build system guide](BuildSystem.md).

Support for building in Windows is also a goal of ours. The Windows build system exists as a separate entity within the top-level `windows` directory. However, this feature is still experimental and should not (yet) be expected to work reliably. Please contact the developers on the [blis-devel](http://groups.google.com/group/blis-devel) mailing list for the latest on the Windows build system.

### Can I build BLIS as a shared library? ###

Yes. By default, most configurations output only a static library archive (e.g. `.a` file). However, you can also request a shared object (e.g. `.so` file), sometimes also called a "dynamically-linked" library. For information on enabling shared library output, please see the [BLIS Configuration guide](ConfigurationHowTo#make_defs.h.md).

### Can I use the mixed domain / mixed precision support in BLIS? ###

While BLIS was designed to eventually support mixed-domain (or even mixed-precision) operations, we do not yet recommend attempting to use this functionality. If this feature is important to you, please contact us via the [blis-devel](http://groups.google.com/group/blis-devel) mailing list.

### Who is involved in the project? ###

Lots of people! For a full list of those involved, see the [CREDITS](https://code.google.com/p/blis/source/browse/CREDITS) file within the BLIS framework source distribution.

### Who funded BLIS's development? ###

BLIS was primarily funded by the [National Science Foundation](http://www.nsf.gov/) (Awards CCF-0917167 and ACI-1148125).

Reminder: _Any opinions, findings and conclusions or recommendations expressed in this material are those of the author(s) and do not necessarily reflect the views of the National Science Foundation (NSF)._

### I found a bug. How do I report it? ###

We request that bug reports be posted to the [blis-devel](http://groups.google.com/group/blis-devel) mailing list. If you have a bug to report, please join this group and then post your report there.

### How do I request a new feature? ###

Feature requests may also be submitted to the [blis-devel](http://groups.google.com/group/blis-devel) mailing list.