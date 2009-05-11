
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

//--------------------------------------------------------------------
// ZFocus functions
//--------------------------------------------------------------------
#define ZINLINE inline

#ifdef _ASSERT_RESULT
 #define  ASSERT_RESULT( f ) dmASSERT( f )
#else
 #define  ASSERT_RESULT( f ) f
#endif 

namespace zfocus 
{
  typedef dmIImage<dmPixelFormat32bppFloat>  image_type;
  typedef dmIImage<dmPixelFormat8bppIndexed> map_type;
  typedef dmIImage<dmPixelFormat24bppRGB>    rgb_type;
  typedef dmIImage<dmPixelFormat8bppIndexed> channel_type;

  typedef image_type::value_type value_type;

  typedef daim::gap<channel_type::value_type> channel_range;

  inline image_type* cast_image( dmImage* p ) {
    return dmIImage<dmPixelFormat32bppFloat>::Cast(p);
  }

  struct greater_abs : public std::binary_function<dm_float,dm_float,bool>  
  {
    result_type operator()( const first_argument_type&  x,
                            const second_argument_type& y)
    {
      return dm_abs(x) > dm_abs(y);
    }
  };

  struct attach
  {
     dmIImageList& aList;
    ~attach() { aList.Detach(); }
     attach( dmIImageList& _aList , cci_Object* obj ) : aList(_aList) {
       aList.Attach(obj);
     }
  }; 

}

#define ZFocus_SetChannel( _This , i ) \
       zfocus::attach __setChannel(_This->ResList,_This->ResChannels[i]);

// from daim kernel library
bool __dmKernel _dmDoImageMath5( dm_int op, dmImage& lhs, 
                                 const dmImage& rhs , 
                                 const dmRegion&, const dmPoint& );
