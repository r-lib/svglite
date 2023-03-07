library("grid")

# Tests requiring manual oversight are registered as such. They
# specify files that should be opened with multiple browsers to make
# sure the SVG appearance is consistent. Use open_manual_tests() after
# running testthat to open them in your default browser.

test_that("text has correct dimensions", {
  register_manual_test("test-scale-text.html")

  ttf <- fontquiver::font("Liberation", "Sans", "Regular")$ttf
  w <- systemfonts::string_width("foobar", path = ttf, index = 0L, res = 1e4) * 72 / 1e4
  h <- max(vapply(systemfonts::glyph_info("foobar", path = ttf, index = 0L, res = 1e4)$bbox, `[[`, numeric(1), "ymax")) * 72 / 1e4

  svglite("test-scale-text.svg",
    width = w / 72, height = h / 72,
    user_fonts = fontquiver::font_families("Liberation")
  )
  on.exit(dev.off())

  grid.newpage()
  grid.rect(0, 1,
    width = unit(w, "bigpts"), height = unit(h, "bigpts"),
    hjust = 0, vjust = 1, gp = gpar(col = "red", lwd = 1)
  )
  grid.text("foobar", 0, 1, hjust = 0, vjust = 1, gp = gpar(fontsize = 12))
  pushViewport(viewport())
})

test_that("lwd has correct dimensions", {
  x <- xmlSVG({
    plot.new()
    segments(0, 1, 0, 0, lwd = 96 / 72)
  })
  line <- xml_find_all(x, "//line")
  expect_equal(xml_attr(line, "style"), "stroke-width: 1.00;")
})
