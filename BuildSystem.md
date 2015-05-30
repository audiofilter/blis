## Introduction ##

This wiki describes how to configure, compile, and install a BLIS library on your local system.

The BLIS build system was designed for use with GNU/Linux (or some other sane UNIX). Other requirements are:

  * GNU `bash` (2.0 or later)
  * GNU `make`
  * a working C compiler

We also require various other shell utilities that are so ubiquitous that they are not worth mentioning (such as `mv`, `mkdir`, `find`, and so forth). If you are missing these utilities, then you have much bigger problems than not being able to build BLIS.


## Obtaining BLIS ##

Before starting, you must obtain a copy of BLIS. For details, please see the [Source tab](http://code.google.com/p/blis/wiki/SourceCode?tm=4).

Once you download the BLIS distribution, the top-level directory should look something like:
```
  $ ls 
  CHANGELOG  INSTALL  Makefile  build   configure  kernels  testsuite  windows
  CREDITS    LICENSE  README    config  frame      test     version
```


## Step 1: Choose a framework configuration ##

The first step is to decide whether you will be using an existing framework configuration, or creating your own.

Generally speaking, a framework configuration (or, "a configuration" for short) consists of:

  * two key header files with important definitions: `bli_config.h` and `bli_kernel.h`
  * a file containing makefile definitions, such as your choice of compiler and compiler flags: `make_defs.mk`
  * optimized kernels, usually specified via a symbolic link to a subdirectory of the top-level `kernels` directory, though the kernel source may, if desired, exist in an actual subdirectory within the configuration directory. (Exception: the `reference` configuration does not contain a kernels symlink/directory because it uses kernels built into the framework.)

The configuration files live inside a subdirectory of the top-level `config` directory, whereby the subdirectory is named after the configuration. Here are the current (as of this writing contents of the `config` directory:

```
  $ ls config
  bgq         loongson3a  piledriver  reference    template
  dunnington  mic         power7      sandybridge
```

As we illustrate use of the build system, we will show shell (command line) snippets that assume that you are using the `reference` configuration, which uses a set of built-in, portable kernels (written in C99) that should work without modification on most, if not all, architectures. These reference kernels, however, should be expected to yield relatively low performance since they do not employ any architecture-specific (ie: instruction-level) optimizations.

If you are a BLIS developer and wish to create your own configuration, either from scratch or using an existing configuration as a starting point, please read the [BLIS configuration guide](ConfigurationHowTo.md).

## Step 2: `make` configuration ##

This step should be somewhat familiar to many people who use open source software. To configure the build system, simply run:
```
  $ ./configure <configname>
```
where `<configname>` is the configuration sub-directory name you chose in [Step 1](BuildSystem#Step_1:_Framework_Configuration.md) above. If `<configname>` is not given, a helpful message is printed reminding you to explicit specify a configuration name.

Upon running configure, you will get output similar to the following. The exact output will depend on whether you cloned BLIS from a `git` repository or whether you obtained BLIS via a downloadable tarball from the BLIS website.
```
  $ ./configure reference
  configure: checking whether we need to update the version file.
  configure: checking version file './version'.
  configure: found '.git' directory; assuming git clone.
  configure: executing git describe --tags.
  configure: got back 0.0.9-24-gda77e96.
  configure: truncating to 0.0.9-24.
  configure: updating version file './version'.
  configure: starting configuration of BLIS 0.0.9-24.
  configure: configuring with 'reference' configuration sub-directory.
  configure: no install prefix given; defaulting to '/u/field/blis'.
  configure: creating ./config.mk from build/config.mk.in
  configure: creating ./obj/reference
  configure: creating ./obj/reference/config
  configure: creating ./obj/reference/frame
  configure: creating ./lib/reference
  configure: mirroring ./config/reference to ./obj/reference/config
  configure: mirroring ./frame to ./obj/reference/frame
  configure: creating makefile fragment in ./config/reference
  configure: creating makefile fragment in ./frame
  configure: creating makefile fragment in ./frame/0
  configure: creating makefile fragment in ./frame/0/absqsc
  configure: creating makefile fragment in ./frame/0/addsc
  configure: creating makefile fragment in ./frame/0/copysc
  configure: creating makefile fragment in ./frame/0/divsc
  configure: creating makefile fragment in ./frame/0/fnormsc
  ...
```

By default, BLIS is configured so that later on, when you run `make install`, the library and header files will be installed in `$(HOME)/blis`. If you want them to be installed somewhere else, use the `-p <PREFIX>` option, where
`<PREFIX>` is the path to which you want to install:
```
  $ ./configure -p /some/other/path <configname>
```
Note that `configure` will create `lib` and `include/blis` directories inside `<PREFIX>` if they do not already exist.

One last note: for a complete list of supported `configure` options and arguments, run `configure` with the `-h` option:
```
  $ ./configure -h
```
The output from this invocation of `configure` should give you an up-to-date list of options and their descriptions.


## Step 3: Compilation ##

Once `configure` is finished, you are ready to instantiate (compile) BLIS into a library by running `make`:
```
  $ make
```
Running `make` will result in output similar to:
```
  Compiling frame/0/absqsc/bli_absqsc.c
  Compiling frame/0/absqsc/bli_absqsc_check.c
  Compiling frame/0/absqsc/bli_absqsc_unb_var1.c
  Compiling frame/0/addsc/bli_addsc.c
  Compiling frame/0/addsc/bli_addsc_check.c
  Compiling frame/0/addsc/bli_addsc_unb_var1.c
  Compiling frame/0/copysc/bli_copysc.c
  Compiling frame/0/copysc/bli_copysc_check.c
  Compiling frame/0/copysc/bli_copysc_unb_var1.c
  Compiling frame/0/divsc/bli_divsc.c
  Compiling frame/0/divsc/bli_divsc_check.c
  Compiling frame/0/divsc/bli_divsc_unb_var1.c
  Compiling frame/0/fnormsc/bli_fnormsc.c
  Compiling frame/0/fnormsc/bli_fnormsc_check.c
  Compiling frame/0/fnormsc/bli_fnormsc_unb_var1.c
```
If you want to see the individual command line invocations of the compiler, edit your configuration's `make_defs.mk` to contain:
```
  BLIS_ENABLE_VERBOSE_MAKE_OUTPUT=yes
```
or, you can temporarily override that value from the command line:
```
  $ make BLIS_ENABLE_VERBOSE_MAKE_OUTPUT=yes
```
Also, if you are compiling on a multicore system, you can get parallelism via:
```
  $ make -j<n>
```
where `<n>` is the number of jobs `make` is allowed to run simultaneously. Generally, you should limit `<n>` to p+1, where p is the number of processor cores on your system.


## Step 4: Installation ##

Toward the end of compilation, you should get output similar to:
```
  Compiling frame/util/randv/bli_randv.c
  Compiling frame/util/randv/bli_randv_unb_var1.c
  Compiling frame/util/sets/bli_sets.c
  Compiling frame/base/noopt/bli_dlamch.c (NOTE: optimizations disabled)
  Compiling frame/base/noopt/bli_lsame.c (NOTE: optimizations disabled)
  Compiling frame/base/noopt/bli_slamch.c (NOTE: optimizations disabled)
  Archiving lib/reference/libblis.a
```
Now you have a BLIS library sitting in the `lib/<configname>/` directory. To install the library and the header files associated with it, simply execute:
```
  $ make install
```
This installs copies of the library and header files, and also creates a symbolic link of the library archive for convenience:
```
  Installing libblis-0.0.9-24-reference.a into /home/field/blis/lib/
  Installing C header files into /home/field/blis/include/blis/
  Installing symlink libblis.a into /home/field/blis/lib/
```
This results in your `<PREFIX>` directory looking like:
```
  # Check the contents of '<PREFIX>'.
  $ ls -l /home/field/blis
  drwxr-xr-x 2 field dept 32768 Dec  6 14:19 include
  drwxr-xr-x 2 field dept  4096 Dec  6 14:19 lib
  # Check the contents of '<PREFIX>/include'.
  $ ls -l /home/field/blis/include
  drwxr-xr-x 2 field dept 45056 Dec  6 14:19 blis
  # Check the contents of '<PREFIX>/lib'.
  $ ls -l /home/field/blis/lib
  -rw-r--r-- 1 field dept 3919726 Dec  6 14:19 libblis-0.0.9-24-reference.a
  lrwxrwxrwx 1 field dept      31 Dec  6 14:19 libblis.a -> libblis-0.0.9-24-reference.a
```
If you were to build and install a new configuration or version of BLIS, `make` would update your `libblis.a` symbolic link automatically. This means that you don't need to use unwieldy names such as `libblis-0.0.9-24-reference.a` in your application's makefile. Rather, you can simply link to `libblis.a`. That way, you will always pull in the BLIS library that was most recently installed.

Note that BLIS header files are _always_ installed to `<PREFIX>/include/blis/` and thus will overwrite header files from a previous installation.



## Linking to BLIS ##

Once you have instantiated (configured, compiled, and installed) a BLIS library, you can link to it in your application's makefile as you would any other library. The following is an abbreviated makefile for a small hypothetical application that has just two external dependencies: BLIS and the standard C math library.
```
  BLIS_PREFIX = $(HOME)/blis
  BLIS_INC    = $(BLIS_PREFIX)/include/blis
  BLIS_LIB    = $(BLIS_PREFIX)/lib/libblis.a

  OTHER_LIBS  = -L/usr/lib -lm

  CC          = gcc
  CFLAGS      = -O2 -g -I$(BLIS_INC)
  LINKER      = $(CC)

  OBJS        = main.o util.o other.o

  %.o: %.c
      $(CC) $(CFLAGS) -c $< -o $@

  all: $(OBJS) 
      $(LINKER) $(OBJS) $(BLIS_LIB) $(OTHER_LIBS) -o my_program.x
```
The above example assumes you will want to include BLIS definitions and function prototypes into your application via `#include blis.h`. (If you are only using the BLIS via the BLAS compatibility layer, including `blis.h` is not necessary.) Since BLIS headers are installed into a `blis` subdirectory of `<PREFIX>/include`, you must make sure that the compiler knows where to find the `blis.h` header file. This can be accomplished one of two ways:
  * Insert `#include "blis.h"` into your source code files and compile with `-I<PREFIX>/include/blis`. (This is the method assumed in the makefile example above.)
  * Insert `#include "blis/blis.h"` into your source code files and compile with `-I<PREFIX>/include`.

The makefile shown above a very simple example. If you need help linking your application to your BLIS library, please contact us through the BLIS mailing lists, [blis-discuss](http://groups.google.com/group/blis-discuss) and/or [blis-devel](http://groups.google.com/group/blis-devel).


### Cleaning out build products ###

If you want to remove various build products, you can use one of the `make` targets already defined for you in the BLIS Makefile:
```
  $ make clean
  Removing .o files from ./obj/reference.
  Removing .a files from ./lib/reference.
```
Executing the `clean` target will remove all binary object files and library builds from the `obj` and `lib` directories, respectively. Notice, though, that only object files for the current configuration are removed. Any other configurations' build products are left untouched.
```
  $ make cleanmk
  Removing makefile fragments from ./config/reference.
  Removing makefile fragments from ./frame.
```
The `cleanmk` target results in removal of all makefile fragments from the framework source tree. (Makefile fragments are named `.fragment.mk` and are generated at configure-time.) Makefile fragments are also removed from the configuration sub-directory, since the build system also compiles any source code residing there.
```
  $ make distclean
  Removing makefile fragments from ./config/reference.
  Removing makefile fragments from ./frame.
  Removing .o files from ./obj/reference.
  Removing .a files from ./lib/reference.
  Removing ./config.mk.
  Removing ./obj.
  Removing ./lib.
```
Running the `distclean` target is like saying, "Remove anything ever created by the build system." In addition to invoking `clean` and `cleanmk`, the `distclean` target removes the `config.mk` file as well as the top-level `obj` and `lib` directories.


## Conclusion ##

If you have feedback, please consider keeping in touch with the project maintainers, contributors, and other users by joining and posting to the [blis-discuss](http://groups.google.com/group/blis-discuss) and/or [blis-devel](http://groups.google.com/group/blis-devel) mailing lists.

Thanks for using BLIS!