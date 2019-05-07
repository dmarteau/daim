#ifndef _dmMaxFilter_h
#define _dmMaxFilter_h

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
// File         : _dmMaxFilter_h.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

namespace daim {

  namespace filter {

    template<class T,class R,class U>
    struct max : public accumulator_base<T,R,U>
    {
      typedef typename accumulator_base<T,R,U>::argument_type argument_type;
      typedef typename accumulator_base<T,R,U>::value_type    value_type;
      typedef typename accumulator_base<T,R,U>::result_type   result_type;

      U _result;

      void reset() { _result = pixel_traits<value_type>::min(); }
      void accumulate( argument_type x ) { 
        if( x >  _result ) _result = x;
      }

      result_type result() { 
        return static_cast<result_type>(_result);
      }
    };

    template<class T,class R,class U>
    struct min : public accumulator_base<T,R,U>
    {
      typedef typename accumulator_base<T,R,U>::argument_type argument_type;
      typedef typename accumulator_base<T,R,U>::value_type    value_type;
      typedef typename accumulator_base<T,R,U>::result_type   result_type;

      U _result;

      void reset() { _result = pixel_traits<value_type>::max(); }
      void accumulate( argument_type x ) { 
        if( x <  _result ) _result = x;
      }

      result_type result() { 
        return static_cast<result_type>(_result);
      }
    };

    template<class T,class R,class U>
    struct middle : public accumulator_base<T,R,U>
    {
      typedef typename accumulator_base<T,R,U>::argument_type argument_type;
      typedef typename accumulator_base<T,R,U>::value_type    value_type;
      typedef typename accumulator_base<T,R,U>::result_type   result_type;

      U _min;
      U _max;

      void reset() { 
         _min = pixel_traits<value_type>::max(); 
         _max = pixel_traits<value_type>::min(); 
      }

      void accumulate( argument_type x ) { 
        if( x < _min ) _min = x; else
        if( x > _max ) _max = x; 
      }

      result_type result() { 
        return static_cast<result_type>((_min+_max)/2);
      }
    };

  } // namespace filter

} // namespace daim

#endif
