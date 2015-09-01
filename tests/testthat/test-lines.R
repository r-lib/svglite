context("Lines")

library(xml2)
test_that("segments don't have fill", {
  x <- xmlSVG({
    plot.new()
    segments(0.5, 0.5, 1, 1)
  })
  expect_equal(xml_attr(xml_find_one(x, ".//line"), "fill"), NA_character_)
})

test_that("lines don't have fill", {
  x <- xmlSVG({
    plot.new()
    lines(c(0.5, 1, 0.5), c(0.5, 1, 1))
  })
  expect_equal(xml_attr(xml_find_one(x, ".//polyline"), "fill"), "none")
})


test_that("lines lty becomes stroke-dasharray", {
  x <- xmlSVG({ plot(1:3,lty=1,type="l") })
  expect_true(
    is.na(xml_attr(suppressWarnings(xml_find_one(x,"//polyline")), "stroke-dasharray"))
  )
  x <- xmlSVG({ plot(1:3,lty=2,type="l") })
  expect_equal(
    xml_attr(suppressWarnings(xml_find_one(x,"//polyline")), "stroke-dasharray"),
    "5,5"
  )
  x <- xmlSVG({ plot(1:3,lty=3,type="l") })
  expect_equal(
    xml_attr(suppressWarnings(xml_find_one(x,"//polyline")), "stroke-dasharray"),
    "1,5"
  )
  x <- xmlSVG({ plot(1:3,lty=4,type="l") })
  expect_equal(
    xml_attr(suppressWarnings(xml_find_one(x,"//polyline")), "stroke-dasharray"),
    "1,5,5,5"
  )
  x <- xmlSVG({ plot(1:3,lty=5,type="l") })
  expect_equal(
    xml_attr(suppressWarnings(xml_find_one(x,"//polyline")), "stroke-dasharray"),
    "10,5"
  )
  x <- xmlSVG({ plot(1:3,lty=6,type="l") })
  expect_equal(
    xml_attr(suppressWarnings(xml_find_one(x,"//polyline")), "stroke-dasharray"),
    "10,5,5,5"
  )
})
