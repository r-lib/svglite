mini_plot <- function(...) graphics::plot(..., axes = FALSE, xlab = "", ylab = "")

plot_dim <- function(dim = c(NA, NA)) {
  if (any(is.na(dim))) {
    if (length(grDevices::dev.list()) == 0) {
      default_dim <- c(7, 7)
    } else {
      default_dim <- grDevices::dev.size()
    }

    dim[is.na(dim)] <- default_dim[is.na(dim)]
    dim_f <- prettyNum(dim, digits = 3)

    message("Saving ", dim_f[1], "\" x ", dim_f[2], "\" image")
  }

  dim
}
