#ifndef _dmHistogram_h
#define _dmHistogram_h

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
// File         : _dmHistogram.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

#include <algorithm>

namespace daim {

enum {
 histogram_size = dmLUT8_MAX_COLORS,
 histogram_max  = dmLUT8_MAX_COLOR_INDEX
};


typedef dm_uint32        histogram_value;
typedef histogram_value  histogram_type[histogram_size];

void cumulate_histogram( histogram_type& _H   , const histogram_type& _data );
void equalize_histogram( dmColorMapArray _cmap, const histogram_type& _data );

//-----------------------------------------------------------------
// Histogram specification :
// the _input map should have been computed from the desired
// histogram equalization
//-----------------------------------------------------------------
void specify_histogram( dmColorMapArray _cmap , const histogram_type& _data,
                        dmColorMapArray _input );

void extend_histogram_map( dmColorMapArray _cmap,
                           const daim::gap<dm_uint8>& _range_from,
                           const daim::gap<dm_uint8>& _range_to
                         );

//-----------------------------------------------------------------
// Compute optimal threshold from histogram values
//-----------------------------------------------------------------
histogram_value bimodal_optimal_thrindex( const histogram_type& , dm_uint8 _thr );

//-----------------------------------------------------------------
// Enhance luminosity/contrast
// bright should be a value beteween 0 and dmLUT8_MAX_COLOR_INDEX
// contr  must be between 0 and 100
//-----------------------------------------------------------------
gap<dm_uint16> enhance_contrast( dmColorMapArray cm, dm_uint16 bright, dm_uint16 contr );
//-----------------------------------------------------------------
// Compute gamma correction
//-----------------------------------------------------------------
void gamma_correction( dmColorMapArray cm, dm_real gamma );
//-----------------------------------------------------------------
// Template for histogram extraction
//-----------------------------------------------------------------
template<class T,class E>
struct histogram_extractor
{
  E extract;
  histogram_type& _histogram;

  histogram_extractor( histogram_type& _h, const E& _e = E() )
  :extract(_e),_histogram(_h) {}

  void operator()( T x ) {
     ++_histogram[extract(x)];
  }
};
//-----------------------------------------------------------------
template<class R,class T,class E>
void extract_histogram( const R& rgn, const image<T>& src,
                        histogram_type& histogram,
                        const E& extract )
{
  daim::for_each(rgn,src,histogram_extractor<T,E>(histogram,extract));
}
//-----------------------------------------------------------------
// Template for histogram extraction of rgb images
//-----------------------------------------------------------------
template<class E>
struct rgb_histogram_extractor
{
  E extract;
  histogram_type& _histogram1;
  histogram_type& _histogram2;
  histogram_type& _histogram3;

  dm_uint8 c1,c2,c3;

  rgb_histogram_extractor(
         histogram_type& _h1,
         histogram_type& _h2,
         histogram_type& _h3,
         const E& _e = E() )

  : _histogram1(_h1)
   ,_histogram2(_h2)
   ,_histogram3(_h3)
   , extract(_e) {}

  void operator()( const image<dm_rgb24>::value_type& x ) {
     extract(x,c1,c2,c3);
     ++_histogram1[c1];
     ++_histogram2[c2];
     ++_histogram3[c3];
  }
};
//-----------------------------------------------------------------
template<class R,class E>
void extract_rgb_histogram( const R& rgn, const image<dm_rgb24>& src,
                            histogram_type& histogram1,
                            histogram_type& histogram2,
                            histogram_type& histogram3,
                            const E& extract )
{
  for_each(rgn,src,rgb_histogram_extractor<E>(histogram1,histogram2,histogram3,extract));
}


//------------------------------------------------------------------------
// Build a 255 values histogram from scalar image and for a given
// range of pixels values. This is intended to be used on images that
// are not 8 bits images
//------------------------------------------------------------------------
template<class T>
struct _extract_range
{
  typedef typename pixel_traits<T>::value_type value_type;

  value_type _xmin;
  value_type _xmax;
  _extract_range( const gap<value_type>& _g )
  : _xmin(_g.min()),_xmax(_g.max()) {}

  dm_uint8 operator()( value_type x )
  {
    if(x<=_xmax && x>=_xmin) {
      return static_cast<dm_uint8>( 255.0*(x-_xmin)/(_xmax - _xmin) + 0.5 );
    } else
      return 0;
  }

};
//------------------------------------------------------------------------
template<class T>
void build_scalar_histogram( const dmRegion& rgn, image<T>& src,
                             gap< typename image<T>::value_type >& range,
                             histogram_type& _histogram )
{
  extract_histogram(rgn,src,_histogram,_extract_range<T>(range));
}
//------------------------------------------------------------------------
// Accumulator functor for computing histogram moments
//------------------------------------------------------------------------
struct _histogram_moment
{
  dm_real  _acc;
  int      _order;

  _histogram_moment( int _o )
  :_acc(0)
  ,_order(_o)
  {}

  template<class In>
  void operator()( In _in, long x1, long x2 )
  {
    int n;
    dm_real v,s;

    for(long x=x1;x<=x2;++x)
    {
      v = (*_in)[0];
      for(s=v,n=_order;--n>=0;)
        s *= v;

      _acc += v;
    }
  }
};


// Warning no check is performed on roi

template<class ROI,class Op,class T>
dm_real histogram_moment( const ROI& rgn, const image<T>& im, int n  )
{
  return _RoiOperation(rgn,im.begin(rgn.Rectangle()),
                       _histogram_moment(n))._acc;
}

//------------------------------------------------------------------------


}; // namespace daim

#endif // _dmHistogram_h
