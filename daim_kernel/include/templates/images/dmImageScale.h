#ifndef dmImageScale_h
#define dmImageScale_h

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
// File         : dmImageScale.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

namespace daim {

namespace scale_fn {

//------------------------------------------------------------------------
template<class T1,class T2>
struct scale_params : public pixel_unary_function<T1,T2>  
{ 
  typedef typename pixel_unary_function<T1,T2>::argument_type argument_type;
  typedef typename pixel_unary_function<T1,T2>::result_type   result_type;

  argument_type  minv,maxv,delta;    // minimum et maximum globaux
  result_type    upper,lower;        // parametre du scaling
  double         fscale;

  scale_params( const gap<argument_type>& minmax, const gap<result_type>& newscale ) 
    : minv(minmax.min()),maxv(minmax.max()), 
    upper(newscale.max()),lower(newscale.min())  
    { 
      if( minv < maxv ) delta = minmax.diff(); else delta = 1;
      fscale = static_cast<double>(upper - lower)/delta;
    }
};
//------------------------------------------------------------------------
// scaling de T1 -> T2
//------------------------------------------------------------------------
template<class T1,class T2>
struct scalerange_noround : public scale_params<T1,T2>   
{ 
  typedef typename scale_params<T1,T2>::argument_type argument_type;
  typedef typename scale_params<T1,T2>::result_type   result_type;

  scalerange_noround(const gap<argument_type>& _minmax, 
                     const gap<result_type>& _newscale )
   : scale_params<T1,T2>(_minmax,_newscale) {}

  result_type operator()( const argument_type& x ) {
    if(x<scale_params<T1,T2>::minv) return scale_params<T1,T2>::lower;
    if(x>scale_params<T1,T2>::maxv) return scale_params<T1,T2>::upper;
    return scale_params<T1,T2>::lower + result_type( 
           scale_params<T1,T2>::fscale * (x - scale_params<T1,T2>::minv) );
  }
};
//------------------------------------------------------------------------
// scale conversion for integer type
//------------------------------------------------------------------------
template<class T1, class T2>
struct scalerange  : public scale_params<T1,T2>   
{
  typedef typename scale_params<T1,T2>::argument_type argument_type;
  typedef typename scale_params<T1,T2>::result_type   result_type;

  scalerange(const gap<argument_type>& _minmax, const gap<result_type>& _newscale )
	 : scale_params<T1,T2>( _minmax, _newscale ) {}

  result_type operator()( const argument_type& x ) {
    if(x<scale_params<T1,T2>::minv) return scale_params<T1,T2>::lower;
    if(x>scale_params<T1,T2>::maxv) return scale_params<T1,T2>::upper;
    return scale_params<T1,T2>::lower + result_type(
           scale_params<T1,T2>::fscale * (x - scale_params<T1,T2>::minv) + 0.5);
  }
};
//------------------------------------------------------------------------
// scale conversion for integer type
template<class T>
void fn_scale_convert( 
     integer_true,
     const gap<typename pixel_traits<T>::value_type>& _minmax,
     const gap<typename pixel_traits<T>::value_type>& _newscale,
     const dmRegion& _rgn, image<T>& _img ) 
{
  daim::transform(_rgn,_img,scalerange<T,T>(_minmax,_newscale));
}

// scale conversion for non-integer type
template<class T>
void fn_scale_convert( 
     integer_false,
     const gap<typename pixel_traits<T>::value_type>& _minmax,
     const gap<typename pixel_traits<T>::value_type>& _newscale,
     const dmRegion& _rgn, image<T>& _img ) 
{
   daim::transform(_rgn,_img,scalerange_noround<T,T>(_minmax,_newscale));
}


//------------------------------------------------------------------------
// Binary scale converters
//------------------------------------------------------------------------

// scale conversion for integer type
template<class T1,class T2>
struct binary_scale_convert
{

//------------------------------------------------------------------------
// Assume that T1 is scalar and T2 is scalar
//------------------------------------------------------------------------

// scale conversion for integer type
//template<class T1,class T2,class V1,class V2>
template<class V1,class V2>
static void scalar_to_scalar( 
     integer_true,
     gap<V1> _minmax,
     gap<V2> _newscale,
     const image<T1>& _src, image<T2>& _dst, 
     const dmRegion& _rgn, 
     const dmPoint&  _p)
{
   daim::transform(_rgn,_p,_src,_dst,scale_fn::scalerange<T1,T2>(_minmax,_newscale));
}

// scale conversion for non-integer type
//template<class T1,class T2,class V1,class V2>
template<class V1,class V2>
static void scalar_to_scalar( 
     integer_false,
     gap<V1> _minmax,
     gap<V2> _newscale,
     const image<T1>& _src, image<T2>& _dst, 
     const dmRegion& _rgn, 
     const dmPoint&  _p)
{
   daim::transform(_rgn,_p,_src,_dst,scale_fn::scalerange_noround<T1,T2>(_minmax,_newscale));
}

//------------------------------------------------------------------------
// Assume that T1 is compound and T2 is scalar
//------------------------------------------------------------------------
// scale conversion for integer type
//template<class T2,class T1>
static void compound_to_scalar(
     integer_true,
     gap<typename pixel_traits<T2>::value_type> _minmax,
     gap<typename pixel_traits<T2>::value_type> _newscale,
     const image<T1>& _src, image<T2>& _dst, 
     const dmRegion& _rgn, 
     const dmPoint&  _p)
{
    daim::transform(_rgn,_p,_src,_dst,
       bind_func(
         pixel_traits<T1>::to_scalar(),
         scale_fn::scalerange<T2,T2>(_minmax,_newscale)
       )
    );
}

// scale conversion for non-integer type
//template<class T2,class T1>
static void compound_to_scalar(
     integer_false,
     gap<typename pixel_traits<T2>::value_type> _minmax,
     gap<typename pixel_traits<T2>::value_type> _newscale,
     const image<T1>& _src, image<T2>& _dst, 
     const dmRegion& _rgn, 
     const dmPoint&  _p)
{
    daim::transform(_rgn,_p,_src,_dst,
       bind_func(
         pixel_traits<T1>::to_scalar(),
         scale_fn::scalerange_noround<T2,T2>(_minmax,_newscale)
       )
    );
}


//------------------------------------------------------------------------
// Assume that T1 is scalar and T2 is compound
//------------------------------------------------------------------------

// scale conversion for integer type
//template<class T2,class T1>
static void scalar_to_compound(
     integer_true,
     gap<typename pixel_traits<T1>::value_type> _minmax,
     gap<typename pixel_traits<T1>::value_type> _newscale,
     const image<T1>& _src, image<T2>& _dst, 
     const dmRegion& _rgn, 
     const dmPoint&  _p)
{
    typedef  typename pixel_traits<T1>::value_type src_value_type;
    typename pixel_traits<T2>::template from_scalar<src_value_type> from_scalar;
    transform(_rgn,_p,_src,_dst,
       bind_func(
         scale_fn::scalerange<T1,T1>(_minmax,_newscale),
         from_scalar
       )
    );
}

// scale conversion for non-integer type
//template<class T2,class T1>
static void scalar_to_compound(
     integer_false,
     gap<typename pixel_traits<T1>::value_type> _minmax,
     gap<typename pixel_traits<T1>::value_type> _newscale,
     const image<T1>& _src, image<T2>& _dst, 
     const dmRegion& _rgn, 
     const dmPoint&  _p)
{
    typedef typename pixel_traits<T1>::value_type src_value_type;
    typename  pixel_traits<T2>::template from_scalar<src_value_type> from_scalar;
    transform(_rgn,_p,_src,_dst,
       bind_func(
         scale_fn::scalerange_noround<T1,T1>(_minmax,_newscale),
         from_scalar
       )
    );
}

};

}; // namespace scale_fn

template<class T>
inline void scale_convert( 
     const gap<typename pixel_traits<T>::value_type>& _minmax,
     const gap<typename pixel_traits<T>::value_type>& _newscale,
     const dmRegion& _rgn, image<T>& _img ) 
{
    typedef typename pixel_traits<T>::integer_type integer_type;
    scale_fn::fn_scale_convert(integer_type(),_minmax,_newscale,_rgn,_img);
}

template<class T1,class T2,class V1,class V2>
void scale_convert_scalar_to_scalar( 
     //gap<typename pixel_traits<T1>::value_type> _minmax,
     //gap<typename pixel_traits<T2>::value_type> _newscale,
     gap<V1> _minmax,
     gap<V2> _newscale,
     const image<T1>& _src, image<T2>& _dst, 
     const dmRegion& _rgn, 
     const dmPoint&  _p)
{
   typename pixel_traits<T2>::integer_type integer_type;

   scale_fn::binary_scale_convert<T1,T2>::scalar_to_scalar( 
     integer_type,
     _minmax,_newscale,_src,_dst,_rgn,_p);
}


template<class T2,class T1>
void scale_convert_compound_to_scalar(
     gap<typename pixel_traits<T2>::value_type> _minmax,
     gap<typename pixel_traits<T2>::value_type> _newscale,
     const image<T1>& _src, image<T2>& _dst, 
     const dmRegion& _rgn, 
     const dmPoint&  _p)
{
    typedef typename pixel_traits<T2>::integer_type integer_type;

    scale_fn::binary_scale_convert<T1,T2>::compound_to_scalar(
      integer_type(),_minmax,_newscale,_src,_dst,_rgn,_p);
}


template<class T1,class T2>
void scale_convert_scalar_to_compound(
     gap<typename pixel_traits<T1>::value_type> _minmax,
     gap<typename pixel_traits<T1>::value_type> _newscale,
     const image<T1>& _src, image<T2>& _dst, 
     const dmRegion& _rgn, 
     const dmPoint&  _p)
{
    typedef typename pixel_traits<T1>::integer_type integer_type;
  
    scale_fn::binary_scale_convert<T1,T2>::scalar_to_compound(
      integer_type(),_minmax,_newscale,_src,_dst,_rgn,_p);
}

}; // namespace daim
//------------------------------------------------------------------------
#endif // dmImageScale_h
