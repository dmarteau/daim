#ifndef dmArithmetics_h
#define dmArithmetics_h

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
// File         : dmArithmetics.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

namespace daim {
//-----------------------------------------------------------------
template<class T> struct add_pixel : public pixel_traits<T>::addpixel {};
template<class T> struct sub_pixel : public pixel_traits<T>::subpixel {};
template<class T> struct max_pixel : public max_of<typename pixel_traits<T>::value_type> {};
template<class T> struct min_pixel : public min_of<typename pixel_traits<T>::value_type> {};
//-----------------------------------------------------------------
template<class T> struct _pixel_binfun :  public std::binary_function<T,T,T> {};
//-----------------------------------------------------------------
template<class T>
struct nsub_pixel : public _pixel_binfun<typename pixel_traits<T>::value_type>
{
  typedef typename pixel_traits<T>::value_type value_type;

  sub_pixel<T> _sub;

  value_type operator()( const value_type&  x1,
                         const value_type& x2 ) {
   return _sub.operator()(x2,x1);
  }
};
//-----------------------------------------------------------------
template<class T>
struct diff_pixel : public _pixel_binfun<typename pixel_traits<T>::value_type> {
  typedef typename pixel_traits<T>::value_type value_type;
  value_type operator()( const value_type&  x1,
                         const value_type& x2 ) {
    return daim::absdiff(x1,x2);
  }
};
//-----------------------------------------------------------------
template<class T>
struct abs_pixel : public _pixel_binfun<typename pixel_traits<T>::value_type> {
  typedef typename pixel_traits<T>::value_type value_type;
  value_type operator()( const value_type&  x1,
                         const value_type&  x2 ) {
    return _get_range_value(daim::abs(x1)+daim::abs(x2),pixel_traits<T>(),integer_true());
  }
};
//-----------------------------------------------------------------
// specialize for float
//-----------------------------------------------------------------
template<> struct sub_pixel<dm_float> :_pixel_binfun<dm_float> {
  dm_float operator()( const dm_float&  x1, const dm_float& x2 ) {
    return x1 - x2;
  }
};
//-----------------------------------------------------------------
// specialize for float
//-----------------------------------------------------------------
template<> struct nsub_pixel<dm_float> :_pixel_binfun<dm_float> {
  dm_float operator()( const dm_float&  x1, const dm_float& x2 ) {
    return x2 - x1;
  }
};
//-----------------------------------------------------------------
// specialize for float
//-----------------------------------------------------------------
template<> struct abs_pixel<dm_float> :_pixel_binfun<dm_float> {
  dm_float operator()( const dm_float&  x1, const dm_float& x2 ) {
    return daim::abs(x2) + daim::abs(x1);
  }
};
//-----------------------------------------------------------------
// The 3 following operators are not applicable for non-integer type
//-----------------------------------------------------------------
template<class T>
struct and_pixel : public _pixel_binfun<typename pixel_traits<T>::value_type> {
  typedef typename pixel_traits<T>::value_type value_type;
  value_type operator()( const value_type& x1,
                         const value_type& x2 ) {
   return x1 & x2;
  }
};
//------------------------------------------------------------
template<class T>
struct or_pixel : public _pixel_binfun<typename pixel_traits<T>::value_type> {
  typedef typename pixel_traits<T>::value_type value_type;
  value_type operator()( const value_type& x1,
                         const value_type& x2 ) {
   return x1 | x2;
 }
};
//------------------------------------------------------------
template<class T>
struct xor_pixel : public _pixel_binfun<typename pixel_traits<T>::value_type> {
  typedef typename pixel_traits<T>::value_type value_type;
  value_type operator()( const value_type& x1,
                         const value_type& x2 ) {
   return x1 ^ x2;
  }
};
//------------------------------------------------------------
template<class T1,class T2>
struct multiply_pixels :
  public std::binary_function<typename pixel_traits<T1>::value_type,
                              typename pixel_traits<T2>::value_type,
                              typename pixel_traits<T2>::value_type>
{
  typedef typename pixel_traits<T1>::value_type first_argument_type;
  typedef typename pixel_traits<T2>::value_type second_argument_type;
  typedef typename pixel_traits<T2>::value_type result_type;

  typedef pixel_traits<T2>  traits_type;
  typedef typename pixel_traits<T2>::integer_type integer_type;

  result_type operator()( const first_argument_type&  x1,
                          const second_argument_type& x2 )
  {
    return _get_range_value( x2 * static_cast<dm_float>(x1),
                             traits_type(), integer_type());
  }
};
//------------------------------------------------------------
template<class T1,class T2>
struct divide_pixels :
  public std::binary_function<typename pixel_traits<T1>::value_type,
                              typename pixel_traits<T2>::value_type,
                              typename pixel_traits<T2>::value_type>
{
  typedef typename pixel_traits<T1>::value_type first_argument_type;
  typedef typename pixel_traits<T2>::value_type second_argument_type;
  typedef typename pixel_traits<T2>::value_type result_type;

  typedef pixel_traits<T2>  traits_type;
  typedef typename pixel_traits<T2>::integer_type integer_type;

  result_type operator()( const first_argument_type&  x1,
                          const second_argument_type& x2 )
  {
    if(x1)
       return _get_range_value(x2 / static_cast<dm_float>(x1),
                               traits_type(),integer_type());
    else
      return pixel_traits<T2>::max();
  }
};
//-------------------------------------------------------------
// Unary operations
//-------------------------------------------------------------
template<class T1,class T2>
struct mul_pixel : std::unary_function<
          typename pixel_traits<T1>::value_type,
          typename pixel_traits<T2>::value_type
       >
{
  typedef pixel_traits<T2>  traits_type;
  typedef typename pixel_traits<T2>::integer_type integer_type;

  typedef typename pixel_traits<T1>::value_type argument_type;
  typedef typename pixel_traits<T2>::value_type result_type;

  dm_float a;
  mul_pixel( dm_float _x ) : a(_x) {}
  result_type operator()( const argument_type& _x ) const
  {
    return _get_range_value(_x * a, traits_type(), integer_type());
  }
};
//-------------------------------------------------------------
template<class T1,class T2>
struct addmul_pixel : std::unary_function<
          typename pixel_traits<T1>::value_type,
          typename pixel_traits<T2>::value_type
       >
{
  typedef pixel_traits<T2>  traits_type;
  typedef typename pixel_traits<T2>::integer_type integer_type;

  typedef typename pixel_traits<T1>::value_type argument_type;
  typedef typename pixel_traits<T2>::value_type result_type;

  dm_float a,b;
  addmul_pixel( dm_float _a, dm_float _b ) : a(_a),b(_b) {}
  result_type operator()( const argument_type& _x ) const
  {
    return _get_range_value( a * _x + b, traits_type(), integer_type());
  }

};
//-------------------------------------------------------------
template<class T1,class T2>
struct sqr_pixel : std::unary_function<
          typename pixel_traits<T1>::value_type,
          typename pixel_traits<T2>::value_type
       >
{
  typedef typename pixel_traits<T1>::value_type argument_type;
  typedef typename pixel_traits<T2>::value_type result_type;
  typedef typename pixel_traits<T2>::integer_type integer_type;

  mutable dm_float y;

  result_type operator()( const argument_type& _x ) const
  {
    y = static_cast<dm_float>(_x);
    return _get_range_value( y*y,pixel_traits<T2>(),integer_type() );
  }

};
//-------------------------------------------------------------
template<class T1,class T2>
struct _gamma_transform : std::unary_function<
          typename pixel_traits<T1>::value_type,
          typename pixel_traits<T2>::value_type>
{
  typedef typename pixel_traits<T1>::value_type   argument_type;
  typedef typename pixel_traits<T2>::value_type   result_type;
  typedef typename pixel_traits<T2>::integer_type integer_type;

  dm_real a,b,g;

  _gamma_transform( dm_real _a, dm_real _b, dm_real _g ) : a(_a),b(_b),g(_g) {}

  result_type operator()( const argument_type& _x ) const
  {
    return _round_value( a + pow( (_x - a)/(b-a), g ) * (b-a) ,
                         pixel_traits<T2>(),integer_type() );
  }
};
//-------------------------------------------------------------
template<class T1,class T2>
struct _sigma_transform : std::unary_function<
          typename pixel_traits<T1>::value_type,
          typename pixel_traits<T2>::value_type>
{
  typedef typename pixel_traits<T1>::value_type argument_type;
  typedef typename pixel_traits<T2>::value_type result_type;
  typedef typename pixel_traits<T2>::integer_type integer_type;

  dm_real a,b,g;

  _sigma_transform( dm_real _a, dm_real _b, dm_real _g ) : a(_a),b(_b),g(_g) {}
  result_type operator()( const argument_type& _x ) const
  {
    return _round_value( a +  (b-a)/( 1.0 + exp( g * ((_x-a)/(b-a) - 0.5)) ),
                         pixel_traits<T2>(),integer_type() );
  }
};
//-------------------------------------------------------------
// this operator expect a parameter value between 0 and 1
//-------------------------------------------------------------
template<class T1,class T2>
struct blend_pixels :
  public std::binary_function<typename pixel_traits<T1>::value_type,
                              typename pixel_traits<T2>::value_type,
                              typename pixel_traits<T2>::value_type>
{
  typedef typename pixel_traits<T1>::value_type value_type1;
  typedef typename pixel_traits<T2>::value_type value_type2;
  typedef typename pixel_traits<T2>::integer_type integer_type;

  dm_float a;
  blend_pixels( dm_float _a ) : a(_a) {}
  value_type2 operator()( const value_type1& x1,
                          const value_type2& x2 ) const {
    return _round_value( (1.0f - a) * x1 + a * x2,
              pixel_traits<T2>(),integer_type() );
  }
};

template<class T>
struct blend_color : std::unary_function<
          typename pixel_traits<T>::value_type,
          typename pixel_traits<T>::value_type>
{
  typedef typename pixel_traits<T>::value_type value_type;
  typedef typename pixel_traits<T>::integer_type integer_type;

  dm_float   a;
  value_type c;
  blend_color( const value_type& _c,  dm_float _a ) : a(_a), c(_c) {}
  value_type operator()( const value_type& x ) const {
    return _round_value( a * c + (1.0f-a) * x,
              pixel_traits<T>(),integer_type() );
  }
};


template<class T1,class T2,class A>
struct blend_pixels_alpha
{
  typedef typename pixel_traits<T1>::value_type   value_type1;
  typedef typename pixel_traits<T2>::value_type   value_type2;
  typedef typename pixel_traits<T2>::integer_type integer_type;

  typedef typename pixel_traits<A>::value_type    alpha_type;

  blend_pixels_alpha() {}
  value_type2 operator()( const value_type1& x1, const alpha_type& a,
                          const value_type2& x2 ) const {
    return _round_value( (x1 * a + (255-a) * x2)/255.0,
              pixel_traits<T2>(),integer_type() );
  }
};


//-----------------------------------------------------------------
// Truncate image values to the output format
//-----------------------------------------------------------------
template<class T1,class T2>
struct truncate_value :
    public std::unary_function<typename pixel_traits<T1>::value_type,
                               typename pixel_traits<T2>::value_type>
{
  typedef pixel_traits<T2>  traits_type;
  typedef typename pixel_traits<T2>::integer_type integer_type;

  typedef typename pixel_traits<T1>::value_type argument_type;
  typedef typename pixel_traits<T2>::value_type result_type;

  result_type operator()( const argument_type&  x ) {
    return _get_range_value(x,traits_type(),integer_type());
  }
};
//------------------------------------------------------------------------
template<class T1,class T2>
void truncate( const dmRegion& rgn,const dmPoint& p,const image<T1>& in ,image<T2>& out )
{ transform(rgn,p,in,out,truncate_value<T1,T2>()); }

template<class T1,class T2>
void truncate( const image<T1>& in ,image<T2>& out )
{ transform(in.rect(),dmPoint(0,0),in,out,truncate_value<T1,T2>()); }


//-----------------------------------------------------------------
// Truncate image values to the output format
//-----------------------------------------------------------------
template<class T1,class T2>
struct clamp_value :
    public std::unary_function<typename pixel_traits<T1>::value_type,
                               typename pixel_traits<T2>::value_type>
{
  typedef pixel_traits<T2>  traits_type;
  typedef typename pixel_traits<T2>::integer_type integer_type;

  typedef typename pixel_traits<T1>::value_type argument_type;
  typedef typename pixel_traits<T2>::value_type result_type;

  argument_type a,b;

  clamp_value( dm_real _a, dm_real _b )
  {
    typedef typename pixel_traits<T1>::integer_type int_type;
    a = get_range_value(_a,pixel_traits<T1>(),int_type());
    b = get_range_value(_b,pixel_traits<T1>(),int_type());
  }

  result_type operator()( const argument_type&  x ) {
    return (x <= a ? a : (x >= b ? b : x));
  }
};
//------------------------------------------------------------------------
template<class T1,class T2>
void clamp( const dmRegion& rgn,const dmPoint& p,const image<T1>& in ,image<T2>& out, dm_real a, dm_real b )
{ transform(rgn,p,in,out,clamp_value<T1,T2>(a,b)); }

template<class T1,class T2>
void clamp( const image<T1>& in , const image<T1>& out, dm_real a, dm_real b )
{ transform(in.rect(),dmPoint(0,0),in,out,truncate_value<T1,T2>(a,b)); }

//-----------------------------------------------------------------

template<class T> void add_images( const dmRegion& rgn,const dmPoint& p,const image<T>& in,image<T>& out )
{ combine(rgn,p,in,out,add_pixel<T>()); }

template<class T> void sub_images( const dmRegion& rgn,const dmPoint& p,const image<T>& in,image<T>& out )
{ combine(rgn,p,in,out,sub_pixel<T>()); }

template<class T> void nsub_images( const dmRegion& rgn,const dmPoint& p,const image<T>& in,image<T>& out )
{ combine(rgn,p,in,out,nsub_pixel<T>()); }

template<class T>
void diff_images( const dmRegion& rgn,const dmPoint& p,const image<T>& in ,image<T>& out )
{ combine(rgn,p,in,out,diff_pixel<T>()); }

template<class T> void or_images( const dmRegion& rgn,const dmPoint& p,const image<T>& in,image<T>& out )
{ combine(rgn,p,in,out,or_pixel<T>()); }

template<class T> void xor_images( const dmRegion& rgn,const dmPoint& p,const image<T>& in,image<T>& out )
{ combine(rgn,p,in,out,xor_pixel<T>()); }

template<class T> void and_images( const dmRegion& rgn,const dmPoint& p,const image<T>& in,image<T>& out )
{ combine(rgn,p,in,out,and_pixel<T>()); }

template<class T> void min_images( const dmRegion& rgn,const dmPoint& p,const image<T>& in,image<T>& out )
{ combine(rgn,p,in,out,min_pixel<T>()); }

template<class T> void max_images( const dmRegion& rgn,const dmPoint& p,const image<T>& in,image<T>& out )
{ combine(rgn,p,in,out,max_pixel<T>()); }

template<class T1,class T2>
void mul_images( const dmRegion& rgn,const dmPoint& p,const image<T1>& in ,image<T2>& out, dm_float y )
{ transform(rgn,p,in,out,mul_pixel<T1,T2>(y)); }

template<class T1,class T2>
void div_images( const dmRegion& rgn,const dmPoint& p,const  image<T1>& in, image<T2>& out, dm_float y )
{ transform(rgn,p,in,out,mul_pixel<T1,T2>(1.0/y)); }

template<class T1,class T2>
void addmul_images( const dmRegion& rgn,const dmPoint& p,const image<T1>& in ,image<T2>& out,
                   dm_float a, dm_float b )
{ transform(rgn,p,in,out,addmul_pixel<T1,T2>(a,b)); }

template<class T1,class T2>
void blend_images( const dmRegion& rgn,const dmPoint& p,const image<T1>& in ,image<T2>& out,
                   dm_float a)
{ combine(rgn,p,in,out,blend_pixels<T1,T2>(a<0.0f?0.0f:(a>1.0f?1.0f:a))); }


template<class T1,class T2, class A>
void blend_alpha( const dmRegion& rgn,const dmPoint& p,
                  const image<T1>& in ,image<T2>& out,
                  const image<A>& alpha )
{
  combine(rgn,p,in,alpha,out,alpha,blend_pixels_alpha<T1,T2,A>());
}



template<class T>
void abs_images( const dmRegion& rgn,const dmPoint& p,const image<T>& in ,image<T>& out )
{ combine(rgn,p,in,out,abs_pixel<T>()); }


//---------------------------------------------------
template<class T1,class T2>
void multiply_images( const dmRegion& rgn,const dmPoint& p, const image<T1>& rhs , image<T2>& lhs )
{
  combine(rgn,p,rhs,lhs,multiply_pixels<T1,T2>());
}
//---------------------------------------------------
template<class T1,class T2>
void divide_images( const dmRegion& rgn,const dmPoint& p, const image<T1>& rhs , image<T2>& lhs )
{
  combine(rgn,p,rhs,lhs,divide_pixels<T1,T2>());
}
//---------------------------------------------------
// Unary versions
// warning : no check performed on ROI

template<class ROI,class T>
void mul_image( const ROI& rgn,image<T>& in, dm_float y )
{ transform(rgn,in,mul_pixel<T,T>(y)); }

template<class ROI,class T>
void div_image( const ROI& rgn,image<T>& in, dm_float y )
{ transform(rgn,in,mul_pixel<T,T>(1.0/y)); }

template<class ROI,class T>
void addmul_image( const ROI& rgn,image<T>& in, dm_float a, dm_float b )
{ transform(rgn,in,addmul_pixel<T,T>(a,b)); }

template<class ROI,class T>
void sqr_image( const ROI& rgn,image<T>& in )
{ transform(rgn,in,sqr_pixel<T,T>()); }

template<class ROI,class T>
void gamma_transform( const ROI& rgn,image<T>& in, dm_real a, dm_real b, dm_real g )
{ transform(rgn,in,_gamma_transform<T,T>(a,b,g)); }

template<class ROI,class T>
void sigma_transform( const ROI& rgn,image<T>& in, dm_real a, dm_real b, dm_real g )
{ transform(rgn,in,_sigma_transform<T,T>(a,b,g)); }

template<class ROI,class T,class X>
void blend_fill( const ROI& rgn, image<T>& in, X c, dm_float a)
{ transform(rgn,in,blend_color<T>(c,a)); }

} //namespace daim

#endif
