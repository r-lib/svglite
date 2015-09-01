context("text")
library(xml2)

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
