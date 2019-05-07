#ifndef dmImageBuffer_h
#define dmImageBuffer_h

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
// File         : dmImageBuffer.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------
#include "templates/processing/dmKernelFamily.h"

typedef enum {
  dmZERO_BOUNDARY     = 0,
  dmOBJECT_BOUNDARY   = 1,
  dmPERIODIC_BOUNDARY = 2,
} dmEBndryLimitType;

//---------------------------------------------------------------------------
class __dmKernel dmImageBuffer : public dmReferenced
{
   private:
    dmLink<dmImage> _buffer;      // Buffer for operations;
    dmRegion        _buffRgn;     // Roi;
    dmRect          _buffRect;    // Actual buffer rectangle
    dmPoint         _buffSrc;

    dmImageBuffer* _Other;       // Enable the creation of another buffer

    void _AllocBuffer( size_t w, size_t h, const dmImageDescriptor& );

   public:
    void CreateBuffer( const dmImageDescriptor&,const dmRegion&,const dmMaskDescription&);
    void CreateBuffer( const dmImageDescriptor&,const dmRegion&);
    void CreateBuffer( const dmImage& _src,const dmImageDescriptor&,const dmRegion&,const dmMaskDescription&,bool _doCopy);
    void CreateBuffer( const dmImage& _src,const dmImageDescriptor&,const dmRegion&,bool _doCopy=true);
    void CreateBuffer( const dmImage& _src,const dmRegion&,const dmMaskDescription&,bool _doCopy=true);
    void CreateBuffer( const dmImage& _src,const dmRegion&,bool _doCopy=true);
    void CreateBuffer( const dmImage& _src,bool _doCopy=true);

    void CreateBuffer( const dmImage& _src,const dmImageDescriptor& _desc,bool _doCopy=true) {
       CreateBuffer(_src,_desc,_src.Rect(),_doCopy);
    }

    void ReleaseBuffer();
    void Clear();
    bool IsEmpty() const { return _buffer.IsNull(); }

    const dmRegion& BufferRgn()   const { return _buffRgn; }
    const dmRect&   BufferRect()  const { return _buffRect;}
    const dmPoint&  BufferSrc()   const { return _buffSrc; }
    dmLink<dmImage> Buffer()      const { return _buffer;  }

    dmLink<dmImage>   CloneBuffer() const;

    dmRect SrcRect() const;

    void BufferToImage( dmImage& , const dmPoint& );
    void BufferToImage( dmImage&  );

    void CopyBuffer( const dmImageBuffer& );
    void SetBufferLimit( const dmRect&,dmEBndryLimitType );

    // Enable the creation of another buffer
    // The buffer is held by its parent for reusing.
    // IMPORTANT: Never delete a buffer if you don't know how
    // it has been created. (may be should add a Detach() method ?)
    dmImageBuffer* GetNewBuffer();

    dmImageBuffer();
    virtual  ~dmImageBuffer();
    
    DM_DECL_ARENA_OPERATOR_NEW(dmImageBuffer)
};
//---------------------------------------------------------------------------
class dmBufferParameters
{
 public:
   dmImageBuffer&   thisBuffer;
   dmImage&         thisImage;
   const dmRegion&  thisRegion;
   dmLink<dmImage>  thisMap;

   dmBufferParameters(
     dmImageBuffer&   _Buffer,
     dmImage&         _Image ,
     const dmRegion&  _Region
   )
   :thisBuffer  (_Buffer)
   ,thisImage   (_Image )
   ,thisRegion  (_Region)
   {}

  ~dmBufferParameters() {}

   void CreateBuffer( const dmImageDescriptor& _Descriptor,const dmMaskDescription& _Mask);
   void CreateBuffer( const dmImageDescriptor& _Descriptor);
   void CreateBuffer( const dmImageDescriptor& _Descriptor,const dmMaskDescription& _Mask,bool _doCopy);
   void CreateBuffer( const dmImageDescriptor& _Descriptor,bool _doCopy);
   void CreateBuffer( const dmMaskDescription& _Descriptor,bool _doCopy=true);
   void CreateBuffer( bool _doCopy=true );
};
//---------------------------------------------------------------------------
inline void dmBufferParameters::CreateBuffer( const dmImageDescriptor& _Descriptor,
                                              const dmMaskDescription& _Mask)
{
  thisBuffer.CreateBuffer( _Descriptor,thisRegion,_Mask);
}
//---------------------------------------------------------------------------
inline void dmBufferParameters::CreateBuffer( const dmImageDescriptor& _Descriptor )
{
  thisBuffer.CreateBuffer( _Descriptor,thisRegion );
}
//---------------------------------------------------------------------------
inline void dmBufferParameters::CreateBuffer( const dmImageDescriptor& _Descriptor,
                                              const dmMaskDescription& _Mask,bool _doCopy )
{
  thisBuffer.CreateBuffer( thisImage,_Descriptor,thisRegion,_Mask,_doCopy );
}
//---------------------------------------------------------------------------
inline void dmBufferParameters::CreateBuffer( const dmImageDescriptor& _Descriptor,bool _doCopy)
{
  thisBuffer.CreateBuffer( thisImage,_Descriptor,thisRegion,_doCopy);
}
//---------------------------------------------------------------------------
inline void dmBufferParameters::CreateBuffer( const dmMaskDescription& _Mask,bool _doCopy )
{
  thisBuffer.CreateBuffer( thisImage,thisRegion,_Mask,_doCopy);
}
//---------------------------------------------------------------------------
inline void dmBufferParameters::CreateBuffer( bool _doCopy )
{
  thisBuffer.CreateBuffer( thisImage,thisRegion,_doCopy);
}
//---------------------------------------------------------------------------
#endif // dmImageBuffer_h
