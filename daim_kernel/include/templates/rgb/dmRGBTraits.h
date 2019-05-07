#ifndef dmRGBTraits_h
#define dmRGBTraits_h

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
// File         : dmRGBTraits.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

//------------------------------------------------------
// Description :
// Specialize pixel_traits<> for  RGB pixels
//-----------------------------------------------------------

class dm_rgb24 
{
  private:
	dmRGBColor _value;

  public:
    // Use the luminance model to convert rgb to gray scale
    static dm_uint8 intensity ( const dmRGBColor& rgb ) {
      return static_cast<dm_uint8>(
        daim::round(0.299f * rgb.r + 0.587f * rgb.g + 0.114f * rgb.b));
    }

    dm_rgb24() { _value.g = _value.r = _value.b = 0; _value.a = 0xff; }
  	dm_rgb24( dm_uint8 r,dm_uint8 g, dm_uint8 b) 
		{ _value.r = r; _value.g = g; _value.b = b; _value.a = 0xff; }

  	dm_rgb24( dm_uint8 v )
		{ _value.r = _value.g = _value.b = v; _value.a = 0xff; }

    dm_rgb24( const dmRGBColor& rgb ) : _value(rgb) {}
   ~dm_rgb24() {} 
	
    operator       dmRGBColor&()       { return _value; }
    operator const dmRGBColor&() const { return _value; }

    const dmRGBColor& value() const { return _value; }
    dmRGBColor&       value() { return _value; }

    dm_rgb24& operator=(const dmRGBColor& x) { _value = x; return *this; } 
    dm_rgb24& operator=(int x) { 
       _value.r = _value.g = _value.b = x; 
       return *this; 
    }

    dm_uint8 red()   const { return _value.r;   }
    dm_uint8 green() const { return _value.g; }
    dm_uint8 blue()  const { return _value.b;  }

};

namespace daim {

//------------------------------------------------------------------
template<> struct basic_traits<dmRGBColor>
{
  typedef dmRGBColor          value_type;
  typedef dmRGBColor*         pointer_type;
  typedef dmRGBColor&         reference_type;
  typedef const dmRGBColor*   const_pointer_type;
  typedef const dmRGBColor&   const_reference_type;

  typedef pixel_compound_tag pixel_category;

  typedef undefined_assertion  signature_type;
  typedef undefined_assertion  integer_type;

  static  value_type min()    { return dm_rgb24(0,0,0);       }
  static  value_type max()    { return dm_rgb24(255,255,255); }
  static  value_type zero()   { return dm_rgb24(0,0,0);       }
  static  value_type object() { return dm_rgb24(255,255,255); }

};
//------------------------------------------------------------------
// pixel traits
//-------------------------------------------------------------------
template<>
struct pixel_traits<dm_rgb24>
{
  //-----------------------------------------------------------------
  typedef basic_traits<dmRGBColor>::pixel_category       pixel_category;
  typedef basic_traits<dmRGBColor>::value_type           value_type;
  typedef basic_traits<dmRGBColor>::pointer_type         pointer_type;
  typedef basic_traits<dmRGBColor>::reference_type       reference_type;
  typedef basic_traits<dmRGBColor>::const_pointer_type   const_pointer_type;
  typedef basic_traits<dmRGBColor>::const_reference_type const_reference_type;
  typedef basic_traits<dmRGBColor>::signature_type       signature_type;
  typedef basic_traits<dmRGBColor>::integer_type         integer_type;

  typedef dm_uint8 channel_type;

  static bool        is_scalar()  { return false; }

  static  size_t     unit_size()  { return sizeof(value_type);      }
  static  size_t     bits_depth() { return sizeof(value_type) << 3; }

  static  value_type min()        { return basic_traits<dmRGBColor>::min();    }
  static  value_type max()        { return basic_traits<dmRGBColor>::max();    }
  static  value_type zero()       { return basic_traits<dmRGBColor>::zero();   }
  static  value_type object()     { return basic_traits<dmRGBColor>::object(); }

  static gap<value_type> bounds() { return gap<value_type>(min(),max()); }

  static dm_uint8 scalar_value(value_type _v) {
       return dm_rgb24::intensity(_v);
  }

  struct red   { dm_uint8 operator()(value_type x) { return x.r; } };
  struct green { dm_uint8 operator()(value_type x) { return x.g; } };
  struct blue  { dm_uint8 operator()(value_type x) { return x.b; } };

  struct to_scalar : public std::unary_function<value_type,dm_uint8> 
  { dm_uint8 operator()( value_type x ) {return dm_rgb24::intensity(x);} };

  template<class T>
  struct from_scalar : public std::unary_function<T,value_type> 
  { 
    value_type operator()( const T& x ) {
        return dm_rgb24(static_cast<channel_type>(x)).value();
    } 
  };

  struct get_red : public std::unary_function<value_type,dm_uint8>  {
     dm_uint8 operator()( const value_type& _rgb ) { return _rgb.r; }
  };

  struct get_green : public std::unary_function<value_type,dm_uint8>  {
     dm_uint8 operator()( const value_type& _rgb ) { return _rgb.g; }
  };

  struct get_blue : public std::unary_function<value_type,dm_uint8>  {
     dm_uint8 operator()( const value_type& _rgb ) { return _rgb.b; }
  };

  struct set_red : public std::binary_function<dm_uint8,value_type,value_type>  {
     value_type& operator()( const dm_uint8& x, value_type& _rgb ) {
       _rgb.r = x; return _rgb;
     }
  };

  struct set_green : public std::binary_function<dm_uint8,value_type,value_type>  {
     value_type& operator()( const dm_uint8& x, value_type& _rgb ) {
       _rgb.g = x; return _rgb;
     }
  };

  struct set_blue : public std::binary_function<dm_uint8,value_type,value_type>  {
     value_type& operator()( const dm_uint8& x, value_type& _rgb ) {
       _rgb.b = x; return _rgb;
     }
  };

}; // pixel_traits

}; // namespace daim

//-----------------------------------------------------------------
#endif // dmRGBTraits_h
