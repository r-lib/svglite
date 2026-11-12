library(xml2)
library(grid)

test_that("alpha mask applied to groups", {
  skip_if_not(getRversion() >= "4.2.0")

  mask <- as.mask(
    rectGrob(x = 0.5, y = 0.5, width = 0.25, height = 0.25,
  		   gp = gpar(col = NA, fill = "white")),
    type = "alpha"
  )

  # without transform (grid.group)
  x_g <- xmlSVG({
    grid.newpage()
    grid.group(rectGrob(gp = gpar(col = NA, fill = "blue")),
               vp = viewport(mask = mask))
  })
  use_node <- xml_find_first(x_g, ".//use")
  expect_equal(xml_attr(use_node, "mask"), "url(#mask-0)")

  # alpha mask applied to group with transform (grid.define/grid.use):
  # mask must be on a wrapper <g> (no transform) and not on the transform <g>
  # so that mask coordinates resolve in the parent coordinate space
  x_u <- xmlSVG({
    grid.newpage()
    grid.define(rectGrob(gp = gpar(col = NA, fill = "blue")), name = "box")
    grid.use("box", vp = viewport(mask = mask))
  })
  g_node <- xml_find_first(x_u, ".//g[contains(@style,'transform:matrix')]")
  expect_true(is.na(xml_attr(g_node, "mask")))
  expect_equal(xml_attr(xml_parent(g_node), "mask"), "url(#mask-0)")
})
