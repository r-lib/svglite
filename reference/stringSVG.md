# Run plotting code and return svg as string

This is useful primarily for testing but can be used as an alternative
to [`svgstring()`](https://svglite.r-lib.org/reference/svgstring.md).

## Usage

``` r
stringSVG(code, ...)
```

## Arguments

- code:

  Plotting code to execute.

- ...:

  Other arguments passed on to
  [`svgstring`](https://svglite.r-lib.org/reference/svgstring.md).

## Examples

``` r
stringSVG(plot(1:10))
#> Saving 6.67" x 6.67" image
#> <?xml version='1.0' encoding='UTF-8' ?>
#> <svg xmlns='http://www.w3.org/2000/svg' xmlns:xlink='http://www.w3.org/1999/xlink' width='480.00pt' height='480.00pt' viewBox='0 0 480.00 480.00'>
#> <g class='svglite'>
#> <defs>
#>   <style type='text/css'><![CDATA[
#>     .svglite line, .svglite polyline, .svglite polygon, .svglite path, .svglite rect, .svglite circle {
#>       fill: none;
#>       stroke: #000000;
#>       stroke-linecap: round;
#>       stroke-linejoin: round;
#>       stroke-miterlimit: 10.00;
#>     }
#>     .svglite text {
#>       white-space: pre;
#>     }
#>     .svglite g.glyphgroup path {
#>       fill: inherit;
#>       stroke: none;
#>     }
#>   ]]></style>
#> </defs>
#> <rect width='100%' height='100%' style='stroke: none; fill: #FFFFFF;'/>
#> <defs>
#>   <clipPath id='cpMC4wMHw0ODAuMDB8MC4wMHw0ODAuMDA='>
#>     <rect x='0.00' y='0.00' width='480.00' height='480.00' />
#>   </clipPath>
#> </defs>
#> <g clip-path='url(#cpMC4wMHw0ODAuMDB8MC4wMHw0ODAuMDA=)'>
#> </g>
#> <defs>
#>   <clipPath id='cpNTkuMDR8NDQ5Ljc2fDU5LjA0fDQwNi41Ng=='>
#>     <rect x='59.04' y='59.04' width='390.72' height='347.52' />
#>   </clipPath>
#> </defs>
#> <g clip-path='url(#cpNTkuMDR8NDQ5Ljc2fDU5LjA0fDQwNi41Ng==)'>
#> <circle cx='73.51' cy='393.69' r='2.70' style='stroke-width: 0.75;' />
#> <circle cx='113.71' cy='357.94' r='2.70' style='stroke-width: 0.75;' />
#> <circle cx='153.91' cy='322.18' r='2.70' style='stroke-width: 0.75;' />
#> <circle cx='194.10' cy='286.43' r='2.70' style='stroke-width: 0.75;' />
#> <circle cx='234.30' cy='250.68' r='2.70' style='stroke-width: 0.75;' />
#> <circle cx='274.50' cy='214.92' r='2.70' style='stroke-width: 0.75;' />
#> <circle cx='314.70' cy='179.17' r='2.70' style='stroke-width: 0.75;' />
#> <circle cx='354.89' cy='143.42' r='2.70' style='stroke-width: 0.75;' />
#> <circle cx='395.09' cy='107.66' r='2.70' style='stroke-width: 0.75;' />
#> <circle cx='435.29' cy='71.91' r='2.70' style='stroke-width: 0.75;' />
#> </g>
#> <g clip-path='url(#cpMC4wMHw0ODAuMDB8MC4wMHw0ODAuMDA=)'>
#> <line x1='113.71' y1='406.56' x2='435.29' y2='406.56' style='stroke-width: 0.75;' />
#> <line x1='113.71' y1='406.56' x2='113.71' y2='413.76' style='stroke-width: 0.75;' />
#> <line x1='194.10' y1='406.56' x2='194.10' y2='413.76' style='stroke-width: 0.75;' />
#> <line x1='274.50' y1='406.56' x2='274.50' y2='413.76' style='stroke-width: 0.75;' />
#> <line x1='354.89' y1='406.56' x2='354.89' y2='413.76' style='stroke-width: 0.75;' />
#> <line x1='435.29' y1='406.56' x2='435.29' y2='413.76' style='stroke-width: 0.75;' />
#> <text x='113.71' y='432.48' text-anchor='middle' style='font-size: 12.00px; font-family: "Liberation Sans";' textLength='6.67px' lengthAdjust='spacingAndGlyphs'>2</text>
#> <text x='194.10' y='432.48' text-anchor='middle' style='font-size: 12.00px; font-family: "Liberation Sans";' textLength='6.67px' lengthAdjust='spacingAndGlyphs'>4</text>
#> <text x='274.50' y='432.48' text-anchor='middle' style='font-size: 12.00px; font-family: "Liberation Sans";' textLength='6.67px' lengthAdjust='spacingAndGlyphs'>6</text>
#> <text x='354.89' y='432.48' text-anchor='middle' style='font-size: 12.00px; font-family: "Liberation Sans";' textLength='6.67px' lengthAdjust='spacingAndGlyphs'>8</text>
#> <text x='435.29' y='432.48' text-anchor='middle' style='font-size: 12.00px; font-family: "Liberation Sans";' textLength='13.34px' lengthAdjust='spacingAndGlyphs'>10</text>
#> <line x1='59.04' y1='357.94' x2='59.04' y2='71.91' style='stroke-width: 0.75;' />
#> <line x1='59.04' y1='357.94' x2='51.84' y2='357.94' style='stroke-width: 0.75;' />
#> <line x1='59.04' y1='286.43' x2='51.84' y2='286.43' style='stroke-width: 0.75;' />
#> <line x1='59.04' y1='214.92' x2='51.84' y2='214.92' style='stroke-width: 0.75;' />
#> <line x1='59.04' y1='143.42' x2='51.84' y2='143.42' style='stroke-width: 0.75;' />
#> <line x1='59.04' y1='71.91' x2='51.84' y2='71.91' style='stroke-width: 0.75;' />
#> <text transform='translate(41.76,357.94) rotate(-90)' text-anchor='middle' style='font-size: 12.00px; font-family: "Liberation Sans";' textLength='6.67px' lengthAdjust='spacingAndGlyphs'>2</text>
#> <text transform='translate(41.76,286.43) rotate(-90)' text-anchor='middle' style='font-size: 12.00px; font-family: "Liberation Sans";' textLength='6.67px' lengthAdjust='spacingAndGlyphs'>4</text>
#> <text transform='translate(41.76,214.92) rotate(-90)' text-anchor='middle' style='font-size: 12.00px; font-family: "Liberation Sans";' textLength='6.67px' lengthAdjust='spacingAndGlyphs'>6</text>
#> <text transform='translate(41.76,143.42) rotate(-90)' text-anchor='middle' style='font-size: 12.00px; font-family: "Liberation Sans";' textLength='6.67px' lengthAdjust='spacingAndGlyphs'>8</text>
#> <text transform='translate(41.76,71.91) rotate(-90)' text-anchor='middle' style='font-size: 12.00px; font-family: "Liberation Sans";' textLength='13.34px' lengthAdjust='spacingAndGlyphs'>10</text>
#> <polygon points='59.04,406.56 449.76,406.56 449.76,59.04 59.04,59.04 ' style='stroke-width: 0.75;' />
#> <text x='254.40' y='461.28' text-anchor='middle' style='font-size: 12.00px; font-family: "Liberation Sans";' textLength='29.34px' lengthAdjust='spacingAndGlyphs'>Index</text>
#> <text transform='translate(12.96,232.80) rotate(-90)' text-anchor='middle' style='font-size: 12.00px; font-family: "Liberation Sans";' textLength='23.34px' lengthAdjust='spacingAndGlyphs'>1:10</text>
#> </g>
#> </g>
#> </svg>
```
