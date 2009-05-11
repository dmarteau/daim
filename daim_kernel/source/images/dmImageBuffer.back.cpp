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
        _buffer->TypeDescriptor()!=_desc || 
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
  CreateBuffer(_src,_src.TypeDescriptor(),_rgn,_mask,_doCopy); 
}
//---------------------------------------------------
void dmImageBuffer::CreateBuffer( const dmImage& _src,
                                  const dmRegion& _rgn, bool _doCopy )
{
  CreateBuffer(_src,_src.TypeDescriptor(),_rgn,_doCopy); 
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
// Create a HSI or RGB mapping image
//---------------------------------------------------
dmLink<dmImage> dmImageBuffer::CreateMap( dmImage& _src, dmELutIndex _buffMapIndex )
{
  if(IsEmpty()) 
    return dmLink<dmImage>();

  // Check for rgb image;
  dmIImage<dmPixelFormat24bppRGB>* rgbimg = dmIImage<dmPixelFormat24bppRGB>::Cast(&_src);

  if(rgbimg) 
  {    
    dmRect r = SrcRect();
    r.Translate( _buffRect.TopLeft() - _buffRgn.Rectangle().TopLeft() );
    dmClipRectangle(r,_src.Rect());

    if(_bufferEx.IsNull()) _bufferEx = _buffer->CreateCopy();  // save originale buffered image
    dmLink<dmImage> mapimg = dmCreateImage<dmPixelFormat8bppIndexed>(_src.Width(),_src.Height());

    if(_buffMapIndex>dmBLUE_DATA && _buffMapIndex<dmNUM_LUT_DATA)
      transform(r,rgbimg->Gen(),rgb_2_hsi()); // transform image to hsi
    
    dmIImage<dmPixelFormat8bppIndexed> *nbimg = dmIImage<dmPixelFormat8bppIndexed>::Cast(mapimg);
    switch(_buffMapIndex) {
      case dmRED_DATA   : transform(r,r.TopLeft(),rgbimg->Gen(),nbimg->Gen(),rgb_red())  ; break;
      case dmGREEN_DATA : transform(r,r.TopLeft(),rgbimg->Gen(),nbimg->Gen(),rgb_green()); break;
      case dmBLUE_DATA  : transform(r,r.TopLeft(),rgbimg->Gen(),nbimg->Gen(),rgb_blue()) ; break;
      case dmHUE_DATA   : transform(r,r.TopLeft(),rgbimg->Gen(),nbimg->Gen(),hsi_hue())  ; break;
      case dmSAT_DATA   : transform(r,r.TopLeft(),rgbimg->Gen(),nbimg->Gen(),hsi_sat())  ; break;
      case dmVAL_DATA   : transform(r,r.TopLeft(),rgbimg->Gen(),nbimg->Gen(),hsi_val())  ; break;
    }
 
    dmPoint offs = _buffSrc - r.TopLeft(); 
    _AllocBuffer(_buffRect.Width(),_buffRect.Height(),mapimg->TypeDescriptor());                       
    _buffer->GetCopy(*mapimg,r,_buffRgn.Rectangle().TopLeft() - offs ); // save map into buffer
    return mapimg;                                                      // return new mapped image
  } 
  return dmNewLink<dmImage>(&_src);  
}
//---------------------------------------------------
void dmImageBuffer::RestoreMap( dmImage& _map, dmImage& _src, dmELutIndex _buffMapIndex )
{ 
  if(IsEmpty()) return;
  if(!_bufferEx.IsNull()) {
    _buffer = _bufferEx; // Restore originale buffer
    _bufferEx.release(); // Clear copy buffer
  }
 
  dmIImage<dmPixelFormat8bppIndexed> *mapimg = dmIImage<dmPixelFormat8bppIndexed>::Cast(&_map);
  dmIImage<dmPixelFormat24bppRGB>    *rgbimg = dmIImage<dmPixelFormat24bppRGB>::Cast(&_src);

  if( mapimg && rgbimg ) 
  {
    dmRect r = SrcRect();
    r.Translate( _buffRect.TopLeft() - _buffRgn.Rectangle().TopLeft() );
    dmClipRectangle(r,_src.Rect());
    
    dmIImage<dmPixelFormat8bppIndexed> *nbimg = dmIImage<dmPixelFormat8bppIndexed>::Cast(mapimg);
    switch(_buffMapIndex) {
      case dmRED_DATA   : combine(r,r.TopLeft(),mapimg->Gen(),rgbimg->Gen(),red_rgb())  ; break;
      case dmGREEN_DATA : combine(r,r.TopLeft(),mapimg->Gen(),rgbimg->Gen(),green_rgb()); break;
      case dmBLUE_DATA  : combine(r,r.TopLeft(),mapimg->Gen(),rgbimg->Gen(),blue_rgb()) ; break;
      case dmHUE_DATA   : combine(r,r.TopLeft(),mapimg->Gen(),rgbimg->Gen(),hue_hsi())  ; break;
      case dmSAT_DATA   : combine(r,r.TopLeft(),mapimg->Gen(),rgbimg->Gen(),sat_hsi())  ; break;
      case dmVAL_DATA   : combine(r,r.TopLeft(),mapimg->Gen(),rgbimg->Gen(),val_hsi())  ; break;
    }

    if(_buffMapIndex>dmBLUE_DATA && _buffMapIndex<dmNUM_LUT_DATA)
      transform(r,rgbimg->Gen(),hsi_2_rgb()); // transform src back to to rgb

  }
}
//---------------------------------------------------
void dmImageBuffer::SetBufferLimit( const dmRect& rect,
                                    dmEBndryLimitType _LimitType )
{
  if(IsEmpty()) return;

  dmRect br       = BufferRect();
  const dmRect& r = BufferRgn().Rectangle();

  // Remet le rectangle du buffer dans le referentiel de l'image
  br.Translate( BufferSrc() - r.TopLeft() );     

  // si le rectangle est entièrement contenue dans l'image
  // pas la peine de continuer
  if( rect >= br ) return; 

  if(_LimitType==dmPERIODIC_BOUNDARY) 
  {
    dmClipRectangle(br,rect);                  // On recupere la partie correspondant à l'image
    br.Translate( r.TopLeft() - BufferSrc() ); // Remet le rectangle dans le referentiel du buffer
    dmMakePeriodic( *Buffer(), br,BufferRect() );
  } else {
    dmRegion rgn(br);
    rgn.SubCoordinates(rect);
    if(!rgn.IsEmptyRoi()) 
    {  
      rgn.Translate(r.TopLeft() - BufferSrc());
      switch(_LimitType) 
      {
        case dmZERO_BOUNDARY    : Buffer()->FillArea(rgn,false); break;
        case dmOBJECT_BOUNDARY  : Buffer()->FillArea(rgn,true) ; break;
      }
    }
  }
}
//----------------------------------------------------
