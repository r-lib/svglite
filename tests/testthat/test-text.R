context("text")
library(xml2)

test_that("par(cex) affects strwidth", {
  devSVG(tempfile())
  on.exit(dev.off())

  plot.new()
  w1 <- strwidth("abc")

  par(cex = 4)
  w4 <- strwidth("abc")

  expect_equal(w4 / w1, 4)
})

test_that("special characters are escaped", {
  x <- xmlSVG({
    plot.new()
    text(0.5, 0.5, "<&>")
  })
  # xml_text unescapes for us - this still tests that the
  # file parses, which it wouldn't otherwise
  expect_equal(xml_text(xml_find_one(x, ".//text")), "<&>")
})

test_that("utf-8 characters are preserved", {
  x <- xmlSVG({
    plot.new()
    text(0.5, 0.5, "\u00b5")
  })
  # xml_text unescapes for us - this still tests that the
  # file parses, which it wouldn't otherwise
  expect_equal(xml_text(xml_find_one(x, ".//text")), "\u00b5")
})

test_that("special characters are escaped", {
  x <- xmlSVG({
    plot.new()
    text(0.5, 0.5, "a", col = "#113399")
  })
  # xml_text unescapes for us - this still tests that the
  # file parses, which it wouldn't otherwise
  expect_equal(xml_attr(xml_find_one(x, ".//text"), "fill"), "#113399")
})

test_that("default point size is 12", {
  x <- xmlSVG({
    plot.new()
    text(0.5, 0.5, "a")
  })
  expect_equal(xml_attr(xml_find_one(x, ".//text"), "font-size"), "12")
})
