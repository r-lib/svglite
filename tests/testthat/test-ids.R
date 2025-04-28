test_that("ids are assigned as expecter", {
  sd <- svgstring()
  plot(1:10, 1:10)
  plot(1:10, 1:10)
  dev.off()
  svg <- sd()
  expect_identical(svg[1], svg[2])

  sd <- svgstring(id = "test")
  plot(1:10, 1:10)
  plot(1:10, 1:10)
  dev.off()
  svg <- sd()
  expect_identical(svg[1], svg[2])

  expect_true(grepl("id='test'", svg[1]))

  sd <- svgstring(id = c("test", "test2"))
  plot(1:10, 1:10)
  plot(1:10, 1:10)
  expect_snapshot_warning(plot(1:10, 1:10))
  dev.off()
  svg <- sd()

  expect_true(grepl("id='test'", svg[1]))

  expect_true(grepl("id='test2'", svg[2]))

  expect_false(grepl("id='test", svg[3]))
})
