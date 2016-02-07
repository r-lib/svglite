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

test_that("intermediate outputs are always valid svg", {
  path <- tempfile()
  svglite(path)

  expect_valid_svg <- function() {
    expect_error(xml2::read_xml(path), NA)
  }

  mini_plot(1:10)
  expect_valid_svg()

  rect(2, 2, 3, 3)
  expect_valid_svg()

  segments(5, 5, 6, 6)
  expect_valid_svg()

  dev.off()
  expect_valid_svg()
})
