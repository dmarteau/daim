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
  return (x.red==y.red && x.green==y.green && x.blue==y.blue);
}

// Use absolute norm !

inline bool operator<(const dmRGBColor& x,const dmRGBColor& y)  {
  return (static_cast<int>(x.red)+x.blue+x.green) <
         (static_cast<int>(y.red)+y.blue+y.green);
}

inline bool operator<=(const dmRGBColor& x,const dmRGBColor& y)  {
  return (static_cast<int>(x.red)+x.blue+x.green) <=
         (static_cast<int>(y.red)+y.blue+y.green);
}

inline bool operator>(const dmRGBColor& x,const dmRGBColor& y)  {
  return (static_cast<int>(x.red)+x.blue+x.green) >
         (static_cast<int>(y.red)+y.blue+y.green);
}

inline bool operator>=(const dmRGBColor& x,const dmRGBColor& y)  {
  return (static_cast<int>(x.red)+x.blue+x.green) >=
         (static_cast<int>(y.red)+y.blue+y.green);
}

//-----------------------------------------------------------------
// Compare in norm
//-----------------------------------------------------------------
template<>
struct max_pixel<dm_rgb24> : _pixel_binfun<dmRGBColor>
{
  dmRGBColor& operator()( const dmRGBColor& src, dmRGBColor& dst ) {
    if(src > dst) dst = src;
    return dst;
  }
};

template<>
struct min_pixel<dm_rgb24> : _pixel_binfun<dmRGBColor>
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
struct add_pixel<dm_rgb24>  : public _pixel_binfun<dmRGBColor>
{
  int ir,ig,ib;
  dmRGBColor& operator()( const dmRGBColor& x1, dmRGBColor& x2 ) {
    x2.red   = ((ir = static_cast<int>(x1.red)   + x2.red  ) >= 256 ? 255: ir);
    x2.green = ((ig = static_cast<int>(x1.green) + x2.green) >= 256 ? 255: ig);
    x2.blue  = ((ib = static_cast<int>(x1.blue)  + x2.blue ) >= 256 ? 255: ib);
    return x2;
  }
};

template<>
struct sub_pixel<dm_rgb24>  : public _pixel_binfun<dmRGBColor>
{
  dmRGBColor& operator()( const dmRGBColor& x1, dmRGBColor& x2 ) {
    x2.red   = (x1.red   >= x2.red   ? x1.red   - x2.red   : 0);
    x2.green = (x1.green >= x2.green ? x1.green - x2.green : 0);
    x2.blue  = (x1.blue  >= x2.blue  ? x1.blue  - x2.blue  : 0);
    return x2;
  }
};

template<>
struct nsub_pixel<dm_rgb24> : public _pixel_binfun<dmRGBColor>
{
  dmRGBColor& operator()( const dmRGBColor& x1, dmRGBColor& x2 ) {
    x2.red   = (x1.red   <= x2.red   ? x2.red   - x1.red   : 0);
    x2.green = (x1.green <= x2.green ? x2.green - x1.green : 0);
    x2.blue  = (x1.blue  <= x2.blue  ? x2.blue  - x1.blue  : 0);
    return x2;
  }
};
//-----------------------------------------------------------------
template<>
struct diff_pixel<dm_rgb24> : public _pixel_binfun<dmRGBColor>
{
  dmRGBColor& operator()( const dmRGBColor& x1, dmRGBColor& x2 )
  {
    x2.red   = daim::absdiff(x1.red,x2.red);
    x2.green = daim::absdiff(x1.green,x2.green);
    x2.blue  = daim::absdiff(x1.blue,x2.blue);
    return x2;
  }
};
//-----------------------------------------------------------------
template<>
struct and_pixel<dm_rgb24> : public _pixel_binfun<dmRGBColor> {
  dmRGBColor& operator()( const dmRGBColor& x1, dmRGBColor& x2 )
  {
    x2.red   = x1.red   & x2.red;
    x2.green = x1.green & x2.green;
    x2.blue  = x1.blue  & x2.blue;
    return x2;
  }
};
//------------------------------------------------------------
template<>
struct or_pixel<dm_rgb24> : public _pixel_binfun<dmRGBColor> {
  dmRGBColor& operator()( const dmRGBColor& x1, dmRGBColor& x2 )
  {
    x2.red   = x1.red   | x2.red;
    x2.green = x1.green | x2.green;
    x2.blue  = x1.blue  | x2.blue;
    return x2;
  }
};
//------------------------------------------------------------
template<>
struct xor_pixel<dm_rgb24> : public _pixel_binfun<dmRGBColor> {
  dmRGBColor& operator()( const dmRGBColor& x1, dmRGBColor& x2 )
  {
    x2.red   = x1.red   ^ x2.red;
    x2.green = x1.green ^ x2.green;
    x2.blue  = x1.blue  ^ x2.blue;
    return x2;
  }
};
//-------------------------------------------------------------
// Unary operations
//-------------------------------------------------------------
template<>
struct mul_pixel<dm_rgb24,dm_rgb24> : _pixel_binfun<dmRGBColor>
{
  dm_float a;
  mul_pixel( dm_float _a ) : a(_a) {}
  dmRGBColor& operator()( const dmRGBColor& x1, dmRGBColor& x2 ) const
  {
    x2.red   = _get_range_value( x1.red   * a, pixel_traits<dm_uint8>(),integer_true());
    x2.green = _get_range_value( x1.green * a, pixel_traits<dm_uint8>(),integer_true());
    x2.blue  = _get_range_value( x1.blue  * a, pixel_traits<dm_uint8>(),integer_true());
    return x2;
  }
};
//-------------------------------------------------------------
template<>
struct addmul_pixel<dm_rgb24,dm_rgb24> : _pixel_binfun<dmRGBColor>
{
  dm_float a,b;
  addmul_pixel( dm_float _a, dm_float _b ) : a(_a),b(_b) {}
  dmRGBColor& operator()( const dmRGBColor& x1, dmRGBColor& x2 ) const
  {
    x2.red   = _get_range_value( x1.red   * a + b, pixel_traits<dm_uint8>(),integer_true());
    x2.green = _get_range_value( x1.green * a + b, pixel_traits<dm_uint8>(),integer_true());
    x2.blue  = _get_range_value( x1.blue  * a + b, pixel_traits<dm_uint8>(),integer_true());
    return x2;
  }
};
//-------------------------------------------------------------
// this operator expect a parameter value between 0 and 1
//-------------------------------------------------------------
template<>
struct blend_pixels<dm_rgb24,dm_rgb24> : public _pixel_binfun<dmRGBColor>
{
  dm_float a;
  blend_pixels( dm_float _a ) : a(_a) {}
  dmRGBColor&  operator()( const dmRGBColor& x1, dmRGBColor& x2 ) const
  {
    x2.red   = _round_value( a * x1.red   + (1.0f-a) * x2.red  , pixel_traits<dm_uint8>(),integer_true());
    x2.green = _round_value( a * x1.green + (1.0f-a) * x2.green, pixel_traits<dm_uint8>(),integer_true());
    x2.blue  = _round_value( a * x1.blue  + (1.0f-a) * x2.blue , pixel_traits<dm_uint8>(),integer_true());
    return x2;
  }
};


struct blend_rgb24 :   public _pixel_binfun<dmRGBColor>
{
  dmRGBColor c;
  dm_float   a;
  blend_rgb24( const dmRGBColor& _c,  dm_float _a ) : c(_c), a(_a) {}
  dmRGBColor& operator()( const dmRGBColor& x1, dmRGBColor& x2 ) const {
    x2.red   = _round_value( (1.0f-a) * x1.red   + a * c.red  , pixel_traits<dm_uint8>(),integer_true());
    x2.green = _round_value( (1.0f-a) * x1.green + a * c.green, pixel_traits<dm_uint8>(),integer_true());
    x2.blue  = _round_value( (1.0f-a) * x1.blue  + a * c.blue , pixel_traits<dm_uint8>(),integer_true());
    return x2;
  }
};

template<class A>
struct _blend_rgb_alpha
{
  typedef typename pixel_traits<A>::value_type  alpha_type;

  _blend_rgb_alpha() {}
  dmRGBColor& operator()( const dmRGBColor& x1, const alpha_type& a,
                          dmRGBColor& x2 ) const {
    x2.red   = _round_value( (a * x1.red   + (255-a) * x2.red  )/255.0, pixel_traits<dm_uint8>(),integer_true());
    x2.green = _round_value( (a * x1.green + (255-a) * x2.green)/255.0, pixel_traits<dm_uint8>(),integer_true());
    x2.blue  = _round_value( (a * x1.blue  + (255-a) * x2.blue )/255.0, pixel_traits<dm_uint8>(),integer_true());
    return x2;
  }
};

struct _rgb_color_transform
{
  dm_real* m;

  _rgb_color_transform( dm_real* matrix ) : m(matrix) {}
  dmRGBColor& operator()( const dmRGBColor& x1, dmRGBColor& x2 ) const {
    x2.red   = _round_value( m[0] * x1.red   + m[1] * x1.green + m[2] * x1.blue, pixel_traits<dm_uint8>(),integer_true());
    x2.green = _round_value( m[3] * x1.red   + m[4] * x1.green + m[5] * x1.blue, pixel_traits<dm_uint8>(),integer_true());
    x2.blue  = _round_value( m[6] * x1.red   + m[7] * x1.green + m[8] * x1.blue, pixel_traits<dm_uint8>(),integer_true());
    return x2;
  }
};

//-----------------------------------------------------------------

inline void mul_image( const dmRegion& rgn,const dmPoint& p,const image<dm_rgb24>& in ,image<dm_rgb24>& out, dm_float y )
{ combine(rgn,p,in,out,mul_pixel<dm_rgb24,dm_rgb24>(y)); }

inline void div_image( const dmRegion& rgn,const dmPoint& p,const  image<dm_rgb24>& in, image<dm_rgb24>& out, dm_float y )
{ combine(rgn,p,in,out,mul_pixel<dm_rgb24,dm_rgb24>(1.0f/y)); }

inline void addmul_images( const dmRegion& rgn,const dmPoint& p,const image<dm_rgb24>& in ,image<dm_rgb24>& out,
                   dm_float a, dm_float b )
{ combine(rgn,p,in,out,addmul_pixel<dm_rgb24,dm_rgb24>(a,b)); }

template<class A>
void blend_rgb_alpha( const dmRegion& rgn,const dmPoint& p,
                  const image<dm_rgb24>& in ,image<dm_rgb24>& out,
                  const image<A>& alpha )
{
  combine(rgn,p,in,alpha,out,alpha,_blend_rgb_alpha<A>());
}

inline void rgb_color_transform( const dmRegion& rgn,const dmPoint& p,const image<dm_rgb24>& in ,image<dm_rgb24>& out, dm_real* m )
{ combine(rgn,p,in,out,_rgb_color_transform(m)); }


// Unary versions

inline void mul_image( const dmRegion& rgn,image<dm_rgb24>& in, dm_float y )
{ combine(rgn,rgn.Rectangle().TopLeft(),in,in,mul_pixel<dm_rgb24,dm_rgb24>(y)); }

inline void div_image( const dmRegion& rgn,image<dm_rgb24>& in, dm_float y )
{ combine(rgn,rgn.Rectangle().TopLeft(),in,in,mul_pixel<dm_rgb24,dm_rgb24>(1.0f/y)); }

inline void addmul_image( const dmRegion& rgn,image<dm_rgb24>& in, dm_float a, dm_float b )
{ combine(rgn,rgn.Rectangle().TopLeft(),in,in,addmul_pixel<dm_rgb24,dm_rgb24>(a,b)); }

inline void blend_fill_rgb( const dmRegion& rgn,image<dm_rgb24>& in, const dmRGBColor& c, dm_float a)
{ combine(rgn,rgn.Rectangle().TopLeft(),in,in,blend_rgb24(c,a)); }

inline void rgb_color_transform( const dmRegion& rgn, image<dm_rgb24>& in, dm_real* m )
{ combine(rgn,rgn.Rectangle().TopLeft(),in,in,_rgb_color_transform(m)); }

//-----------------------------------------------------------------

} //namespace daim

#endif // dmRGBArithmetic_h
