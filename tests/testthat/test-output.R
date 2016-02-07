context("Output")

test_that("different outputs produce identical code", {
  simple_plot <- function(path) {
    svglite(path)
    plot(1:5)
    invisible(dev.off())
  }

  ## 1. Write to a file
  f1 <- tempfile()
  simple_plot(f1)
  out1 <- readLines(f1)

  ## 2. Direct to R terminal
  out2 <- capture.output(simple_plot(":terminal:"))

  ## 3. Write to a string stream
  simple_plot(":string:")
  out3 <- strsplit(stringSVG(), "\n")[[1]]

  expect_equal(out1, out2)
  expect_equal(out1, out2)
})
