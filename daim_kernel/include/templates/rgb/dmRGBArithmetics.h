#ifndef dmRGBArithmetic_h
#define dmRGBArithmetic_h

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
// File         : dmRGBArithmetic.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

//-----------------------------------------------------------
// Comparison operator
//-----------------------------------------------------------
namespace daim {

inline bool operator==(const dmRGBColor& x,const dmRGBColor& y)  {
  return (x.r==y.r && x.g==y.g && x.b==y.b);
}

// Use absolute norm !

inline bool operator<(const dmRGBColor& x,const dmRGBColor& y)  {
  return (static_cast<int>(x.r)+x.b+x.g) <
         (static_cast<int>(y.r)+y.b+y.g);
}

inline bool operator<=(const dmRGBColor& x,const dmRGBColor& y)  {
  return (static_cast<int>(x.r)+x.b+x.g) <=
         (static_cast<int>(y.r)+y.b+y.g);
}

inline bool operator>(const dmRGBColor& x,const dmRGBColor& y)  {
  return (static_cast<int>(x.r)+x.b+x.g) >
         (static_cast<int>(y.r)+y.b+y.g);
}

inline bool operator>=(const dmRGBColor& x,const dmRGBColor& y)  {
  return (static_cast<int>(x.r)+x.b+x.g) >=
         (static_cast<int>(y.r)+y.b+y.g);
}

//-----------------------------------------------------------------
// Compare in norm
//-----------------------------------------------------------------

namespace fn {

struct rgb_binary_fun : public std::binary_function<dmRGBColor,dmRGBColor,dmRGBColor> {};

template<>
struct max_pixel<dm_rgb24>  : public rgb_binary_fun
{
  dmRGBColor& operator()( const dmRGBColor& src, dmRGBColor& dst ) {
    if(src > dst) dst = src;
    return dst;
  }
};

template<>
struct min_pixel<dm_rgb24> : public rgb_binary_fun
{
  dmRGBColor& operator()( const dmRGBColor& x1, dmRGBColor& x2 ) {
    if(x1 < x2) x2 = x1;
    return x2;
  }
};

//-----------------------------------------------------------
// basic Arithmetics
//-----------------------------------------------------------

template<>
struct add_pixel<dm_rgb24>  : public rgb_binary_fun
{
  int ir,ig,ib;
  dmRGBColor& operator()( const dmRGBColor& x1, dmRGBColor& x2 ) {
    x2.r = ((ir = static_cast<int>(x1.r) + x2.r) >= 256 ? 255: ir);
    x2.g = ((ig = static_cast<int>(x1.g) + x2.g) >= 256 ? 255: ig);
    x2.b = ((ib = static_cast<int>(x1.b) + x2.b) >= 256 ? 255: ib);
    return x2;
  }
};

template<>
struct sub_pixel<dm_rgb24>  : public rgb_binary_fun
{
  dmRGBColor& operator()( const dmRGBColor& x1, dmRGBColor& x2 ) {
    x2.r = (x1.r >= x2.r ? x1.r - x2.r : 0);
    x2.g = (x1.g >= x2.g ? x1.g - x2.g : 0);
    x2.b = (x1.b >= x2.b ? x1.b - x2.b : 0);
    return x2;
  }
};

template<>
struct nsub_pixel<dm_rgb24> : public rgb_binary_fun
{
  dmRGBColor& operator()( const dmRGBColor& x1, dmRGBColor& x2 ) {
    x2.r = (x1.r <= x2.r ? x2.r - x1.r : 0);
    x2.g = (x1.g <= x2.g ? x2.g - x1.g : 0);
    x2.b = (x1.b <= x2.b ? x2.b - x1.b : 0);
    return x2;
  }
};
//-----------------------------------------------------------------
template<>
struct diff_pixel<dm_rgb24> : public rgb_binary_fun
{
  dmRGBColor& operator()( const dmRGBColor& x1, dmRGBColor& x2 )
  {
    x2.r = daim::absdiff(x1.r,x2.r);
    x2.g = daim::absdiff(x1.g,x2.g);
    x2.b = daim::absdiff(x1.b,x2.b);
    return x2;
  }
};
//-----------------------------------------------------------------
template<>
struct and_pixel<dm_rgb24> : public rgb_binary_fun {
  dmRGBColor& operator()( const dmRGBColor& x1, dmRGBColor& x2 )
  {
    x2.r = x1.r & x2.r;
    x2.g = x1.g & x2.g;
    x2.b = x1.b & x2.b;
    return x2;
  }
};
//------------------------------------------------------------
template<>
struct or_pixel<dm_rgb24> : public rgb_binary_fun {
  dmRGBColor& operator()( const dmRGBColor& x1, dmRGBColor& x2 )
  {
    x2.r = x1.r | x2.r;
    x2.g = x1.g | x2.g;
    x2.b = x1.b | x2.b;
    return x2;
  }
};
//------------------------------------------------------------
template<>
struct xor_pixel<dm_rgb24> : public rgb_binary_fun {
  dmRGBColor& operator()( const dmRGBColor& x1, dmRGBColor& x2 )
  {
    x2.r = x1.r ^ x2.r;
    x2.g = x1.g ^ x2.g;
    x2.b = x1.b ^ x2.b;
    return x2;
  }
};
//-------------------------------------------------------------
// Unary operations
//-------------------------------------------------------------
template<>
struct mul_pixel<dm_rgb24,dm_rgb24> : rgb_binary_fun
{
  double a;
  mul_pixel( double _a ) : a(_a) {}
  dmRGBColor& operator()( const dmRGBColor& x1, dmRGBColor& x2 ) const
  {
    x2.r = pixel_traits<dm_uint8>::clamp( x1.r * a );
    x2.g = pixel_traits<dm_uint8>::clamp( x1.g * a );
    x2.b = pixel_traits<dm_uint8>::clamp( x1.b * a );
    return x2;
  }
};
//-------------------------------------------------------------
template<>
struct addmul_pixel<dm_rgb24,dm_rgb24> : rgb_binary_fun
{
  double a,b;
  addmul_pixel( double _a, double _b ) : a(_a),b(_b) {}
  dmRGBColor& operator()( const dmRGBColor& x1, dmRGBColor& x2 ) const
  {
    x2.r = pixel_traits<dm_uint8>::clamp( x1.r * a + b);
    x2.g = pixel_traits<dm_uint8>::clamp( x1.g * a + b);
    x2.b = pixel_traits<dm_uint8>::clamp( x1.b * a + b);
    return x2;
  }
};
//-------------------------------------------------------------
// this operator expect a parameter value between 0 and 1
//-------------------------------------------------------------
template<>
struct blend_pixels<dm_rgb24,dm_rgb24> : public rgb_binary_fun
{
  float a;
  blend_pixels( float _a ) : a(_a) {}
  dmRGBColor&  operator()( const dmRGBColor& x1, dmRGBColor& x2 ) const
  {
    x2.r = pixel_traits<dm_uint8>::round_value( a * x1.r + (1.0f-a) * x2.r );
    x2.g = pixel_traits<dm_uint8>::round_value( a * x1.g + (1.0f-a) * x2.g );
    x2.b = pixel_traits<dm_uint8>::round_value( a * x1.b + (1.0f-a) * x2.b );
    return x2;
  }
};


struct blend_rgb24 :   public rgb_binary_fun
{
  dmRGBColor c;
  double   a;
  blend_rgb24( const dmRGBColor& _c,  double _a ) : c(_c), a(_a) {}
  dmRGBColor& operator()( const dmRGBColor& x1, dmRGBColor& x2 ) const {
    x2.r = pixel_traits<dm_uint8>::round_value( (1.0-a) * x1.r + a * c.r);
    x2.g = pixel_traits<dm_uint8>::round_value( (1.0-a) * x1.g + a * c.g);
    x2.b = pixel_traits<dm_uint8>::round_value( (1.0-a) * x1.b + a * c.b);
    return x2;
  }
};

struct blend_rgb_alpha
{
  blend_rgb_alpha() {}
  dmRGBColor& operator()( const dmRGBColor& x1,  dm_uint8 a,
                          dmRGBColor& x2 ) const {
    x2.r = pixel_traits<dm_uint8>::round_value( (a * x1.r + (255-a) * x2.r)/255.0 );
    x2.g = pixel_traits<dm_uint8>::round_value( (a * x1.g + (255-a) * x2.g)/255.0 );
    x2.b = pixel_traits<dm_uint8>::round_value( (a * x1.b + (255-a) * x2.b)/255.0 );
    return x2;
  }
};

struct rgb_color_transform
{
  double* m;

  rgb_color_transform( double* matrix ) : m(matrix) {}
  dmRGBColor& operator()( const dmRGBColor& x1, dmRGBColor& x2 ) const {
    x2.r = pixel_traits<dm_uint8>::round_value( m[0] * x1.r + m[1] * x1.g + m[2] * x1.b);
    x2.g = pixel_traits<dm_uint8>::round_value( m[3] * x1.r + m[4] * x1.g + m[5] * x1.b);
    x2.b = pixel_traits<dm_uint8>::round_value( m[6] * x1.r + m[7] * x1.g + m[8] * x1.b);
    return x2;
  }
};

}; // namespace fn

//-----------------------------------------------------------------

inline void mul_image( const dmRegion& rgn,const dmPoint& p,const image<dm_rgb24>& in ,image<dm_rgb24>& out, double y )
{ daim::combine(rgn,p,in,out,fn::mul_pixel<dm_rgb24,dm_rgb24>(y)); }

inline void div_image( const dmRegion& rgn,const dmPoint& p,const  image<dm_rgb24>& in, image<dm_rgb24>& out, double y )
{ daim::combine(rgn,p,in,out,fn::mul_pixel<dm_rgb24,dm_rgb24>(1.0/y)); }

inline void addmul_images( const dmRegion& rgn,const dmPoint& p,const image<dm_rgb24>& in ,image<dm_rgb24>& out,
                           double a, double b )
{ daim::combine(rgn,p,in,out,fn::addmul_pixel<dm_rgb24,dm_rgb24>(a,b)); }

inline void blend_rgb_alpha( const dmRegion& rgn,const dmPoint& p,
                             const image<dm_rgb24>& in ,image<dm_rgb24>& out,
                             const image<dm_uint8>& alpha )
{
  daim::combine(rgn,p,in,alpha,out,fn::blend_rgb_alpha());
}

inline void rgb_color_transform( const dmRegion& rgn,const dmPoint& p,const image<dm_rgb24>& in ,image<dm_rgb24>& out, dm_real* m )
{ daim::combine(rgn,p,in,out,fn::rgb_color_transform(m)); }


// Unary versions

inline void mul_image( const dmRegion& rgn,image<dm_rgb24>& in, float y )
{ daim::combine(rgn,rgn.Rectangle().TopLeft(),in,in,fn::mul_pixel<dm_rgb24,dm_rgb24>(y)); }

inline void div_image( const dmRegion& rgn,image<dm_rgb24>& in, float y )
{ daim::combine(rgn,rgn.Rectangle().TopLeft(),in,in,fn::mul_pixel<dm_rgb24,dm_rgb24>(1.0f/y)); }

inline void addmul_image( const dmRegion& rgn,image<dm_rgb24>& in, float a, float b )
{ daim::combine(rgn,rgn.Rectangle().TopLeft(),in,in,fn::addmul_pixel<dm_rgb24,dm_rgb24>(a,b)); }

inline void blend_fill_rgb( const dmRegion& rgn,image<dm_rgb24>& in, const dmRGBColor& c, float a)
{ daim::combine(rgn,rgn.Rectangle().TopLeft(),in,in,fn::blend_rgb24(c,a)); }

inline void rgb_color_transform( const dmRegion& rgn, image<dm_rgb24>& in, dm_real* m )
{ daim::combine(rgn,rgn.Rectangle().TopLeft(),in,in,fn::rgb_color_transform(m)); }

//-----------------------------------------------------------------

} //namespace daim

#endif // dmRGBArithmetic_h
