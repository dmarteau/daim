#ifndef dmImage_h
#define dmImage_h

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
// File         : dmImage.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

class dmImageDescriptor;
//-------------------------------------------------------------
class __dmKernel dmImage : public dmMemoryObject
{
  private:
    dmLink<dmSpatialUnits> _Units;
    dmRect                 _Rect;

  protected:
    friend class dmImageDescriptor;

    dmImage(size_t _Width,size_t _Height);

  public:
    virtual ~dmImage();

    size_t Width()  const { return _Rect.Width();  }
    size_t Height() const { return _Rect.Height(); }

    dmLink<dmImage> CreateCopy( const dmImageDescriptor& _desc,  const dmRect& ) const;
    dmLink<dmImage> CreateCopy( const dmImageDescriptor& _desc ) const;
    dmLink<dmImage> CreateCopy( const dmRect& ) const;
    dmLink<dmImage> CreateCopy() const;

    virtual dmImageDescriptor* TypeDescriptor() const = 0;
    virtual bool GetImageData( dmImageData& )   const = 0;

    EPixelFormat  PixelFormat() const;
    const dmRect& Rect()        const { return _Rect; }
    dmPoint  Dimensions()       const { return dmPoint(Width(),Height()); }

    virtual void FillArea( const dmRect&  , bool _object = true ) = 0;
    virtual void FillArea( const dmRegion&, bool _object = true ) = 0;

    void ClearArea( const dmRect&   rct ) { FillArea(rct,false); }
    void ClearArea( const dmRegion& rgn ) { FillArea(rgn,false); }

    void Clear() { ClearArea(_Rect); }
    void Fill()  { FillArea (_Rect); }

    // Copy region area at location p 
    void GetCopy( const dmImage&, const dmRegion&, const dmPoint& p=dmPoint(0,0) );
    void GetCopy( const dmImage& im, const dmPoint& p=dmPoint(0,0) ) {
      GetCopy(im,im.Rect(),p);
    }

    //---------------------------------------
    // Spatial calibration
    //---------------------------------------
    dmConstLink<dmSpatialUnits> Units() const;
    
    void SetUnits(const dmSpatialUnits&);
    bool GetUnits(dmSpatialUnits&) const;
    void ApplyUnits(const dmImage&);

    bool HasUnits() const;
    void ClearUnits();
};
//------------------------------------------------------------
class dmImageDescriptor
{
  protected:
    dmImageDescriptor() {};

  public:
    virtual ~dmImageDescriptor() {};

    virtual EPixelFormat     PixelFormat() const = 0;
    virtual dmLink<dmImage>  CreateImage(size_t _width,size_t _height) const = 0;
    virtual dmLink<dmImage>  CreateImage( const dmImageData& ) const = 0;

    bool operator==(const dmImageDescriptor& _desc) { return PixelFormat() == _desc.PixelFormat(); }
    bool operator!=(const dmImageDescriptor& _desc) { return PixelFormat() != _desc.PixelFormat(); }

    int  GetPixelFormatIndex     () { return dmGetPixelFormatIndex     (PixelFormat()); }
    int  GetPixelFormatBits      () { return dmGetPixelFormatBits      (PixelFormat()); }
    bool IsPixelFormatIndexed    () { return dmIsPixelFormatIndexed    (PixelFormat())==1; }
    bool IsPixelFormatDisplayable() { return dmIsPixelFormatDisplayable(PixelFormat())==1; }
    bool IsPixelFormatScalar     () { return dmIsPixelFormatScalar     (PixelFormat())==1; }
    bool IsPixelFormatSigned     () { return dmIsPixelFormatSigned     (PixelFormat())==1; }

    static __dmKernel dmImageDescriptor* GetDescriptor( EPixelFormat );
};
//-------------------------------------------------------------
inline dmImageDescriptor* dmGetDescriptor( EPixelFormat _fmt ) {
  return dmImageDescriptor::GetDescriptor(_fmt);
}
//-------------------------------------------------------------
inline dmLink<dmImage> dmCreateCopy( const dmImage* anImage, EPixelFormat _fmt ) 
{
  if(anImage) {
     dmImageDescriptor* aDesc = dmGetDescriptor(_fmt);
     if(aDesc) 
        return anImage->CreateCopy(*aDesc);
  }
  return dmReturnLink((dmImage*)NULL);
}
//-------------------------------------------------------------
inline dmLink<dmImage> dmCreateCopy( const dmImage* anImage, const dmRect& aRect, EPixelFormat _fmt ) 
{
  if(anImage) {
     dmImageDescriptor* aDesc = dmGetDescriptor(_fmt);
     if(aDesc) 
        return anImage->CreateCopy(*aDesc,aRect);
  }
  return dmReturnLink((dmImage*)NULL);
}
//-------------------------------------------------------------------------
// inline members
//-------------------------------------------------------------------------

inline dmLink<dmImage> dmImage::CreateCopy( const dmImageDescriptor& _desc ) const {
  return CreateCopy(_desc, Rect() );
}

inline dmLink<dmImage> dmImage::CreateCopy( const dmRect& _rect ) const {
  return CreateCopy( *TypeDescriptor(), _rect );
}

inline dmLink<dmImage> dmImage::CreateCopy() const { 
  return CreateCopy( *TypeDescriptor() ); 
}

inline bool dmImage::HasUnits() const {
  return !_Units.IsNull();
}

inline void dmImage::ClearUnits() {
  _Units.Release();
}

inline dmConstLink<dmSpatialUnits> dmImage::Units() const { 
  return _Units; 
}

inline  EPixelFormat dmImage::PixelFormat() const { 
  return TypeDescriptor()->PixelFormat(); 
}
//---------------------------------------------------------------------------
inline bool dmSameImageSize( const dmImage& _im1, const dmImage& _im2 )
{
  return (_im1.Width() ==_im2.Width()  &&
          _im1.Height()==_im2.Height());
}
//---------------------------------------------------------------------------
inline bool dmSameImageTypeAndSize( const dmImage& _im1, const dmImage& _im2 )
{
  return (_im1.Width() ==_im2.Width()  &&
          _im1.Height()==_im2.Height() && 
          _im1.PixelFormat()==_im2.PixelFormat());
}
//---------------------------------------------------------------------------

#endif // dmImage_h
