context("Text")
library(xml2)

style_attr <- function(nodes, attr) {
  style <- xml_attr(nodes, "style")
  ifelse(grepl(sprintf("%s: [^;]*;", attr), style),
         gsub(sprintf(".*%s: ([^;]*);.*", attr), "\\1", style),
         NA_character_)
}

test_that("par(cex) affects strwidth", {
  svglite(tempfile())
  on.exit(dev.off())

  plot.new()
  w1 <- strwidth("X")

  par(cex = 4)
  w4 <- strwidth("X")

  expect_equal(w4 / w1, 4, tol = 1e-4)
})

test_that("cex affects strwidth", {
  inlineSVG(height = 7, width = 7, {
    plot.new()
    w1 <- strwidth("X")
    w4 <- strwidth("X", cex = 4)
  })
  expect_equal(w4 / w1, 4, tol = 1e-4)
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
  skip_on_os("windows") # skip because of xml2 buglet

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
  expect_equal(style_attr(xml_find_one(x, ".//text"), "fill"), "#113399")
})

test_that("default point size is 12", {
  x <- xmlSVG({
    plot.new()
    text(0.5, 0.5, "a")
  })
  expect_equal(style_attr(xml_find_one(x, ".//text"), "font-size"), "12.00pt")
})

test_that("cex generates fractional font sizes", {
  x <- xmlSVG({
    plot.new()
    text(0.5, 0.5, "a", cex = 0.1)
  })
  expect_equal(style_attr(xml_find_one(x, ".//text"), "font-size"), "1.20pt")
})

test_that("font sets weight/style", {
  x <- xmlSVG({
    plot.new()
    text(0.5, seq(0.9, 0.1, length = 4), "a", font = 1:4)
  })
  text <- xml_find_all(x, ".//text")
  expect_equal(style_attr(text, "font-weight"), c(NA, "bold", NA, "bold"))
  expect_equal(style_attr(text, "font-style"), c(NA, NA, "italic", "italic"))
})

test_that("font sets weight/style", {
  x <- xmlSVG({
    plot.new()
    text(0.5, 0.1, "a", family = "serif")
    text(0.5, 0.5, "a", family = "sans")
    text(0.5, 0.9, "a", family = "mono")
  })
  text <- xml_find_all(x, ".//text")
  expect_equal(style_attr(text, "font-family"), c("Times New Roman", "Arial", "courier"))
})

test_that("a symbol has width greater than 0", {
  xmlSVG({
    plot(c(0,2), c(0,2), type = "n")
    strw <- strwidth(expression(symbol("\042")))
  })
  expect_less_than(.Machine$double.eps, strw)
})

test_that("symbol font family is 'symbol'", {
  x <- xmlSVG({
    plot(c(0,2), c(0,2), type = "n", axes = FALSE, xlab = "", ylab = "")
    text(1, 1, expression(symbol("\042")))
  })
  text <- xml_find_all(x, ".//text")
  expect_equal(style_attr(text, "font-family"), c("symbol"))
})

test_that("strwidth and height correctly computed", {
  svglite("test-text.svg", 4, 4)
  on.exit(dev.off())

  plot.new()
  str <- "This is a string"
  text(0.5, 0.5, str)

  h <- strheight(str)
  w <- strwidth(str)

  rect(0.5 - w / 2, 0.5 - h / 2, 0.5 + w / 2, 0.5 + h / 2)
})
