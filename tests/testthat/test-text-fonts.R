
context("Fonts")

test_that("font sets weight/style", {
  x <- xmlSVG({
    plot.new()
    text(0.5, seq(0.9, 0.1, length = 4), "a", font = 1:4)
  })
  text <- xml_find_all(x, ".//text")
  expect_equal(style_attr(text, "font-weight"), c(NA, "bold", NA, "bold"))
  expect_equal(style_attr(text, "font-style"), c(NA, NA, "italic", "italic"))
})

test_that("metrics are computed for different weight/style", {
  x <- xmlSVG({
    plot.new()
    text(1, 1, "text")
    text(1, 1, "text", font = 2)
    text(1, 1, "text", font = 4)
  })
  text <- xml_find_all(x, ".//text")
  x <- xml_attr(text, "x")
  y <- xml_attr(text, "y")
  expect_false(any(x[2:3] == x[1]))
  expect_false(any(y[2:3] == y[1]))
})

test_that("symbol font family is 'symbol'", {
  x <- xmlSVG({
    plot(c(0,2), c(0,2), type = "n", axes = FALSE, xlab = "", ylab = "")
    text(1, 1, expression(symbol("\042")))
  })
  text <- xml_find_all(x, ".//text")
  expect_equal(style_attr(text, "font-family"), c("symbol"))
})

test_that("partial aliases are checked", {
  aliases <- list(
    sans = fontquiver::font_faces("Bitstream Vera", "Sans"),
    mono = fontquiver::font_faces("Bitstream Vera", "Mono")
  )
  missing_faces <- check_aliases(aliases)$families
  expect_equal(missing_faces, c("serif", "symbol"))
})

test_that("throw on malformed alias", {
  expect_error(validate_aliases(list(mono = letters)), "not valid")
})

test_that("fonts are aliased", {
  aliases <- list(
    sans = "foobar",
    mono = fontquiver::font_faces("Bitstream Vera", "Mono")
  )
  x <- xmlSVG({
    plot.new()
    text(0.5, 0.1, "a", family = "serif")
    text(0.5, 0.5, "a", family = "sans")
    text(0.5, 0.9, "a", family = "mono")
  }, font_aliases = aliases)
  text <- xml_find_all(x, ".//text")

  expect_equal(
    style_attr(text, "font-family"),
    c("Times New Roman", "foobar", "Bitstream Vera Sans Mono")
  )
})

test_that("metrics are computed for different fonts", {
  aliases <- list(
    sans = "courier",
    mono = fontquiver::font_faces("Bitstream Vera", "Mono")
  )
  x <- xmlSVG({
    plot.new()
    text(0.5, 0.9, "a", family = "sans")
    text(0.5, 0.9, "a", family = "mono")
  }, font_aliases = aliases)
  text <- xml_find_all(x, ".//text")
  x_attr <- xml_attr(text, "x")
  y_attr <- xml_attr(text, "y")

  expect_false(x_attr[[1]] == x_attr[[2]])
  expect_false(y_attr[[1]] == y_attr[[2]])
  expect_equal(c(x_attr[[2]], y_attr[[2]]), c("261.58", "111.56"))
})
