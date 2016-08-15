style_attr <- function(nodes, attr) {
  style <- xml_attr(nodes, "style")
  ifelse(
    grepl(sprintf("%s: [^;]*;", attr), style),
    gsub(sprintf(".*%s: ([^;]*);.*", attr), "\\1", style),
    NA_character_
  )
}

dash_array <- function(...) {
  x <- xmlSVG(mini_plot(1:3, ..., type = "l"))
  dash <- style_attr(xml_find_first(x, "//polyline"), "stroke-dasharray")
  as.numeric(strsplit(dash, ",")[[1]])
}
