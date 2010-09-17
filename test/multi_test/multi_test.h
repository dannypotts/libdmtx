/*
libdmtx - Data Matrix Encoding/Decoding Library

Copyright (C) 2010 Mike Laughton

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

Contact: mblaughton@users.sourceforge.net
*/

/* $Id: multi_test.c 561 2008-12-28 16:28:58Z mblaughton $ */

#define max(N,M) ((N > M) ? N : M)
#define min(N,M) ((N < M) ? N : M)

#define LOCAL_SIZE            64
#define MAXIMA_SORT_MAX_COUNT  8
#define ANGLE_SORT_MAX_COUNT   8
#define TIMING_SORT_MAX_COUNT  8

#define NFFT                  64 /* FFT input size */
#define HOUGH_D_EXTENT        64
#define HOUGH_PHI_EXTENT     128

/* Layout constants */
#define CTRL_COL1_X          380
#define CTRL_COL2_X          445
#define CTRL_COL3_X          510
#define CTRL_COL4_X          575
#define CTRL_ROW1_Y            0
#define CTRL_ROW2_Y           65
#define CTRL_ROW3_Y          130
#define CTRL_ROW4_Y          195
#define CTRL_ROW5_Y          259
#define CTRL_ROW6_Y          324
#define CTRL_ROW7_Y          388

#define MODULE_LOW             0
#define MODULE_HIGH            1
#define MODULE_UNKNOWN         2

#define RotateCCW(N) ((N < 8) ? (N << 1) : 1)
#define RotateCW(N)  ((N > 1) ? (N >> 1) : 8)

typedef struct UserOptions_struct {
   const char *imagePath;
} UserOptions;

typedef struct AppState_struct {
   int         adjust;
   int         windowWidth;
   int         windowHeight;
   int         imageWidth;
   int         imageHeight;
   int         activeExtent;
   DmtxBoolean displayVanish;
   DmtxBoolean displayTiming;
   DmtxBoolean printValues;
   Sint16      imageLocX;
   Sint16      imageLocY;
   Uint8       leftButton;
   Uint8       rightButton;
   Uint16      pointerX;
   Uint16      pointerY;
   DmtxBoolean quit;
} AppState;

typedef struct Flow_struct {
   int mag;
} Flow;

typedef struct HoughCache_struct {
   int offExtent;
   int phiExtent;
   char isMax[HOUGH_D_EXTENT * HOUGH_PHI_EXTENT];
   unsigned int mag[HOUGH_D_EXTENT * HOUGH_PHI_EXTENT];
} HoughCache;

typedef struct HoughMaximaSort_struct {
   int count;
   int mag[MAXIMA_SORT_MAX_COUNT];
} HoughMaximaSort;

typedef struct VanishPointSum_struct {
   int phi;
   int mag;
} VanishPointSum;

typedef struct VanishPointSort_struct {
   int count;
   VanishPointSum vanishSum[ANGLE_SORT_MAX_COUNT];
} VanishPointSort;

typedef struct Timing_struct {
   int phi;
   double shift;
   double period;
   double mag;
} Timing;

typedef struct TimingSort_struct {
   int count;
   Timing timing[TIMING_SORT_MAX_COUNT];
} TimingSort;

typedef struct AlignmentGrid_struct {
   int rowCount;
   int colCount;
   DmtxMatrix3 raw2fitActive;
   DmtxMatrix3 raw2fitFull;
   DmtxMatrix3 fit2rawActive;
   DmtxMatrix3 fit2rawFull;
} AlignmentGrid;

typedef struct GridRegion_struct {
   AlignmentGrid grid;
   int x;
   int y;
   int width;
   int height;
   int sizeIdx;
   int onColor;
   int offColor;
   int finderSides;
   int contrast;
} GridRegion;

typedef struct RegionLines_struct {
   int gridCount;
   Timing timing;
   double dA, dB;
   DmtxRay2 line[2];
} RegionLines;

/* All values in GridRegionGrowth should be negative because list
 * is combined with the positive values of DmtxSymbolSize enum */
typedef enum {
   GridRegionGrowthUp       = -5,
   GridRegionGrowthLeft     = -4,
   GridRegionGrowthDown     = -3,
   GridRegionGrowthRight    = -2,
   GridRegionGrowthError    = -1
} GridRegionGrowth;

typedef enum {
   DmtxBarNone     = 0x00,
   DmtxBarTiming   = 0x01 << 0,
   DmtxBarFinder   = 0x01 << 1,
   DmtxBarInterior = 0x01 << 2,
   DmtxBarExterior = 0x01 << 3
} DmtxBarType;

/* Only used internally */
typedef struct ColorTally_struct {
   int evnCount;
   int oddCount;
   int evnColor;
   int oddColor;
} ColorTally;

struct StripStats_struct {
   int jumps;
   int surprises;
   int finderErrors;
   int timingErrors;
   int contrast;
   int finderBest;
   int timingBest;
};
typedef struct StripStats_struct StripStats;

/* Application level functions */
UserOptions GetDefaultOptions(void);
DmtxPassFail HandleArgs(UserOptions *opt, int *argcp, char **argvp[]);
AppState InitAppState(void);
SDL_Surface *SetWindowSize(int windowWidth, int windowHeight);
void captureLocalPortion(SDL_Surface *local, SDL_Surface *localTmp,
      SDL_Surface *picture, SDL_Surface *screen, AppState *state, SDL_Rect imageLoc);
DmtxPassFail HandleEvent(SDL_Event *event, AppState *state,
      SDL_Surface *picture, SDL_Surface **screen);
DmtxPassFail NudgeImage(int windowExtent, int pictureExtent, Sint16 *imageLoc);
/*static void WriteDiagnosticImage(DmtxDecode *dec, char *imagePath);*/

/* Image processing functions */
void PopulateFlowCache(Flow *sFlow, Flow *bFlow, Flow *hFlow, Flow *vFlow, DmtxImage *img);
int GetCompactOffset(int x, int y, int phiIdx, int extent);
double UncompactOffset(double d, int phiIdx, int extent);
void PopulateHoughCache(HoughCache *hough, Flow *sFlow, Flow *bFlow, Flow *hFlow, Flow *vFlow);
void NormalizeHoughCache(HoughCache *hough, Flow *sFlow, Flow *bFlow, Flow *hFlow, Flow *vFlow);
void MarkHoughMaxima(HoughCache *hough);
void AddToVanishPointSort(VanishPointSort *sort, VanishPointSum vanishSum);
VanishPointSort FindVanishPoints(HoughCache *hough);
void AddToMaximaSort(HoughMaximaSort *sort, int maximaMag);
VanishPointSum GetAngleSumAtPhi(HoughCache *hough, int phi);
void AddToTimingSort(TimingSort *sort, Timing timing);
TimingSort FindGridTiming(HoughCache *hough, VanishPointSort *sort, AppState *state);
DmtxRay2 HoughLineToRay2(int phi, double d);
DmtxPassFail BuildGridFromTimings(AlignmentGrid *grid, Timing vp0, Timing vp1, AppState *state);
DmtxPassFail FindRegionWithinGrid(GridRegion *region, DmtxImage *img, AlignmentGrid *grid, DmtxDecode *dec, SDL_Surface *screen, AppState *state);
DmtxBarType TestSideForPattern(GridRegion *region, DmtxImage *img, DmtxDirection side, int offset);
DmtxPassFail RegionExpand(GridRegion *region, DmtxDirection dir);
int GetSizeIdx(int a, int b);
DmtxPassFail RegionUpdateCorners(DmtxMatrix3 fit2raw, DmtxMatrix3 raw2fit, DmtxVector2 p00, DmtxVector2 p10, DmtxVector2 p11, DmtxVector2 p01);
DmtxPassFail DecodeSymbol(GridRegion *region, DmtxDecode *dec);
DmtxPassFail GetOnOffColors(GridRegion *region, const DmtxDecode *dec, int *onColor, int *offColor);
ColorTally GetTimingColors(GridRegion *region, const DmtxDecode *dec, int colBeg, int rowBeg, DmtxDirection dir);

/* Process visualization functions */
void BlitFlowCache(SDL_Surface *screen, Flow *flowCache, int maxFlowMag, int screenY, int screenX);
void BlitHoughCache(SDL_Surface *screen, HoughCache *hough, int screenY, int screenX);
void BlitActiveRegion(SDL_Surface *screen, SDL_Surface *active, int zoom, int screenY, int screenX);
void PlotPixel(SDL_Surface *surface, int x, int y);
int Ray2Intersect(double *t, DmtxRay2 p0, DmtxRay2 p1);
int IntersectBox(DmtxRay2 ray, DmtxVector2 bb0, DmtxVector2 bb1, DmtxVector2 *p0, DmtxVector2 *p1);
void DrawActiveBorder(SDL_Surface *screen, int activeExtent);
void DrawLine(SDL_Surface *screen, int baseExtent, int screenX, int screenY, int phi, double d, int displayScale);
void DrawTimingLines(SDL_Surface *screen, Timing timing, int displayScale, int screenY, int screenX);
void DrawVanishingPoints(SDL_Surface *screen, VanishPointSort sort, int screenY, int screenX);
void DrawTimingDots(SDL_Surface *screen, Timing timing, int screenY, int screenX);
void DrawNormalizedRegion(SDL_Surface *screen, DmtxImage *img, AlignmentGrid *grid, AppState *state, int screenY, int screenX);
int ReadModuleColor(DmtxImage *img, AlignmentGrid *grid, int symbolRow, int symbolCol, int colorPlane);
Sint16 Clamp(Sint16 x, Sint16 xMin, Sint16 extent);
void DrawSymbolPreview(SDL_Surface *screen, DmtxImage *img, AlignmentGrid *grid, AppState *state, int screenY, int screenX);
void DrawPerimeterPatterns(SDL_Surface *screen, GridRegion *region, AppState *state, DmtxDirection side, DmtxBarType type);
void DrawPerimeterSide(SDL_Surface *screen, int x00, int y00, int x11, int y11, int dispModExtent, DmtxDirection side, DmtxBarType type);