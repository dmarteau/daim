#ifndef dmDownSampling_h
#define dmDownSampling_h

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
// File         : dmDownSampling.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------
#include "_dmConvolution.h"

namespace daim {

// Generate a down sampling of the input image
//

template<class T1,class T2>
void downSampling( const dmRect& rect, 
                   const image<T1>& in, image<T2>& out, 
                   const dmPoint& step )
{
  dmDEBUG_ASSERT( step.x > 1 );
  dmDEBUG_ASSERT( step.y > 1 );

  typename image<T1>::const_line_type lin  = in.begin(rect);
  typename image<T2>::line_type       lout = out.begin();

  int width  = rect.Width()  / step.x;
  int height = rect.Height() / step.y;
  int i,j;

  dmDEBUG_ASSERT( in.rect()    >=  rect   );
  dmDEBUG_ASSERT( out.Width()  >=  width  );
  dmDEBUG_ASSERT( out.Height() >=  height );

  dm_int xstart,xx;

  xstart = rect.Left();

  // xx : location of the source pixel in row
  // i  : location  of the dest pixel in row

  for(j=height;--j>=0;lin = lin + step.y,++lout) {
    for(xx=xstart,i=0;i<width;++i,xx+=step.x)
       (*lout)[i] = (*lin)[xx];
  }
} 


// Generate a down sampling of the input image to the output
// by applying the filter using the kernel K and the accumulator A. 
// Note that no check is performed on the ROI given by rect.

template<class K,class A,class T1,class T2>
void downSampling( const K& k, A a, const dmRect& rect, 
                   const image<T1>& in, image<T2>& out, 
                   const dmPoint& step )
{

  dmDEBUG_ASSERT( step.x > 1 );
  dmDEBUG_ASSERT( step.y > 1 );

  typename image<T1>::const_line_type lin  = in.begin(rect);
  typename image<T2>::line_type       lout = out.begin();

  int    stride = daim::line_stride(lin);
  size_t width  = rect.Width()  / step.x;
  size_t height = rect.Height() / step.y;
  size_t i,j,w,h,ii,jj;

  dmDEBUG_ASSERT( in.rect()    >=  rect   );
  dmDEBUG_ASSERT( out.width()  >=  width  );
  dmDEBUG_ASSERT( out.height() >=  height );

  dm_int xstart,xx;

  xstart = rect.Left() - k.x();
  lin    = lin - k.y();

  const typename A::argument_type *corner;
  const typename K::value_type    *data;

  w = k.width();
  h = k.height();

  // xx : location of the source pixel in row
  // i  : location  of the dest pixel in row

  for(j=height;j>0;--j,lin = lin + step.y,++lout) {
    for(xx=xstart,i=0;i<width;++i,xx+=step.x)
    {
      a.reset();
      data   = k.data();
      corner = *lin + xx;

      for(jj=h;jj>0;--jj) {
        for(ii=0;ii<w;++ii)
          a.accumulate(corner[ii],static_cast<typename A::value_type>(*data++));
          
          corner = daim::const_offset_ptr(corner,stride);
      }
      (*lout)[i] = a.result();
    }
  }
} 


namespace conv {

// Integer down sampling

template<class K,class T1,class T2>
void _downSampling( const K& k, bool _unsigned, 
                    const dmRect& rect, 
                    const image<T1>& in, image<T2>& out, 
                    const dmPoint& step,        
                    integer_true ) 
{
  typedef typename image<T1>::traits_type traits_type1;
  typedef typename image<T2>::traits_type traits_type2;
  
  if(k.norm()>0) {
    if(_unsigned) 
     daim::downSampling(k,bind_accumulator(k,unsigned_true(),
                                           true_assertion(),
                                           traits_type1(),
                                           traits_type2()),
                  rect,in,out,step);
    else
       daim::downSampling(k,bind_accumulator(k,unsigned_false(),
                                            true_assertion(),
                                            traits_type1(),
                                            traits_type2()),
                  rect,in,out,step);

  } else {
     daim::downSampling(k,bind_accumulator(k,unsigned_true(),
                                           false_assertion(),
                                           traits_type1(),
                                           traits_type2()),
                  rect,in,out,step);
  }
}

// Non integer down sampling

template<class K,class T1,class T2>
void _downSampling( const K& k, bool _unsigned, 
                    const dmRect& rect, 
                    const image<T1>& in, image<T2>& out, 
                    const dmPoint& step,        
                    integer_false ) 

{
  float n = static_cast<float>(k.norm());
  if(_unsigned)
    daim::downSampling(k,conv::unsigned_accumulator<T1,T2,float>(n==0?1.0f:n),
                       rect,in,out,step);
  else
    daim::downSampling(k,conv::accumulator<T1,T2,float>(n==0?1.0f:n),
                       rect,in,out,step);
}


// Apply a down sampling operation with a list a kernels
// results are combined with the binary operator f

template<class K,class T1,class T2,class BinaryFun>
void downSampling( const std::vector<K>& kernels, bool _unsigned,  
                   const dmRect& rect, 
                   const image<T1>& in, image<T2>& out, 
                   const dmPoint& step, const BinaryFun& f )
{
  dmDEBUG_ASSERT( !kernels.empty() );

  typedef typename pixel_traits<T2>::integer_type integer_type;

  cont_image<T2> tmp(out.width(),out.height());
  _downSampling(kernels[0],_unsigned,rect,in,tmp,step,integer_type());

  for(size_t n=1;n<kernels.size();++n) {
    _downSampling(kernels[n],_unsigned,rect,in,out,step,integer_type());
    combine(rect,dmPoint(0,0),out,tmp,f);
  }

  out.copy(tmp);
}

} // namespace conv


} // namespace daim



#endif // dmDownSampling_h
