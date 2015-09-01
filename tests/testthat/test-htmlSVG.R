context("htmlSVG")
library(htmltools)
library(xml2)

test_that("produces html", {
  h <- htmlSVG(plot(1:3))
  expect_is(h, "html")
  expect_true( attr(h, "browsable_html") )
})
