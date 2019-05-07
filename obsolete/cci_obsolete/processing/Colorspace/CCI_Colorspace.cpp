
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

/* $ChangeLog  v1.1, 27 août 2006 20:02:25 , David
 *
 * Added method ToColorSpace()
 */

#define dmUseCCI
#define dmUseKernelImageTemplates
#define dmUseUtilitiesExtra
#define dmUseKernelBuffer
#include "daim_kernel.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_Parameters.h"
#include "daim_modules/CCI_ImageList.h"

#include "daim_modules/processing/CCI_ColorMap.h"
#include "daim_modules/processing/CCI_Colorspace.h"

//------------------------------------------------------------------
#define CCIC_NAME CCIC_ColorSpace
#define CCIC_SUPER CCIC_ImageList
#define CCIC_SUPER_VERSION CCI_ImageList_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_ColorSpace_Version
#define CCI_REVISION  1
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))
//--------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"

#include "daim/daim_base.h"
#include "daim/daim_interfaces.h"

#include "common/dmUserLib.h"
#include "images/dmChannels.h"

#include "templates/processing/dmDensityMap.h"

//---------------------------------------------------------------------
CCI_DECL_DATA( ColorSpace )
{
  dm_uint Dummy;
};
//--------------------------------------------------------------------
static inline CCI_CONSTRUCTOR( ColorSpace )
{
  CCI_CONSTRUCT_SUPER()
  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
static inline CCI_DESTRUCTOR( ColorSpace )
{
  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( ColorSpace )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( ColorSpace )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// ToColorSpace
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ColorSpace, ToColorSpace )
{
  CCI_INSTANCE( ColorSpace )

  //---------------------------------------------------
  // No parameters, transform only colormap
  //---------------------------------------------------
  if(CCI_P(params)==dm_null) 
  {
    dmIColorMap _ColorMap;
    if(_ColorMap.QueryInterface(CCI_P(colormap))) 
    {
      dm_uint32         count;
      const dm_uint32*  lut;
        	
      if(!_ColorMap.GetLUT(count,lut))
         CCI_RETURN_FAIL()
    }
    
    CCI_RETURN_INVALID_PARAM()
  } 

  //---------------------------------------------------
  // Regular parameter, apply transformation to image
  //---------------------------------------------------
  dmIParameters _Params;
  if(_Params.QueryInterface(CCI_P(params)))
  {
  	dmImage*  image = _Params.GetImage();
  	dmRegion* roi   = _Params.GetRoi(); 

  	if(image) 
  	{
  		dmLink<dmImage> img = dmNewLink<dmImage>(image);
		
      dmIColorSpace _Colorspace;      
      if(_Colorspace.QueryInterface(_THIS_OBJECT))
      {      	
        const dmRect _Rect = roi->Rectangle(); 

        _Colorspace.SetRect( &_Rect );

        dm_int  _NChans = _Colorspace.NumChannels();
        dm_uint _Format = _Colorspace.GetChannelFmt();
              
        dmIColorMap _ColorMap;
        if(_ColorMap.QueryInterface(CCI_P(colormap)))
        { 
        	dm_uint32         count;
        	const dm_uint32*  lut;
        	
        	if(!_ColorMap.GetLUT(count,lut))
        	   CCI_RETURN_FAIL()

  	  	  if(count > dmLUT8_MAX_COLORS)
  		       count = dmLUT8_MAX_COLORS;

          dmColorIndex  Cmap[dmLUT8_MAX_COLORS];
                        
          if( _NChans > 4)
              _NChans = 4;               
                                         
          for(int i=0;i<_NChans;++i) 
          {
          	_Colorspace.CreateBuffer(i,dmPixelFormat8bppIndexed,_Rect.Width(),_Rect.Height());
          	dmImage* band = _Colorspace.GetBuffer(i);
        	 
          	band->GetCopy(*image,_Rect);
        	
          	dmIImage<dmPixelFormat8bppIndexed>*
          	   _img = dmIImage<dmPixelFormat8bppIndexed>::Cast(band);
        	 
          	// Create the color map
          	for(dm_uint c=0;c<count;++c) {
          		Cmap[c] = static_cast<dmColorIndex>(DM_GetByteColor(i,lut[c]));
          	}
          	   
          	daim::apply_map(_img->Gen(),_img->Rect(),Cmap);  
          }
        } 
        else 
        {
          // no colormap, convert to RGB then split
          img = dmCreateCopy(img,_Rect,dmPixelFormat24bppRGB);
          _Colorspace.SplitImage(img);
        }                 
        CCI_RETURN_OK()
      }
  	}
  }
  
  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( ColorSpace )
  CCI_REGISTER_METHOD_PP( ColorSpace , ToColorSpace )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( ColorSpace )
{
  CCI_INVOKE_FACTORY(ColorSpace)
}
//---------------------------------------------------------------------
