#ifndef dmEdgeSpreadFunction_h
#define dmEdgeSpreadFunction_h

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
// File         : dmEdgeSpreadFunction.h
// Date         : 11/2005
// Author       : David Marteau
//--------------------------------------------------------

//--------------------------------------------------------
// dmEdgeSpreadFunction:
//
// Compute the scattered edge spread function across a line 
// given by (x1,y1) (x1,y2)
// Note: Coordinates are scaled values (not pixels !)
//--------------------------------------------------------

int dmEdgeSpreadFunction(const dmRegion& rgn,
                         const dmImage&  image,
                         std::vector<dm_real>& xx,
                         std::vector<dm_real>& yy,
                         dm_real x1,  //
                         dm_real y1,
                         dm_real x2, 
                         dm_real y2,
                         dm_real xscale,
                         dm_real yscale );


#endif // dmEdgeSpreadFunction_h
