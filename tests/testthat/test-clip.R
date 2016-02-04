context("Clipping")

test_that("create a plot that uses clipping", {
  svglite("test-clip.svg", 16, 8)
  par(mfrow = c(1, 2))

  set.seed(0)
  mat <- matrix(runif(64), nrow = 8)

  plot(0, 0, xlim = c(-1, 1), ylim = c(-1, 1), asp = 1, main = "Without clipping")
  rect(-0.5, -0.5, 0.5, 0.5, col = "blue")
  text(0, 0.5, "Clipping", cex = 5, srt = 30)
  abline(h = 0.5, col = "red")
  rasterImage(mat, -0.5, -1, 0.5, -0.5, angle = -30)

  plot(0, 0, xlim = c(-1, 1), ylim = c(-1, 1), asp = 1, main = "With clipping")
  clip(-1, 0, -1, 0)
  rect(-0.5, -0.5, 0.5, 0.5, col = "blue")
  clip(0, 1, 0, 1)
  text(0, 0.5, "Clipping", cex = 5, srt = 30)
  clip(-1, 0, 0, 1)
  abline(h = 0.5, col = "red")
  clip(0, 1, -1, 0)
  rasterImage(mat, -0.5, -1, 0.5, -0.5, angle = -30)

  dev.off()
})
