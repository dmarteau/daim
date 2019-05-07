#ifndef dmRegionsProcessing_h
#define dmRegionsProcessing_h

/* ::: BEGIN LICENSE BLOCK:::
 *
 * Copyright (c) 2004-2005 David Marteau
 *
 * This file is part of the DAIM Image Processing Library (DAIM library)
 *
 * The DAIM library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License (LGPL)
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * The DAIM library is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the DAIM library; see the file LICENSE.  If
 * not, write to the Free Software Foundation, Inc., 59 Temple Place -
 * Suite 330, Boston, MA 02111-1307, USA.
 *
 *  ::: END LICENSE BLOCK::: */

//--------------------------------------------------------
// File         : dmRegionsProcessing.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

/*---------------------------------------------------------------------------------------
 function    : dmRgnConvexHull 
 description : Compute the convex hull of the input polygon
 note        : Polygon must be not self intersecting !!!
---------------------------------------------------------------------------------------*/
size_t dmRgnConvexHull( const dmPoly& _input, dmPoly& _output );

/*---------------------------------------------------------------------------------------
 function    : dmRgnComputeRotationPts 
 description : Compute the rotation points of the input polygon.
               rotation points are the significative angular points of
               the input polygon. These points are computed by mesuring
               the angle formed between points located at 'seglentgh' before and after
               a given point from the input polygon.
 note        : Polygon must be closed and not self intersecting !!!
---------------------------------------------------------------------------------------*/
size_t dmRgnComputeRotationPts( const dmPoly& _input, dmVector<size_t>& _output, 
                                size_t seglength );
/*---------------------------------------------------------------------------------------
 function    : dmRgnContourSketch 
 description : Compute a sketch of the input contour 
               by reducing the number of input points that describe
               the original contour.
               First rotation points are computed using the seglength, then new points
               are inserted at location where chord distance is larger than mindist 
 note        : Polygon must be not self intersecting !!!
---------------------------------------------------------------------------------------*/
size_t dmRgnContourSketch( const dmPoly& _input, dmPoly& _output, 
                           size_t seglength, size_t mindist );

//--------------------------------------------------------
#endif // dmRegionsProcessing_h
