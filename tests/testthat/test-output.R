context("Output")

test_that("different string and file output produce identical svg", {
  ## 1. Write to a file
  f1 <- tempfile()
  svglite(f1)
  plot(1:5)
  dev.off()

  out1 <- readLines(f1)

  ## 2. Write to a string stream
  s <- svgstring()
  plot(1:5)
  dev.off()
  out2 <- strsplit(s(), "\n")[[1]]

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
