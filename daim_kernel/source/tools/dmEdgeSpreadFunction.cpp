
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
#include "tools/dmEdgeSpreadFunction.h"

#include "templates/processing/dmSpreadFunction.h"
//--------------------------------------------------------
namespace ns_SpreadFunction 
{
  struct Functor
  {
    int   retval;
    const dmRegion&       rgn;
    std::vector<dm_real>& xx;
    std::vector<dm_real>& yy;
    dm_real x1;
    dm_real y1;
    dm_real x2; 
    dm_real y2;
    dm_real xscale;
    dm_real yscale;

    Functor( const dmRegion& _rgn,
             std::vector<dm_real>& _xx,
             std::vector<dm_real>& _yy,
             dm_real _x1, 
             dm_real _y1,
             dm_real _x2, 
             dm_real _y2,
             dm_real _xscale,
             dm_real _yscale ) 
    : rgn(_rgn)
     ,xx(_xx)
     ,yy(_yy)
     ,x1(_x1)
     ,y1(_y1)
     ,x2(_x2)
     ,y2(_y2)
     ,xscale(_xscale)
     ,yscale(_yscale) 
    {}
 
    template<EPixelFormat _PixelFormat>
    void operator()( const dmIImage<_PixelFormat>& aSrc ) 
    {
      retval =  daim::edge_spread_function(rgn,aSrc.Gen(),xx,yy,
                                           x1,y1,x2,y2,xscale,yscale);
    }
  };    
}


int dmEdgeSpreadFunction(const dmRegion& rgn,
                         const dmImage& image,
                         std::vector<dm_real>& xx,
                         std::vector<dm_real>& yy,
                         dm_real x1, 
                         dm_real y1,
                         dm_real x2, 
                         dm_real y2,
                         dm_real xscale,
                         dm_real yscale ) 
{
  ns_SpreadFunction::Functor _Functor(rgn,xx,yy,x1,y1,x2,y2,xscale,yscale);
  dmImplementOperation(_Functor,image);
  
  return _Functor.retval;
}


