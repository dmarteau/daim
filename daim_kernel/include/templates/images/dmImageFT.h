#ifndef dmImageFT_h
#define dmImageFT_h

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
// File         : dmImageFT.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

#include <math.h>

namespace daim {

typedef dmFT_Data ft_data_t;

class ft_image : public tmp_image<dm_float>
{
  private:
    ft_data_t _ft;
    bool      _is_ft;

  public:
   ~ft_image();
    ft_image();
    ft_image( size_t w, size_t h ); 
    ft_image( const ft_image&   ); 
    ft_image( const ft_data_t&, bool _is_fourier_space );

    void initialize( size_t w, size_t h ); 

    bool is_ft() const { return _is_ft;     }

    // NOTE: You MUST be in direct space
    void swap_blocks();

    operator       ft_data_t& ()       { return _ft; }
    operator const ft_data_t& () const { return _ft; }

    void compute_ft( int way, bool bswap );
    void copy_ft   ( const ft_data_t&, bool _is_fourier_space );

    ft_image& operator=( const ft_image& );

    template<class T>
    void image_to_ft( const image<T>& _src, const dmRect& r ) {
      _is_ft = false;
      fill(traits_type::zero());
      daim::copy(r,dmPoint(0,0),_src,*this);
    }

    template<class T>
    void ft_to_image( image<T>& _dst, const dmPoint& p, 
                      const gap<typename pixel_traits<T>::value_type>& _bounds,
                      bool  swap_real_blocks
                    ) 
    {
      compute_ft(dmFT_COMPUTE_REVERSE,swap_real_blocks);
      scale_convert_scalar_to_scalar(
         minmax(rect(),*this),_bounds,*this,_dst,rect(),p);
    }

    void ft_to_image( image_type& _dst, const dmPoint& p, 
                       const gap<traits_type::value_type>& _bounds,
                       bool  swap_real_blocks
                     ) 
    {
      compute_ft(dmFT_COMPUTE_REVERSE,swap_real_blocks);
      if(_bounds.diff() <= 0)
        daim::copy(dmRect(p,width(),height()),p,*this,_dst);
      else
        scale_convert_scalar_to_scalar(
           minmax(rect(),*this),_bounds,*this,_dst,rect(),p);
    }


    template<class T>
    void ft_to_image( image<T>& _dst, const dmPoint& p,
                      bool swap_real_blocks ) 
    {
      ft_to_image(_dst,p,pixel_traits<T>::bounds(),swap_real_blocks );
    }

    // NOTE: You MUST be in fourier space in order to
    // perform the following
    void ft_convolution( const ft_data_t&, int way );
    void ft_correlation( const ft_data_t& ); 
};


inline bool is_power_of_2( unsigned long x )  {
   return ((((x) - 1) & (x)) == 0);
}

inline size_t get_base_2( dm_double x ) {
  return (x>=2?static_cast<size_t>(log(x)/log(2.0)):0); 
}

inline int get_base_2_size( dm_uint x ) { 
  size_t n = get_base_2(x);
  return (n>0?1L<<n:0);
}

inline dmRect get_base_2_rect( const dmRect& r ) {
  return dmRect(r.TopLeft(),
            get_base_2_size(r.Width()),
            get_base_2_size(r.Height())
         );
}
//-----------------------------------------------------------------------------

}; // namespace daim

#endif // dmImageFT_h
