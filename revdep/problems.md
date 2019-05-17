# DeLorean

Version: 1.5.0

## In both

*   checking whether package ‘DeLorean’ can be installed ... WARNING
    ```
    Found the following significant warnings:
      Warning: package ‘Rcpp’ was built under R version 3.5.2
    See ‘/Users/lionel/Desktop/svglite/revdep/checks.noindex/DeLorean/new/DeLorean.Rcheck/00install.out’ for details.
    ```

*   checking installed package size ... NOTE
    ```
      installed size is  8.0Mb
      sub-directories of 1Mb or more:
        libs   5.1Mb
    ```

*   checking dependencies in R code ... NOTE
    ```
    Namespace in Imports field not imported from: ‘lattice’
      All declared Imports should be used.
    ```

*   checking for GNU extensions in Makefiles ... NOTE
    ```
    GNU make is a SystemRequirements.
    ```

# gganimate

Version: 1.0.3

## In both

*   checking whether package ‘gganimate’ can be installed ... WARNING
    ```
    Found the following significant warnings:
      Warning: package ‘ggplot2’ was built under R version 3.5.2
    See ‘/Users/lionel/Desktop/svglite/revdep/checks.noindex/gganimate/new/gganimate.Rcheck/00install.out’ for details.
    ```

# ggExtra

Version: 0.8

## In both

*   checking dependencies in R code ... NOTE
    ```
    Namespace in Imports field not imported from: ‘grDevices’
      All declared Imports should be used.
    ```

# ggplot2

Version: 3.1.1

## In both

*   checking installed package size ... NOTE
    ```
      installed size is  7.3Mb
      sub-directories of 1Mb or more:
        doc   1.8Mb
        R     3.8Mb
    ```

*   checking dependencies in R code ... NOTE
    ```
    Namespaces in Imports field not imported from:
      ‘mgcv’ ‘reshape2’ ‘viridisLite’
      All declared Imports should be used.
    ```

# leafpop

Version: 0.0.1

## In both

*   checking dependencies in R code ... NOTE
    ```
    Namespace in Imports field not imported from: ‘Rcpp’
      All declared Imports should be used.
    ```

# LLSR

Version: 0.0.2.19

## In both

*   checking whether package ‘LLSR’ can be installed ... ERROR
    ```
    Installation failed.
    See ‘/Users/lionel/Desktop/svglite/revdep/checks.noindex/LLSR/new/LLSR.Rcheck/00install.out’ for details.
    ```

## Installation

### Devel

```
* installing *source* package ‘LLSR’ ...
** package ‘LLSR’ successfully unpacked and MD5 sums checked
** R
** data
*** moving datasets to lazyload DB
** inst
** byte-compile and prepare package for lazy loading
Unable to find any JVMs matching version "(null)".
No Java runtime present, try --request to install.
Warning in system("/usr/libexec/java_home", intern = TRUE) :
  running command '/usr/libexec/java_home' had status 1
Error : .onLoad failed in loadNamespace() for 'rJava', details:
  call: dyn.load(file, DLLpath = DLLpath, ...)
  error: unable to load shared object '/Users/lionel/Desktop/svglite/revdep/library.noindex/LLSR/rJava/libs/rJava.so':
  dlopen(/Users/lionel/Desktop/svglite/revdep/library.noindex/LLSR/rJava/libs/rJava.so, 6): Library not loaded: /Library/Java/JavaVirtualMachines/jdk-11.0.1.jdk/Contents/Home/lib/server/libjvm.dylib
  Referenced from: /Users/lionel/Desktop/svglite/revdep/library.noindex/LLSR/rJava/libs/rJava.so
  Reason: image not found
ERROR: lazy loading failed for package ‘LLSR’
* removing ‘/Users/lionel/Desktop/svglite/revdep/checks.noindex/LLSR/new/LLSR.Rcheck/LLSR’

```
### CRAN

```
* installing *source* package ‘LLSR’ ...
** package ‘LLSR’ successfully unpacked and MD5 sums checked
** R
** data
*** moving datasets to lazyload DB
** inst
** byte-compile and prepare package for lazy loading
Unable to find any JVMs matching version "(null)".
No Java runtime present, try --request to install.
Warning in system("/usr/libexec/java_home", intern = TRUE) :
  running command '/usr/libexec/java_home' had status 1
Error : .onLoad failed in loadNamespace() for 'rJava', details:
  call: dyn.load(file, DLLpath = DLLpath, ...)
  error: unable to load shared object '/Users/lionel/Desktop/svglite/revdep/library.noindex/LLSR/rJava/libs/rJava.so':
  dlopen(/Users/lionel/Desktop/svglite/revdep/library.noindex/LLSR/rJava/libs/rJava.so, 6): Library not loaded: /Library/Java/JavaVirtualMachines/jdk-11.0.1.jdk/Contents/Home/lib/server/libjvm.dylib
  Referenced from: /Users/lionel/Desktop/svglite/revdep/library.noindex/LLSR/rJava/libs/rJava.so
  Reason: image not found
ERROR: lazy loading failed for package ‘LLSR’
* removing ‘/Users/lionel/Desktop/svglite/revdep/checks.noindex/LLSR/old/LLSR.Rcheck/LLSR’

```
# mapview

Version: 2.7.0

## In both

*   checking tests ...
    ```
     ERROR
    Running the tests in ‘tests/testthat.R’ failed.
    Last 13 lines of output:
             if (logical.return) 
                 message(paste("Error:", msg), domain = NA)
             else stop(msg, call. = FALSE, domain = NA)
         })
      3: tryCatchList(expr, classes, parentenv, handlers)
      4: tryCatchOne(expr, names, parentenv, handlers[[1L]])
      5: value[[3L]](cond)
      6: stop(msg, call. = FALSE, domain = NA)
      
      ══ testthat results  ═══════════════════════════════════════════════════════════
      OK: 62 SKIPPED: 0 WARNINGS: 1 FAILED: 1
      1. Error: getSimpleClass() runs without any issues (@test-mapviewControls.R#25) 
      
      Error: testthat unit tests failed
      Execution halted
    ```

*   checking dependencies in R code ... NOTE
    ```
    Namespaces in Imports field not imported from:
      ‘brew’ ‘svglite’ ‘uuid’
      All declared Imports should be used.
    ```

*   checking Rd cross-references ... NOTE
    ```
    Packages unavailable to check Rd xrefs: ‘leafsync’, ‘slideview’
    ```

# metacoder

Version: 0.3.2

## In both

*   checking whether package ‘metacoder’ can be installed ... WARNING
    ```
    Found the following significant warnings:
      Warning: package ‘taxa’ was built under R version 3.5.2
    See ‘/Users/lionel/Desktop/svglite/revdep/checks.noindex/metacoder/new/metacoder.Rcheck/00install.out’ for details.
    ```

*   checking dependencies in R code ... NOTE
    ```
    Namespaces in Imports field not imported from:
      ‘ggrepel’ ‘reshape’ ‘svglite’
      All declared Imports should be used.
    ```

# mlr

Version: 2.14.0

## In both

*   checking whether package ‘mlr’ can be installed ... WARNING
    ```
    Found the following significant warnings:
      Warning: package ‘ParamHelpers’ was built under R version 3.5.2
    See ‘/Users/lionel/Desktop/svglite/revdep/checks.noindex/mlr/new/mlr.Rcheck/00install.out’ for details.
    ```

*   checking package dependencies ... NOTE
    ```
    Packages suggested but not available for checking: ‘fpc’ ‘liquidSVM’
    ```

*   checking installed package size ... NOTE
    ```
      installed size is  9.1Mb
      sub-directories of 1Mb or more:
        data   2.3Mb
        help   1.4Mb
        R      5.1Mb
    ```

*   checking dependencies in R code ... NOTE
    ```
    Unable to find any JVMs matching version "(null)".
    No Java runtime present, try --request to install.
    Unable to find any JVMs matching version "(null)".
    No Java runtime present, try --request to install.
    Unable to find any JVMs matching version "(null)".
    No Java runtime present, try --request to install.
    ```

# netSEM

Version: 0.5.1

## In both

*   checking re-building of vignette outputs ... WARNING
    ```
    Error in re-building vignettes:
      ...
    pandoc-citeproc: when expecting a product (:*:), encountered Object instead
    Error running filter /usr/local/bin/pandoc-citeproc:
    Filter returned error status 1
    Error: processing vignette 'Crack.Rmd' failed with diagnostics:
    pandoc document conversion failed with error 83
    Execution halted
    ```

# phantasus

Version: 1.2.1

## In both

*   checking installed package size ... NOTE
    ```
      installed size is 14.9Mb
      sub-directories of 1Mb or more:
        doc   2.6Mb
        www  11.5Mb
    ```

*   checking dependencies in R code ... NOTE
    ```
    Unexported objects imported by ':::' calls:
      'GEOquery:::getDirListing' 'opencpu:::rookhandler'
      'opencpu:::tmp_root' 'opencpu:::win_or_mac'
      See the note in ?`:::` about the use of this operator.
    ```

# radiant.data

Version: 0.9.9

## In both

*   checking whether package ‘radiant.data’ can be installed ... WARNING
    ```
    Found the following significant warnings:
      Warning: package ‘ggplot2’ was built under R version 3.5.2
      Warning: package ‘tidyr’ was built under R version 3.5.2
      Warning: package ‘dplyr’ was built under R version 3.5.2
    See ‘/Users/lionel/Desktop/svglite/revdep/checks.noindex/radiant.data/new/radiant.data.Rcheck/00install.out’ for details.
    ```

# rsvg

Version: 1.3

## In both

*   checking whether package ‘rsvg’ can be installed ... ERROR
    ```
    Installation failed.
    See ‘/Users/lionel/Desktop/svglite/revdep/checks.noindex/rsvg/new/rsvg.Rcheck/00install.out’ for details.
    ```

## Installation

### Devel

```
* installing *source* package ‘rsvg’ ...
** package ‘rsvg’ successfully unpacked and MD5 sums checked
Package libffi was not found in the pkg-config search path.
Perhaps you should add the directory containing `libffi.pc'
to the PKG_CONFIG_PATH environment variable
Package 'libffi', required by 'gobject-2.0', not found
Found pkg-config cflags and libs!
Using PKG_CFLAGS=
Using PKG_LIBS=-L/usr/local/Cellar/librsvg/2.44.13/lib -L/usr/local/Cellar/glib/2.60.0_1/lib -L/usr/local/Cellar/gdk-pixbuf/2.38.1/lib -L/usr/local/Cellar/glib/2.60.0_1/lib -L/usr/local/opt/gettext/lib -L/usr/local/Cellar/cairo/1.16.0/lib -lrsvg-2 -lm -lgio-2.0 -lgdk_pixbuf-2.0 -lgobject-2.0 -lglib-2.0 -lintl -lcairo
------------------------- ANTICONF ERROR ---------------------------
Configuration failed because librsvg-2.0 was not found. Try installing:
 * deb: librsvg2-dev (Debian, Ubuntu, etc)
 * rpm: librsvg2-devel (Fedora, EPEL)
 * csw: librsvg_dev, sunx11_devel (Solaris)
 * brew: librsvg (OSX)
If librsvg-2.0 is already installed, check that 'pkg-config' is in your
PATH and PKG_CONFIG_PATH contains a librsvg-2.0.pc file. If pkg-config
is unavailable you can set INCLUDE_DIR and LIB_DIR manually via:
R CMD INSTALL --configure-vars='INCLUDE_DIR=... LIB_DIR=...'
--------------------------------------------------------------------
ERROR: configuration failed for package ‘rsvg’
* removing ‘/Users/lionel/Desktop/svglite/revdep/checks.noindex/rsvg/new/rsvg.Rcheck/rsvg’

```
### CRAN

```
* installing *source* package ‘rsvg’ ...
** package ‘rsvg’ successfully unpacked and MD5 sums checked
Package libffi was not found in the pkg-config search path.
Perhaps you should add the directory containing `libffi.pc'
to the PKG_CONFIG_PATH environment variable
Package 'libffi', required by 'gobject-2.0', not found
Found pkg-config cflags and libs!
Using PKG_CFLAGS=
Using PKG_LIBS=-L/usr/local/Cellar/librsvg/2.44.13/lib -L/usr/local/Cellar/glib/2.60.0_1/lib -L/usr/local/Cellar/gdk-pixbuf/2.38.1/lib -L/usr/local/Cellar/glib/2.60.0_1/lib -L/usr/local/opt/gettext/lib -L/usr/local/Cellar/cairo/1.16.0/lib -lrsvg-2 -lm -lgio-2.0 -lgdk_pixbuf-2.0 -lgobject-2.0 -lglib-2.0 -lintl -lcairo
------------------------- ANTICONF ERROR ---------------------------
Configuration failed because librsvg-2.0 was not found. Try installing:
 * deb: librsvg2-dev (Debian, Ubuntu, etc)
 * rpm: librsvg2-devel (Fedora, EPEL)
 * csw: librsvg_dev, sunx11_devel (Solaris)
 * brew: librsvg (OSX)
If librsvg-2.0 is already installed, check that 'pkg-config' is in your
PATH and PKG_CONFIG_PATH contains a librsvg-2.0.pc file. If pkg-config
is unavailable you can set INCLUDE_DIR and LIB_DIR manually via:
R CMD INSTALL --configure-vars='INCLUDE_DIR=... LIB_DIR=...'
--------------------------------------------------------------------
ERROR: configuration failed for package ‘rsvg’
* removing ‘/Users/lionel/Desktop/svglite/revdep/checks.noindex/rsvg/old/rsvg.Rcheck/rsvg’

```
# svgPanZoom

Version: 0.3.3

## In both

*   checking package dependencies ... NOTE
    ```
    Packages which this enhances but not available for checking:
      ‘gridSVG’ ‘knitr’ ‘XML’ ‘xml2’
    ```

# tidybayes

Version: 1.0.4

## In both

*   checking tests ...
    ```
     ERROR
    Running the tests in ‘tests/testthat.R’ failed.
    Last 13 lines of output:
             modules = modules, factories = factories, jags = jags, call.setup = TRUE, method = method, 
             mutate = mutate)
      10: setup.jags(model = outmodel, monitor = outmonitor, data = outdata, n.chains = n.chains, 
             inits = outinits, modules = modules, factories = factories, response = response, 
             fitted = fitted, residual = residual, jags = jags, method = method, mutate = mutate)
      11: loadandcheckrjags()
      12: stop("Loading the rjags package failed (diagnostics are given above this error message)", 
             call. = FALSE)
      
      ══ testthat results  ═══════════════════════════════════════════════════════════
      OK: 223 SKIPPED: 43 WARNINGS: 5 FAILED: 1
      1. Error: tidy_draws works with runjags (@test.tidy_draws.R#87) 
      
      Error: testthat unit tests failed
      Execution halted
    ```

# viridis

Version: 0.5.1

## In both

*   checking re-building of vignette outputs ... WARNING
    ```
    ...
    
    Attaching package: 'raster'
    
    The following object is masked from 'package:colorspace':
    
        RGB
    
    Loading required package: lattice
    Loading required package: latticeExtra
    Loading required package: RColorBrewer
    
    Attaching package: 'latticeExtra'
    
    The following object is masked from 'package:ggplot2':
    
        layer
    
    Quitting from lines 205-214 (intro-to-viridis.Rmd) 
    Error: processing vignette 'intro-to-viridis.Rmd' failed with diagnostics:
    Cannot create RasterLayer object from this file; perhaps you need to install rgdal first
    Execution halted
    ```

*   checking dependencies in R code ... NOTE
    ```
    Namespace in Imports field not imported from: ‘stats’
      All declared Imports should be used.
    ```

