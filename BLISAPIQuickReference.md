# Introduction #

This wiki summarizes the user-level BLIS APIs, including BLIS-specific type definitions, header files, and function prototypes. This document also includes APIs to key [micro-]kernels which are used to accelerate and optimize various level-2 and level-3 operations.

There are many functions that BLIS implements that are not listed here, either because they are lower-level functions, or they are considered for use primarily by developers and experts.


## BLIS types ##

The following tables list various types used throughout the BLIS API.

### Integer-based types ###

| BLIS type    | Definition   | Used to represent...             |
|:-------------|:-------------|:---------------------------------|
| `gint_t`     | `int32_t` or `int64_t` | general-purpose signed integer; used to define signed integer types. |
| `dim_t`      | `gint_t`     | ...matrix and vector dimensions. |
| `inc_t`      | `gint_t`     | ...matrix row/column strides and vector increments. |
| `doff_t`     | `gint_t`     | ...matrix diagonal offset: if _k_ < 0, diagonal begins at element (-_k_,0); otherwise diagonal begins at element (0,_k_). |

### Floating-point types ###

| BLIS type  | BLIS char | Definition                             | Used to represent...                 |
|:-----------|:----------|:---------------------------------------|:-------------------------------------|
| `float`    | `s`       | _N/A_                                  | ...single-precision real numbers.    |
| `double`   | `d`       | _N/A_                                  | ...double-precision real numbers.    |
| `scomplex` | `c`       | `struct { float real; float imag; }`   | ...single-precision complex numbers. |
| `dcomplex` | `z`       | `struct { double real; double imag; }` | ...double-precision complex numbers. |

### Enumerated parameter types ###

| `trans_t`                | Semantic meaning: Corresponding matrix operand... |
|:-------------------------|:--------------------------------------------------|
| `BLIS_NO_TRANSPOSE`      | ...will be used as given.                         |
| `BLIS_TRANSPOSE`         | ...will be implicitly transposed.                 |
| `BLIS_CONJ_NO_TRANSPOSE` | ...will be implicitly conjugated.                 |
| `BLIS_CONJ_TRANSPOSE`    | ...will be implicitly transposed _and_ conjugated. |

| `conj_t`             | Semantic meaning: Corresponding matrix/vector operand... |
|:---------------------|:---------------------------------------------------------|
| `BLIS_NO_CONJUGATE`  | ...will be used as given.                                |
| `BLIS_CONJUGATE`     | ...will be implicitly conjugated.                        |

| `side_t`     | Semantic meaning: Corresponding matrix operand...  |
|:-------------|:---------------------------------------------------|
| `BLIS_LEFT`  | ...appears on the left.                            |
| `BLIS_RIGHT` | ...appears on the right.                           |

| `uplo_t`     | Semantic meaning: Corresponding matrix operand... |
|:-------------|:--------------------------------------------------|
| `BLIS_LOWER` | ...is stored in (and will be accessed only from) the lower triangle. |
| `BLIS_UPPER` | ...is stored in (and will be accessed only from) the upper triangle. |
| `BLIS_DENSE` | ...is stored as a full matrix (ie: in both triangles).               |

| `diag_t`            | Semantic meaning: Corresponding matrix operand... |
|:--------------------|:--------------------------------------------------|
| `BLIS_NONUNIT_DIAG` | ...has a non-unit diagonal that should be explicitly read from. |
| `BLIS_UNIT_DIAG`    | ...has a unit diagonal that should be implicitly assumed (and not read from). |



## BLIS header file ##

All BLIS definitions and prototypes may be included in your C source file by including a single header file:

```
#include "blis.h"
```


## Initialization and Cleanup ##

BLIS libraries require explicit initialization and finalization at runtime.

```
err_t bli_init( void );
err_t bli_finalize( void );
```

`bli_init()` must be called by the application code before any other BLIS function. Similarly, after the application is done using BLIS functionality, it should call `bli_finalize()`.

`bli_init()` returns an `err_t` value: `BLIS_SUCCESS` if BLIS was actually initialized and `BLIS_FAILURE` if BLIS was already initialized. Similarly, `bli_finalize()` returns `BLIS_SUCCESS` if BLIS was actually finalized and `BLIS_FAILURE` if BLIS was already uninitialized.

**Note:** There are **two** exceptions to the rule of needing to call `bli_init()` and `bli_finalize()` explicitly:
  * The first exception is in the case of invoking BLIS via the BLAS compatibility layer. When BLAS interfaces are used to invoke BLIS implementations, initialization happens automatically the first time BLIS functionality is called.
  * The second exception is in the case of invoking [BLIS info query functions](BLISAPIQuickReference#Query_function_reference.md). BLIS does **not** need to be initialized in order to call any routine beginning with `bli_info_`.


# Computational function reference #

Notes for interpreting the following prototypes:

  * Any occurrence of `?` should be replaced with `s`, `d`, `c`, or `z` to form an actual function name.
  * Any occurrence of `ctype` should be replaced with the actual C type corresponding to the datatype instance in question, while `rtype` should be replaced by the real projection of `ctype`. For example:
    * If we consider the prototype for `bli_zaxpyv()` below, `ctype` refers to `dcomplex`.
    * If we consider the prototype for `bli_zfnormv()` below, `ctype` refers to `dcomplex` while `rtype` refers to `double`.
  * Any occurrence of `itype` should be replaced with the general-purpose signed integer type, `gint_t`.
  * All vector arguments have associated increments that proceed them, typically listed as `incX` for a given vector `x`. The semantic meaning of a vector increment is "the distance, in units of elements, between any two adjacent elements in the vector."
  * All matrix arguments have associated row and column strides arguments that proceed them, typically listed as `rsX` and `csX` for a given matrix `X`. Row strides are always listed first, and column strides are always listed second. The semantic meaning of a row stride is "the distance, in units of elements, to the next row (within a column)," and the meaning of a column stride is "the distance, in units of elements, to the next column (within a row)." Thus, unit row stride implies column-major storage and unit column stride implies row-major storage.

Notes for interpreting function descriptions:
  * `conjX()` and `transX()` should be interpreted as predicates that capture the operand X with any value of `conj_t` or `trans_t` applied. For example:
    * `conjx(x)` refers to a vector `x` that is either conjugated or used as given.
    * `transa(A)` refers to a matrix `A` that is either transposed, conjugated _and_ transposed, conjugated only, or used as given.
  * Any operand marked with `conj()` is unconditionally conjugated.
  * Any operand marked with `^T` is unconditionally transposed. Similarly, any operand that is marked with `^H` is unconditionally conjugate-transposed.
  * All occurrences of `alpha`, `beta`, and `rho` parameters are scalars.


---


## Level-1v operations ##

Level-1v operations perform various level-1 BLAS-like operations on vectors (hence the _v_).


---

#### addv ####
```
void bli_?addv( conj_t conjx,
                dim_t  m,
                ctype* x, inc_t incx,
                ctype* y, inc_t incy );
```
Perform
```
  y := y + conjx(x)
```

where `y` and `x` are vectors of length _m_.

---

#### axpyv ####
```
void bli_?axpyv( conj_t conjx,
                 dim_t  m,
                 ctype* alpha,
                 ctype* x, inc_t incx,
                 ctype* y, inc_t incy );
```
Perform
```
  y := y + alpha * conjx(x)
```

where `y` and `x` are vectors of length _m_.

---

#### copyv ####
```
void bli_?copyv( conj_t conjx,
                 dim_t  m,
                 ctype* x, inc_t incx,
                 ctype* y, inc_t incy );
```
Perform
```
  y := conjx(x)
```

where `y` and `x` are vectors of length _m_.

---

#### dotv ####
```
void bli_?dotv( conj_t conjx,
                conj_t conjy,
                dim_t  m,
                ctype* x, inc_t incx,
                ctype* y, inc_t incy,
                ctype* rho );
```
Perform
```
  rho := conjx(x)^T * conjy(y)
```

where `y` and `x` are vectors of length _m_ and `rho` is a scalar.

---

#### dotxv ####
```
void bli_?dotxv( conj_t conjx,
                 conj_t conjy,
                 dim_t  m,
                 ctype* alpha,
                 ctype* x, inc_t incx,
                 ctype* y, inc_t incy,
                 ctype* beta,
                 ctype* rho );
```
Perform
```
  rho := beta * rho + alpha * conjx(x)^T * conjy(y)
```

where `y` and `x` are vectors of length _m_ and `rho` is a scalar.

---

#### invertv ####
```
void bli_?invertv( dim_t  m,
                   ctype* x, inc_t incx );
```
Invert all elements of an _m_-length vector `x`.

---

#### fnormv ####
```
void bli_?fnormv( dim_t  m,
                  ctype* x, inc_t incx,
                  rtype* norm );
```
Compute the Frobenius norm of an _m_-length vector `x` and store the result in a scalar `norm`.

**Note:** The floating-point type of `norm` is always the real projection of the floating-point type of `x`.

---

#### scalv ####
```
void bli_?scalv( conj_t conjalpha,
                 dim_t  m,
                 ctype* alpha,
                 ctype* x, inc_t incx );
```
Perform
```
  x := conjalpha(alpha) * x
```

where `x` is a vector of length _m_.

---

#### scal2v ####
```
void bli_?scal2v( conj_t conjx,
                  dim_t  m,
                  ctype* alpha,
                  ctype* x, inc_t incx,
                  ctype* y, inc_t incy );
```
Perform
```
  y := alpha * conjx(x)
```

where `y` and `x` are vectors of length _m_.

---

#### setv ####
```
void bli_?setv( dim_t  m,
                ctype* alpha,
                ctype* x, inc_t incx );
```
Set all elements of an _m_-length vector `x` to `alpha`.

---

#### subv ####
```
void bli_?subv( conj_t conjx,
                dim_t  m,
                ctype* x, inc_t incx,
                ctype* y, inc_t incy );
```
Perform
```
  y := y - conjx(x)
```

where `y` and `x` are vectors of length _m_.

---

#### swapv ####
```
void bli_?swapv( dim_t  m,
                 ctype* x, inc_t incx,
                 ctype* y, inc_t incy );
```
Swap corresponding elements of two _m_-length vectors `x` and `y`.

---





## Level-1d operations ##

Level-1d operations perform various level-1 BLAS-like operations on matrix diagonals (hence the _d_).

These operations are similar to their level-1m counterparts, except they only read and update matrix diagonals. Please see the descriptions for the corresponding level-1m operation for a description of the arguments.

---

#### addd ####
```
void bli_?addd( doff_t  diagoffa,
                diag_t  diaga,
                trans_t transa,
                dim_t   m,
                dim_t   n,
                ctype*  a, inc_t rsa, inc_t csa,
                ctype*  b, inc_t rsb, inc_t csb );
```

---

#### axpyd ####
```
void bli_?axpyd( doff_t  diagoffa,
                 diag_t  diaga,
                 trans_t transa,
                 dim_t   m,
                 dim_t   n,
                 ctype*  alpha,
                 ctype*  a, inc_t rsa, inc_t csa,
                 ctype*  b, inc_t rsb, inc_t csb );
```

---

#### copyd ####
```
void bli_?copyd( doff_t  diagoffa,
                 diag_t  diaga,
                 trans_t transa,
                 dim_t   m,
                 dim_t   n,
                 ctype*  a, inc_t rsa, inc_t csa,
                 ctype*  b, inc_t rsb, inc_t csb );
```

---

#### invertd ####
```
void bli_?invertd( doff_t  diagoffa,
                   dim_t   m,
                   dim_t   n,
                   ctype*  a, inc_t rsa, inc_t csa );
```

---

#### scald ####
```
void bli_?scald( conj_t  conjalpha,
                 doff_t  diagoffa,
                 dim_t   m,
                 dim_t   n,
                 ctype*  alpha,
                 ctype*  a, inc_t rsa, inc_t csa );
```

---

#### scal2d ####
```
void bli_?scal2d( doff_t  diagoffa,
                  diag_t  diaga,
                  trans_t transa,
                  dim_t   m,
                  dim_t   n,
                  ctype*  alpha,
                  ctype*  a, inc_t rsa, inc_t csa,
                  ctype*  b, inc_t rsb, inc_t csb );
```

---

#### setd ####
```
void bli_?setd( doff_t  diagoffa,
                dim_t   m,
                dim_t   n,
                ctype*  alpha,
                ctype*  a, inc_t rsa, inc_t csa );
```

---

#### subd ####
```
void bli_?subd( doff_t  diagoffa,
                diag_t  diaga,
                trans_t transa,
                dim_t   m,
                dim_t   n,
                ctype*  a, inc_t rsa, inc_t csa,
                ctype*  b, inc_t rsb, inc_t csb );
```

---



## Level-1m operations ##

Level-1m operations perform various level-1 BLAS-like operations on matrices (hence the _m_).

---

#### addm ####
```
void bli_?addm( doff_t  diagoffa,
                diag_t  diaga,
                uplo_t  uploa,
                trans_t transa,
                dim_t   m,
                dim_t   n,
                ctype*  a, inc_t rsa, inc_t csa,
                ctype*  b, inc_t rsb, inc_t csb );
```
Perform
```
  B := B + transa(A)
```

where `B` is an _m x n_ matrix, `A` is stored as a dense matrix, or lower- or upper-triangular/trapezoidal matrix, as specified by `uploa`, with the diagonal offset of `A` specified by `diagoffa` and unit/non-unit nature of the diagonal specified by `diaga`. If `uploa` indicates lower or upper storage, only that part of matrix `A` will be referenced and used to update `B`.

---

#### axpym ####
```
void bli_?axpym( doff_t  diagoffa,
                 diag_t  diaga,
                 uplo_t  uploa,
                 trans_t transa,
                 dim_t   m,
                 dim_t   n,
                 ctype*  alpha,
                 ctype*  a, inc_t rsa, inc_t csa,
                 ctype*  b, inc_t rsb, inc_t csb );
```
Perform
```
  B := B + alpha * transa(A)
```

where `B` is an _m x n_ matrix, `A` is stored as a dense matrix, or lower- or upper-triangular/trapezoidal matrix, as specified by `uploa`, with the diagonal offset of `A` specified by `diagoffa` and unit/non-unit nature of the diagonal specified by `diaga`. If `uploa` indicates lower or upper storage, only that part of matrix `A` will be referenced and used to update `B`.

---

#### copym ####
```
void bli_?copym( doff_t  diagoffa,
                 diag_t  diaga,
                 uplo_t  uploa,
                 trans_t transa,
                 dim_t   m,
                 dim_t   n,
                 ctype*  a, inc_t rsa, inc_t csa,
                 ctype*  b, inc_t rsb, inc_t csb );
```
Perform
```
  B := transa(A)
```

where `B` is an _m x n_ matrix, `A` is stored as a dense matrix, or lower- or upper-triangular/trapezoidal matrix, as specified by `uploa`, with the diagonal offset of `A` specified by `diagoffa` and unit/non-unit nature of the diagonal specified by `diaga`. If `uploa` indicates lower or upper storage, only that part of matrix `A` will be referenced and used to update `B`.

---

#### fnormm ####
```
void bli_?fnormm( doff_t  diagoffa,
                  diag_t  diaga,
                  uplo_t  uploa,
                  dim_t   m,
                  dim_t   n,
                  ctype*  a, inc_t rsa, inc_t csa,
                  rtype*  norm );
```
Compute the Frobenius norm of an _m x n_ matrix `A` and store the result in a scalar `norm`. `A` is stored as a dense matrix, or lower- or upper-triangular/trapezoidal matrix, as specified by `uploa`, with the diagonal offset of `A` specified by `diagoffa` and unit/non-unit nature of the diagonal specified by `diaga`. If `uploa` indicates lower or upper storage, only that part of matrix `A` will be referenced and used to compute `norm`.

---

#### scalm ####
```
void bli_?scalm( conj_t  conjalpha,
                 doff_t  diagoffa,
                 uplo_t  uploa,
                 dim_t   m,
                 dim_t   n,
                 ctype*  alpha,
                 ctype*  a, inc_t rsa, inc_t csa );
```
Perform
```
  A := conjalpha(alpha) * A
```

where `A` is an _m x n_ matrix stored as a dense matrix, or lower- or upper-triangular/trapezoidal matrix, as specified by `uploa`, with the diagonal offset of `A` specified by `diagoffa`. If `uploa` indicates lower or upper storage, only that part of matrix `A` will be updated.

---

#### scal2m ####
```
void bli_?scal2m( doff_t  diagoffa,
                  diag_t  diaga,
                  uplo_t  uploa,
                  trans_t transa,
                  dim_t   m,
                  dim_t   n,
                  ctype*  alpha,
                  ctype*  a, inc_t rsa, inc_t csa,
                  ctype*  b, inc_t rsb, inc_t csb );
```
Perform
```
  B := alpha * transa(A)
```

where `B` is an _m x n_ matrix, `A` is stored as a dense matrix, or lower- or upper-triangular/trapezoidal matrix, as specified by `uploa`, with the diagonal offset of `A` specified by `diagoffa` and unit/non-unit nature of the diagonal specified by `diaga`. If `uploa` indicates lower or upper storage, only that part of matrix `A` will be referenced and used to update `B`.

---

#### setm ####
```
void bli_?setm( doff_t  diagoffa,
                diag_t  diaga,
                uplo_t  uploa,
                dim_t   m,
                dim_t   n,
                ctype*  alpha,
                ctype*  a, inc_t rsa, inc_t csa );
```
Set all elements of an _m x n_ matrix `A` to `alpha`, where `A` is stored as a dense matrix, or lower- or upper- triangular/trapezoidal matrix, as specified by `uploa`, with the diagonal offset of `A` specified by `diagoffa` and unit/non-unit nature of the diagonal specified by `diaga`. If `uploa` indicates lower or upper storage, only that part of matrix `A` will be updated.

---

#### subm ####
```
void bli_?subm( doff_t  diagoffa,
                diag_t  diaga,
                uplo_t  uploa,
                trans_t transa,
                dim_t   m,
                dim_t   n,
                ctype*  a, inc_t rsa, inc_t csa,
                ctype*  b, inc_t rsb, inc_t csb );
```
Perform
```
  B := B - transa(A)
```

where `B` is an _m x n_ matrix, `A` is stored as a dense matrix, or lower- or upper-triangular/trapezoidal matrix, as specified by `uploa`, with the diagonal offset of `A` specified by `diagoffa` and unit/non-unit nature of the diagonal specified by `diaga`. If `uploa` indicates lower or upper storage, only that part of matrix `A` will be referenced and used to update `B`.

---





## Level-1f kernels ##

Level-1f kernels implement various fused combinations of level-1 operations (hence the _f_). These kernels are important when optimized level-2 operations are desired.


---

#### axpy2v ####
```
void bli_?axpy2v( conj_t  conjx,
                  conj_t  conjy,
                  dim_t   m,
                  ctype*  alpha1,
                  ctype*  alpha2,
                  ctype*  x, inc_t incx,
                  ctype*  y, inc_t incy,
                  ctype*  z, inc_t incz );
```
Perform
```
  y := y + alpha1 * conjx(x) + alpha2 * conjy(y)
```

where `x`, `y`, and `z` are vectors of length _m_. The kernel, if optimized, is implemented as a fused pair of calls to [axpyv](BLISAPIQuickReference#axpyv.md).

---

#### dotaxpyv ####
```
void bli_?dotaxpyv( conj_t  conjxt,
                    conj_t  conjx,
                    conj_t  conjy,
                    dim_t   m,
                    ctype*  alpha,
                    ctype*  x, inc_t incx,
                    ctype*  y, inc_t incy,
                    ctype*  rho,
                    ctype*  z, inc_t incz );
```
Perform
```
  rho := conjxt(x^T) * conjy(y)
  y   := y + alpha * conjx(x)
```

where `x`, `y`, and `z` are vectors of length _m_ and `alpha` and `rho` are scalars. The kernel, if optimized, is implemented as a fusion of calls to [dotv](BLISAPIQuickReference#dotv.md) and [axpyv](BLISAPIQuickReference#axpyv.md).

---

#### axpyf ####
```
void bli_?axpyf( conj_t  conja,
                 conj_t  conjx,
                 dim_t   m,
                 dim_t   nf,
                 ctype*  alpha,
                 ctype*  a, inc_t inca, inc_t lda,
                 ctype*  x, inc_t incx,
                 ctype*  y, inc_t incy );
```
Perform
```
  y := y + alpha * conja(A) * conjx(x)
```

where `A` is an _m x nf_ matrix, and `y` and `x` are vectors. The kernel, if optimized, is implemented as a fused series of calls to [axpyv](BLISAPIQuickReference#axpyv.md) where _nf_ is less than or equal to an implementation-dependent fusing factor specific to `axpyf`.

---

#### dotxf ####
```
void bli_?dotxf( conj_t  conjat,
                 conj_t  conjx,
                 dim_t   m,
                 dim_t   nf,
                 ctype*  alpha,
                 ctype*  a, inc_t inca, inc_t lda,
                 ctype*  x, inc_t incx,
                 ctype*  beta,
                 ctype*  y, inc_t incy );
```
Perform
```
  y := y + alpha * conjat(A^T) * conjx(x)
```

where `A` is an _m x nf_ matrix, and `y` and `x` are vectors. The kernel, if optimized, is implemented as a fused series of calls to [dotxv](BLISAPIQuickReference#dotxv.md) where _nf_ is less than or equal to an implementation-dependent fusing factor specific to `dotxf`.

---

#### dotxaxpyf ####
```
void bli_?dotxaxpyf( conj_t  conjat,
                     conj_t  conja,
                     conj_t  conjw,
                     conj_t  conjx,
                     dim_t   m,
                     dim_t   nf,
                     ctype*  alpha,
                     ctype*  a, inc_t inca, inc_t lda,
                     ctype*  w, inc_t incw,
                     ctype*  x, inc_t incx,
                     ctype*  beta,
                     ctype*  y, inc_t incy,
                     ctype*  z, inc_t incz );
```
Perform
```
  y := beta * y + alpha * conjat(A^T) * conjw(w)
  z :=        z + alpha * conja(A)    * conjx(x)
```

where `A` is an _m x nf_ matrix, `w` and `z` are vectors of length _m_, `x` and `y` are vectors of length `nf`, and `alpha` and `beta` are scalars. The kernel, if optimized, is implemented as a fusion of calls to [dotxf](BLISAPIQuickReference#dotxf.md) and [axpyf](BLISAPIQuickReference#axpyf.md).



## Level-2 operations ##

Level-2 operations perform various level-2 BLAS-like operations.


---

#### gemv ####
```
void bli_?gemv( trans_t transa,
                conj_t  conjx,
                dim_t   m,
                dim_t   n,
                ctype*  alpha,
                ctype*  a, inc_t rsa, inc_t csa,
                ctype*  x, inc_t incx,
                ctype*  beta,
                ctype*  y, inc_t incy );
```
Perform
```
  y := beta * y + alpha * transa(A) * conjx(x)
```

where `transa(A)` is an _m x n_ matrix, and `y` and `x` are vectors.

---

#### ger ####
```
void bli_?ger( conj_t  conjx,
               conj_t  conjy,
               dim_t   m,
               dim_t   n,
               ctype*  alpha,
               ctype*  x, inc_t incx,
               ctype*  y, inc_t incy,
               ctype*  a, inc_t rsa, inc_t csa );
```
Perform
```
  A := A + alpha * conjx(x) * conjy(y)^T
```

where `A` is an _m x n_ matrix, and `x` and `y` are vectors of length _m_ and _n_, respectively.

---

#### hemv ####
```
void bli_?hemv( uplo_t  uploa,
                conj_t  conja,
                conj_t  conjx,
                dim_t   m,
                ctype*  alpha,
                ctype*  a, inc_t rsa, inc_t csa,
                ctype*  x, inc_t incx,
                ctype*  beta,
                ctype*  y, inc_t incy );
```
Perform
```
  y := beta * y + alpha * conja(A) * conjx(x)
```

where `A` is an _m x m_ Hermitian matrix stored in the lower or upper triangle as specified by `uploa`, and `y` and `x` are vectors of length _m_.

---

#### her ####
```
void bli_?her( uplo_t  uploa,
               conj_t  conjx,
               dim_t   m,
               rtype*  alpha,
               ctype*  x, inc_t incx,
               ctype*  a, inc_t rsa, inc_t csa );
```
Perform
```
  A := A + alpha * conjx(x) * conjx(x)^H
```

where `A` is an _m x m_ Hermitian matrix stored in the lower or upper triangle as specified by `uploa`, and `x` is a vector of length _m_.

**Note:** The floating-point type of `alpha` is always the real projection of the floating-point types of `x` and `A`.

---

#### her2 ####
```
void bli_?her2( uplo_t  uploa,
                conj_t  conjx,
                dim_t   m,
                ctype*  alpha,
                ctype*  x, inc_t incx,
                ctype*  y, inc_t incy,
                ctype*  a, inc_t rsa, inc_t csa );
```
Perform
```
  A := A + alpha * conjx(x) * conjy(y)^H + conj(alpha) * conjy(y) * conjx(x)^H
```

where `A` is an _m x m_ Hermitian matrix stored in the lower or upper triangle as specified by `uploa`, and `x` and `y` are vectors of length _m_.

---

#### symv ####
```
void bli_?symv( uplo_t  uploa,
                conj_t  conja,
                conj_t  conjx,
                dim_t   m,
                ctype*  alpha,
                ctype*  a, inc_t rsa, inc_t csa,
                ctype*  x, inc_t incx,
                ctype*  beta,
                ctype*  y, inc_t incy );
```
Perform
```
  y := beta * y + alpha * conja(A) * conjx(x)
```

where `A` is an _m x m_ symmetric matrix stored in the lower or upper triangle as specified by `uploa`, and `y` and `x` are vectors of length _m_.

---

#### syr ####
```
void bli_?syr( uplo_t  uploa,
               conj_t  conjx,
               dim_t   m,
               ctype*  alpha,
               ctype*  x, inc_t incx,
               ctype*  a, inc_t rsa, inc_t csa );
```
Perform
```
  A := A + alpha * conjx(x) * conjx(x)^T
```

where `A` is an _m x m_ symmetric matrix stored in the lower or upper triangle as specified by `uploa`, and `x` is a vector of length _m_.

---

#### syr2 ####
```
void bli_?syr2( uplo_t  uploa,
                conj_t  conjx,
                dim_t   m,
                ctype*  alpha,
                ctype*  x, inc_t incx,
                ctype*  y, inc_t incy,
                ctype*  a, inc_t rsa, inc_t csa );
```
Perform
```
  A := A + alpha * conjx(x) * conjy(y)^T + conj(alpha) * conjy(y) * conjx(x)^T
```

where `A` is an _m x m_ symmetric matrix stored in the lower or upper triangle as specified by `uploa`, and `x` and `y` are vectors of length _m_.

---

#### trmv ####
```
void bli_?trmv( uplo_t  uploa,
                trans_t transa,
                diag_t  diaga,
                dim_t   m,
                ctype*  alpha,
                ctype*  a, inc_t rsa, inc_t csa,
                ctype*  x, inc_t incx );
```
Perform
```
  x := alpha * transa(A) * x
```

where `A` is an _m x m_ triangular matrix stored in the lower or upper triangle as specified by `uploa` with unit/non-unit nature specified by `diaga`, and `x` is a vector of length _m_.

---

#### trsv ####
```
void bli_?trsv( uplo_t  uploa,
                trans_t transa,
                diag_t  diaga,
                dim_t   m,
                ctype*  alpha,
                ctype*  a, inc_t rsa, inc_t csa,
                ctype*  y, inc_t incy );
```
Solve the linear system
```
  transa(A) * x = alpha * y
```

where `A` is an _m x m_ triangular matrix stored in the lower or upper triangle as specified by `uploa` with unit/non-unit nature specified by `diaga`, and `x` and `y` are vectors of length _m_. The right-hand side vector operand `y` is overwritten with the solution vector `x`.

---



## Level-3 operations ##

Level-3 operations perform various level-3 BLAS-like operations.


---

#### gemm ####
```
void bli_?gemm( trans_t transa,
                trans_t transb,
                dim_t   m,
                dim_t   n,
                dim_t   k,
                ctype*  alpha,
                ctype*  a, inc_t rsa, inc_t csa,
                ctype*  b, inc_t rsb, inc_t csb,
                ctype*  beta,
                ctype*  c, inc_t rsc, inc_t csc );
```
Perform
```
  C := beta * C + alpha * transa(A) * transb(B)
```

where C is an _m x n_ matrix, `transa(A)` is an _m x k_ matrix, and `transb(B)` is a _k x n_ matrix.

---

#### hemm ####
```
void bli_?hemm( side_t  sidea,
                uplo_t  uploa,
                conj_t  conja,
                trans_t transb,
                dim_t   m,
                dim_t   n,
                ctype*  alpha,
                ctype*  a, inc_t rsa, inc_t csa,
                ctype*  b, inc_t rsb, inc_t csb,
                ctype*  beta,
                ctype*  c, inc_t rsc, inc_t csc );
```
Perform
```
  C := beta * C + alpha * conja(A) * transb(B)
```
if `sidea` is `BLIS_LEFT`, or
```
  C := beta * C + alpha * transb(B) * conja(A)
```
if `sidea` is `BLIS_RIGHT`, where `C` and `B` are _m x n_ matrices and `A` is a Hermitian matrix stored in the lower or upper triangle as specified by `uploa`. When `sidea` is `BLIS_LEFT`, `A` is _m x m_, and when `sidea` is `BLIS_RIGHT`, `A` is _n x n_.

---

#### herk ####
```
void bli_?herk( uplo_t  uploc,
                trans_t transa,
                dim_t   m,
                dim_t   k,
                rtype*  alpha,
                ctype*  a, inc_t rsa, inc_t csa,
                rtype*  beta,
                ctype*  c, inc_t rsc, inc_t csc );
```
Perform
```
  C := beta * C + alpha * transa(A) * transa(A)^H
```

where C is an _m x m_ Hermitian matrix stored in the lower or upper triangle as specified by `uploa` and `transa(A)` is an _m x k_ matrix.

**Note:** The floating-point types of `alpha` and `beta` are always the real projection of the floating-point types of `A` and `C`.

---

#### her2k ####
```
void bli_?her2k( uplo_t  uploc,
                 trans_t transa,
                 dim_t   m,
                 dim_t   k,
                 ctype*  alpha,
                 ctype*  a, inc_t rsa, inc_t csa,
                 ctype*  b, inc_t rsb, inc_t csb,
                 rtype*  beta,
                 ctype*  c, inc_t rsc, inc_t csc );
```
Perform
```
  C := beta * C + alpha * transa(A) * transb(B)^H + conj(alpha) * transb(B) * transa(A)^H
```

where C is an _m x m_ Hermitian matrix stored in the lower or upper triangle as specified by `uploa` and `transa(A)` and `transb(B)` are _m x k_ matrices.

**Note:** The floating-point type of `beta` is always the real projection of the floating-point types of `A` and `C`.

---

#### symm ####
```
void bli_?symm( side_t  sidea,
                uplo_t  uploa,
                conj_t  conja,
                trans_t transb,
                dim_t   m,
                dim_t   n,
                ctype*  alpha,
                ctype*  a, inc_t rsa, inc_t csa,
                ctype*  b, inc_t rsb, inc_t csb,
                ctype*  beta,
                ctype*  c, inc_t rsc, inc_t csc );
```
Perform
```
  C := beta * C + alpha * conja(A) * transb(B)
```
if `sidea` is `BLIS_LEFT`, or
```
  C := beta * C + alpha * transb(B) * conja(A)
```
if `sidea` is `BLIS_RIGHT`, where `C` and `B` are _m x n_ matrices and `A` is a symmetric matrix stored in the lower or upper triangle as specified by `uploa`. When `sidea` is `BLIS_LEFT`, `A` is _m x m_, and when `sidea` is `BLIS_RIGHT`, `A` is _n x n_.

---

#### syrk ####
```
void bli_?syrk( uplo_t  uploc,
                trans_t transa,
                dim_t   m,
                dim_t   k,
                ctype*  alpha,
                ctype*  a, inc_t rsa, inc_t csa,
                ctype*  beta,
                ctype*  c, inc_t rsc, inc_t csc );
```
Perform
```
  C := beta * C + alpha * transa(A) * transa(A)^T
```

where C is an _m x m_ symmetric matrix stored in the lower or upper triangle as specified by `uploa` and `transa(A)` is an _m x k_ matrix.

---

#### syr2k ####
```
void bli_?syr2k( uplo_t  uploc,
                 trans_t transa,
                 dim_t   m,
                 dim_t   k,
                 ctype*  alpha,
                 ctype*  a, inc_t rsa, inc_t csa,
                 ctype*  b, inc_t rsb, inc_t csb,
                 ctype*  beta,
                 ctype*  c, inc_t rsc, inc_t csc );
```
Perform
```
  C := beta * C + alpha * transa(A) * transb(B)^T + alpha * transb(B) * transa(A)^T
```

where C is an _m x m_ symmetric matrix stored in the lower or upper triangle as specified by `uploa` and `transa(A)` and `transb(B)` are _m x k_ matrices.

---

#### trmm ####
```
void bli_?trmm( side_t  sidea,
                uplo_t  uploa,
                trans_t transa,
                diag_t  diaga,
                dim_t   m,
                dim_t   n,
                ctype*  alpha,
                ctype*  a, inc_t rsa, inc_t csa,
                ctype*  b, inc_t rsb, inc_t csb );
```
Perform
```
  B := alpha * transa(A) * B
```
if `sidea` is `BLIS_LEFT`, or
```
  B := alpha * B * transa(A)
```
if `sidea` is `BLIS_RIGHT`, where `B` is an _m x n_ matrix and `A` is a triangular matrix stored in the lower or upper triangle as specified by `uploa` with unit/non-unit nature specified by `diaga`. When `sidea` is `BLIS_LEFT`, `A` is _m x m_, and when `sidea` is `BLIS_RIGHT`, `A` is _n x n_.

---

#### trmm3 ####
```
void bli_?trmm3( side_t  sidea,
                 uplo_t  uploa,
                 trans_t transa,
                 diag_t  diaga,
                 trans_t transb,
                 dim_t   m,
                 dim_t   n,
                 ctype*  alpha,
                 ctype*  a, inc_t rsa, inc_t csa,
                 ctype*  b, inc_t rsb, inc_t csb,
                 ctype*  beta,
                 ctype*  c, inc_t rsc, inc_t csc );
```
Perform
```
  C := beta * C + alpha * transa(A) * transb(B)
```
if `sidea` is `BLIS_LEFT`, or
```
  C := beta * C + alpha * transb(B) * transa(A)
```
if `sidea` is `BLIS_RIGHT`, where `C` and `transb(B)` are _m x n_ matrices and `A` is a triangular matrix stored in the lower or upper triangle as specified by `uploa` with unit/non-unit nature specified by `diaga`. When `sidea` is `BLIS_LEFT`, `A` is _m x m_, and when `sidea` is `BLIS_RIGHT`, `A` is _n x n_.

---

#### trsm ####
```
void bli_?trsm( side_t  sidea,
                uplo_t  uploa,
                trans_t transa,
                diag_t  diaga,
                dim_t   m,
                dim_t   n,
                ctype*  alpha,
                ctype*  a, inc_t rsa, inc_t csa,
                ctype*  b, inc_t rsb, inc_t csb );
```
Solve the linear system with multiple right-hand sides
```
  transa(A) * X = alpha * B
```
if `sidea` is `BLIS_LEFT`, or
```
  X * transa(A) = alpha * B
```
if `sidea` is `BLIS_RIGHT`, where `X` and `B` are an _m x n_ matrices and `A` is a triangular matrix stored in the lower or upper triangle as specified by `uploa` with unit/non-unit nature specified by `diaga`. When `sidea` is `BLIS_LEFT`, `A` is _m x m_, and when `sidea` is `BLIS_RIGHT`, `A` is _n x n_. The right-hand side matrix operand `B` is overwritten with the solution matrix `X`.

---


## Utility operations ##

---

#### abmaxv ####
```
void bli_?abmaxv( dim_t   n,
                  ctype*  x, inc_t incx,
                  itype*  index );
```
Find the element of vector `x` which contains the maximum absolute value. The index of the element found is stored to `index`.

---

#### absumm ####
```
void bli_?absumm( doff_t  diagoffa,
                  uplo_t  uploa,
                  dim_t   m,
                  dim_t   n,
                  ctype*  a, inc_t rs_a, inc_t cs_a,
                  rtype*  absum );
```
Compute the sum of the absolute values of the elements in an _m x n_ matrix `A`. If `uploa` is `BLIS_LOWER` or `BLIS_UPPER` then `A` is assumed to be lower or upper triangular, respectively, with the main diagonal located at offset `diagoffa`. The resulting sum is stored to `absum`.

---

#### absumv ####
```
void bli_?absumv( dim_t   n,
                  ctype*  x, inc_t incx,
                  rtype*  absum );
```
Compute the sum of the absolute values of the elements in a vector `x` of length _n_. The resulting sum is stored to `absum`.

---

#### mkherm ####
```
void bli_?mkherm( uplo_t  uploa,
                  dim_t   m,
                  ctype*  a, inc_t rs_a, inc_t cs_a );
```
Make an _m x m_ matrix `A` explicitly Hermitian by copying the conjugate of the triangle specified by `uploa` to the opposite triangle. Imaginary components of diagonal elements are explicitly set to zero. It is assumed that the diagonal offset of `A` is zero.

---

#### mksymm ####
```
void bli_?mksymm( uplo_t  uploa,
                  dim_t   m,
                  ctype*  a, inc_t rs_a, inc_t cs_a );
```
Make an _m x m_ matrix `A` explicitly symmetric by copying the triangle specified by `uploa` to the opposite triangle. It is assumed that the diagonal offset of `A` is zero.

---

#### mktrim ####
```
void bli_?mktrim( uplo_t  uploa,
                  dim_t   m,
                  ctype*  a, inc_t rs_a, inc_t cs_a );
```
Make an _m x m_ matrix `A` explicitly triangular by preserving the triangle specified by `uploa` and zeroing the elements in the opposite triangle. It is assumed that the diagonal offset of `A` is zero.

---

#### fprintv ####
```
void bli_?fprintv( FILE*   file,
                   char*   s1,
                   dim_t   m,
                   ctype*  x, inc_t incx,
                   char*   format,
                   char*   s2 );
```
Print a vector `x` of length _m_ to file stream `file`, where `file` is a file pointer returned by the standard C library function `fopen()`. The caller may also pass in a global file pointer such as `stdout` or `stderr`. The strings `s1` and `s2` are printed immediately before and after the output (respectively), and the format specifier `format` is used to format the individual elements. For valid format specifiers, please see documentation for the standard C library function `printf()`.

**Note:** For complex datatypes, the format specifier is applied to both the real and imaginary components **individually**. Therefore, you should use format specifiers such as `"%5.2f"`, but **not** `"%5.2f + %5.2f"`.

---

#### fprintm ####
```
void bli_?fprintm( FILE*   file,
                   char*   s1,
                   dim_t   m,
                   dim_t   n,
                   ctype*  a, inc_t rs_a, inc_t cs_a,
                   char*   format,
                   char*   s2 );
```
Print an _m x n_ matrix `A` to file stream `file`, where `file` is a file pointer returned by the standard C library function `fopen()`. The caller may also pass in a global file pointer such as `stdout` or `stderr`. The strings `s1` and `s2` are printed immediately before and after the output (respectively), and the format specifier `format` is used to format the individual elements. For valid format specifiers, please see documentation for the standard C library function `printf()`.

**Note:** For complex datatypes, the format specifier is applied to both the real and imaginary components **individually**. Therefore, you should use format specifiers such as `"%5.2f"`, but **not** `"%5.2f + %5.2f"`.

---

#### printv ####
```
void bli_?printv( char*   s1,
                  dim_t   m,
                  ctype*  x, inc_t incx,
                  char*   format,
                  char*   s2 );
```
Print a vector `x` of length _m_ to standard output. This function call is equivalent to calling `bli_?fprintv()` with `stdout` as the file pointer.

---

#### printm ####
```
void bli_?printm( char*   s1,
                  dim_t   m,
                  dim_t   n,
                  ctype*  a, inc_t rs_a, inc_t cs_a,
                  char*   format,
                  char*   s2 );
```
Print an _m x n_ matrix `a` to standard output. This function call is equivalent to calling `bli_?fprintm()` with `stdout` as the file pointer.

---

#### randv ####
```
void bli_?randv( dim_t   m,
                 ctype*  x, inc_t incx );
```
Set the elements of a vector `x` of length _m_ to random values on the interval `[-1,1)`.

**Note:** For complex datatypes, the real and imaginary components of each element are randomized individually and independently of one another.

---

#### randm ####
```
void bli_?randm( diagoff_t diagoffa,
                 uplo_t    uploa,
                 dim_t     m,
                 dim_t     n,
                 ctype*    a, inc_t rs_a, inc_t cs_a );
```
Set the elements of an _m x n_ matrix `A` to random values on the interval `[-1,1)`. If `uploa` is `BLIS_LOWER` or `BLIS_UPPER`, then additional scaling occurs so that the resulting matrix is diagonally dominant. Specifically, the diagonal elements (identified by diagonal offset `diagoffa`) are shifted so that they lie on the interval `[1,2)` and the off-diagonal elements (in the triangle specified by `uploa`) are scaled by `1.0/max(m,n)`.

**Note:** For complex datatypes, the real and imaginary components of each off-diagonal element are randomized individually and independently of one another.

---

#### sumsqv ####
```
void bli_?sumsqv( dim_t   m,
                  ctype*  x, inc_t incx,
                  rtype*  scale,
                  rtype*  sumsq );
```
Compute the sum of the squares of the elements in a vector `x` of length _m_. The result is computed in scaled form, and in such a way that it may be used repeatedly to accumulate the sum of the squares of several vectors.

The function computes scale\_new and sumsq\_new such that
```
  scale_new^2 * sumsq_new = x[0]^2 + x[1]^2 + ... x[m-1]^2 + scale_old^2 * sumsq_old
```

where, on entry, `scale` and `sumsq` contain `scale_old` and `sumsq_old`, respectively, and on exit, `scale` and `sumsq` contain `scale_new` and `sumsq_new`, respectively.

**Note:** This function mimics the approach taken by the algorithm for computing the Frobenius norm in netlib LAPACK's `?lassq()`.

---


## Level-3 micro-kernels ##

**Note:** The `*` in level-3 micro-kernel function names shown below reflect that there is no exact naming convention required for the micro-kernels, except that they must begin with `bli_?`. We strongly recommend, however, that the micro-kernel function names include the name of the micro-kernel itself. For example, the `gemm` micro-kernel should be named with the prefix `bli_?gemm_` and the `trsm` micro-kernels should be named with the prefixes `bli_?trsm_l_` (lower triangular) and `bli_?trsm_u_` (upper triangular).

---

#### gemm micro-kernel ####
```
void bli_?gemm_*(  dim_t           k,
                   ctype* restrict alpha,
                   ctype* restrict a1,
                   ctype* restrict b1,
                   ctype* restrict beta,
                   ctype* restrict c11, inc_t rsc, inc_t csc,
                   auxinfo_t*      data );
```
Perform
```
  C11 := beta * C11 + alpha * A1 * B1
```
where `C11` is an _MR x NR_ matrix, `A1` is an _MR x k_ "micro-panel" matrix stored in packed (column-stored) format, `B1` is a _k x NR_ "micro-panel" matrix in packed (row-stored) format, and alpha and beta are scalars. The storage of `C11` is specified by its row and column strides, `rsc` and `csc`.

Please see the [BLIS Kernel wiki](KernelsHowTo.md) for more information on the `gemm` micro-kernel.


---

#### trsm micro-kernels ####
```
void bli_?trsm_l_*( ctype* restrict a11,
                    ctype* restrict b11,
                    ctype* restrict c11, inc_t rsc, inc_t csc
                    auxinfo_t*      data );

void bli_?trsm_u_*( ctype* restrict a11,
                    ctype* restrict b11,
                    ctype* restrict c11, inc_t rsc, inc_t csc
                    auxinfo_t*      data );
```
Perform
```
  B11 := inv(A11) * B11
  C11 := B11
```
where `A11` is an _MR x MR_ lower or upper triangular matrix stored in packed (column-stored) format, `B11` is an _MR x NR_ matrix stored in packed (row-stored) format, and `C11` is an _MR x NR_ matrix stored according to row and column strides `rsc` and `csc`.

Please see the [BLIS Kernel wiki](KernelsHowTo.md) for more information on the `trsm` micro-kernel.

---

#### gemmtrsm micro-kernels ####
```
void bli_?gemmtrsm_l_*( dim_t           k,
                        ctype* restrict alpha,
                        ctype* restrict a10,
                        ctype* restrict a11,
                        ctype* restrict b01,
                        ctype* restrict b11,
                        ctype* restrict c11, inc_t rs_c, inc_t cs_c,
                        auxinfo_t*      data );

void bli_?gemmtrsm_u_*( dim_t           k,
                        ctype* restrict alpha,
                        ctype* restrict a12,
                        ctype* restrict a11,
                        ctype* restrict b21,
                        ctype* restrict b11,
                        ctype* restrict c11, inc_t rs_c, inc_t cs_c,
                        auxinfo_t*      data );
```
Perform
```
  B11 := alpha * B11 - A10 * B01
  B11 := inv(A11) * B11
  C11 := B11
```
if `A11` is lower triangular, or
```
  B11 := alpha * B11 - A12 * B21
  B11 := inv(A11) * B11
  C11 := B11
```
if `A11` is upper triangular.

Please see the [BLIS Kernel wiki](KernelsHowTo.md) for more information on the `gemmtrsm` micro-kernel.



# Query function reference #

BLIS allows applications to query information about how BLIS was configured. The `bli_info_` API provides several categories of query routines. Most values are returned as a `gint_t`, which is a signed integer. The size of this integer can be queried through a special routine that returns the size in a character string:
```
char* bli_info_get_int_type_size_str( void );
```
**Note:** All of the `bli_info_` functions are **always** thread-safe, no matter how BLIS was configured.

## General library information ##

The following routine returns the address the full BLIS version string:
```
char* bli_info_get_version_str( void );
```

## General configuration ##

The following routines return the corresponding constants set in [bli\_config.h](ConfigurationHowTo#bli_config.h.md).

```
gint_t bli_info_get_int_type_size( void );
gint_t bli_info_get_num_fp_types( void );
gint_t bli_info_get_max_type_size( void );
gint_t bli_info_get_max_num_threads( void );
gint_t bli_info_get_num_mc_x_kc_blocks( void );
gint_t bli_info_get_num_kc_x_nc_blocks( void );
gint_t bli_info_get_num_mc_x_nc_blocks( void );
gint_t bli_info_get_max_preload_byte_offset( void );
gint_t bli_info_get_simd_align_size( void );
gint_t bli_info_get_stack_buf_align_size( void );
gint_t bli_info_get_heap_addr_align_size( void );
gint_t bli_info_get_heap_stride_align_size( void );
gint_t bli_info_get_contig_addr_align_size( void );
gint_t bli_info_get_enable_stay_auto_init( void );
gint_t bli_info_get_enable_blas2blis( void );
gint_t bli_info_get_blas2blis_int_type_size( void );
```

## Kernel configuration ##

The following routines return the corresponding constants set in [bli\_kernel.h](ConfigurationHowTo#bli_kernel.h.md).

```
// Default cache blocksizes

gint_t bli_info_get_default_mc( num_t dt );
gint_t bli_info_get_default_mc_s( void );
gint_t bli_info_get_default_mc_d( void );
gint_t bli_info_get_default_mc_c( void );
gint_t bli_info_get_default_mc_z( void );

gint_t bli_info_get_default_kc( num_t dt );
gint_t bli_info_get_default_kc_s( void );
gint_t bli_info_get_default_kc_d( void );
gint_t bli_info_get_default_kc_c( void );
gint_t bli_info_get_default_kc_z( void );

gint_t bli_info_get_default_nc( num_t dt );
gint_t bli_info_get_default_nc_s( void );
gint_t bli_info_get_default_nc_d( void );
gint_t bli_info_get_default_nc_c( void );
gint_t bli_info_get_default_nc_z( void );

// Maximum cache blocksizes

gint_t bli_info_get_maximum_mc( num_t dt );
gint_t bli_info_get_maximum_mc_s( void );
gint_t bli_info_get_maximum_mc_d( void );
gint_t bli_info_get_maximum_mc_c( void );
gint_t bli_info_get_maximum_mc_z( void );

gint_t bli_info_get_maximum_kc( num_t dt );
gint_t bli_info_get_maximum_kc_s( void );
gint_t bli_info_get_maximum_kc_d( void );
gint_t bli_info_get_maximum_kc_c( void );
gint_t bli_info_get_maximum_kc_z( void );

gint_t bli_info_get_maximum_nc( num_t dt );
gint_t bli_info_get_maximum_nc_s( void );
gint_t bli_info_get_maximum_nc_d( void );
gint_t bli_info_get_maximum_nc_c( void );
gint_t bli_info_get_maximum_nc_z( void );

// Default register blocksizes

gint_t bli_info_get_default_mr( num_t dt );
gint_t bli_info_get_default_mr_s( void );
gint_t bli_info_get_default_mr_d( void );
gint_t bli_info_get_default_mr_c( void );
gint_t bli_info_get_default_mr_z( void );

gint_t bli_info_get_default_kr( num_t dt );
gint_t bli_info_get_default_kr_s( void );
gint_t bli_info_get_default_kr_d( void );
gint_t bli_info_get_default_kr_c( void );
gint_t bli_info_get_default_kr_z( void );

gint_t bli_info_get_default_nr( num_t dt );
gint_t bli_info_get_default_nr_s( void );
gint_t bli_info_get_default_nr_d( void );
gint_t bli_info_get_default_nr_c( void );
gint_t bli_info_get_default_nr_z( void );

// Packing register blocksizes

gint_t bli_info_get_packdim_mr( num_t dt );
gint_t bli_info_get_packdim_mr_s( void );
gint_t bli_info_get_packdim_mr_d( void );
gint_t bli_info_get_packdim_mr_c( void );
gint_t bli_info_get_packdim_mr_z( void );

gint_t bli_info_get_packdim_nr( num_t dt );
gint_t bli_info_get_packdim_nr_s( void );
gint_t bli_info_get_packdim_nr_d( void );
gint_t bli_info_get_packdim_nr_c( void );
gint_t bli_info_get_packdim_nr_z( void );

// Level-2 cache blocksizes

gint_t bli_info_get_default_l2_mc_s( void );
gint_t bli_info_get_default_l2_mc_d( void );
gint_t bli_info_get_default_l2_mc_c( void );
gint_t bli_info_get_default_l2_mc_z( void );

gint_t bli_info_get_default_l2_nc_s( void );
gint_t bli_info_get_default_l2_nc_d( void );
gint_t bli_info_get_default_l2_nc_c( void );
gint_t bli_info_get_default_l2_nc_z( void );

// Level-1f fusing factors

gint_t bli_info_get_default_l1f_fuse_fac( num_t dt );
gint_t bli_info_get_default_l1f_fuse_fac_s( void );
gint_t bli_info_get_default_l1f_fuse_fac_d( void );
gint_t bli_info_get_default_l1f_fuse_fac_c( void );
gint_t bli_info_get_default_l1f_fuse_fac_z( void );

gint_t bli_info_get_axpyf_fuse_fac( num_t dt );
gint_t bli_info_get_axpyf_fuse_fac_s( void );
gint_t bli_info_get_axpyf_fuse_fac_d( void );
gint_t bli_info_get_axpyf_fuse_fac_c( void );
gint_t bli_info_get_axpyf_fuse_fac_z( void );

gint_t bli_info_get_dotxf_fuse_fac( num_t dt );
gint_t bli_info_get_dotxf_fuse_fac_s( void );
gint_t bli_info_get_dotxf_fuse_fac_d( void );
gint_t bli_info_get_dotxf_fuse_fac_c( void );
gint_t bli_info_get_dotxf_fuse_fac_z( void );

gint_t bli_info_get_dotxaxpyf_fuse_fac( num_t dt );
gint_t bli_info_get_dotxaxpyf_fuse_fac_s( void );
gint_t bli_info_get_dotxaxpyf_fuse_fac_d( void );
gint_t bli_info_get_dotxaxpyf_fuse_fac_c( void );
gint_t bli_info_get_dotxaxpyf_fuse_fac_z( void );
```

The following routines allow the caller to obtain a string that describes the type of each micro-kernel. Possible micro-kernel types are: reference, virtual-4m, virtual-3m, optimized.

```
char* bli_info_get_gemm_ukr_type_string( num_t dt );
char* bli_info_get_gemmtrsm_l_ukr_type_string( num_t dt );
char* bli_info_get_gemmtrsm_u_ukr_type_string( num_t dt );
char* bli_info_get_trsm_l_ukr_type_string( num_t dt );
char* bli_info_get_trsm_u_ukr_type_string( num_t dt );
```


## Static memory pools ##

The size of the static memory pools are determined by constants set in [bli\_config.h](ConfigurationHowTo#bli_config.h.md) (see "Memory allocator"). The following routines will allow the caller to query the sizes of those pools.

```
gint_t bli_info_get_mk_pool_size( void );
gint_t bli_info_get_kn_pool_size( void );
gint_t bli_info_get_mn_pool_size( void );
```


## Implementation types ##

The following routines allow the caller to obtain a string that describes the implementation used by each level-3 operation. Possible implementation types are: native, 4m, 4mh, 3m, 3mh.
```
char* bli_info_get_gemm_impl_string( num_t dt );
char* bli_info_get_hemm_impl_string( num_t dt );
char* bli_info_get_herk_impl_string( num_t dt );
char* bli_info_get_her2k_impl_string( num_t dt );
char* bli_info_get_symm_impl_string( num_t dt );
char* bli_info_get_syrk_impl_string( num_t dt );
char* bli_info_get_syr2k_impl_string( num_t dt );
char* bli_info_get_trmm_impl_string( num_t dt );
char* bli_info_get_trmm3_impl_string( num_t dt );
char* bli_info_get_trsm_impl_string( num_t dt );
```