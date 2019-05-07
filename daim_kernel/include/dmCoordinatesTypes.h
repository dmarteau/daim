#ifndef  dmCoordinatesTypes_h
#define  dmCoordinatesTypes_h

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
// File         : dmCoordinatesTypes.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

struct dm_point   { long x; long y;                        };
struct dm_rect    { dm_point top_left; dm_point bottom_right;      };
struct dm_ellipse { dm_point center;   long rx; long ry; };
struct dm_circle  { dm_point center;   long r;                };
struct dm_line    { dm_point start;    dm_point end;               };


/* special value for generic shapes type */
#define dmNoShape_t      0
#define dmRectangle_t    1
#define dmEllipse_t      2
#define dmCircle_t       3
#define dmPoly_t         4
#define dmLine_t         5


//-------------------------------------------------

#endif   /* dmCoordinatesTypes_h */
