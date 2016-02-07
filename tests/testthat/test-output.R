context("Output")

test_that("three types of output", {

  ## 1. Write to a file
  f1 <- tempfile()
  svglite(f1)
  plot(1:5, (1:5)^2, main = "Examples")
  dev.off()

  ## 2. Direct to R terminal
  f2 <- tempfile()
  sink(f2) # capture the output
  svglite(":terminal:")
  plot(1:5, (1:5)^2, main = "Examples")
  invisible(dev.off())
  sink()

  ## 3. Write to a string stream
  svglite(":string:")
  plot(1:5, (1:5)^2, main = "Examples")
  dev.off()
  svgstr <- stringSVG()

  str1 <- readLines(f1)
  str2 <- readLines(f2)
  str3 <- unlist(strsplit(svgstr, "\n"))

  expect_equal(str1, str2)
  expect_equal(str1, str3)
})
