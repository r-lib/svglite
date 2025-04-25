test_that("font sets weight/style", {
  x <- xmlSVG({
    plot.new()
    text(0.5, seq(0.9, 0.1, length = 4), "a", font = 1:4)
  })
  text <- xml_find_all(x, ".//text")
  skip_on_os("windows") # win-builder has issues with systemfonts ATM
  expect_equal(style_attr(text, "font-weight"), c(NA, "bold", NA, "bold"))
  expect_equal(style_attr(text, "font-style"), c(NA, NA, "italic", "italic"))
})

test_that("metrics are computed for different weight/style", {
  x <- xmlSVG(user_fonts = fontquiver::font_families("Bitstream Vera"), {
    plot.new()
    text(1, 1, "text")
    text(1, 1, "text", font = 2)
    text(1, 1, "text", font = 4)
  })
  text <- xml_find_all(x, ".//text")
  x <- xml_attr(text, "textLength")
  expect_false(any(x[2:3] == x[1]))
})

test_that("symbol font family is 'Symbol'", {
  symbol_font <- alias_lookup()["symbol"]
  matched_symbol_font <- paste0('"', match_family(symbol_font), '"')

  x <- xmlSVG({
    plot(c(0, 2), c(0, 2), type = "n", axes = FALSE, xlab = "", ylab = "")
    text(1, 1, expression(symbol("\042")))
  })
  text <- xml_find_all(x, ".//text")
  expect_equal(style_attr(text, "font-family"), matched_symbol_font)
})

test_that("throw on malformed alias", {
  expect_snapshot(validate_aliases(list(mono = letters), list()), error = TRUE)
  skip_on_cran()
  skip_on_os(c("windows", "linux"))
  expect_snapshot(validate_aliases(list(sans = "foobar"), list()))
})

test_that("fonts are aliased", {
  matched <- match_family("cursive")
  x <- xmlSVG(
    system_fonts = list(sans = matched),
    user_fonts = list(mono = fontquiver::font_faces("Bitstream Vera", "Mono")),
    {
      plot.new()
      text(0.5, 0.1, "a", family = "serif")
      text(0.5, 0.5, "a", family = "sans")
      text(0.5, 0.9, "a", family = "mono")
    }
  )
  text <- xml_find_all(x, ".//text")
  families <- style_attr(text, "font-family")

  expect_false(families[[1]] == '"serif"')
  expect_true(all(
    families[2:3] == paste0('"', c(matched, "Bitstream Vera Sans Mono"), '"')
  ))
})

test_that("metrics are computed for different fonts", {
  aliases <- fontquiver::font_families("Bitstream Vera")
  x <- xmlSVG(user_fonts = aliases, {
    plot.new()
    text(0.5, 0.9, "a", family = "serif")
    text(0.5, 0.9, "a", family = "mono")
  })
  text <- xml_find_all(x, ".//text")
  x_attr <- xml_attr(text, "textLength")
  y_attr <- xml_attr(text, "y")

  expect_false(x_attr[[1]] == x_attr[[2]])
  expect_false(y_attr[[1]] == y_attr[[2]])
})

test_that("unicode characters in plotmath are handled", {
  rho <- as.name("\u03c1")
  expr <- call("*", rho, rho)

  x <- xmlSVG({
    plot.new()
    text(0.5, 0.5, as.expression(expr))
  })
  text <- xml_find_all(x, ".//text")
  x_attr <- as.double(xml_attr(text, "x"))

  expect_true(x_attr[2] - x_attr[1] > 0)
})
