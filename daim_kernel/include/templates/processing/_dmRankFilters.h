#ifndef _dmRankFilters_h
#define _dmRankFilters_h

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
// File         : _dmRankFilters.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

#define _SWAP(a,b) _aux=a; a=b; b=_aux;

namespace daim {

  namespace filter {

    //===========================================================
    // Rank filter accumulator
    // This should be an O(n) implementation
    //===========================================================

    template<class T,class R,class U>    
    struct R_filter : public vector_accumulator<T,R,U>
    {
      typedef vector_accumulator<T,R,U> accumulator_type;
      typedef typename vector_accumulator<T,R,U>::result_type   result_type;
      typedef typename vector_accumulator<T,R,U>::argument_type argument_type;
      typedef typename vector_accumulator<T,R,U>::value_type    value_type;

      dm_uint _rank;

      R_filter( dm_uint _r, dm_uint _size ) 
      :vector_accumulator<T,R,U>(_size)
      ,_rank(_r)
      {}

      R_filter( const R_filter& _f ) 
      :vector_accumulator<T,R,U>(_f)
      ,_rank(_f._rank) 
      {}

      // temporary variables
      dm_uint i,ir,j,l,mid;
      U       *arr,a,_aux;
      
      result_type result() 
      { 
        arr = &accumulator_type::_data[0]-1;

        l=1;
        ir=accumulator_type::_data.size();
        for (;;) {
          if (ir <= l+1) {
            if (ir == l+1 && arr[ir] < arr[l]) { _SWAP(arr[l],arr[ir]); }
            return static_cast<result_type>(arr[_rank]);
          } else {
            mid=(l+ir) >> 1;
            _SWAP(arr[mid],arr[l+1]);
            if (arr[l+1] > arr[ir]) { _SWAP(arr[l+1],arr[ir]); }
            if (arr[l]   > arr[ir]) { _SWAP(arr[l],arr[ir]  ); }
            if (arr[l+1] > arr[l] ) { _SWAP(arr[l+1],arr[l] ); }
            i=l+1;
            j=ir;
            a=arr[l];
            for (;;) {
              do ++i; while (arr[i] < a);
              do --j; while (arr[j] > a);
              if (j < i) break;
                 _SWAP(arr[i],arr[j]);
            }
            arr[l]=arr[j];
            arr[j]=a;
            if (j >= _rank) ir=j-1;
            if (j <= _rank) l=i;
          }
        } 
      }

    };


    template<class T,class R, class U> 
    struct median : public R_filter<T,R,U> {
       median(dm_uint _size) : R_filter<T,R,U>((_size+1)/2,_size) {} 
    }; 

    //===========================================================
    // L filter accumulator
    // This should be an O(nlog n) implementation
    //===========================================================

    template<class T,class R, class U>    
    struct L_filter : public vector_accumulator<T,R,U>
    {
      typedef vector_accumulator<T,R,U> accumulator_type;
      typedef typename vector_accumulator<T,R,U>::result_type   result_type;
      typedef typename vector_accumulator<T,R,U>::argument_type argument_type;
      typedef typename vector_accumulator<T,R,U>::value_type    value_type;
      
      typedef typename pixel_traits<result_type>::integer_type  integer_type;

      const std::vector<value_type>& _coeffs;

      L_filter( const std::vector<value_type>& _c, dm_uint _size ) 
      :vector_accumulator<T,R,U>(_size)
      ,_coeffs(_c) 
      {}

      L_filter( const L_filter& _f ) 
      :vector_accumulator<T,R,U>(_f)
      ,_coeffs(_f._coeffs)
      {}

      value_type _result; 

      result_type result() 
      { 
        _result = 0;
        std::sort(accumulator_type::_data.begin(),accumulator_type::_data.end());
        for(int i=_coeffs.size();--i>=0;) _result += _coeffs[i]*accumulator_type::_data[i];
        return _get_range_value(_result,pixel_traits<result_type>(),integer_type());
      }
   };

  } // namespace filter

} // namespace daim

#undef _SWAP

#endif // _dmRankFilters
