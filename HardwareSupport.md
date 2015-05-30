## Introduction ##

This wiki is intended to track the support for various hardware types within the BLIS framework source distribution.

If you are a developer and have already contributed kernels, and need this wiki updated, please contact the [blis-devel](http://groups.google.com/group/blis-devel) mailing list.


## Level-3 micro-kernels ##

The following table lists architectures for which there exist optimized level-3 micro-kernels, which micro-kernels are optimized, the name of the author or maintainer, and the current status of the micro-kernels.

A few remarks / reminders:
  * Optimizing only the [gemm micro-kernel](KernelsHowTo#gemm_micro-kernel.md) will result in optimal performance for all [level-3 operations](BLISAPIQuickReference#Level-3_operations.md) except `trsm` (which will typically achieve 60 - 80% of attainable peak performance).
  * The [trsm](BLISAPIQuickReference#trsm.md) operation needs the [gemmtrsm micro-kernel(s)](KernelsHowTo#gemmtrsm_micro-kernels.md), in addition to the aforementioned [gemm micro-kernel](KernelsHowTo#gemm_micro-kernel.md), in order reach optimal performance.
  * The [trsm](BLISAPIQuickReference#trsm.md) operation can be accelerated (though not optimized) by instead providing an optimized [trsm micro-kernel](KernelsHowTo#trsm_micro-kernels.md) in addition to the [gemm micro-kernel](KernelsHowTo#gemm_micro-kernel.md).

| Vendor | Architecture                  | BLIS configuration name | `gemm` | `trsm` | `gemmtrsm` | Author/maintainer(s) | Status/Comments |
|:-------|:------------------------------|:------------------------|:-------|:-------|:-----------|:---------------------|:----------------|
| AMD    | Piledriver (AVX/FMA3)         | `piledriver`            |  `d`   |        |            | Tyler Smith          | Testing         |
| ARM    | Cortex-A9/A15 (NEON intrinsics) | `cortex-a9, cortex-a15`  | `sd`   |        |            | Francisco Igual      | Testing         |
| IBM    | Blue Gene/Q (QPX intrinsics)  | `bgq`                   |  `d`   |        |            | Tyler Smith          | Testing         |
| IBM    | Power7 (QPX intrinsics)       | `power7`                |  `d`   |        |            | Mike Kistler         | Testing         |
| Intel  | Dunnington (SSE3)             | `dunnington`            | `sd`   |  `d`   |  `d`       | Field Van Zee        | Testing         |
| Intel  | MIC (AVX/FMA3)                | `mic`                   |  `d`   |        |            | Tyler Smith          | Testing         |
| Intel  | Sandy Bridge (AVX intrinsics) | `sandybridge`           |  `d`   |        |            | Francisco Igual      | Testing         |
| MIPS   | Loongson 3A                   | `loongon3a`             |  `d`   |        |            | Xianyi Zhang         | Testing         |
|        | reference (C99)               | `reference`             | `sdcz` | `sdcz` | `sdcz`     | Field Van Zee        | Stable          |
|        | template (C99)                | `template`              | `sdcz` | `sdcz` | `sdcz`     | Field Van Zee        | Stable          |