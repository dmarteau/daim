
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

#include "daim_utilities.h"
#include "dmKernelBase.h"

#include "templates/images/dmImageErrors.h"

//-------------------------------------------------------------------------------
void __dmKernel dmAssertValidImageData( void* _scan0, size_t w, size_t h, int _stride, size_t _unitsz )
{
  if(_scan0 == NULL) dmTHROW( EInvalidBasePointer() );  
  if(_stride < 0 ) _stride = -_stride;
  if(unsigned(_stride) < (w*_unitsz) || h<=0 || w<=0) 
      dmTHROW(EInvalidImageSize());           
}
//-------------------------------------------------------------------------------

