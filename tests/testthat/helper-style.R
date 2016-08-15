style_attr <- function(nodes, attr) {
  style <- xml_attr(nodes, "style")
  ifelse(
    grepl(sprintf("%s: [^;]*;", attr), style),
    gsub(sprintf(".*%s: ([^;]*);.*", attr), "\\1", style),
    NA_character_
  )
}

