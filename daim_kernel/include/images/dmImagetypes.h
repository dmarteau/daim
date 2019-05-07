#ifndef dmImagetypes_h
#define dmImagetypes_h

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
// File         : dmImagetypes.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

//----------------------------------------------------------
// Implementation of dmImage int term  of image
// PixelFormat will be used to specify pixel type
//---------------------------------------------------------
template<EPixelFormat _PixelFormat> class dmIImageDescriptor;
  

template<EPixelFormat _PixelFormat>
class dmIImage : public dmImage 
{
   friend class dmIImageDescriptor<_PixelFormat>;
 public:
   typedef typename dmPixelFormat<_PixelFormat>::pixel_type pixel_type;

   typedef typename daim::image<pixel_type> image_type;
   typedef typename image_type::value_type  value_type;
   typedef typename image_type::traits_type traits_type;

 private:
   class dmGenImage : public image_type
   {
     friend class dmIImage<_PixelFormat>;
     // default protected constructor
     dmGenImage() {}
     void Construct(typename dmGenImage::value_type* _scan0,size_t _w,size_t _h,int _stride) {
        image_type::construct(_scan0,_w,_h,_stride);
     }
   };

   dmGenImage  _Gen;
   value_type* _LocalBuffer;

 public:
  ~dmIImage();
   dmIImage( size_t w, size_t h   );
   dmIImage( const image_type& im );

   // WARNING : You must ensure that the buffer 
   // stay valid during the lifetime of the image object
   dmIImage( const dmImageData& _Data );

   const image_type& Gen() const { return _Gen; }
         image_type& Gen()       { return _Gen; }       

   virtual dmImageDescriptor* TypeDescriptor() const;
   virtual bool GetImageData( dmImageData& )   const;

   dmIImage<_PixelFormat>& operator=(const dmIImage<_PixelFormat>& im) { 
     _Gen.copy(im._Gen); 
     return *this; 
   }

   virtual void FillArea( const dmRect&  , bool _object );
   virtual void FillArea( const dmRegion&, bool _object );

   void Fill( const dmRegion& aRoi , value_type x ); 
   void Fill( const dmRect&   aRect, value_type x ) { _Gen.fill(aRect,x); }
   void Fill( value_type x )                        { _Gen.fill( x ); }

   static dmIImage<_PixelFormat>* Cast( const dmImage* );
};

//-----------------------------------------------------------------
// dmCreateImage
//-----------------------------------------------------------------
template<EPixelFormat _PixelFormat>
struct dmCreateImage : public dmAutoLink<dmImage> {
  dmCreateImage( size_t w, size_t h ) :
   dmAutoLink<dmImage>(new dmIImage<_PixelFormat>(w,h)) {}

  dmCreateImage( const dmImageData& _Data ) :
   dmAutoLink<dmImage>(new  dmIImage<_PixelFormat>(_Data)) {}
};
//----------------------------------------------------------
#endif // dmImagetypes_h
