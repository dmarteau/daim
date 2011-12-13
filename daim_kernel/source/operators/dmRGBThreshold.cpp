
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

#define dmUseKernelImageTemplates
#define dmUseKernelBuffer
#include "daim_kernel.h"
#include "common/dmUserLib.h"

#include <math.h>
#include "templates/rgb/dmRGBThreshold.h"

//--------------------------------------------------------------------
bool dmRGBThreshold( const dmImage&    _Src, 
                     const dmRegion&   _Rgn, 
                     const dmRGBColor& _Color,
                     float rv,float rg, float rb, 
                     dmRegion& _Result )
{
  
  const dmIImage<dmPixelFormat24bppRGB>* _Image = dmIImage<dmPixelFormat24bppRGB>::Cast(&_Src);
  
  if(_Image) 
  {
    daim::create_rgnroi(_Image->Gen(),daim::rgb_threshold(_Color,rv,rg,rb),_Result,_Rgn);
    return true;
  }
  return false;
}
//--------------------------------------------------------------------
