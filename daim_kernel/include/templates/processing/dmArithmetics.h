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

namespace fn {

//-----------------------------------------------------------------
// pixels operations 
//-----------------------------------------------------------------
template<typename T>
struct sub_pixel : public pixel_binary_function<T,T,T> 
{
   typedef typename pixel_traits<T>::value_type value_type;
  
   value_type _min;
   sub_pixel() : _min(pixel_traits<T>::min()) {} 
   value_type operator()( value_type _x1, value_type _x2 ) const
   { return (_x1 - _min >= _x2 ? (_x1 - _x2) : _min ); }
};
//
template<typename T>
struct add_pixel : public pixel_binary_function<T,T,T> 
{
  typedef typename pixel_traits<T>::value_type value_type;

   value_type _max;
   add_pixel() : _max(pixel_traits<T>::max()) {} 
   value_type operator()( value_type _x1, value_type _x2 ) const
   { 
     return (_x1 <= _max - _x2 ? (_x1 + _x2) : _max ); 
   }
};

//-----------------------------------------------------------------
//template<class T> struct add_pixel : public pixel_traits<T>::addpixel {};
//template<class T> struct sub_pixel : public pixel_traits<T>::subpixel {};
template<class T> struct max_pixel : public max_of<typename pixel_traits<T>::value_type> {};
template<class T> struct min_pixel : public min_of<typename pixel_traits<T>::value_type> {};
//-----------------------------------------------------------------

template<class T>
struct nsub_pixel: public pixel_binary_function<T,T,T>
{
  typedef typename pixel_traits<T>::value_type value_type;

  sub_pixel<T> _sub;

  value_type operator()( value_type x1,
                         value_type x2 ) 
  {
    return _sub.operator()(x2,x1);
  }
};
//-----------------------------------------------------------------
template<class T>
struct diff_pixel: public pixel_binary_function<T,T,T>
{
  typedef typename pixel_traits<T>::value_type value_type;
  value_type operator()( value_type x1,
                         value_type x2 ) 
  {
    return daim::absdiff(x1,x2);
  }
};
//-----------------------------------------------------------------
template<class T>
struct abs_pixel: public pixel_binary_function<T,T,T>
{
  typedef typename pixel_traits<T>::value_type value_type;
  value_type operator()( value_type x1,
                         value_type x2 ) 
  {
    return pixel_traits<T>::clamp(daim::abs(x1)+daim::abs(x2));
  }
};
//-----------------------------------------------------------------
// specialize for float
//-----------------------------------------------------------------
template<> struct sub_pixel<float> : std::binary_function<float,float,float> {
  float operator()( float x1, float x2 ) {
    return x1 - x2;
  }
};
//-----------------------------------------------------------------
// specialize for float
//-----------------------------------------------------------------
template<> struct nsub_pixel<float> : std::binary_function<float,float,float> {
  float operator()( float x1, float x2 ) {
    return x2 - x1;
  }
};
//-----------------------------------------------------------------
// specialize for float
//-----------------------------------------------------------------
template<> struct abs_pixel<float> : std::binary_function<float,float,float> {
  float operator()( float x1,  float x2 ) {
    return daim::abs(x2) + daim::abs(x1);
  }
};
//-----------------------------------------------------------------
// The 3 following operators are not applicable for non-integer type
//-----------------------------------------------------------------
template<class T>
struct and_pixel : public pixel_binary_function<T,T,T>  {
  typedef typename pixel_traits<T>::value_type value_type;
  value_type operator()( value_type x1,
                         value_type x2 ) {
   return x1 & x2;
  }
};
//------------------------------------------------------------
template<class T>
struct or_pixel : public pixel_binary_function<T,T,T>  {
  typedef typename pixel_traits<T>::value_type value_type;
  value_type operator()( value_type x1,
                         value_type x2 ) {
   return x1 | x2;
 }
};
//------------------------------------------------------------
template<class T>
struct xor_pixel : public pixel_binary_function<T,T,T>  {
  typedef typename pixel_traits<T>::value_type value_type;
  value_type operator()( value_type x1,
                         value_type x2 ) {
   return x1 ^ x2;
  }
};
//------------------------------------------------------------
template<class T1,class T2>
struct multiply_pixels : public pixel_binary_function<T1,T2,T2> 
{
  typedef typename pixel_traits<T1>::value_type first_argument_type;
  typedef typename pixel_traits<T2>::value_type second_argument_type;
  typedef typename pixel_traits<T2>::value_type result_type;

  result_type operator()( first_argument_type  x1,
                          second_argument_type x2 )
  {
    return pixel_traits<T2>::clamp( x2 * static_cast<float>(x1) );
  }
};
//------------------------------------------------------------
template<class T1,class T2>
struct divide_pixels : public pixel_binary_function<T1,T2,T2>
{
  typedef typename pixel_traits<T1>::value_type first_argument_type;
  typedef typename pixel_traits<T2>::value_type second_argument_type;
  typedef typename pixel_traits<T2>::value_type result_type;

  result_type operator()( const first_argument_type&  x1,
                          const second_argument_type& x2 )
  {
    if(x1)
       return pixel_traits<T2>::clamp(x2 / static_cast<float>(x1));
    else
      return pixel_traits<T2>::max();
  }
};
//-------------------------------------------------------------
// Unary operations
//-------------------------------------------------------------
template<class T1,class T2>
struct mul_pixel : public pixel_unary_function<T1,T2>
{
  typedef typename pixel_traits<T1>::value_type argument_type;
  typedef typename pixel_traits<T2>::value_type result_type;

  double a;
  mul_pixel( double _a ) : a(_a) {}
  result_type operator()(  argument_type _x ) const
  {
    return pixel_traits<T2>::clamp(_x * a);
  }
};
//-------------------------------------------------------------
template<class T1,class T2>
struct addmul_pixel : public pixel_unary_function<T1,T2>
{
  typedef typename pixel_traits<T1>::value_type argument_type;
  typedef typename pixel_traits<T2>::value_type result_type;

  double a,b;
  addmul_pixel( double _a, double _b ) : a(_a),b(_b) {}
  result_type operator()( argument_type _x ) const
  {
    return pixel_traits<T2>::clamp( a * _x + b );
  }

};
//-------------------------------------------------------------
template<class T1,class T2>
struct sqr_pixel : public pixel_unary_function<T1,T2>
{
  typedef typename pixel_traits<T1>::value_type argument_type;
  typedef typename pixel_traits<T2>::value_type result_type;

  result_type operator()( argument_type x ) const
  {
    return pixel_traits<T2>::clamp( static_cast<double>(x)*x );
  }

};
//-------------------------------------------------------------
template<class T1,class T2>
struct gamma_transform : public pixel_unary_function<T1,T2>
{
  typedef typename pixel_traits<T1>::value_type   argument_type;
  typedef typename pixel_traits<T2>::value_type   result_type;
  typedef typename pixel_traits<T2>::integer_type integer_type;

  double a,b,g;

  gamma_transform( double _a, double _b, double _g ) : a(_a),b(_b),g(_g) {}

  result_type operator()( const argument_type& _x ) const
  {
    return pixel_traits<T2>::round_value( a + pow( (_x - a)/(b-a), g ) * (b-a) );
  }
};
//-------------------------------------------------------------
template<class T1,class T2>
struct sigma_transform : public pixel_unary_function<T1,T2>
{
  typedef typename pixel_traits<T1>::value_type argument_type;
  typedef typename pixel_traits<T2>::value_type result_type;

  double a,b,g;

  sigma_transform( double _a, double _b, double _g ) : a(_a),b(_b),g(_g) {}
  result_type operator()( const argument_type& _x ) const
  {
    return pixel_traits<T2>::round_value( a +  (b-a)/( 1.0 + exp( g * ((_x-a)/(b-a) - 0.5)) ) );
  }
};
//-------------------------------------------------------------
// this operator expect a parameter value between 0 and 1
//-------------------------------------------------------------
template<class T1,class T2>
struct blend_pixels : public pixel_binary_function<T1,T2,T2>
{
  typedef typename pixel_traits<T1>::value_type value_type1;
  typedef typename pixel_traits<T2>::value_type value_type2;
 
  double a;
  blend_pixels( double _a ) : a(_a) {}
  value_type2 operator()( const value_type1& x1,
                          const value_type2& x2 ) const {
    return pixel_traits<T2>::round_value( (1.0 - a) * x1 + a * x2 );
  }
};

template<class T>
struct blend_color : public pixel_unary_function<T,T>
{
  typedef typename pixel_traits<T>::value_type value_type;

  double   a;
  value_type c;
  blend_color( value_type _c,  double _a ) : a(_a), c(_c) {}
  value_type operator()( const value_type& x ) const {
    return pixel_traits<T>::round_value( a * c + (1.0-a) * x );
  }
};


template<class T1,class T2>
struct blend_alpha
{
  typedef typename pixel_traits<T1>::value_type   value_type1;
  typedef typename pixel_traits<T2>::value_type   value_type2;

  blend_alpha() {}
  value_type2 operator()( value_type1 x1,  dm_uint8 a,
                          value_type2 x2 ) const {
    return pixel_traits<T2>::round_value( (x1 * a + (255-a) * x2)/255.0 );
  }
};


//-----------------------------------------------------------------
// Truncate image values to the output format
//-----------------------------------------------------------------

template<class T1,class T2>
struct truncate : public pixel_unary_function<T1,T2>
{
  typedef typename pixel_traits<T1>::value_type argument_type;
  typedef typename pixel_traits<T2>::value_type result_type;

  result_type operator()( argument_type  x ) {
    return pixel_traits<T2>::clamp(x);
  }
};

//-----------------------------------------------------------------
// Truncate image values to the output format
//-----------------------------------------------------------------
template<class T1,class T2>
struct clamp : public pixel_unary_function<T1,T2>
{
  typedef pixel_traits<T2>  traits_type;
  typedef typename pixel_traits<T2>::integer_type integer_type;

  typedef typename pixel_traits<T1>::value_type argument_type;
  typedef typename pixel_traits<T2>::value_type result_type;

  argument_type a,b;

  clamp( double _a, double _b )
  {
    typedef typename pixel_traits<T1>::integer_type int_type;
    a = pixel_traits<T1>::clamp(_a);
    b = pixel_traits<T1>::clamp(_b);
  }

  result_type operator()( argument_type x ) {
    return daim::clamp(x,a,b);
  }
};

}; // namespace fn
//------------------------------------------------------------------------
template<class T1,class T2>
void truncate( const dmRegion& rgn,const dmPoint& p,const image<T1>& in ,image<T2>& out )
{ daim::transform(rgn,p,in,out,fn::truncate<T1,T2>()); }

template<class T1,class T2>
void truncate( const image<T1>& in ,image<T2>& out )
{ daim::transform(in.rect(),dmPoint(0,0),in,out,fn::truncate<T1,T2>()); }

//------------------------------------------------------------------------
template<class T1,class T2>
void clamp( const dmRegion& rgn,const dmPoint& p,const image<T1>& in ,image<T2>& out, double a, double b )
{ daim::transform(rgn,p,in,out,fn::clamp<T1,T2>(a,b)); }

template<class T1,class T2>
void clamp( const image<T1>& in , const image<T1>& out, double a, double b )
{ daim::transform(in.rect(),dmPoint(0,0),in,out,fn::truncate<T1,T2>(a,b)); }

//-----------------------------------------------------------------

template<class T> void add_images( const dmRegion& rgn,const dmPoint& p,const image<T>& in,image<T>& out )
{ daim::combine(rgn,p,in,out,fn::add_pixel<T>()); }

template<class T> void sub_images( const dmRegion& rgn,const dmPoint& p,const image<T>& in,image<T>& out )
{ daim::combine(rgn,p,in,out,fn::sub_pixel<T>()); }

template<class T> void nsub_images( const dmRegion& rgn,const dmPoint& p,const image<T>& in,image<T>& out )
{ daim::combine(rgn,p,in,out,fn::nsub_pixel<T>()); }

template<class T>
void diff_images( const dmRegion& rgn,const dmPoint& p,const image<T>& in ,image<T>& out )
{ daim::combine(rgn,p,in,out,fn::diff_pixel<T>()); }

template<class T> void or_images( const dmRegion& rgn,const dmPoint& p,const image<T>& in,image<T>& out )
{ daim::combine(rgn,p,in,out,fn::or_pixel<T>()); }

template<class T> void xor_images( const dmRegion& rgn,const dmPoint& p,const image<T>& in,image<T>& out )
{ daim::combine(rgn,p,in,out,fn::xor_pixel<T>()); }

template<class T> void and_images( const dmRegion& rgn,const dmPoint& p,const image<T>& in,image<T>& out )
{ daim::combine(rgn,p,in,out,fn::and_pixel<T>()); }

template<class T> void min_images( const dmRegion& rgn,const dmPoint& p,const image<T>& in,image<T>& out )
{ daim::combine(rgn,p,in,out,fn::min_pixel<T>()); }

template<class T> void max_images( const dmRegion& rgn,const dmPoint& p,const image<T>& in,image<T>& out )
{ daim::combine(rgn,p,in,out,fn::max_pixel<T>()); }

template<class T1,class T2>
void mul_images( const dmRegion& rgn,const dmPoint& p,const image<T1>& in ,image<T2>& out, float y )
{ daim::transform(rgn,p,in,out,fn::mul_pixel<T1,T2>(y)); }

template<class T1,class T2>
void div_images( const dmRegion& rgn,const dmPoint& p,const  image<T1>& in, image<T2>& out, double y )
{ daim::transform(rgn,p,in,out,fn::mul_pixel<T1,T2>(1.0/y)); }

template<class T1,class T2>
void addmul_images( const dmRegion& rgn,const dmPoint& p,const image<T1>& in ,image<T2>& out,
                    float a, float b )
{ daim::transform(rgn,p,in,out,fn::addmul_pixel<T1,T2>(a,b)); }

template<class T1,class T2>
void blend_images( const dmRegion& rgn,const dmPoint& p,const image<T1>& in ,image<T2>& out,  double a)
{ daim::combine(rgn,p,in,out,fn::blend_pixels<T1,T2>(daim::clamp(a,0.0,1.0))); }

template<class T1,class T2>
void blend_alpha( const dmRegion& rgn,const dmPoint& p,
                  const image<T1>& in ,image<T2>& out,
                  const image<dm_uint8>& alpha )
{ daim::combine(rgn,p,in,alpha,out,alpha,fn::blend_alpha<T1,T2>()); }

template<class T>
void abs_images( const dmRegion& rgn,const dmPoint& p,const image<T>& in ,image<T>& out )
{ daim::combine(rgn,p,in,out,fn::abs_pixel<T>()); }

//---------------------------------------------------
template<class T1,class T2>
void multiply_images( const dmRegion& rgn,const dmPoint& p, const image<T1>& rhs , image<T2>& lhs )
{ daim::combine(rgn,p,rhs,lhs,fn::multiply_pixels<T1,T2>()); }
//---------------------------------------------------
template<class T1,class T2>
void divide_images( const dmRegion& rgn,const dmPoint& p, const image<T1>& rhs , image<T2>& lhs )
{ daim::combine(rgn,p,rhs,lhs,fn::divide_pixels<T1,T2>()); }
//---------------------------------------------------
// Unary versions
// warning : no check performed on ROI

template<class ROI,class T>
void mul_image( const ROI& rgn,image<T>& in, double y )
{ daim::transform(rgn,in,fn::mul_pixel<T,T>(y)); }

template<class ROI,class T>
void div_image( const ROI& rgn,image<T>& in, double y )
{ daim::transform(rgn,in,fn::mul_pixel<T,T>(1.0/y)); }

template<class ROI,class T>
void addmul_image( const ROI& rgn,image<T>& in, double a, double b )
{ daim::transform(rgn,in,fn::addmul_pixel<T,T>(a,b)); }

template<class ROI,class T>
void sqr_image( const ROI& rgn,image<T>& in )
{ daim::transform(rgn,in,fn::sqr_pixel<T,T>()); }

template<class ROI,class T>
void gamma_transform( const ROI& rgn,image<T>& in, double a, double b, double g )
{ daim::transform(rgn,in,fn::gamma_transform<T,T>(a,b,g)); }

template<class ROI,class T>
void sigma_transform( const ROI& rgn,image<T>& in, double a, double b, double g )
{ daim::transform(rgn,in,fn::sigma_transform<T,T>(a,b,g)); }

template<class ROI,class T,class X>
void blend_fill( const ROI& rgn, image<T>& in, X c, double a)
{ daim::transform(rgn,in,fn::blend_color<T>(c,a)); }  

//--------------------------------------------------------------
template<class ROI,class T> 
inline gap<typename image<T>::value_type> minmax( const ROI& _roi, const image<T>& _img ) 
{
  typedef typename image<T>::traits_type traits_type;
  return for_each(_roi,_img,traits_type::bounds().inverted()); 
}

} //namespace daim



#endif
