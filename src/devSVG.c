//  (C) 2012 Matthieu Decorde: UTF-8 support, XML reserved characters and XML header
//  (C) 2008 Tony Plate: Line type support from RSVGTipsDevice package
//  (C) 2002 T Jake Luciani: SVG device, based on PicTex device
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
#include <string.h>

#include "R.h"
#include "Rinternals.h"
#include "R_ext/GraphicsEngine.h"

// device-specific information per SVG device //

#define DOTSperIN       72.27
#define in2dots(x)      (DOTSperIN * x)

typedef struct {
  FILE *texfp;
  char filename[1024];
  int pageno;
  double width;
  double height;
  double clipleft, clipright, cliptop, clipbottom;

  Rboolean xmlHeader;
  Rboolean useNS;

} SVGDesc;

static double charwidth[4][128] = {
  { 0.5416690, 0.8333360, 0.7777810,
    0.6111145, 0.6666690, 0.7083380, 0.7222240, 0.7777810, 0.7222240,
    0.7777810, 0.7222240, 0.5833360, 0.5361130, 0.5361130, 0.8138910,
    0.8138910, 0.2388900, 0.2666680, 0.5000020, 0.5000020, 0.5000020,
    0.5000020, 0.5000020, 0.6666700, 0.4444460, 0.4805580, 0.7222240,
    0.7777810, 0.5000020, 0.8611145, 0.9722260, 0.7777810, 0.2388900,
    0.3194460, 0.5000020, 0.8333360, 0.5000020, 0.8333360, 0.7583360,
    0.2777790, 0.3888900, 0.3888900, 0.5000020, 0.7777810, 0.2777790,
    0.3333340, 0.2777790, 0.5000020, 0.5000020, 0.5000020, 0.5000020,
    0.5000020, 0.5000020, 0.5000020, 0.5000020, 0.5000020, 0.5000020,
    0.5000020, 0.2777790, 0.2777790, 0.3194460, 0.7777810, 0.4722240,
    0.4722240, 0.6666690, 0.6666700, 0.6666700, 0.6388910, 0.7222260,
    0.5972240, 0.5694475, 0.6666690, 0.7083380, 0.2777810, 0.4722240,
    0.6944480, 0.5416690, 0.8750050, 0.7083380, 0.7361130, 0.6388910,
    0.7361130, 0.6458360, 0.5555570, 0.6805570, 0.6875050, 0.6666700,
    0.9444480, 0.6666700, 0.6666700, 0.6111130, 0.2888900, 0.5000020,
    0.2888900, 0.5000020, 0.2777790, 0.2777790, 0.4805570, 0.5166680,
    0.4444460, 0.5166680, 0.4444460, 0.3055570, 0.5000020, 0.5166680,
    0.2388900, 0.2666680, 0.4888920, 0.2388900, 0.7944470, 0.5166680,
    0.5000020, 0.5166680, 0.5166680, 0.3416690, 0.3833340, 0.3611120,
    0.5166680, 0.4611130, 0.6833360, 0.4611130, 0.4611130, 0.4347230,
    0.5000020, 1.0000030, 0.5000020, 0.5000020, 0.5000020 },
  { 0.5805590,
        0.9166720, 0.8555600, 0.6722260, 0.7333370, 0.7944490, 0.7944490,
        0.8555600, 0.7944490, 0.8555600, 0.7944490, 0.6416700, 0.5861150,
        0.5861150, 0.8916720, 0.8916720, 0.2555570, 0.2861130, 0.5500030,
        0.5500030, 0.5500030, 0.5500030, 0.5500030, 0.7333370, 0.4888920,
        0.5652800, 0.7944490, 0.8555600, 0.5500030, 0.9472275, 1.0694500,
        0.8555600, 0.2555570, 0.3666690, 0.5583360, 0.9166720, 0.5500030,
        1.0291190, 0.8305610, 0.3055570, 0.4277800, 0.4277800, 0.5500030,
        0.8555600, 0.3055570, 0.3666690, 0.3055570, 0.5500030, 0.5500030,
        0.5500030, 0.5500030, 0.5500030, 0.5500030, 0.5500030, 0.5500030,
        0.5500030, 0.5500030, 0.5500030, 0.3055570, 0.3055570, 0.3666690,
        0.8555600, 0.5194470, 0.5194470, 0.7333370, 0.7333370, 0.7333370,
        0.7027820, 0.7944490, 0.6416700, 0.6111145, 0.7333370, 0.7944490,
        0.3305570, 0.5194470, 0.7638930, 0.5805590, 0.9777830, 0.7944490,
        0.7944490, 0.7027820, 0.7944490, 0.7027820, 0.6111145, 0.7333370,
        0.7638930, 0.7333370, 1.0388950, 0.7333370, 0.7333370, 0.6722260,
        0.3430580, 0.5583360, 0.3430580, 0.5500030, 0.3055570, 0.3055570,
        0.5250030, 0.5611140, 0.4888920, 0.5611140, 0.5111140, 0.3361130,
        0.5500030, 0.5611140, 0.2555570, 0.2861130, 0.5305590, 0.2555570,
        0.8666720, 0.5611140, 0.5500030, 0.5611140, 0.5611140, 0.3722250,
        0.4216690, 0.4041690, 0.5611140, 0.5000030, 0.7444490, 0.5000030,
        0.5000030, 0.4763920, 0.5500030, 1.1000060, 0.5500030, 0.5500030,
        0.550003 }, { 0.5416690, 0.8333360, 0.7777810, 0.6111145, 0.6666690,
            0.7083380, 0.7222240, 0.7777810, 0.7222240, 0.7777810, 0.7222240,
            0.5833360, 0.5361130, 0.5361130, 0.8138910, 0.8138910, 0.2388900,
            0.2666680, 0.5000020, 0.5000020, 0.5000020, 0.5000020, 0.5000020,
            0.7375210, 0.4444460, 0.4805580, 0.7222240, 0.7777810, 0.5000020,
            0.8611145, 0.9722260, 0.7777810, 0.2388900, 0.3194460, 0.5000020,
            0.8333360, 0.5000020, 0.8333360, 0.7583360, 0.2777790, 0.3888900,
            0.3888900, 0.5000020, 0.7777810, 0.2777790, 0.3333340, 0.2777790,
            0.5000020, 0.5000020, 0.5000020, 0.5000020, 0.5000020, 0.5000020,
            0.5000020, 0.5000020, 0.5000020, 0.5000020, 0.5000020, 0.2777790,
            0.2777790, 0.3194460, 0.7777810, 0.4722240, 0.4722240, 0.6666690,
            0.6666700, 0.6666700, 0.6388910, 0.7222260, 0.5972240, 0.5694475,
            0.6666690, 0.7083380, 0.2777810, 0.4722240, 0.6944480, 0.5416690,
            0.8750050, 0.7083380, 0.7361130, 0.6388910, 0.7361130, 0.6458360,
            0.5555570, 0.6805570, 0.6875050, 0.6666700, 0.9444480, 0.6666700,
            0.6666700, 0.6111130, 0.2888900, 0.5000020, 0.2888900, 0.5000020,
            0.2777790, 0.2777790, 0.4805570, 0.5166680, 0.4444460, 0.5166680,
            0.4444460, 0.3055570, 0.5000020, 0.5166680, 0.2388900, 0.2666680,
            0.4888920, 0.2388900, 0.7944470, 0.5166680, 0.5000020, 0.5166680,
            0.5166680, 0.3416690, 0.3833340, 0.3611120, 0.5166680, 0.4611130,
            0.6833360, 0.4611130, 0.4611130, 0.4347230, 0.5000020, 1.0000030,
            0.5000020, 0.5000020, 0.5000020 }, { 0.5805590, 0.9166720, 0.8555600,
                0.6722260, 0.7333370, 0.7944490, 0.7944490, 0.8555600, 0.7944490,
                0.8555600, 0.7944490, 0.6416700, 0.5861150, 0.5861150, 0.8916720,
                0.8916720, 0.2555570, 0.2861130, 0.5500030, 0.5500030, 0.5500030,
                0.5500030, 0.5500030, 0.8002530, 0.4888920, 0.5652800, 0.7944490,
                0.8555600, 0.5500030, 0.9472275, 1.0694500, 0.8555600, 0.2555570,
                0.3666690, 0.5583360, 0.9166720, 0.5500030, 1.0291190, 0.8305610,
                0.3055570, 0.4277800, 0.4277800, 0.5500030, 0.8555600, 0.3055570,
                0.3666690, 0.3055570, 0.5500030, 0.5500030, 0.5500030, 0.5500030,
                0.5500030, 0.5500030, 0.5500030, 0.5500030, 0.5500030, 0.5500030,
                0.5500030, 0.3055570, 0.3055570, 0.3666690, 0.8555600, 0.5194470,
                0.5194470, 0.7333370, 0.7333370, 0.7333370, 0.7027820, 0.7944490,
                0.6416700, 0.6111145, 0.7333370, 0.7944490, 0.3305570, 0.5194470,
                0.7638930, 0.5805590, 0.9777830, 0.7944490, 0.7944490, 0.7027820,
                0.7944490, 0.7027820, 0.6111145, 0.7333370, 0.7638930, 0.7333370,
                1.0388950, 0.7333370, 0.7333370, 0.6722260, 0.3430580, 0.5583360,
                0.3430580, 0.5500030, 0.3055570, 0.3055570, 0.5250030, 0.5611140,
                0.4888920, 0.5611140, 0.5111140, 0.3361130, 0.5500030, 0.5611140,
                0.2555570, 0.2861130, 0.5305590, 0.2555570, 0.8666720, 0.5611140,
                0.5500030, 0.5611140, 0.5611140, 0.3722250, 0.4216690, 0.4041690,
                0.5611140, 0.5000030, 0.7444490, 0.5000030, 0.5000030, 0.4763920,
                0.5500030, 1.1000060, 0.5500030, 0.5500030, 0.550003 } };

void write_escaped(FILE* f, const char* text) {
  for(const char* cur = text; *cur != '\0'; ++cur) {
    switch(*cur) {
    case '&': fprintf(f, "&amp;"); break;
    case '<': fprintf(f, "&lt;"); break;
    case '>': fprintf(f, "&gt;"); break;
    default: fputc(*cur, f);
    }
  }
}

void write_colour(FILE* f, unsigned int col) {
  int alpha = R_ALPHA(col);

  if (col == NA_INTEGER || alpha == 0) {
    fprintf(f, "none");
    return;
  } else if (alpha == 255) {
    fprintf(f, "#%02X%02X%02X", R_RED(col), R_GREEN(col), R_BLUE(col));
  } else {
    fprintf(f, "rgba(%i, %i, %i, %0.2f)", R_RED(col), R_GREEN(col), R_BLUE(col),
      alpha / 255.0);
  }
}

static void SetLinetype(int newlty, int newlwd, pDevDesc dd, int fgcol, int col) {
  SVGDesc *ptd = (SVGDesc *) dd->deviceSpecific;
  int i, dashleft;;

  fprintf(ptd->texfp, "stroke='");
  write_colour(ptd->texfp, col);
  fprintf(ptd->texfp, "' ");

  fprintf(ptd->texfp, "fill='");
  write_colour(ptd->texfp, fgcol);
  fprintf(ptd->texfp, "' ");

  // Set line size + color
  fprintf(ptd->texfp, "style=\"stroke-width:%d;", newlwd);

  // Set line pattern type
  // ADD: mdecorde from SVGTips device, (C) 2008 Tony Plate
  dashleft = newlty;
  if (dashleft) {
    fprintf(ptd->texfp, ";stroke-dasharray:");

    dashleft = newlty;
    for(i=0 ; i<8 && dashleft & 15 ; i++) {
      // dashlen is dashleft & 15
      if (i>0)
        fprintf(ptd->texfp, ", ");
      fprintf(ptd->texfp, "%d", dashleft & 15);
      dashleft = dashleft >> 4;
    }
    //    switch (ptd->lty) {
    //
    //    case LTY_BLANK:
    //      break;
    //    case LTY_SOLID:
    //      fprintf(ptd->texfp, ";stroke-dasharray");
    //      break;
    //    case LTY_DOTTED:
    //      fprintf(ptd->texfp, ";stroke-dasharray:1, 3");
    //      break;
    //    case LTY_DASHED:
    //      fprintf(ptd->texfp, ";stroke-dasharray:4, 4");
    //      break;
    //    case LTY_LONGDASH:
    //      fprintf(ptd->texfp, ";stroke-dasharray:6, 6");
    //      break;
    //    case LTY_DOTDASH:
    //      fprintf(ptd->texfp, ";stroke-dasharray:4, 2, 1, 2");
    //      break;
    //    case LTY_TWODASH:
    //      fprintf(ptd->texfp, ";stroke-dasharray:8, 2, 1, 2");
    //      break;
    //    default:
    //      fprintf(ptd->texfp, ";stroke-dasharray %d", ptd->lty);
    //      break;
    //    }
  }
  fprintf(ptd->texfp, "\"");
}

static void SetFont(int face, int size, unsigned int col, SVGDesc *ptd) {
  int lface = face, lsize = size;
  if (lface < 1 || lface > 4)
    lface = 1;
  if (lsize < 1 || lsize > 24)
    lsize = 10;

  fprintf(ptd->texfp, " font-size='%d'", lsize);

  fprintf(ptd->texfp, " fill='");
  write_colour(ptd->texfp, col);
  fprintf(ptd->texfp, "'");
}

static void SVG_MetricInfo(int c, const pGEcontext gc, double* ascent,
    double* descent, double* width, pDevDesc dd) {

  //  Rboolean Unicode = mbcslocale && (gc->fontface != 5);
    //     if (c < 0) { Unicode = TRUE; c = -c; }
    //     if(Unicode) UniCharMetric(c, ...); else CharMetric(c, ...);
  // metric information not available => return 0,0,0 */
  *ascent = 0.0;
  *descent = 0.0;
  *width = 0.0;
}

static void SVG_Clip(double x0, double x1, double y0, double y1, pDevDesc dd) {
  SVGDesc *ptd = (SVGDesc *) dd->deviceSpecific;

  ptd->clipleft = x0;
  ptd->clipright = x1;
  ptd->clipbottom = y0;
  ptd->cliptop = y1;
}

// Start a new page

static void SVG_NewPage(const pGEcontext gc, pDevDesc dd) {
  SVGDesc *ptd = (SVGDesc *) dd->deviceSpecific;

  if (ptd->pageno > 0) {
    Rf_error("RSvgDevice only supports one page");
  }

  if (ptd->xmlHeader)
    fprintf(ptd->texfp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");

  fprintf(ptd->texfp, "<svg ");
  if (ptd->useNS)
    fprintf(ptd->texfp, "xmlns=\"http://www.w3.org/2000/svg\" ");

  fprintf(ptd->texfp, "width=\"%.2f\" height=\"%.2f\" ",
      in2dots(ptd->width), in2dots(ptd->height));
  fprintf(ptd->texfp, "viewBox=\"0,0,%.2f,%.2f\">\n", in2dots(ptd->width),
      in2dots(ptd->height));

  fprintf(ptd->texfp, "<rect width=\"100%%\" height=\"100%%\" fill='");
  write_colour(ptd->texfp, dd->startfill);
  fprintf(ptd->texfp, "' />\n");

  ptd->pageno++;
}

// Close down the driver

static void SVG_Close(pDevDesc dd) {
  SVGDesc *ptd = (SVGDesc *) dd->deviceSpecific;

  fprintf(ptd->texfp, "</svg>\n");

  fclose(ptd->texfp);

  free(ptd);
}

static void SVG_Line(double x1, double y1, double x2, double y2,
    const pGEcontext gc, pDevDesc dd) {
  SVGDesc *ptd = (SVGDesc *) dd->deviceSpecific;

  fprintf(ptd->texfp, "<line x1=\"%.2f\" y1=\"%.2f\" x2=\"%.2f\" ", x1, y1,
      x2);

  fprintf(ptd->texfp, "y2=\"%.2f\" ", y2);

  SetLinetype(gc->lty, gc->lwd, dd, NA_INTEGER, gc->col);
  fprintf(ptd->texfp, "/>\n");
}

static void SVG_Polyline(int n, double *x, double *y, const pGEcontext gc,
      pDevDesc dd) {
  int i;
  SVGDesc *ptd = (SVGDesc *) dd->deviceSpecific;
  fprintf(ptd->texfp, "<polyline points=\"");

  for (i = 0; i < n; i++) {
    fprintf(ptd->texfp, "%.2f , %.2f ", x[i], y[i]);
  }
  fprintf(ptd->texfp, "\" ");

  SetLinetype(gc->lty, gc->lwd, dd, NA_INTEGER, gc->col);

  fprintf(ptd->texfp, "/>\n");
}

static double SVG_StrWidth(const char *str, const pGEcontext gc, pDevDesc dd) {
  int fontface = gc->fontface == NA_INTEGER ? 0 : gc->fontface;

  double sum = 0;
  for (const char *p = str; *p; p++)
    sum += charwidth[fontface][(int) *p];

  int size = gc->cex * gc->ps + 0.5;
  return sum * size;
}

static void SVG_Rect(double x0, double y0, double x1, double y1,
    const pGEcontext gc, pDevDesc dd) {
  double tmp;
  SVGDesc *ptd = (SVGDesc *) dd->deviceSpecific;

  // Make sure width and height are positive
  if (x0 >= x1) {
    tmp = x0;
    x0 = x1;
    x1 = tmp;
  }

  if (y0 >= y1) {
    tmp = y0;
    y0 = y1;
    y1 = tmp;
  }

  fprintf(ptd->texfp,
      "<rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\" ", x0,
      y0, x1 - x0, y1 - y0);

  SetLinetype(gc->lty, gc->lwd, dd, gc->fill, gc->col);
  fprintf(ptd->texfp, " />\n");
}

static void SVG_Circle(double x, double y, double r, const pGEcontext gc,
    pDevDesc dd) {
  SVGDesc *ptd = (SVGDesc *) dd->deviceSpecific;

  fprintf(ptd->texfp, "<circle cx=\"%.2f\" cy=\"%.2f\" r=\"%.2f\" ", x, y,
      r * 1.5);

  SetLinetype(gc->lty, gc->lwd, dd, gc->fill, gc->col);

  fprintf(ptd->texfp, " />\n");

}

static void SVG_Polygon(int n, double *x, double *y, const pGEcontext gc,
    pDevDesc dd) {
  int i;

  SVGDesc *ptd = (SVGDesc *) dd->deviceSpecific;

  fprintf(ptd->texfp, "<polygon points=\"");

  for (i = 0; i < n; i++) {
    fprintf(ptd->texfp, "%.2f , %.2f ", x[i], y[i]);
  }

  fprintf(ptd->texfp, "\" ");

  SetLinetype(gc->lty, gc->lwd, dd, gc->fill, gc->col);

  fprintf(ptd->texfp, " />\n");
}

// Rotated Text
static void SVG_Text(double x, double y, const char *str, double rot,
                     double hadj, const pGEcontext gc, pDevDesc dd) {

  SVGDesc *ptd = (SVGDesc *) dd->deviceSpecific;

  fprintf(ptd->texfp, "<text transform=\"translate(%.2f,%.2f)", x, y);
  if (rot != 0)
    fprintf(ptd->texfp, " rotate(%0.0f)", -1.0 * rot);
  fprintf(ptd->texfp, "\" ");
  int size = gc->cex * gc->ps + 0.5;
  SetFont(gc->fontface, size, gc->col, ptd);
  fprintf(ptd->texfp, ">");

  write_escaped(ptd->texfp, str);

  fprintf(ptd->texfp, "</text>\n");
}

static void SVG_Size(double *left, double *right, double *bottom, double *top,
                     pDevDesc dd) {
  *left = dd->left;
  *right = dd->right;
  *bottom = dd->bottom;
  *top = dd->top;
}

Rboolean SVGDeviceDriver(pDevDesc dd, char *filename, char *bg, char *fg,
    double width, double height, Rboolean xmlHeader, Rboolean useNS) {

  dd->startfill = R_GE_str2col(bg);
  dd->startcol = R_GE_str2col(fg);
  dd->startps = 10;
  dd->startlty = 0;
  dd->startfont = 1;
  dd->startgamma = 1;

  dd->activate = NULL;
  dd->deactivate = NULL;
  dd->close = SVG_Close;
  dd->clip = SVG_Clip;
  dd->size = SVG_Size;
  dd->newPage = SVG_NewPage;
  dd->line = SVG_Line;
  dd->text = SVG_Text;
  dd->textUTF8 = SVG_Text; // UTF-8 support
  dd->strWidth = SVG_StrWidth;
  dd->strWidthUTF8 = SVG_StrWidth; // UTF-8 support
  dd->rect = SVG_Rect;
  dd->circle = SVG_Circle;
  dd->polygon = SVG_Polygon;
  dd->polyline = SVG_Polyline;
  dd->mode = NULL;
  dd->metricInfo = SVG_MetricInfo;
  dd->cap = NULL;
  dd->raster = NULL; // not implemented

  // UTF-8 support
  dd->wantSymbolUTF8 = 1;
  dd->hasTextUTF8 = 1;

  // Screen Dimensions in Pixels

  dd->left = 0;
  dd->top = 0;
  dd->right = in2dots(width);
  dd->bottom = in2dots(height);

  SVGDesc *ptd = malloc(sizeof(SVGDesc));
  if (ptd == NULL)
    return FALSE;

  strcpy(ptd->filename, filename);
  ptd->texfp = fopen(R_ExpandFileName(ptd->filename), "w");
  if (ptd->texfp == NULL)
    return FALSE;

  ptd->width = width;
  ptd->height = height;
  ptd->xmlHeader = xmlHeader;
  ptd->useNS = useNS;

  // Base Pointsize: Nominal Character Sizes in Pixels

  dd->cra[0] = (6.0 / 12.0) * 10.0;
  dd->cra[1] = (10.0 / 12.0) * 10.0;

  // Character Addressing Offsets: These offsets should center a single,
  // plotting character over the plotting point.

  dd->xCharOffset = 0; // 0.4900;
  dd->yCharOffset = 0; // 0.3333;
  dd->yLineBias = 0; // 0.1;

  // Inches per Raster Unit: We use printer points, i.e. 72.27 dots per inch :
  dd->ipr[0] = dd->ipr[1] = 1. / DOTSperIN;

  dd->canClip = FALSE;
  dd->canHAdj = 0;
  dd->canChangeGamma = FALSE;

  ptd->pageno = 0;

  dd->deviceSpecific = (void *) ptd;
  dd->displayListOn = FALSE;
  return TRUE;
}


void do_SVG(char **file, char **bg, char **fg, double *width, double *height,
            int *xmlHeader, int *useNS) {
  pGEDevDesc dd;
  pDevDesc dev;

  R_GE_checkVersionOrDie(R_GE_version);
  R_CheckDeviceAvailable();
  BEGIN_SUSPEND_INTERRUPTS {
    if (!(dev = (pDevDesc) Calloc(1, DevDesc)))
      error("unable to allocate memory for DevDesc");

    if (!SVGDeviceDriver(dev, file[0], bg[0], fg[0], width[0], height[0],
        xmlHeader[0], useNS[0])) {
      free(dev);
      error("unable to start device SVG");
    }
    dd = GEcreateDevDesc(dev);

    GEaddDevice2(dd, "devSVG");
    GEinitDisplayList(dd);

  }END_SUSPEND_INTERRUPTS;
}

