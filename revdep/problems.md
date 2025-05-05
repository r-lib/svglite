# ggfixest

<details>

* Version: 0.2.0
* GitHub: https://github.com/grantmcdermott/ggfixest
* Source code: https://github.com/cran/ggfixest
* Date/Publication: 2025-01-22 01:00:02 UTC
* Number of recursive dependencies: 76

Run `revdepcheck::cloud_details(, "ggfixest")` for more info

</details>

## Newly broken

*   checking tests ... ERROR
    ```
      Running ‘tinytest.R’
    Running the tests in ‘tests/tinytest.R’ failed.
    Complete output:
      > ## Throttle CPU threads if R CMD check (for CRAN)
      > 
      > if (any(grepl("_R_CHECK", names(Sys.getenv()), fixed = TRUE))) {
      +     # fixest
      +     if (requireNamespace("fixest", quietly = TRUE)) {
      +         library(fixest)
      +         setFixest_nthreads(1)
    ...
      ----- FAILED[]: test_ggiplot.R<206--206>
       call| expect_snapshot_plot(p20a, label = "ggiplot_multi_sw_pt_join1")
       diff| 8467
       info| Diff plot saved to: _tinysnapshot_review/ggiplot_multi_sw_pt_join1.png
      ----- FAILED[]: test_ggiplot.R<207--207>
       call| expect_snapshot_plot(p20b, label = "ggiplot_multi_sw_pt_join2")
       diff| 8676
       info| Diff plot saved to: _tinysnapshot_review/ggiplot_multi_sw_pt_join2.png
      Error: 41 out of 103 tests failed
      Execution halted
    ```

# gtExtras

<details>

* Version: 0.5.0
* GitHub: https://github.com/jthomasmock/gtExtras
* Source code: https://github.com/cran/gtExtras
* Date/Publication: 2023-09-15 22:32:06 UTC
* Number of recursive dependencies: 105

Run `revdepcheck::cloud_details(, "gtExtras")` for more info

</details>

## Newly broken

*   checking tests ... ERROR
    ```
      Running ‘testthat.R’
    Running the tests in ‘tests/testthat.R’ failed.
    Complete output:
      > library(testthat)
      > library(gtExtras)
      Loading required package: gt
      
      Attaching package: 'gt'
      
      The following object is masked from 'package:testthat':
    ...
      `expected` is a character vector ('000000;', '000000;', 'A020F0;', 'A020F0;', '00FF00;', ...)
      ── Failure ('test-gt_win_loss.R:128:3'): SVG exists and has expected colors ────
      `pill_colors` (`actual`) not equal to `exp_pill_colors` (`expected`).
      
      `actual` is NULL
      `expected` is a character vector (NA, NA, 'A020F0;', 'A020F0;', '00FF00;', ...)
      
      [ FAIL 22 | WARN 15 | SKIP 23 | PASS 94 ]
      Error: Test failures
      Execution halted
    ```

# parttree

<details>

* Version: 0.1.0
* GitHub: https://github.com/grantmcdermott/parttree
* Source code: https://github.com/cran/parttree
* Date/Publication: 2025-01-16 10:30:02 UTC
* Number of recursive dependencies: 127

Run `revdepcheck::cloud_details(, "parttree")` for more info

</details>

## Newly broken

*   checking tests ... ERROR
    ```
      Running ‘tinytest.R’
    Running the tests in ‘tests/tinytest.R’ failed.
    Complete output:
      > 
      > if ( requireNamespace("tinytest", quietly=TRUE) ){
      +   Sys.setenv("OMP_THREAD_LIMIT" = 2) # https://github.com/Rdatatable/data.table/issues/5658
      +   tinytest::test_package("parttree")
      + }
      
      test_ctree.R..................    0 tests    Loading required package: partykit
    ...
       call| expect_snapshot_plot(f, label = "iris_regression")
       diff| 7854
       info| Diff plot saved to: _tinysnapshot_review/iris_regression.png
      Error: 3 out of 10 tests failed
      In addition: Warning messages:
      1: In library(package, lib.loc = lib.loc, character.only = TRUE, logical.return = TRUE,  :
        there is no package called 'tidymodels'
      2: In library(package, lib.loc = lib.loc, character.only = TRUE, logical.return = TRUE,  :
        there is no package called 'tidymodels'
      Execution halted
    ```

