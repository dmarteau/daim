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
    x2.r = ((ir = static_cast<int>(x1.r) + x2.r) >= 256 ? 255: ir);
    x2.g = ((ig = static_cast<int>(x1.g) + x2.g) >= 256 ? 255: ig);
    x2.b = ((ib = static_cast<int>(x1.b) + x2.b) >= 256 ? 255: ib);
    return x2;
  }
};

template<>
struct sub_pixel<dm_rgb24>  : public _pixel_binfun<dmRGBColor>
{
  dmRGBColor& operator()( const dmRGBColor& x1, dmRGBColor& x2 ) {
    x2.r = (x1.r >= x2.r ? x1.r - x2.r : 0);
    x2.g = (x1.g >= x2.g ? x1.g - x2.g : 0);
    x2.b = (x1.b >= x2.b ? x1.b - x2.b : 0);
    return x2;
  }
};

template<>
struct nsub_pixel<dm_rgb24> : public _pixel_binfun<dmRGBColor>
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
struct diff_pixel<dm_rgb24> : public _pixel_binfun<dmRGBColor>
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
struct and_pixel<dm_rgb24> : public _pixel_binfun<dmRGBColor> {
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
struct or_pixel<dm_rgb24> : public _pixel_binfun<dmRGBColor> {
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
struct xor_pixel<dm_rgb24> : public _pixel_binfun<dmRGBColor> {
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
struct mul_pixel<dm_rgb24,dm_rgb24> : _pixel_binfun<dmRGBColor>
{
  dm_float a;
  mul_pixel( dm_float _a ) : a(_a) {}
  dmRGBColor& operator()( const dmRGBColor& x1, dmRGBColor& x2 ) const
  {
    x2.r = _get_range_value( x1.r * a, pixel_traits<dm_uint8>(),integer_true());
    x2.g = _get_range_value( x1.g * a, pixel_traits<dm_uint8>(),integer_true());
    x2.b = _get_range_value( x1.b * a, pixel_traits<dm_uint8>(),integer_true());
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
    x2.r = _get_range_value( x1.r * a + b, pixel_traits<dm_uint8>(),integer_true());
    x2.g = _get_range_value( x1.g * a + b, pixel_traits<dm_uint8>(),integer_true());
    x2.b = _get_range_value( x1.b * a + b, pixel_traits<dm_uint8>(),integer_true());
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
    x2.r = _round_value( a * x1.r + (1.0f-a) * x2.r, pixel_traits<dm_uint8>(),integer_true());
    x2.g = _round_value( a * x1.g + (1.0f-a) * x2.g, pixel_traits<dm_uint8>(),integer_true());
    x2.b = _round_value( a * x1.b + (1.0f-a) * x2.b, pixel_traits<dm_uint8>(),integer_true());
    return x2;
  }
};


struct blend_rgb24 :   public _pixel_binfun<dmRGBColor>
{
  dmRGBColor c;
  dm_float   a;
  blend_rgb24( const dmRGBColor& _c,  dm_float _a ) : c(_c), a(_a) {}
  dmRGBColor& operator()( const dmRGBColor& x1, dmRGBColor& x2 ) const {
    x2.r = _round_value( (1.0f-a) * x1.r + a * c.r, pixel_traits<dm_uint8>(),integer_true());
    x2.g = _round_value( (1.0f-a) * x1.g + a * c.g, pixel_traits<dm_uint8>(),integer_true());
    x2.b = _round_value( (1.0f-a) * x1.b + a * c.b, pixel_traits<dm_uint8>(),integer_true());
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
    x2.r = _round_value( (a * x1.r + (255-a) * x2.r)/255.0, pixel_traits<dm_uint8>(),integer_true());
    x2.g = _round_value( (a * x1.g + (255-a) * x2.g)/255.0, pixel_traits<dm_uint8>(),integer_true());
    x2.b = _round_value( (a * x1.b + (255-a) * x2.b)/255.0, pixel_traits<dm_uint8>(),integer_true());
    return x2;
  }
};

struct _rgb_color_transform
{
  dm_real* m;

  _rgb_color_transform( dm_real* matrix ) : m(matrix) {}
  dmRGBColor& operator()( const dmRGBColor& x1, dmRGBColor& x2 ) const {
    x2.r = _round_value( m[0] * x1.r + m[1] * x1.g + m[2] * x1.b, pixel_traits<dm_uint8>(),integer_true());
    x2.g = _round_value( m[3] * x1.r + m[4] * x1.g + m[5] * x1.b, pixel_traits<dm_uint8>(),integer_true());
    x2.b = _round_value( m[6] * x1.r + m[7] * x1.g + m[8] * x1.b, pixel_traits<dm_uint8>(),integer_true());
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
