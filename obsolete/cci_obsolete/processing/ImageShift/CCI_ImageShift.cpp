
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

#define dmUseCCI
#define dmUseKernelImageTemplates
#define dmUseUtilitiesExtra
#define dmUseKernelBuffer
#include "daim_kernel.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_Parameters.h"
#include "daim_modules/CCI_ImageList.h"
#include "daim_modules/CCI_MaskBuffer.h"

#include "daim_modules/processing/CCI_ImageShift.h"

//------------------------------------------------------------------
#define CCIC_NAME CCIC_ImageShift
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_ImageShift_Version
#define CCI_REVISION  0
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))
//--------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"

#include "daim/daim_base.h"
#include "daim/daim_interfaces.h"
#include "common/dmUserLib.h"

#define dmUseMatrixFT
#include "daim_matrix.h"

#include "operators/dmImageMatrix.h"
#include "operators/dmImageFT.h"

#define MIN_FT_SIZE    32
#define DFLT_TEST_SIZE 512

#define _NOINDEX_ ~0UL
//--------------------------------------------------------------------
CCI_DECL_DATA( ImageShift )
{
  dmImageFT FT1;
  dmImageFT FT2;
};

//-----------------------------------------------------
static void ImageShift_RemoveMeanValue( dmImageFT& ft )
{
  dm_real sz  = (1.0f *  ft.width())*ft.height(); 
  dm_real sum = daim::accumulate(ft,0.0f);
  daim::transform(ft,std::bind2nd(std::minus<dm_float>(),sum/sz));
}
//-----------------------------------------------------
static void ImageShift_GetShiftVector( dmImageFT& ft , dmPoint& pt )
{
  dm_int i,j,xm=0,ym=0;
  dm_int ni = ft.height();
  dm_int nj = ft.width();

  dm_real fmax = ft[0][0],temp;
  dmImageFT::const_line_type a = ft.begin();

  for(i=0;i<ni;++i,++a) {
    for(j=0;j<nj;++j) {
      if( (temp= (*a)[j]) > fmax) {
        fmax = temp; 
        ym = i; xm = j;
      }
    }
  }

  pt.x =  nj/2-xm;
  pt.y =  ni/2-ym;
}
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( ImageShift )
{
  CCI_CONSTRUCT_SUPER()
  CCI_INSTANCE( ImageShift)

  CCI_INIT_MEMBER(FT1)
  CCI_INIT_MEMBER(FT2)

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( ImageShift )
{
  CCI_INSTANCE( ImageShift )

  CCI_DESTROY_MEMBER(FT1)
  CCI_DESTROY_MEMBER(FT2)

  CCI_DESTROY_SUPER()
}
/*
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( ImageShift )
  CCI_ATTR_GET( ImageShift,XXX, CCIA_RETVAL = _This->XXX  )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( ImageShift )
  CCI_ATTR_SET( ImageShift,XXX, _This->XXX = CCIA_DATA )
CCI_END_SETTER()
*/
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( ImageShift )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( ImageShift )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// Method: ApplyKernel
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageShift, Compute )
{
  CCI_INSTANCE( ImageShift )

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmPoint        pt(CCI_P(x),CCI_P(y));
    dmRegion*      rgn    = _IParams.GetRoi();
    dmImage*       image1 = _IParams.GetImage();

    if(image1==NULL) {
      CCI_SET_ERROR_MSG(_TXT("Missing image in parameters !"));
      CCI_RETURN_INVALID_PARAM()
    }

    dmDEBUG_ASSERT( rgn != NULL );
    dmDEBUG_ASSERT(!rgn->IsEmptyRoi() );

    dmRect rect = rgn->Rectangle();
    if(rect.Width()<MIN_FT_SIZE || rect.Height()<MIN_FT_SIZE) {
       CCI_SET_ERROR_MSG(_TXT("ROI must be at least 32 pixels wide !"));
       CCI_RETURN_INVALID_PARAM()
    }

    dmIImageList _IImages;
    if(_IImages.QueryInterface(_IParams.GetImageList()))
    {
      dmImage* image2 = _IImages.GetBuffer(CCI_P(index));
      if(image2==NULL) {
        CCI_SET_ERROR_MSG(_TXT("No image in buffer !"));
        CCI_RETURN_INVALID_PARAM()
      }

      dmCopyImageDataFT(*image1,_This->FT1,rect);

      rect.Translate(pt - rect.TopLeft());
      dmCopyImageDataFT(*image2,_This->FT2,rect);

      ImageShift_RemoveMeanValue(_This->FT1);
      ImageShift_RemoveMeanValue(_This->FT2);

      // Compute the correlation
      dmFT_Real_Correlation(_This->FT1,_This->FT2,_This->FT2);
      
      // Get the displacement vector
      ImageShift_GetShiftVector(_This->FT2,pt);
      CCI_RETVAL_P(x) = pt.x;
      CCI_RETVAL_P(y) = pt.y;

      CCI_RETURN_OK()
    }
  } else 
    CCI_SET_ERROR_MSG(_TXT("Invalid parameters !"));

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: CropImages
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageShift, CropImages )
{
  CCI_UNUSUED_INSTANCE( ImageShift )

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
     dmIImageList _IList;
     if(!_IList.QueryInterface(CCI_P(imagelist))) {
       CCI_SET_ERROR_MSG(_TXT("Imagelist missing !"));
       CCI_RETURN_INVALID_PARAM()
     }

     dmImage* ref_image = NULL;

    // Get the reference image from parameters
    if(CCI_P(index)!=_NOINDEX_) {
      dmIImageList _IImages;
      if(_IImages.QueryInterface(_IParams.GetImageList()))
      ref_image =  _IImages.GetBuffer(CCI_P(index));
    } else
      ref_image = _IParams.GetImage();

    //------------------------
    // Processing input list
    //------------------------

    if(ref_image!=NULL) 
    {
      dmRegion testArea;
      if(CCI_P(testarea)!=NULL)
         testArea.SetRectRoi(*CCI_P(testarea));

      dm_uint  index;
      dm_uint  count = _IList.Size(); 
      dmRect roi_rect = _IParams.GetRoi()->Rectangle();

      if(roi_rect.IsEmpty())
        roi_rect = ref_image->Rect();      

      dmImage* src_image;

      // Precompute the roi  
      for(index=0;index<count;++index) {
         src_image = _IList.GetBuffer(index);
         if(src_image)  roi_rect.Clip(src_image->Rect());
      }

      if(!roi_rect.Clip(ref_image->Rect())) {
        CCI_SET_ERROR_MSG(_TXT("Empty roi !"));
        CCI_RETURN_FAIL() 
      }

      if(testArea.IsEmptyRoi()) 
      {
        dm_uint w = DFLT_TEST_SIZE/2 + 1;
        dmPoint p = roi_rect.Center() - dmPoint(w,w);
        testArea.SetRectRoi(dmRect(p,DFLT_TEST_SIZE,DFLT_TEST_SIZE));
      }

      testArea.AndCoordinates(roi_rect);

      if(testArea.IsEmptyRoi()        ||
        testArea.Rectangle().Width() <MIN_FT_SIZE||
        testArea.Rectangle().Height()<MIN_FT_SIZE)
      {
        CCI_SET_ERROR_MSG(_TXT("Test area too small !"));
        CCI_RETURN_FAIL() 
      }

      // Set ref image and roi as the default paarameter
      _IParams.SetImage(ref_image);
      _IParams.SetRoi(&testArea);
      _IParams.SetImageList(_IList);

      dmRect  im_rect_full(ref_image->Rect());
      dmRect  im_rect_part(ref_image->Rect());

      dmIImageShift _ISelf;
      _ISelf.QueryInterface(_THIS_OBJECT);

      // Collect shift informations

      std::vector<dmRect> _RectList(count); 
      dmPoint  p;

      for(index=0;index<count;++index)
      {
        p = testArea.Rectangle().TopLeft();  
        src_image  = _IList.GetBuffer(index);
        if(src_image!=NULL && _ISelf.Compute(_IParams,p,index)) {
          p.x = -p.x;
          p.y = -p.y;
          im_rect_full.Add (dmRect(p,src_image->Width(),src_image->Height()));
          im_rect_part.Clip(dmRect(p,src_image->Width(),src_image->Height()));
          _RectList[index] = dmRect(p,1,1);
        }
      }

      // Compute cropping rectangles
      for(index=0;index<count;++index)
      {
        dmRect&  rect = _RectList[index];
        if(!rect.IsEmpty())
        {
          // Get the rectangle into the coordinates of the image
          p = im_rect_part.TopLeft() - rect.TopLeft();

          dmDEBUG_ASSERT( p.x >= 0 );
          dmDEBUG_ASSERT( p.y >= 0 );

          // compute the final rectangle for the image
          rect = dmRect(p,im_rect_part.Width(),im_rect_part.Height());
        }
      }  

      // Store rectangles in the mask list
      if(dmIImageShift::flROI == CCI_P(flags))
      {
        // Get the mask buffer list from the parameter

        dmIMaskBuffer _IMasks;
        if(!_IMasks.QueryInterface(_IParams.GetMaskBuffer()))
          CCI_RETURN_FAIL()

        _IMasks.SetHeapSize(count);
       
        dmRegion mask;
        
        for(index=0;index<count;++index)
        {
          mask.SetRectRoi(_RectList[index]);
          src_image = _IList.GetBuffer(index);
          if(src_image==NULL) continue;

          // Store cropped image
          if(mask.IsEmptyRoi())
             mask.SetRectRoi(src_image->Rect());

          _IMasks.StoreMask(&mask,index);
        }
      }
      else
      {
        // Crop images
        for(index=0;index<count;++index)
        {
          dmRect&  rect = _RectList[index];
          src_image = _IList.GetBuffer(index);
          if(src_image==NULL) continue;

          // Store cropped image
          if(!rect.IsEmpty())
            _IList.StoreBuffer(index,src_image,&rect); 
        }
      }

      // Clear Image param
      _IParams.SetImage(NULL);
      _IParams.KillRoi();

      CCI_RETURN_OK()

    } else
      CCI_SET_ERROR_MSG(_TXT("Null reference image !"));
     
  } else 
    CCI_SET_ERROR_MSG(_TXT("Invalid parameters !"));

  CCI_RETURN_FAIL()
}

//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( ImageShift )
  CCI_REGISTER_METHOD_PP( ImageShift, Compute    )
  CCI_REGISTER_METHOD_PP( ImageShift, CropImages )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( ImageShift )
{
  CCI_INVOKE_FACTORY(ImageShift)
}
//---------------------------------------------------------------------
