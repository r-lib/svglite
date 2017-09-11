# DeLorean

Version: 1.2.5

## In both

*   checking S3 generic/method consistency ... WARNING
    ```
    filter:
      function(x, filter, method, sides, circular, init)
    filter.cells:
      function(dl, .filter, number, cells)
    
    filter:
      function(x, filter, method, sides, circular, init)
    filter.genes:
      function(dl, .filter, number, genes)
    
    See section ‘Generic functions and methods’ in the ‘Writing R
    Extensions’ manual.
    ```

# ggExtra

Version: 0.7

## In both

*   checking dependencies in R code ... NOTE
    ```
    Namespace in Imports field not imported from: ‘grDevices’
      All declared Imports should be used.
    ```

# ggplot2

Version: 2.2.1

## In both

*   checking dependencies in R code ... NOTE
    ```
    Namespace in Imports field not imported from: ‘reshape2’
      All declared Imports should be used.
    ```

# mlr

Version: 2.11

## In both

*   checking tests ...
    ```
     ERROR
    Running the tests in ‘tests/run-base.R’ failed.
    Last 13 lines of output:
      7: requirePackages(package, why = stri_paste("learner", id, sep = " "), default.method = "load")
      8: stopf("For %s please install the following packages: %s", why, ps)
      
      testthat results ================================================================
      OK: 2477 SKIPPED: 1 FAILED: 7
      1. Error: clustering performance (@test_base_clustering.R#15) 
      2. Error: clustering performance with missing clusters (@test_base_clustering.R#27) 
      3. Error: clustering resample (@test_base_clustering.R#41) 
      4. Error: clustering benchmark (@test_base_clustering.R#52) 
      5. Error: clustering tune (@test_base_clustering.R#65) 
      6. Error: hyperpars (@test_base_hyperpars.R#12) 
      7. Error: check measure calculations (@test_base_measures.R#186) 
      
      Error: testthat unit tests failed
      Execution halted
    ```

*   checking dependencies in R code ... NOTE
    ```
    Unable to find any JVMs matching version "(null)".
    No Java runtime present, try --request to install.
    Unable to find any JVMs matching version "(null)".
    No Java runtime present, try --request to install.
    Unable to find any JVMs matching version "(null)".
    No Java runtime present, try --request to install.
    Missing or unexported object: ‘kohonen::bdk’
    ```

# svgPanZoom

Version: 0.3.3

## In both

*   checking package dependencies ... NOTE
    ```
    Packages which this enhances but not available for checking:
      ‘gridSVG’ ‘XML’ ‘xml2’
    ```

# viridis

Version: 0.4.0

## Newly fixed

*   checking tests ...
    ```
     ERROR
    Running the tests in ‘tests/testthat.R’ failed.
    Last 13 lines of output:
      > 
      > test_check("viridis")
      1. Error: visual elements are correct (@test-viridis.R#44) ---------------------
      function 'gdtools_RcppExport_validate' not provided by package 'gdtools'
      1: vdiffr::expect_doppelganger("a", A) at testthat/test-viridis.R:44
      2: write_svg(fig, testcase, title, user_fonts)
      3: svglite::svglite(file, user_fonts = user_fonts)
      4: svglite_(file, bg, width, height, pointsize, standalone, aliases)
      
      testthat results ================================================================
      OK: 5 SKIPPED: 0 FAILED: 1
      1. Error: visual elements are correct (@test-viridis.R#44) 
      
      Error: testthat unit tests failed
      Execution halted
    ```

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
    
    Quitting from lines 204-213 (intro-to-viridis.Rmd) 
    Error: processing vignette 'intro-to-viridis.Rmd' failed with diagnostics:
    Cannot create RasterLayer object from this file; perhaps you need to install rgdal first
    Execution halted
    ```

*   checking dependencies in R code ... NOTE
    ```
    Namespace in Imports field not imported from: ‘stats’
      All declared Imports should be used.
    ```

