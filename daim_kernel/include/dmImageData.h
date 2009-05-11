#ifndef dmImageData_h
#define dmImageData_h

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
// File         : dmImageData.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Store attributs of image data.
 *
 *  Used to set/get data from internal Daim image buffers.
 */
//------------------------------------------------------------------------------------
class dmImageData
{
  public:
   EPixelFormat PixelFormat; /*!< Pixels format */
   dm_uint32    Width;       /*!< Number of pixels in one scan line of the image */
   dm_uint32    Height;      /*!< Number of scan lines in the image  */

   /*! Offset, in bytes, between consecutive scan lines of the image.
    *  If the stride is positive, the bitmap is top-down.
    *  If the stride is negative, the bitmap is bottom-up.
    */
   dm_int32     Stride;
   dm_ptr       Scan0;  /*!< Pointer to the first (index 0) scan line of the bitmap */
};

#endif //
