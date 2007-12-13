/*
libdmtx - Data Matrix Encoding/Decoding Library
Copyright (C) 2007 Mike Laughton

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

/* $Id: callback.c,v 1.9 2006/10/13 01:54:35 mblaughton Exp $ */

#include <stdlib.h>
#include "gltest.h"
#include "callback.h"
#include "display.h"
#include "image.h"
#include "dmtx.h"

/**
 *
 *
 */
/*
void CrossScanCallback(DmtxScanRange *range, DmtxGradient *gradient, DmtxEdgeScan *scan)
{
   if(range->dir & DmtxDirHorizontal)
      plotPoint(&passOneImage, range->lineNbr, scan->edge.offset + scan->subPixelOffset, ColorWhite);
   else
      plotPoint(&passOneImage, scan->edge.offset + scan->subPixelOffset, range->lineNbr, ColorWhite);
}
*/

/**
 *
 *
 */
/*
void FollowScanCallback(DmtxEdgeFollower *follower)
{
   int color;

   switch(follower->dir) {
      case DmtxDirUp:
         color = ColorRed;
         break;
      case DmtxDirDown:
         color = ColorGreen;
         break;
      case DmtxDirLeft:
         color = ColorBlue;
         break;
      case DmtxDirRight:
         color = ColorYellow;
         break;
      default:
         color = ColorWhite;
         break;
   }

   if(follower->dir & DmtxDirVertical)
      plotPoint(&passOneImage, follower->paraOffset, follower->perpOffset, color);
   else
      plotPoint(&passOneImage, follower->perpOffset, follower->paraOffset, color);
}
*/

/**
 *
 *
 */
/*
void FinderBarCallback(DmtxRay2 *ray)
{
   DmtxVector2 p0, p1, pTmp;

   if(ray->isDefined == 0)
      return;

   dmtxVector2Add(&p0, &(ray->p), dmtxVector2Scale(&pTmp, &(ray->v), ray->tMin));
   dmtxVector2Add(&p1, &(ray->p), dmtxVector2Scale(&pTmp, &(ray->v), ray->tMax));

   glViewport(646, 324, 320, 320);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(-0.5, 319.5, -0.5, 319.5, -1.0, 10.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   glDisable(GL_TEXTURE_2D);
   glPolygonMode(GL_FRONT, GL_LINE);
   glColor3f(1.0, 1.0, 1.0);

   glBegin(GL_LINES);
   glVertex2f(p0.X, p0.Y);
   glVertex2f(p1.X, p1.Y);
   glEnd();
}
*/

/**
 *
 *
 */
void BuildMatrixCallback2(DmtxMatrixRegion *region)
{
   int i, j;
   float scale = 1.0/200.0;
   DmtxColor3 clr;
   DmtxVector2 point;
   DmtxMatrix3 m0, m1, mInv;

   dmtxMatrix3Translate(m0, -(320 - 200)/2.0, -(320 - 200)/2.0);
   dmtxMatrix3Scale(m1, scale, scale);
   dmtxMatrix3Multiply(mInv, m0, m1);
   dmtxMatrix3MultiplyBy(mInv, region->fit2raw);

   glDisable(GL_TEXTURE_2D);
   glPolygonMode(GL_FRONT, GL_LINE);

   for(i = 0; i < 320; i++) {
      for(j = 0; j < 320; j++) {
         point.X = j;
         point.Y = i;
         dmtxMatrix3VMultiplyBy(&point, mInv);
         dmtxColor3FromImage2(&clr, captured, point);
         dmtxPixelFromColor3(&(passTwoImage.pxl[i*320+j]), &clr);
      }
   }

   DrawPane3(NULL, &passTwoImage);

   glViewport(646, 324, 320, 320);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(-0.5, 319.5, -0.5, 319.5, -1.0, 10.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   glColor3f(0.0, 1.0, 0.0);
   glBegin(GL_QUADS);
   glVertex2f( 60.0,  60.0);
   glVertex2f(260.0,  60.0);
   glVertex2f(260.0, 260.0);
   glVertex2f( 60.0, 260.0);
   glEnd();
}

/**
 *
 *
 */
void BuildMatrixCallback3(DmtxMatrix3 mChainInv)
{
   int i, j;
   float scale = 1.0/100.0;
   DmtxColor3 clr;
   DmtxVector2 point;
   DmtxMatrix3 m0, m1, mInv;

   dmtxMatrix3Scale(m0, scale, scale);
   dmtxMatrix3Translate(m1, -(320 - 200)/2.0, -(320 - 200)/2.0);

   dmtxMatrix3Multiply(mInv, m1, m0);
   dmtxMatrix3MultiplyBy(mInv, mChainInv);

   glDisable(GL_TEXTURE_2D);
   glPolygonMode(GL_FRONT, GL_LINE);

   for(i = 0; i < 320; i++) {
      for(j = 0; j < 320; j++) {
         point.X = j;
         point.Y = i;
         dmtxMatrix3VMultiplyBy(&point, mInv);
         dmtxColor3FromImage2(&clr, captured, point);
         dmtxPixelFromColor3(&(passTwoImage.pxl[i*320+j]), &clr);
      }
   }

   DrawPane4(NULL, &passTwoImage);

   glViewport(2, 2, 320, 320);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(-0.5, 319.5, -0.5, 319.5, -1.0, 10.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   glColor3f(0.0, 1.0, 0.0);
   glBegin(GL_QUADS);
   glVertex2f( 60.0,  60.0);
   glVertex2f(160.0,  60.0);
   glVertex2f(160.0, 160.0);
   glVertex2f( 60.0, 160.0);
/*
   glVertex2f( 60.0,  60.0);
   glVertex2f(260.0,  60.0);
   glVertex2f(260.0, 260.0);
   glVertex2f( 60.0, 260.0);
*/
   glEnd();
}

/**
 *
 *
 */
void BuildMatrixCallback4(DmtxMatrix3 mChainInv)
{
   int i, j;
   float scale = 1.0/200.0;
   DmtxColor3 clr;
   DmtxVector2 point;
   DmtxMatrix3 m0, m1, mInv;

   dmtxMatrix3Scale(m0, scale, scale);
   dmtxMatrix3Translate(m1, -(320 - 200)/2.0, -(320 - 200)/2.0);

   dmtxMatrix3Multiply(mInv, m1, m0);
   dmtxMatrix3MultiplyBy(mInv, mChainInv);

   glDisable(GL_TEXTURE_2D);
   glPolygonMode(GL_FRONT, GL_LINE);

   for(i = 0; i < 320; i++) {
      for(j = 0; j < 320; j++) {
         point.X = j;
         point.Y = i;
         dmtxMatrix3VMultiplyBy(&point, mInv);
         dmtxColor3FromImage2(&clr, captured, point);
         dmtxPixelFromColor3(&(passTwoImage.pxl[i*320+j]), &clr);
      }
   }

   DrawPane5(NULL, &passTwoImage);

   glViewport(324, 2, 320, 320);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(-0.5, 319.5, -0.5, 319.5, -1.0, 10.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   glColor3f(0.0, 1.0, 0.0);
   glBegin(GL_QUADS);
   glVertex2f( 60.0,  60.0);
   glVertex2f(260.0,  60.0);
   glVertex2f(260.0, 260.0);
   glVertex2f( 60.0, 260.0);
   glEnd();
}

/**
 *
 *
 */
void PlotPointCallback(DmtxVector2 point, int colorInt, int paneNbr, int dispType)
{
   int color;
   DmtxImage *image = NULL;

   switch(paneNbr) {
      case 1:
         glViewport(2, 324, 320, 320);
         break;
      case 2:
         glViewport(324, 324, 320, 320);
         image = &passOneImage;
         break;
      case 3:
         glViewport(646, 324, 320, 320);
         break;
      case 4:
         glViewport(2, 2, 320, 320);
         break;
      case 5:
         glViewport(324, 2, 320, 320);
         break;
      case 6:
         glViewport(646, 2, 320, 320);
         break;
   }

   if(image != NULL) {
      switch(colorInt) {
         case 1:
            color = ColorRed;
            break;
         case 2:
            color = ColorGreen;
            break;
         case 3:
            color = ColorBlue;
            break;
         case 4:
            color = ColorYellow;
            break;
         default:
            color = ColorWhite;
            break;
      }

      plotPoint(image, point.Y, point.X, color);
//    plotPoint(image, point.Y + 1, point.X - 1, color);
//    plotPoint(image, point.Y + 1, point.X + 1, color);
//    plotPoint(image, point.Y - 1, point.X - 1, color);
//    plotPoint(image, point.Y - 1, point.X + 1, color);
   }
   else {
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(-0.5, 319.5, -0.5, 319.5, -1.0, 10.0);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();

      glDisable(GL_TEXTURE_2D);
      glPolygonMode(GL_FRONT, GL_LINE);

      switch(colorInt) {
         case 1:
            glColor3f(1.0, 0.0, 0.0);
            break;
         case 2:
            glColor3f(0.0, 1.0, 0.0);
            break;
         case 3:
            glColor3f(0.0, 0.0, 1.0);
            break;
         case 4:
            glColor3f(1.0, 1.0, 0.0);
            break;
         default:
            glColor3f(1.0, 1.0, 1.0);
            break;
      }

      if(dispType == DMTX_DISPLAY_SQUARE) {
         glBegin(GL_QUADS);
         glVertex2f(point.X - 3, point.Y - 3);
         glVertex2f(point.X + 3, point.Y - 3);
         glVertex2f(point.X + 3, point.Y + 3);
         glVertex2f(point.X - 3, point.Y + 3);
         glEnd();
      }
      else if(dispType == DMTX_DISPLAY_POINT) {
         int x = (int)(point.X + 0.5);
         int y = (int)(point.Y + 0.5);

         glBegin(GL_POINTS);
         glVertex2f(x, y);
         glEnd();
      }
   }
}

/**
 *
 *
 */
void XfrmPlotPointCallback(DmtxVector2 point, DmtxMatrix3 xfrm, int paneNbr, int dispType)
{
   float scale = 100.0;
   DmtxMatrix3 m, m0, m1;

   if(xfrm != NULL) {
      dmtxMatrix3Copy(m, xfrm);
   }
   else {
      dmtxMatrix3Identity(m);
   }

   dmtxMatrix3Scale(m0, scale, scale);
   dmtxMatrix3Translate(m1, (320 - 200)/2.0, (320 - 200)/2.0);
   dmtxMatrix3MultiplyBy(m, m0);
   dmtxMatrix3MultiplyBy(m, m1);

   dmtxMatrix3VMultiplyBy(&point, m);
   PlotPointCallback(point, 3, paneNbr, dispType);
}

/**
 *
 *
 */
void FinalCallback(DmtxDecode *decode, DmtxMatrixRegion *region)
{
   int row, col;
   int symbolRows, symbolCols;
   int moduleStatus;
   DmtxColor3 black = { 0.0, 0.0, 0.0 };
   DmtxColor3 white = { 255.0, 255.0, 255.0 };

   symbolRows = dmtxGetSymbolAttribute(DmtxSymAttribSymbolRows, region->sizeIdx);
   symbolCols = dmtxGetSymbolAttribute(DmtxSymAttribSymbolCols, region->sizeIdx);

   for(row = 0; row < symbolRows; row++) {
      for(col = 0; col < symbolCols; col++) {
         moduleStatus = dmtxSymbolModuleStatus(region, row, col);
         PlotModuleCallback(decode, region, row, col, (moduleStatus & DMTX_MODULE_ON_RGB) ? black : white);
      }
   }
}

/**
 *
 *
 */
void PlotModuleCallback(DmtxDecode *info, DmtxMatrixRegion *region, int row, int col, DmtxColor3 color)
{
   int modSize, halfModsize, padSize;
// float t;

   // Adjust for addition of finder bar
   row++;
   col++;

   halfModsize = (int)(100.0 / (region->mappingCols + 2) + 0.5); // Because 100 == 200/2
   modSize = 2 * halfModsize;
   padSize = (320 - ((region->mappingCols + 2) * modSize))/2;

   // Set for 6th pane
   DrawPaneBorder(645, 1, 322, 322);
   glRasterPos2i(1, 1);

   glPolygonMode(GL_FRONT, GL_FILL);

   // Clamp color to extreme foreground or background color
// t = dmtxDistanceAlongRay3(&(region->gradient.ray), &color);
// t = (t < region->gradient.tMid) ? region->gradient.tMin : region->gradient.tMax;
// dmtxPointAlongRay3(&color, &(region->gradient.ray), t);

   glColor3f(color.R/255, color.G/255, color.B/255);
   glBegin(GL_QUADS);
   glVertex2f(modSize*(col+0.5) + padSize - halfModsize, modSize*(row+0.5) + padSize - halfModsize);
   glVertex2f(modSize*(col+0.5) + padSize + halfModsize, modSize*(row+0.5) + padSize - halfModsize);
   glVertex2f(modSize*(col+0.5) + padSize + halfModsize, modSize*(row+0.5) + padSize + halfModsize);
   glVertex2f(modSize*(col+0.5) + padSize - halfModsize, modSize*(row+0.5) + padSize + halfModsize);
   glEnd();
}