#define dmUseCCI
#define dmUseKernelImageTemplates
#define dmUseUtilitiesExtra
#define dmUseKernelBuffer
#include "daim_kernel.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_Parameters.h"
#include "daim_modules/CCI_ImageList.h"

#include "daim_modules/processing/CCI_Colorspaces.h"

//------------------------------------------------------------------
#define CCIC_NAME    COLORSPACE_CLSID
#define CCI_VERSION  COLORSPACE_VERSION
#define CCI_REVISION COLORSPACE_REVISION
#define AUTHOR       COLORSPACE_AUTHOR

#define CCIC_SUPER         CCIC_ColorSpace
#define CCIC_SUPER_VERSION CCI_ColorSpace_Version
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))
//--------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"

#include "daim/daim_base.h"
#include "daim/daim_interfaces.h"

#include "common/dmUserLib.h"
#include "images/dmChannels.h"

#define RGB_FMT dmPixelFormat24bppRGB

#define CHANNEL_8BITS dmPixelFormat8bppIndexed
#define CHANNEL_FLOAT dmPixelFormat32bppFloat

#define _NOINDEX_ dm_noindex
//--------------------------------------------------------------------
CCI_DECL_DATA( ColorSpace )
{
  dm_uint      ChannelFmt;
  dmIImageList ImageList;
  dmRect       Rect;
};
//--------------------------------------------------------------------
// ColorSpace_InitChannels
//--------------------------------------------------------------------
bool ColorSpace_InitChannels( CCI_INSTANCE_PTR(ColorSpace) _This,
                              dm_uint _Channels,
                              bool    _Split )
{
  dmDEBUG_ASSERT( _Channels > 0 )
  if(_Split) 
  {
    _This->ImageList.SetCapacity(_Channels);
    for(dm_uint i=0;i<_Channels;++i) {
      if(!_This->ImageList.CreateBuffer(i,_This->ChannelFmt,_This->Rect.Width(),
                                                            _This->Rect.Height()))
        return false;
    }
    return true;
  }
  else {
    // Convert formats to channels f
    return _This->ImageList.Size() >= _Channels;
  }
}

#undef RGB // this macros is really stupid !!

//--------------------------------------------------------------------
// ColorSpace_Extract
//--------------------------------------------------------------------
template<EPixelFormat _PixelFormat>
dm_param inline ColorSpace_Extract( CCI_INSTANCE_PTR(ColorSpace) _This,
                                    dm_uint channel, const dmIImage<RGB_FMT>* _src, 
                                    dmIImage<_PixelFormat>* _dst, const dmRegion& rgn )
{
  typedef typename dmIImage<_PixelFormat>::traits_type::integer_type integer_type ;

  dmDEBUG_ASSERT( _src != NULL );
  dmDEBUG_ASSERT( _dst != NULL );

  switch(channel)
  {
    COLORSPACE_EXTRACT_CHANNELS()
  } 
  
  CCI_RETURN_FAIL()
}
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( ColorSpace )
{
  CCI_GENERIC_CONSTRUCT(ColorSpace)
  _This->ImageList.QueryInterface(_THIS_OBJECT);
  _This->ChannelFmt = CHANNEL_8BITS;

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( ColorSpace )
{
  CCI_GENERIC_DESTROY(ColorSpace)
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( ColorSpace )
  CCI_ATTR_GET( ColorSpace,ChannelFmt , CCIA_RETVAL = _This->ChannelFmt )
  CCI_ATTR_GET( ColorSpace,NumChannels, CCIA_RETVAL = COLORSPACE_NUMCHANNELS )
  CCI_ATTR_GET( ColorSpace,CanMerge   , CCIA_RETVAL = COLORSPACE_CAN_MERGE   )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( ColorSpace )
  CCI_ATTR_SET( ColorSpace,ChannelFmt , _This->ChannelFmt = CCIA_DATA )
  CCIA_DISCARD( ColorSpace,NumChannels ) 
  CCIA_DISCARD( ColorSpace,CanMerge    )
CCI_END_SETTER()
//---------------------------------------------------------------------
// Method: Split
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ColorSpace, Split )
{
  CCI_INSTANCE( ColorSpace )

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  { 
    dmRegion* rgn   = _IParams.GetRoi();
    dmImage*  image = _IParams.GetImage();

    if(image==NULL) CCI_RETURN_FAIL()
    dmASSERT( rgn != NULL );

    _This->Rect = rgn->Rectangle();

    dmIImageList& _ImageList = _This->ImageList;

    dmIImage<RGB_FMT>* src = dmIImage<RGB_FMT>::Cast(image);
    if(src!=NULL && ColorSpace_InitChannels(_This,COLORSPACE_NUMCHANNELS,true)) {
      if( COLORSPACE_SPLIT(*src,_This->Rect,dmPoint(0,0)) )
        CCI_RETURN_OK()
    }

  } else 
    CCI_SET_ERROR_MSG(_TXT("Invalid parameters !"));

  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method: Merge
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ColorSpace, Merge )
{
#ifndef COLORSPACE_NO_MERGE_SUPPORT

  CCI_INSTANCE( ColorSpace )

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  { 
    dmImage*  image = _IParams.GetImage();

    if(image==NULL) CCI_RETURN_FAIL()

    // Rect is supposed to be the rectangle computed in the split function
    // if empty get the offset from the roi

    dmRect  _Rect = _This->Rect;
    dmPoint _Offset;

    if(_Rect.IsEmpty())
       _Rect = image->Rect();

    _Offset = _Rect.TopLeft();
    _Rect.Translate(-_Offset);

    dmIImageList& _ImageList = _This->ImageList;

    dmIImage<RGB_FMT>* src = dmIImage<RGB_FMT>::Cast(image);
    if(src!=NULL && ColorSpace_InitChannels(_This,COLORSPACE_NUMCHANNELS,false)) {
       if( COLORSPACE_MERGE(*src,_Rect,_Offset) )
           CCI_RETURN_OK()
    }

  } else 
    CCI_SET_ERROR_MSG(_TXT("Invalid parameters !"));

#else // COLORSPACE_NO_MERGE_SUPPORT_DEFINED

   CCI_RETURN_FAIL()

#endif

  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method: Extract
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ColorSpace, Extract )
{
  CCI_INSTANCE( ColorSpace )

  if(CCI_P(channel) < 1 || CCI_P(channel) > COLORSPACE_NUMCHANNELS)
    CCI_RETURN_INVALID_PARAM()

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {   
    dmIImage<RGB_FMT>* _src = dmIImage<RGB_FMT>::Cast(_IParams.GetImage());
    if(_src==NULL) CCI_RETURN_FAIL()

    dmRegion* rgn  = _IParams.GetRoi();
    dmImage*  _Dst = NULL;

    dmASSERT( rgn != NULL );

    if(CCI_P(index)!=_NOINDEX_) 
    {
      dmIImageList _ImageList;
      if(_ImageList.QueryInterface(_IParams.GetImageList()) &&
        _ImageList.CreateBuffer(CCI_P(index),_This->ChannelFmt,
          rgn->Rectangle().Width(),rgn->Rectangle().Height())) {

        _Dst =  _ImageList.GetBuffer(CCI_P(index));
      }
    }
    else {
      dmImageDescriptor* _Desc = dmGetDescriptor(static_cast<EPixelFormat>(_This->ChannelFmt));
      if(_Desc!=NULL) {
         dmImageBuffer* _buffer = _IParams.GetImageBuffer();
         dmASSERT( _buffer != NULL );
 
        _buffer->CreateBuffer(*_Desc,*rgn);
        _Dst = _buffer->Buffer();
      }
    }
    
    if(_Dst!=NULL) {
      switch( _This->ChannelFmt )
      {
        case CHANNEL_8BITS: {
          dmIImage<CHANNEL_8BITS>* _dst = dmIImage<CHANNEL_8BITS>::Cast(_Dst);
          return ColorSpace_Extract( _This, CCI_P(channel), _src, _dst, *rgn );
        } break;

        case CHANNEL_FLOAT: {
          dmIImage<CHANNEL_FLOAT>* _dst = dmIImage<CHANNEL_FLOAT>::Cast(_Dst); 
          return ColorSpace_Extract( _This, CCI_P(channel), _src, _dst, *rgn );
        } break;
      }
    }
    
    CCI_RETURN_FAIL()
  }
  CCI_RETURN_INVALID_PARAM()

};
//---------------------------------------------------------------------
// Method: SetRect
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ColorSpace, SetRect )
{
  CCI_INSTANCE( ColorSpace )

  if(CCI_P(rect)!=NULL)
  {
    _This->Rect = *CCI_P(rect);  
  } else
    _This->Rect.Clear();

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: SplitColor
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ColorSpace, SplitColor )
{
  CCI_UNUSUED_INSTANCE( ColorSpace )

  daim::colorspace::rgb_triple rgb;
  dm_uint8 c1,c2,c3;

  rgb.red   = static_cast<dm_uint8>(CCI_P(q1));
  rgb.green = static_cast<dm_uint8>(CCI_P(q2));
  rgb.blue  = static_cast<dm_uint8>(CCI_P(q3));

  COLORSPACE_NAMESPACE::splitter _Splitter;
  _Splitter( rgb, c1, c2, c3 );
 
  CCI_P(q1) = c1;
  CCI_P(q2) = c2;
  CCI_P(q3) = c3;


  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: MergeColor
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ColorSpace, MergeColor )
{
  CCI_UNUSUED_INSTANCE( ColorSpace )

#ifndef COLORSPACE_NO_MERGE_SUPPORT

  daim::colorspace::rgb_triple rgb;
  dm_uint8 c1,c2,c3;

  c1 = static_cast<dm_uint8>(CCI_P(q1));
  c2 = static_cast<dm_uint8>(CCI_P(q2));
  c3 = static_cast<dm_uint8>(CCI_P(q3));

  COLORSPACE_NAMESPACE::merger _Merger;
  _Merger( rgb, c1, c2, c3 );

  CCI_P(q1) = rgb.red  ;
  CCI_P(q2) = rgb.green;
  CCI_P(q3) = rgb.blue ;

  CCI_RETURN_OK()

#else // COLORSPACE_NO_MERGE_SUPPORT_DEFINED

   CCI_RETURN_FAIL()

#endif

}
//---------------------------------------------------------------------
// Method: SplitImage
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ColorSpace, SplitImage )
{
  CCI_INSTANCE( ColorSpace )

  const dmImage*  image = CCI_P(image);

  if(image==NULL) 
     CCI_RETURN_FAIL()

  _This->Rect = image->Rect();

  dmIImageList& _ImageList = _This->ImageList;
  dmIImage<RGB_FMT>* src = dmIImage<RGB_FMT>::Cast(image);
  if(src!=NULL && ColorSpace_InitChannels(_This,COLORSPACE_NUMCHANNELS,true)) {
    if( COLORSPACE_SPLIT(*src,_This->Rect,dmPoint(0,0)) )
      CCI_RETURN_OK()
  }

  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( ColorSpace )
  CCI_REGISTER_METHOD_PP( ColorSpace, Split      )
  CCI_REGISTER_METHOD_PP( ColorSpace, Merge      )
  CCI_REGISTER_METHOD_PP( ColorSpace, Extract    )
  CCI_REGISTER_METHOD_PP( ColorSpace, SetRect    )
  CCI_REGISTER_METHOD_PP( ColorSpace, MergeColor )
  CCI_REGISTER_METHOD_PP( ColorSpace, SplitColor )
  CCI_REGISTER_METHOD_PP( ColorSpace, SplitImage )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( ColorSpace )
{
  CCI_INVOKE_FACTORY(ColorSpace)
}
//---------------------------------------------------------------------
