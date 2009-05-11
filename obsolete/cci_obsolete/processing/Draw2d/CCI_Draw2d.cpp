
/* :::BEGIN LICENSE BLOCK:::
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
 *  :::END LICENSE BLOCK::: */

#define dmUseCCI
#define dmUseKernelImageTemplates
#define dmUseKernelBuffer
#include "daim_kernel.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_ImageList.h"
#include "daim_modules/CCI_MaskBuffer.h"
#include "daim_modules/CCI_Parameters.h"

#include "daim_modules/processing/CCI_BinaryMath.h"
#include "daim_modules/processing/CCI_Draw2d.h"

//------------------------------------------------------------------------
#define CCIC_NAME  CCIC_Draw2d
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_Draw2d_Version
#define CCI_REVISION  0
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2006"))
//--------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"

//--------------------------------------------------------------------
CCI_DECL_DATA( Draw2d )
{
  dmIParameters IParams;
  dmIBinaryMath IMath;
  
  dmRegion   region;
  dmRegion   roi;
  
  dmLink<dmImage> image;
  
  dm_uint       pensize;
  dm_uint       pattern;
  dm_real       opacity;

  dm_uint       drMode;
  
  dm_uint16     c1,c2,c3;
  dm_real       grey;
  
  dm_bool       pathMode;
  
};

typedef CCI_INSTANCE_PTR(Draw2d) _Instance_Ptr;

#define H8_KERNEL _TXT("Morpho8c/H(8)")

//--------------------------------------------------------------------
static dm_param Draw2d_DrawRegion( _Instance_Ptr _This )
{
  if(_This->image.IsNull())
     CCI_RETURN_FAIL()
  
  _This->IParams.SetRoi(&_This->region);
    
  if(_This->pattern != 0 || (_This->drMode & CCIV_Draw2d_DRAW_OUTLINE)) // Draw border of region
  {
    _This->IParams.RoiToMask();
    _This->IParams.KillRoi();
    
    if(_This->drMode & CCIV_Draw2d_DRAW_OUTLINE) 
    {
       _This->IParams.StoreMask(0,0);
       _This->IMath.ApplyKernel(_This->IParams,H8_KERNEL,dmTk::Math::Dilation,
                                _This->pensize,0,-1);
     
       _This->IParams.CallMask(0,dmTk::Math::SubPixels);
    } 
    else 
    {
      _This->IMath.ApplyKernel(_This->IParams,H8_KERNEL,dmTk::Math::Thinning,1,0,-1);   
      if(_This->pensize > 1)
         _This->IMath.ApplyKernel(_This->IParams,H8_KERNEL,dmTk::Math::Dilation,
                                  _This->pensize,0,-1);
    }
    _This->IParams.MaskToRoi();
  } 
  
  dmRegion* pRegion = _This->IParams.GetRoi();
  pRegion->AndRoi(_This->roi);

  bool result = dmIsPixelFormatScalar(_This->image->PixelFormat())
          ? _This->IParams.FillChannel(_This->grey,_This->opacity)
          : _This->IParams.FillRGB(_This->c1,_This->c2,_This->c3,_This->opacity);
          
  return (result ? CCI_ERR_OK : CCI_ERR_METHOD_FAILED);
}  
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( Draw2d )
{
  CCI_GENERIC_CONSTRUCT(Draw2d)

  if(!_This->IParams.Create())
     CCI_RETURN_FAILED_TO_CONSTRUCT()  
    
  if(!_This->IMath.Create())
     CCI_RETURN_FAILED_TO_CONSTRUCT()  

  _This->c1 = 255;
  _This->c2 = 255;
  _This->c3 = 255;
  
  _This->grey = 255.0;

  _This->pattern = 0; // Fill
  _This->pensize = 1;
  _This->opacity = 1.0;
  _This->drMode  = 0;
  
  _This->pathMode = dm_false;
     
  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( Draw2d )
{
  CCI_INSTANCE(Draw2d)
  
  _This->IMath.Release();
  _This->IParams.Release();

  CCI_DESTROY_DATA()
  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( Draw2d )
   CCI_ATTR_GET( Draw2d, Pattern , CCIA_RETVAL = _This->pattern )
   CCI_ATTR_GET( Draw2d, PenSize , CCIA_RETVAL = _This->pensize )
   CCI_ATTR_GET( Draw2d, DrawMode, CCIA_RETVAL = _This->drMode  )
   CCI_ATTR_GET( Draw2d, Width   , CCIA_RETVAL = (_This->image.IsNull()?0:_This->image->Width())   )
   CCI_ATTR_GET( Draw2d, Height  , CCIA_RETVAL = (_This->image.IsNull()?0:_This->image->Height())  )
   CCI_ATTR_GET( Draw2d, Format  , CCIA_RETVAL = (_This->image.IsNull()?0:_This->image->PixelFormat()))
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( Draw2d )
   CCI_ATTR_SET( Draw2d, Pattern , _This->pattern = CCIA_DATA )
   CCI_ATTR_SET( Draw2d, PenSize , _This->pensize = CCIA_DATA )
   CCIA_DISCARD( Draw2d, Width  )
   CCIA_DISCARD( Draw2d, Height )
   CCIA_DISCARD( Draw2d, Format )
CCI_END_SETTER()
/*
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( Draw2d )
{
  CCI_UNUSUED_INSTANCE(Draw2d)
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( Draw2d )
{
  CCI_UNUSUED_INSTANCE(Draw2d)
  CCI_RETURN_INVOKE_SUPER()
}
*/
//---------------------------------------------------------------------
// Method : Initialize
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Draw2d , Initialize )
{
  CCI_INSTANCE(Draw2d)
  
  dmIParameters aParam;
  if(aParam.QueryInterface(CCI_P(params)))
  {    
    dmLink<dmImage> _anImage;
    
    // Share image
    if(aParam.GetImageA(_anImage) && 
       _This->IParams.SetImageA(_anImage))
    {
      _This->image = _anImage;
      _This->roi   = *aParam.GetRoi(); // Get the roi
      CCI_RETURN_OK()
    } 
  }
  
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method : SetColor
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Draw2d , SetColor )
{
  CCI_INSTANCE(Draw2d)
  
  _This->c1 = static_cast<dm_uint16>(CCI_P(c1));
  _This->c2 = static_cast<dm_uint16>(CCI_P(c2));
  _This->c3 = static_cast<dm_uint16>(CCI_P(c3));

  _This->grey = (CCI_P(c1)+CCI_P(c2)+CCI_P(c3))/3.0;

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method : DrawMask
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Draw2d , DrawMask )
{
  CCI_INSTANCE(Draw2d)

  dmIParameters aParam;
  if(aParam.QueryInterface(CCI_P(params)))
  {    
    dmRegion* mask = aParam.GetMask();
    if(mask) 
    {
      if(_This->pathMode) {
         _This->region.AddRoi(*mask);
         CCI_RETURN_OK()
      } else {     
        _This->region = *mask;
        return Draw2d_DrawRegion(_This);
      }
    }
  }  
  
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method : DrawLine
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Draw2d , DrawLine )
{
  CCI_INSTANCE(Draw2d)

  dmLine aLine(CCI_P(x0),CCI_P(y0),CCI_P(x1),CCI_P(y1));

  if(_This->pathMode) {
     _This->region.AddRoi(aLine);
     CCI_RETURN_OK()
  } else {     
    _This->region.SetRoi(aLine);
    return Draw2d_DrawRegion(_This);
  }
}
//---------------------------------------------------------------------
// Method : DrawRectangle
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Draw2d , DrawRectangle )
{
  CCI_INSTANCE(Draw2d)
  
  dmRect aRect(dmPoint(CCI_P(x0),CCI_P(y0)),
               dmPoint(CCI_P(x1),CCI_P(y1)));
  
  if(_This->pathMode) {
     _This->region.AddCoordinates(aRect);
     CCI_RETURN_OK()
  } else {       
    _This->region.SetRoi(aRect);
    return Draw2d_DrawRegion(_This);
  }
}
//---------------------------------------------------------------------
// Method : DrawEllipse
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Draw2d , DrawEllipse )
{
  CCI_INSTANCE(Draw2d)
  
  dm_uint rh = static_cast<dm_uint>(CCI_P(r1));
  dm_uint rv = static_cast<dm_uint>(CCI_P(r2));

  dmEllipse aEllipse(CCI_P(x0),CCI_P(y0),rh,rv);
  
  if(_This->pathMode) {
     _This->region.AddRoi(aEllipse);
     CCI_RETURN_OK()
  } else {
    _This->region.SetRoi(aEllipse);
    return Draw2d_DrawRegion(_This);
  }
}
//---------------------------------------------------------------------
// Method : DrawCircle
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Draw2d , DrawCircle )
{
  CCI_INSTANCE(Draw2d)
  
  dm_uint radius = static_cast<dm_uint>(CCI_P(r0));
  
  dmCircle aCircle(CCI_P(x0),CCI_P(y0),radius);

  if(_This->pathMode) {
     _This->region.AddRoi(aCircle);
     CCI_RETURN_OK()
  } else {
    _This->region.SetRoi(aCircle);
    return Draw2d_DrawRegion(_This);
  }
}
//---------------------------------------------------------------------
// Method : DrawPoly
//---------------------------------------------------------------------

//////////////////////////////////////////////////
static inline CCI_IMPL_METHOD( Draw2d , DrawPoly )
{
  CCI_INSTANCE(Draw2d)
  
  dm_point* points = CCI_P(points);
  dm_uint   count  = CCI_P(count);

  if(count < 2 || points == NULL)
     CCI_RETURN_FAIL()
  
  dmRegion& rgn = _This->region;
  rgn.KillRoi();

  --count;

  for(dm_uint i=0;i<count;++i) {
    rgn.AddRoi(dmLine(points[i],points[i+1]));
  }

  if(_This->pathMode) {
     CCI_RETURN_OK()
  } else {
    return Draw2d_DrawRegion(_This);
  }
}
//---------------------------------------------------------------------
// Method : Opacity
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Draw2d , Opacity )
{
  CCI_INSTANCE(Draw2d)
  switch(CCI_P(me)) {
    case CCI_ME_SET: _This->opacity =  CCI_P(opacity); CCI_RETURN_OK();
    case CCI_ME_GET: CCI_P(opacity) = _This->opacity ; CCI_RETURN_OK();
  }
  
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method : InitPath
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Draw2d , InitPath )
{
  CCI_INSTANCE(Draw2d)

  if(!_This->pathMode) {
    _This->region.KillRoi();
    _This->pathMode = dm_true;
    CCI_RETURN_OK()
  }
  
  CCI_RETURN_FAIL() 
}
//---------------------------------------------------------------------
// Method : ClosePath
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( Draw2d , ClosePath )
{
  CCI_INSTANCE(Draw2d)
  
  if(_This->pathMode) {
    _This->pathMode = dm_false;
    return Draw2d_DrawRegion(_This);
  }
  
  CCI_RETURN_FAIL() 
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( Draw2d )
  CCI_REGISTER_METHOD_PP( Draw2d , Initialize   )
  CCI_REGISTER_METHOD_PP( Draw2d , SetColor     ) 
  CCI_REGISTER_METHOD_PP( Draw2d , DrawMask     ) 
  CCI_REGISTER_METHOD_PP( Draw2d , DrawLine     )
  CCI_REGISTER_METHOD_PP( Draw2d , DrawRectangle) 
  CCI_REGISTER_METHOD_PP( Draw2d , DrawEllipse  ) 
  CCI_REGISTER_METHOD_PP( Draw2d , DrawCircle   ) 
  CCI_REGISTER_METHOD_PP( Draw2d , DrawPoly     ) 
  CCI_REGISTER_METHOD_PP( Draw2d , Opacity      ) 
  CCI_REGISTER_METHOD_PP( Draw2d , InitPath     ) 
  CCI_REGISTER_METHOD_NP( Draw2d , ClosePath    ) 
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( Draw2d )
{   
  CCI_INVOKE_FACTORY(Draw2d)
}
//---------------------------------------------------------------------
