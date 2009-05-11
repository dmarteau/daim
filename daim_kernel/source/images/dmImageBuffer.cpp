
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

#define dmUseKernelImageTemplates
#define dmUseKernelBuffer
#include "daim_kernel.h"
#include "common/dmUserLib.h"

using namespace daim;

//---------------------------------------------------
dmImageBuffer::dmImageBuffer()
: _Other(NULL)
{}
//---------------------------------------------------
dmImageBuffer::~dmImageBuffer()
{
  if(_Other)
    delete _Other;
}
//---------------------------------------------------
void dmImageBuffer::_AllocBuffer( size_t w, size_t h, const dmImageDescriptor& _desc)
{
    if( _buffer.IsNull() ||
        _buffer->PixelFormat()!=_desc.PixelFormat() ||
        _buffer->Width()<w   ||
        _buffer->Height()<h  )
    {
        _buffer = _desc.CreateImage( w, h );
    }
}
//---------------------------------------------------
void dmImageBuffer::CreateBuffer( const dmImage& _src,
                                  const dmRegion& _rgn,
                                  const dmMaskDescription& _mask,  bool _doCopy )
{
  CreateBuffer(_src,*_src.TypeDescriptor(),_rgn,_mask,_doCopy);
}
//---------------------------------------------------
void dmImageBuffer::CreateBuffer( const dmImage& _src,
                                  const dmRegion& _rgn, bool _doCopy )
{
  CreateBuffer(_src,*_src.TypeDescriptor(),_rgn,_doCopy);
}
//---------------------------------------------------
#define __SET_UP_REGION_MASK                          \
   dmRect  r = _mask.GetRect( _buffRgn.Rectangle() ); \
   _buffSrc = _buffRgn.Rectangle().TopLeft();         \
   _buffRgn.Translate(_mask.Ox()-_buffRgn.Rectangle().Left(),_mask.Oy()-_buffRgn.Rectangle().Top()); \
   _buffRect = dmRect( 0,0, r.Width(),r.Height() );
//---------------------------------------------------
void dmImageBuffer::CreateBuffer( const dmImage& _src, const dmImageDescriptor& _desc,
                                  const dmRegion& _rgn,
                                  const dmMaskDescription& _mask,  bool _doCopy )
{
    _buffRgn = _rgn;
    _buffRgn.ClipToRect(_src.Rect());
    if(!_buffRgn.IsEmptyRoi())
    {
      __SET_UP_REGION_MASK
      _AllocBuffer(r.Width(),r.Height(),_desc);

      if(_doCopy) {
        dmPoint offs = r.TopLeft();
        dmASSERT( dmClipRectangle(r,_src.Rect()) );
        _buffer->GetCopy( _src,r,r.TopLeft() - offs );
      }
    }
}
//---------------------------------------------------
void dmImageBuffer::CreateBuffer(const dmImageDescriptor& _desc,const dmRegion& _rgn,const dmMaskDescription& _mask)
{
    _buffRgn = _rgn;
    if(!_buffRgn.IsEmptyRoi())
    {
      __SET_UP_REGION_MASK
      _AllocBuffer(r.Width(),r.Height(),_desc);
    }
}
//---------------------------------------------------
#define __SET_UP_REGION \
   _buffSrc = _buffRgn.Rectangle().TopLeft();  \
   _buffRgn.Translate( - _buffRgn.Rectangle().Left(), - _buffRgn.Rectangle().Top() ); \
   _buffRect = dmRect(0,0,_buffRgn.Rectangle().Width(),_buffRgn.Rectangle().Height() );
//---------------------------------------------------
void dmImageBuffer::CreateBuffer(const dmImageDescriptor& _desc,const dmRegion& _rgn)
{
    _buffRgn = _rgn;
    if(!_buffRgn.IsEmptyRoi())
    {
       _AllocBuffer(_buffRgn.Rectangle().Width(),
                    _buffRgn.Rectangle().Height(),_desc);
       __SET_UP_REGION
     }
}
//---------------------------------------------------
void dmImageBuffer::CreateBuffer( const dmImage& _src, const dmImageDescriptor& _desc,
                                  const dmRegion& _rgn, bool _doCopy )
{
  _buffRgn = _rgn;
  _buffRgn.ClipToRect(_src.Rect());
  if(!_buffRgn.IsEmptyRoi())
  {
     _AllocBuffer(_buffRgn.Rectangle().Width(),_buffRgn.Rectangle().Height(),_desc);
     if(_doCopy)
       _buffer->GetCopy( _src,_buffRgn.Rectangle());

     __SET_UP_REGION
  }
}
//---------------------------------------------------
void dmImageBuffer::CreateBuffer( const dmImage& _src, bool _doCopy )
{
   CreateBuffer( _src, _src.Rect(), _doCopy );
}
//---------------------------------------------------
void dmImageBuffer::ReleaseBuffer()
{
  _buffRgn.KillRoi();
  _buffRect.Clear();
  _buffSrc.x = 0;
  _buffSrc.y = 0;

  _buffer.Release();
}
//---------------------------------------------------
void dmImageBuffer::CopyBuffer( const dmImageBuffer& _toCopy )
{
  _buffer   = _toCopy._buffer->CreateCopy();
  _buffRgn  = _toCopy._buffRgn;
  _buffRect = _toCopy._buffRect;
  _buffSrc  = _toCopy._buffSrc;

}
//---------------------------------------------------
void dmImageBuffer::BufferToImage( dmImage& _dest, const dmPoint& p )
{
  if(!IsEmpty())
    _dest.GetCopy(*_buffer,_buffRgn,p);
}
//---------------------------------------------------
void dmImageBuffer::BufferToImage( dmImage& _dest )
{
  BufferToImage( _dest, _buffSrc );
}
//---------------------------------------------------
void dmImageBuffer::Clear()
{
  if(!IsEmpty())
    _buffer->FillArea(_buffRect,false);
}
//---------------------------------------------------
dmLink<dmImage> dmImageBuffer::CloneBuffer() const
{
  if(!IsEmpty())
   return _buffer->CreateCopy(_buffRgn.Rectangle());
  else
   return dmLink<dmImage>();
}
//---------------------------------------------------
dmImageBuffer* dmImageBuffer::GetNewBuffer()
{
  if(_Other==NULL)
    _Other = new dmImageBuffer;

  return _Other;
}
//---------------------------------------------------
dmRect dmImageBuffer::SrcRect() const
{
  return dmRect(_buffSrc,_buffRect.Width(),_buffRect.Height());
}
//---------------------------------------------------
void dmImageBuffer::SetBufferLimit( const dmRect& rect,
                                    dmEBndryLimitType _LimitType )
{
  if(IsEmpty()) return;

  dmRect br       = BufferRect();
  const dmRect& r = BufferRgn().Rectangle();

  // Set the buffer rectangle in image coordinate system
  br.Translate( BufferSrc() - r.TopLeft() );

  // If the rectangle is totally contained in image rect, no need
  // to process further
  if( rect >= br ) return;

  if(_LimitType==dmPERIODIC_BOUNDARY)
  {
    dmClipRectangle(br,rect);                     // Extract the area corresponding to image
    br.Translate( r.TopLeft() - BufferSrc() );    // Set back rectangle into buffer coordinate system
    dmMakePeriodic( *Buffer(), br,BufferRect() ); // Make borders periodic
  }
  else
  {
    dmRegion rgn(br);
    rgn.SubCoordinates(rect);
    if(!rgn.IsEmptyRoi())
    {
      rgn.Translate(r.TopLeft() - BufferSrc());
      Buffer()->FillArea(rgn,_LimitType==dmOBJECT_BOUNDARY);
    }
  }
}
//----------------------------------------------------
