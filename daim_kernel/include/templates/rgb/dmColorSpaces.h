#ifndef dmColorSpaces_h
#define dmColorSpaces_h

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
// File         : dmColorSpaces.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

namespace daim {

 namespace colorspace {

   typedef image<dm_rgb24>::value_type rgb_triple;

   inline dm_uint8 to_rgb_channel( dm_real x ) {
     return _get_range_value(x,pixel_traits<dm_uint8>(),integer_true());
   }

   #include "colorspaces/_dmRGB.h"
   #include "colorspaces/_dmHSV.h"
   #include "colorspaces/_dmHLS.h"
   #include "colorspaces/_dmYUV.h"
   #include "colorspaces/_dmYIQ.h"
   #include "colorspaces/_dmOCP.h"
   #include "colorspaces/_dmGSC.h"
   #include "colorspaces/_dmGSL.h"
   #include "colorspaces/_dmHSI.h"

  } // namespace colorspace

} // namespace daim

#endif // dmColorSpaces_h
