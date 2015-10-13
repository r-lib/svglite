//  (C) 2002 T Jake Luciani: SVG device, based on PicTex device
//  (C) 2008 Tony Plate: Line type support from RSVGTipsDevice package
//  (C) 2012 Matthieu Decorde: UTF-8 support, XML reserved characters and XML header
//  (C) 2015 RStudio (Hadley Wickham): modernisation & refactoring
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#include "Rcpp.h"
#include <gdtools.h>
#include <string.h>
#include "R_ext/GraphicsEngine.h"

// SVG device metadata
class SVGDesc {
public:
  FILE *file;
  std::string filename;
  int pageno;
  double clipleft, clipright, cliptop, clipbottom;
  bool standalone;
  XPtrCairoContext cc;

  SVGDesc(std::string filename_, bool standalone_):
      filename(filename_),
      pageno(0),
      standalone(standalone_),
      cc(gdtools::context_create()) {
    file = fopen(R_ExpandFileName(filename.c_str()), "w");
  }

  bool ok() const {
    return file != NULL;
  }

  ~SVGDesc() {
    if (ok())
      fclose(file);
  }
};

inline bool is_bold(int face) {
  return face == 2 || face == 4;
}
inline bool is_italic(int face) {
  return face == 3 || face == 4;
}

inline std::string fontname(const char* family_, int face) {
  std::string family(family_);
  if( face == 5 ) return "symbol";

  if (family == "mono") {
    return "courier";
  } else if (family == "serif") {
    return "Times New Roman";
  } else if (family == "sans" || family == "") {
    return "Arial";
  } else {
    return family;
  }
}

void write_escaped(FILE* f, const char* text) {
  for(const char* cur = text; *cur != '\0'; ++cur) {
    switch(*cur) {
    case '&': fputs("&amp;", f); break;
    case '<': fputs("&lt;",  f); break;
    case '>': fputs("&gt;",  f); break;
    default:  fputc(*cur,    f);
    }
  }
}

void write_attr_col(FILE* f, const char* attr, unsigned int col) {
  int alpha = R_ALPHA(col);

  if (col == NA_INTEGER || alpha == 0) {
    fprintf(f, " %s='none'", attr);
    return;
  } else {
    fprintf(f, " %s='#%02X%02X%02X'", attr, R_RED(col), R_GREEN(col), R_BLUE(col));
    if (alpha != 255)
      fprintf(f, " %s-opacity='%0.2f'", attr, alpha / 255.0);
  }
}

void write_attr_dbl(FILE* f, const char* attr, double value) {
  fprintf(f, " %s='%.2f'", attr, value);
}

void write_attr_str(FILE* f, const char* attr, const char* value) {
  fprintf(f, " %s='%s'", attr, value);
}

static void write_attrs_linetype(FILE* f, int lty, double lwd, int col) {
  write_attr_col(f, "stroke", col);

  // 1 lwd = 1/96", but units in rest of document are 1/72"
  write_attr_dbl(f, "stroke-width", lwd / 96 * 72);

  // Set line pattern type
  switch (lty) {
  case LTY_BLANK: // never called: blank lines never get to this point
  case LTY_SOLID: // default svg setting, so don't need to write out
    break;
  default:
    // See comment in GraphicsEngine.h for how this works
    fputs(" stroke-dasharray='", f);
    for(int i = 0 ; i < 8 && lty & 15; i++) {
      fprintf(f, "%i ", (int) lwd * (lty & 15));
      lty = lty >> 4;
    }
    fputs("'", f);
    break;
  }
}

// Callback functions for graphics device --------------------------------------

static void svg_metric_info(int c, const pGEcontext gc, double* ascent,
                            double* descent, double* width, pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;

  // Convert to string - negative implies unicode code point
  char str[16];
  if (c < 0) {
    Rf_ucstoutf8(str, (unsigned int) -c);
  } else {
    str[0] = (char) c;
    str[1] = '\0';
  }

  gdtools::context_set_font(svgd->cc, fontname(gc->fontfamily, gc->fontface),
    gc->cex * gc->ps, is_bold(gc->fontface), is_italic(gc->fontface));
  FontMetric fm = gdtools::context_extents(svgd->cc, std::string(str));

  *ascent = fm.ascent;
  *descent = fm.descent;
  *width = fm.width;
}

static void svg_clip(double x0, double x1, double y0, double y1, pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;

  svgd->clipleft = x0;
  svgd->clipright = x1;
  svgd->clipbottom = y0;
  svgd->cliptop = y1;
}

static void svg_new_page(const pGEcontext gc, pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;

  if (svgd->pageno > 0) {
    Rf_error("RSvgDevice only supports one page");
  }

  if (svgd->standalone)
    fputs("<?xml version='1.0' encoding='UTF-8' ?>\n", svgd->file);

  fputs("<svg ", svgd->file);
  if (svgd->standalone){
    fputs("xmlns='http://www.w3.org/2000/svg' ", svgd->file);
    fputs("xmlns:xlink='http://www.w3.org/1999/xlink' ", svgd->file);//http://www.w3.org/wiki/SVG_Links
  }

  fprintf(svgd->file, "viewBox='0 0 %.2f %.2f'>\n", dd->right, dd->bottom);

  fputs("<rect width='100%' height='100%'", svgd->file);
  write_attr_col(svgd->file, "fill", gc->fill);
  fputs("/>\n", svgd->file);

  svgd->pageno++;
}

static void svg_close(pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;

  if (svgd->pageno > 0)
    fputs("</svg>\n", svgd->file);

  delete(svgd);
}

static void svg_line(double x1, double y1, double x2, double y2,
                     const pGEcontext gc, pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;

  fprintf(svgd->file, "<line x1='%.2f' y1='%.2f' x2='%.2f' y2='%.2f'",
    x1, y1, x2, y2);

  write_attrs_linetype(svgd->file, gc->lty, gc->lwd, gc->col);
  fputs(" />\n", svgd->file);
}

void svg_poly(int n, double *x, double *y, int filled, const pGEcontext gc,
              pDevDesc dd) {

  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;
  fputs("<polyline points='", svgd->file);

  for (int i = 0; i < n; i++) {
    fprintf(svgd->file, "%.2f,%.2f ", x[i], y[i]);
  }
  fputs("'", svgd->file);

  write_attr_col(svgd->file, "fill", filled ? gc->fill : NA_INTEGER);
  write_attrs_linetype(svgd->file, gc->lty, gc->lwd, gc->col);

  fputs(" />\n", svgd->file);
}

static void svg_polyline(int n, double *x, double *y, const pGEcontext gc,
                         pDevDesc dd) {
  svg_poly(n, x, y, 0, gc, dd);
}
static void svg_polygon(int n, double *x, double *y, const pGEcontext gc,
                        pDevDesc dd) {
  svg_poly(n, x, y, 1, gc, dd);
}

static double svg_strwidth(const char *str, const pGEcontext gc, pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;

  gdtools::context_set_font(svgd->cc, fontname(gc->fontfamily, gc->fontface),
    gc->cex * gc->ps, is_bold(gc->fontface), is_italic(gc->fontface));
  FontMetric fm = gdtools::context_extents(svgd->cc, std::string(str));

  return fm.width;
}

static void svg_rect(double x0, double y0, double x1, double y1,
                     const pGEcontext gc, pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;

  // x and y give top-left position
  fprintf(svgd->file,
      "<rect x='%.2f' y='%.2f' width='%.2f' height='%.2f'",
      fmin(x0, x1), fmin(y0, y1), fabs(x1 - x0), fabs(y1 - y0));

  write_attr_col(svgd->file, "fill", gc->fill);
  write_attrs_linetype(svgd->file, gc->lty, gc->lwd, gc->col);
  fputs(" />\n", svgd->file);
}

static void svg_circle(double x, double y, double r, const pGEcontext gc,
                       pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;

  fprintf(svgd->file, "<circle cx='%.2f' cy='%.2f' r='%.2f'", x, y, r * 1.5);
  write_attr_col(svgd->file, "fill", gc->fill);
  write_attrs_linetype(svgd->file, gc->lty, gc->lwd, gc->col);
  fputs(" />\n", svgd->file);
}

static void svg_text(double x, double y, const char *str, double rot,
                     double hadj, const pGEcontext gc, pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;


  fputs("<text", svgd->file);
  if (rot == 0) {
    write_attr_dbl(svgd->file, "x", x);
    write_attr_dbl(svgd->file, "y", y);
  } else {
    fprintf(svgd->file, " transform='translate(%.2f,%.2f) rotate(%0.0f)'", x, y,
      -1.0 * rot);
  }

  write_attr_dbl(svgd->file, "font-size", gc->cex * gc->ps);
  if (is_bold(gc->fontface))
    write_attr_str(svgd->file, "font-weight", "bold");
  if (is_italic(gc->fontface))
    write_attr_str(svgd->file, "font-style", "italic");
  if (gc->col != -16777216) // black
    write_attr_col(svgd->file, "fill", gc->col);

  std::string font = fontname(gc->fontfamily, gc->fontface);
  write_attr_str(svgd->file, "font-family", font.c_str());

  fputs(">", svgd->file);

  write_escaped(svgd->file, str);

  fputs("</text>\n", svgd->file);
}

static void svg_size(double *left, double *right, double *bottom, double *top,
                     pDevDesc dd) {
  *left = dd->left;
  *right = dd->right;
  *bottom = dd->bottom;
  *top = dd->top;
}

static void svg_raster(unsigned int *raster, int w, int h,
                       double x, double y,
                       double width, double height,
                       double rot,
                       Rboolean interpolate,
                       const pGEcontext gc, pDevDesc dd)
{
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;

  if (height < 0)
    height = -height;

  std::vector<unsigned int> raster_(w*h);
  for ( int i = 0 ; i < raster_.size(); i++) {
    raster_[i] = raster[i] ;
  }

  std::string base64_str = gdtools::raster_to_str(raster_, w, h, width, height, (Rboolean) interpolate);

  fputs("<image", svgd->file);
  write_attr_dbl(svgd->file, "width", width);
  write_attr_dbl(svgd->file, "height", height);
  write_attr_dbl(svgd->file, "x", x);
  write_attr_dbl(svgd->file, "y", y - height);

  if( rot != 0 ){
    fprintf(svgd->file, " transform='rotate(%0.0f,%.2f,%.2f)'", -1.0 * rot, x, y);
  }

  fprintf(svgd->file, " xlink:href='data:image/png;base64,%s'", base64_str.c_str());
  fputs( "/>", svgd->file);
}


pDevDesc svg_driver_new(std::string filename, int bg, double width,
                        double height, int pointsize, bool standalone) {

  pDevDesc dd = (DevDesc*) calloc(1, sizeof(DevDesc));
  if (dd == NULL)
    return dd;

  dd->startfill = bg;
  dd->startcol = R_RGB(0, 0, 0);
  dd->startps = pointsize;
  dd->startlty = 0;
  dd->startfont = 1;
  dd->startgamma = 1;

  // Callbacks
  dd->activate = NULL;
  dd->deactivate = NULL;
  dd->close = svg_close;
  dd->clip = svg_clip;
  dd->size = svg_size;
  dd->newPage = svg_new_page;
  dd->line = svg_line;
  dd->text = svg_text;
  dd->strWidth = svg_strwidth;
  dd->rect = svg_rect;
  dd->circle = svg_circle;
  dd->polygon = svg_polygon;
  dd->polyline = svg_polyline;
  dd->mode = NULL;
  dd->metricInfo = svg_metric_info;
  dd->cap = NULL;
  dd->raster = svg_raster;

  // UTF-8 support
  dd->wantSymbolUTF8 = (Rboolean) 1;
  dd->hasTextUTF8 = (Rboolean) 1;
  dd->textUTF8 = svg_text;
  dd->strWidthUTF8 = svg_strwidth;

  // Screen Dimensions in pts
  dd->left = 0;
  dd->top = 0;
  dd->right = width * 72;
  dd->bottom = height * 72;

  // Magic constants copied from other graphics devices
  // nominal character sizes in pts
  dd->cra[0] = 0.9 * pointsize;
  dd->cra[1] = 1.2 * pointsize;
  // character alignment offsets
  dd->xCharOffset = 0.4900;
  dd->yCharOffset = 0.3333;
  dd->yLineBias = 0.2;
  // inches per pt
  dd->ipr[0] = 1.0 / 72.0;
  dd->ipr[1] = 1.0 / 72.0;

  // Capabilities
  dd->canClip = FALSE;
  dd->canHAdj = 0;
  dd->canChangeGamma = FALSE;
  dd->displayListOn = FALSE;
  dd->haveTransparency = 2;
  dd->haveTransparentBg = 2;

  dd->deviceSpecific = new SVGDesc(filename, standalone);
  return dd;
}

// [[Rcpp::export]]
bool devSVG_(std::string file, std::string bg_, int width, int height,
             int pointsize, bool standalone) {

  int bg = R_GE_str2col(bg_.c_str());

  R_GE_checkVersionOrDie(R_GE_version);
  R_CheckDeviceAvailable();
  BEGIN_SUSPEND_INTERRUPTS {
    pDevDesc dev = svg_driver_new(file, bg, width, height, pointsize, standalone);
    if (dev == NULL)
      Rcpp::stop("Failed to start SVG device");

    pGEDevDesc dd = GEcreateDevDesc(dev);
    GEaddDevice2(dd, "devSVG");
    GEinitDisplayList(dd);

  } END_SUSPEND_INTERRUPTS;

  return true;
}

