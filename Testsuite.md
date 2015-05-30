## Introduction ##

This wiki explains how to use the test suite included with the BLIS framework.

The test suite exists in the `testsuite` directory within the top-level source distribution:
```
  $ ls
  CHANGELOG  INSTALL  Makefile  build   configure  kernels  testsuite  windows
  CREDITS    LICENSE  README    config  frame      test     version
```
There, you will find a `Makefile`, two input files, and two directories:
```
  $ cd testsuite
  $ ls
  Makefile  input.general  input.operations  obj  src
```
As you would expect, the test suite's source code lives in `src` and the object files, upon being built, are placed in `obj`. The two `input.*` files control how the test suite runs, while the `Makefile controls how the test suite executable is compiled and linked.

## Compiling ##

Before running the test suite, you must first configure, compile, and install a BLIS library. For directions on how to build and install a BLIS library, please see the [BLIS build system](BuildSystem.md) wiki.

Once BLIS is installed, you are ready to compile the test suite.

**Note:** The `Makefile` includes the same `make_defs.mk` file that was used by the top-level `Makefile` when building BLIS. This is meant to serve as a convenience so you don't have to specify things like the C compiler or compiler flags a second time. If you do wish to tweak these parameters, you may override the values included from `make_defs.mk` by editing the local `Makefile` within the `testsuite` directory. Scroll down to the section labeled "Optional overrides" and uncomment/edit values as needed.

Unless special circumstances apply in your situation (such as the optional overrides mentioned above), the only value you should have to modify in `testsuite/Makefile` (if any) is the linker library flags variable, `LDFLAGS`. You may need to modify it to include the path to your standard C libraries, such as `libm` (oftentimes communicated to the linker via `-lm`):
```
  LDFLAGS   := -L/path/to/system/libs -lm
```

When you are ready to compile, simply run `make`:
```
  $ make
```
Running `make` will result in output similar to:
```
  gcc -O2 -Wall -std=c99 -D_POSIX_C_SOURCE=200112L -I/home/field/blis/include -Isrc -c src/test_addm.c -o obj/test_addm.o
  gcc -O2 -Wall -std=c99 -D_POSIX_C_SOURCE=200112L -I/home/field/blis/include -Isrc -c src/test_addv.c -o obj/test_addv.o
  gcc -O2 -Wall -std=c99 -D_POSIX_C_SOURCE=200112L -I/home/field/blis/include -Isrc -c src/test_axpy2v.c -o obj/test_axpy2v.o
  gcc -O2 -Wall -std=c99 -D_POSIX_C_SOURCE=200112L -I/home/field/blis/include -Isrc -c src/test_axpyf.c -o obj/test_axpyf.o
  gcc -O2 -Wall -std=c99 -D_POSIX_C_SOURCE=200112L -I/home/field/blis/include -Isrc -c src/test_axpym.c -o obj/test_axpym.o
  gcc -O2 -Wall -std=c99 -D_POSIX_C_SOURCE=200112L -I/home/field/blis/include -Isrc -c src/test_axpyv.c -o obj/test_axpyv.o
  gcc -O2 -Wall -std=c99 -D_POSIX_C_SOURCE=200112L -I/home/field/blis/include -Isrc -c src/test_copym.c -o obj/test_copym.o
  gcc -O2 -Wall -std=c99 -D_POSIX_C_SOURCE=200112L -I/home/field/blis/include -Isrc -c src/test_copyv.c -o obj/test_copyv.o
```
As with compiling a BLIS library, if you are working in a multicore environment, you may use the `-j<n>` option to compile source code in parallel with `<n>` parallel jobs:
```
  $ make -j4
```
After `make` is complete, an executable named `test_libblis.x` is created:
```
  $ ls
  Makefile  input.general  input.operations  obj  src  test_libblis.x
```

## Setting test parameters ##

The BLIS test suite reads two input files, `input.general` and `input.operations`, to determine which tests to run and how those tests are run. Each file is contains comments and thus you may find them intuitive to use without formal instructions. However, for completeness and as a reference-of-last-resort, we describe each file and its contents in detail.

### `input.general` ###

The `input.general` input file, as its name suggests, contains parameters that control the general behavior of the test suite. These parameters (more or less) apply to all operations that get tested. Below is a representative example of the default contents of `input.general`.
```
# ----------------------------------------------------------------------
#
#  input.general   
#  BLIS test suite
#
#  This file contains input values that control how BLIS operations are
#  tested. Comments explain the purpose of each parameter as well as
#  accepted values.
#

1       # Number of repeats per experiment (best result is reported)
c       # Matrix storage scheme(s) to test:
        #   'c' = col-major storage; 'g' = general stride storage;
        #   'r' = row-major storage
c       # Vector storage scheme(s) to test:
        #   'c' = colvec / unit stride; 'j' = colvec / non-unit stride;
        #   'r' = rowvec / unit stride; 'i' = rowvec / non-unit stride
0       # Test all combinations of storage schemes?
32      # General stride spacing (for cases when testing general stride)
sdcz    # Datatype(s) to test:
        #   's' = single real; 'c' = single complex;
        #   'd' = double real; 'z' = double complex
100     # Problem size: first to test
300     # Problem size: maximum to test
100     # Problem size: increment between experiments
1       # Error-checking level:
        #   '0' = disable error checking; '1' = full error checking
i       # Reaction to test failure:
        #   'i' = ignore; 's' = sleep() and continue; 'a' = abort
0       # Output results in matlab/octave format? ('1' = yes; '0' = no)
0       # Output results to stdout AND files? ('1' = yes; '0' = no)
```
The remainder of this section explains each parameter switch in detail.

> _**Number of repeats.**_ This is the number of times an operation is run for each result that is reported. The result with the best performance is reported.

> _**Matrix storage scheme.**_ This string encodes all of the matrix storage schemes that are tested (for operations that contain matrix operands). There are three valid values: `'c'` for column storage, `'r'` for row storage, and `'g'` for general stride storage. You may choose one storage scheme, or combine more than one. The order of the characters determines the order in which the corresponding storage schemes are tested.

> _**Vector storage scheme.**_ Similar to the matrix storage scheme string, this string determines which vector storage schemes are tested (for operations that contain vector operands). There are four valid values: `'c'` for column vectors with unit stride, `'r'` for row vectors with unit stride, `'j'` for column vectors with non-unit stride, and `'i'` for row vectors with non-unit stride. You may choose any one storage scheme, or combine more than one. The ordering behaves similarly to that of the matrix storage scheme string.

> _**Test all combinations of storage schemes?**_ Enabling this option causes all combinations of storage schemes to be tested. For example, if the option is disabled, a matrix storage scheme string of `cr` would cause the `gemm` test module to test execution where all matrix operands are column-stored, and then where all matrix operands are row-stored. Enabling this option with the same matrix storage string (`cr`) would cause the test suite to test `gemm` under all eight scenarios where the three `gemm` matrix operands are either column-stored or row-stored.

> _**General stride spacing.**_ This value determines the simulated "inner" stride when testing general stride storage. For simplicity, the test suite always generates and tests general stride storage that is ["column-tilted"](FAQ#What_does_it_mean_when_a_matrix_with_general_stride_is_column-ti.md). If general stride storage is not being tested, then this value is ignored.

> _**Datatype(s) to test.**_ This string determines which floating-point datatypes are tested. There are four valid values: `'s'` for single-precision real, `'d'` for double-precision real, `'c'` for single-precision complex, and `'z'` for double-precision complex. You may choose one datatype, or combine more than one. The order of the datatype characters determines the order in which they are tested.

> _**Problem size.**_ These values determine the first problem size to test, the maximum problem size to test, and the increment between problem sizes. Note that the maximum problem size only bounds the range of problem sizes; it is not guaranteed to be tested. Example: If the initial problem size is 128, the maximum is 1000, and the increment is 64, then the last problem size to be tested will be 960.

> _**Error-checking level.**_ BLIS supports various "levels" of error checking prior to executing most operations. For now, only two error-checking levels are implemented: fully disabled (`'0'`) and fully enabled (`'1'`). Disabling error-checking may improve performance on some systems for small problem sizes, but generally speaking the cost is negligible.

> _**Reaction to test failure.**_ If the test suite executes a test that results in a numerical result that is considered a "failure", this character determines how the test suite should proceed. There are three valid values: `'i'` will cause the test suite to ignore the failure and immediately continue with all remaining tests, `'s'` will cause the test suite to sleep for some short period of time before continuing, and `'a'` will cause the test suite to abort all remaining tests. The user must specify only **one** option via its character encoding.

> _**Output results in Matlab/Octave format?**_ When this option is disabled, the test suite outputs results in a simple human-readable format of one experiment per line. When this option is enabled, the test suite similarly outputs results for one experiment per line, but in a format that may be read into Matlab or Octave. This is useful if the user intends to use the results of the test suite to plot performance data using one of these tools.

> _**Output results to `stdout` AND files?**_ When this option is disabled, the test suite outputs only to standard output. When enabled, the test suite also writes its output to files, one for each operation tested. As with the Matlab/Octave option above, this option may be useful to some users who wish to gather and retain performance data for later use.


### `input.operations` ###

The `input.operations` input file determines **which** operations are tested, which parameter combinations are tested, and the relative sizes of the operation's dimensions. The file itself contains comments that explain various sections. However, we reproduce this information here for your convenience.

> _**Enabling/disabling entire sections.**_ The values in the "Section overrides" section allow you to disable all operations in a given "level". Enabling a level here by itself does not enable every operation in that level; it simply means that the individual switches for each operation (in that level) determine whether or not the tests are executed.

> _**Enabling/disabling individual operation tests.**_ Given that an operation's section override switch is set to 1 (enabled, whether or not that operation will get tested is determined by its local switch. For example, if the level-1v section override is set to 1, and there is a 1 on the line marked `addv`, then the `addv` operation will be tested. NOTE: You may ignore the lines marked "test sequential front-end." These lines are for future use, to distinguish tests of the sequential implementation from tests of the multithreaded implementation. For now, BLIS only contains sequential codes, so these should be left set to 1.

> _**Changing the problem size/shapes tested.**_ The problem sizes tested by an operation are determined by the dimension specifiers on the line marked `dimensions: <spec_labels>`. If, for example, `<spec_labels>` contains two dimension labels (e.g. `m n`), then the line should begin with two dimension specifiers. Dimension specifiers of `-1` cause the corresponding dimension to be bound to the problem size, which is determined by values set in `input.general`. Positive values cause the corresponding dimension to be fixed to that value and held constant. Examples of dimension specifiers (where the dimensions are _m_ and _n_):
    * `-1 -1 `   ...Dimensions m and n grow with problem size (resulting in square matrices).
    * `-1 150 `   ...Dimension m grows with problem size and n is fixed at 150.
    * `-1 -2 `   ...Dimension m grows with problem size and n grows proportional to half the problem size.

> _**Changing parameter combinations tested.**_ The parameter combinations tested by an operation are determined by the parameter specifier characters on the line marked `parameters: <param_labels>`. If, for example, `<param_labels>` contains two parameter labels (e.g. `transa conjx`), then the line should contain two parameter specifier characters. The `'?'` specifier character serves as a wildcard--it causes all possible values of that parameter to be tested. A character such as `'n'` or `'t'` causes only that value to be tested. Examples of parameter specifiers (where the parameters are `transa` and `conjx`):
    * `??`   ...All combinations of the `transa` and `conjx` parameters are tested: `nn, nc, tn, tc, cn, cc, hn, hc`.
    * `?n`   ...`conjx` is fixed to "no conjugate" but `transa` is allowed to vary: `nn, tn, cn, hn`.
    * `hc`   ...Only the case where `transa` is "Hermitian-transpose" and `conjx` is "conjugate" is tested.
> Here is a full list of the parameter types used by the various BLIS operations along with their possible character encodings:
    * `side`: `l` = left,  `r` = right
    * `uplo`: `l` = lower-stored, `u` = upper-stored
    * `trans`: `n` = no transpose, `t` = transpose, `c` = conjugate, `h` = Hermitian-transpose (conjugate-transpose)
    * `conj`: `n` = no conjugate, `c` = conjugate
    * `diag`: `n` = non-unit diagonal, `u` = unit diagonal


## Running tests ##

Running the test suite is easy. Once `input.general` and `input.operations` have been tailored to your liking, simply run the test suit executable:
```
  $ ./test_libblis.x
```
For sanity-checking purposes, the test suite begins by echoing the parameters it found in `input.general` to standard output. This is useful when troubleshooting the test suite if/when it exhibits strange behavior (such as seemingly skipped tests).

## Interpreting the results ##

The output to the test suite is more-or-less intuitive. Here is an snippet of output for the `gemm` test module when problem sizes of 100 to 300 in increments of 100 are tested.
```
% --- gemm ---
%
% test gemm seq front-end?    1
% gemm m n k                  -1 -1 -2
% gemm operand params         ??
%

% blis_<dt><oper>_<params>_<storage>           m     n     k   gflops  resid       result
blis_sgemm_nn_ccc                            100   100    50   1.447   1.14e-07    PASS
blis_sgemm_nn_ccc                            200   200   100   1.537   1.18e-07    PASS
blis_sgemm_nn_ccc                            300   300   150   1.532   1.38e-07    PASS
blis_sgemm_nc_ccc                            100   100    50   1.449   7.79e-08    PASS
blis_sgemm_nc_ccc                            200   200   100   1.540   1.23e-07    PASS
blis_sgemm_nc_ccc                            300   300   150   1.537   1.54e-07    PASS
blis_sgemm_nt_ccc                            100   100    50   1.479   7.40e-08    PASS
blis_sgemm_nt_ccc                            200   200   100   1.549   1.33e-07    PASS
blis_sgemm_nt_ccc                            300   300   150   1.534   1.44e-07    PASS
blis_sgemm_nh_ccc                            100   100    50   1.477   9.23e-08    PASS
blis_sgemm_nh_ccc                            200   200   100   1.547   1.13e-07    PASS
blis_sgemm_nh_ccc                            300   300   150   1.535   1.51e-07    PASS
blis_sgemm_cn_ccc                            100   100    50   1.477   9.62e-08    PASS
blis_sgemm_cn_ccc                            200   200   100   1.548   1.36e-07    PASS
blis_sgemm_cn_ccc                            300   300   150   1.539   1.51e-07    PASS
blis_sgemm_cc_ccc                            100   100    50   1.481   8.66e-08    PASS
blis_sgemm_cc_ccc                            200   200   100   1.549   1.41e-07    PASS
blis_sgemm_cc_ccc                            300   300   150   1.539   1.63e-07    PASS
blis_sgemm_ct_ccc                            100   100    50   1.484   7.09e-08    PASS
blis_sgemm_ct_ccc                            200   200   100   1.549   1.08e-07    PASS
blis_sgemm_ct_ccc                            300   300   150   1.539   1.33e-07    PASS
blis_sgemm_ch_ccc                            100   100    50   1.471   8.06e-08    PASS
blis_sgemm_ch_ccc                            200   200   100   1.546   1.24e-07    PASS
blis_sgemm_ch_ccc                            300   300   150   1.539   1.66e-07    PASS
```

Before each operation is tested, the test suite echos information it obtained from the `input.operations` file, such as the dimension specifier string (in this case, `"-1 -1 -2"`) and parameter specifier string (`"??"`).

Each line of output contains several sections. We will cover them now, from left to right.

> _**Test identifier.**_ The left-most labels are strings which identify the specific test being performed. This string generally a concatenation of substrings, joined by underscores, which identify the operation being run, the parameter combination tested, and the storage scheme of each operand. When outputting to Matlab/Octave formatting is abled, these identifiers service as the names of the arrays in which the data are stored.

> _**Dimensions.**_ The values near the middle of the output show the size of each dimension. Different operations have different dimension sets. For example, `gemv` only has two dimensions, _m_ and _n_, while `gemm` has an additional _k_ dimension. In the snippet above, you can see that the dimension specifier string, `"-1 -1 -2"`, explains the relative sizes of the dimensions for each test: _m_ and _n_ are bound to the problem size, while _k_ is always equal to half the problem size.

> _**Performance.**_ The next value output is raw performance, reported in GFLOPS (billions of floating-point operations per second).

> _**Residual.**_ The next value, which we loosely refer to as a "residual", reports the result of the numerical correctness test for the operation. The actual method of computing the residual (and hence its exact meaning) depends on the operation being tested. However, these residuals are always computed such that the result should be no more than 2-3 orders of magnitude away from machine precision for the datatype being tested. Thus, "good" results are typically in the neighborhood of `1e-07` for single precision and `1e-15` for double precision.

> _**Test result.**_ The BLIS test suite compares the residual to internally-defined accuracy thresholds to categorize the test as either `PASS`, `MARGINAL`, or `FAIL`. The vast majority of tests should result in a `PASS` result, with perhaps a handful resulting in `MARGINAL`.

Note that the various sections of output, which line up nicely as columns, are labeled on a line beginning with `%` immediately before the results:
```
% blis_<dt><oper>_<params>_<storage>           m     n     k   gflops  resid       result
blis_sgemm_nn_ccc                            100   100    50   1.447   1.14e-07    PASS
```
These labels are useful as concise reminders of the meaning of each column. They are especially useful in differentiating the various dimensions from each other for operations that contain two or three dimensions.