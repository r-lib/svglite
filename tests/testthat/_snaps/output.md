# bg is validated before opening the device

    Code
      svglite(tempfile(), bg = 1)
    Condition
      Error in `validate_bg()`:
      ! `bg` must be a single string naming a colour.

---

    Code
      svgstring(bg = 1)
    Condition
      Error in `validate_bg()`:
      ! `bg` must be a single string naming a colour.

