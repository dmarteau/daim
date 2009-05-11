
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
#define dmUseUtilitiesExtra
#define dmUseKernelBuffer
#define dmUseKernelTemplates
#define dmUseKernelImageTemplates
#include "daim_kernel.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_Parameters.h"
#include "daim_modules/CCI_ImageList.h"

#include "daim_modules/processing/CCI_ImageParts.h"
#include "daim_modules/processing/CCI_Colorspace.h"
#include "daim_modules/CCI_MaskBuffer.h"

#include "templates/processing/dmPartition.h"

//------------------------------------------------------------------
#define CCIC_NAME CCIC_ImageParts
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_ImageParts_Version
#define CCI_REVISION  0
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2005"))
//--------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"
#include "common/dmUserLib.h"

#include "daim/daim_base.h"
#include "daim/daim_interfaces.h"

#define _NOINDEX_ ~0L

//--------------------------------------------------------------------
CCI_DECL_DATA(ImageParts)
{
  daim::region_partition  _PartionMap;
  daim::labels_array_type _Labels;
  dmRect                  _RoiRect;
  dm_bool                 _Built;
};

#define IMAGEPARTS_ENSURE_BUILT() \
  if(!_This->_Built) {\
    CCI_SET_ERROR_MSG(_TXT("Image parts must be built before use"));\
    CCI_RETURN_FAIL()\
  }

#define IMAGEPARTS_VERIFY_LABEL( lbl ) \
  if(lbl < 0 || lbl >=  static_cast<dm_int>(_This->_PartionMap.partition().size())) { \
    CCI_SET_ERROR_MSG(_TXT("Label is out of range"));\
    CCI_RETURN_FAIL()\
  }

#define PARTITION( m ) _This->_PartionMap.m

#include "ImageParts.cpp"
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( ImageParts )
{
  CCI_GENERIC_CONSTRUCT(ImageParts)

  _This->_Built = dm_false;

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( ImageParts )
{
  CCI_GENERIC_DESTROY(ImageParts)
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( ImageParts )
  CCI_ATTR_GET(ImageParts,Count, CCIA_RETVAL = _This->_PartionMap.rgn_count()) 
CCI_END_GETTER()

//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
/*
CCI_IMPL_SETTER( ImageParts )
CCI_END_SETTER()
*/
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( ImageParts )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// Method: BuildPartition
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageParts, BuildPartition )
{
  CCI_INSTANCE(ImageParts)

  _This->_Built = dm_false;

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*  img = _IParams.GetImage();
    dmRegion* rgn = _IParams.GetRoi();

    if(img) 
    {
      dmRegion      _Region = *rgn;

      // Save ROI rectangle for offset
      _This->_RoiRect = _Region.Rectangle();;

      daim::connectivity _connect = (CCI_P(connectivity)==8
                                     ?daim::connect8
                                     :daim::connect4);
      // ======================
      // Check for Colorspace
      // ======================
      if(!dmIsPixelFormatScalar(img->PixelFormat())) 
      {
        dmIColorSpace _ColorSpace;
        if(_ColorSpace.QueryInterface(_IParams.GetColorSpace())) 
        {
          dm_uint channel = _IParams.GetChannel();
          if(_ColorSpace.Split(_IParams))
          {
            //========================
            // Apply to all channels
            //========================
            if(channel == dmIParameters::flAllChannels) 
            {
               // Reconstruct image as a pseudo RGB image 
               // and compute partition 

               // Create an RGB image
               dmLink<dmImage> _src = dmCreateImage<dmPixelFormat24bppRGB>(
                                               _Region.Rectangle().Width(),
                                               _Region.Rectangle().Height());

               if(_ColorSpace.NumChannels()>=3) 
               {
                 dmImage* imgs[3] = {
                   _ColorSpace.GetBuffer(0),
                   _ColorSpace.GetBuffer(1),
                   _ColorSpace.GetBuffer(2),
                 };

                 // Check validity of all images
                 for(int i= 0;i<3;++i) { 
                   if(imgs[i]==NULL) {
                      CCI_SET_ERROR_MSG(_TXT("Null channel !"));
                      CCI_RETURN_FAIL();
                   }
                 }

                 // Merge as RGB planes 
                 dmCopyRGBData(*_src,*imgs[0],*imgs[1],*imgs[2],true);
            
                 // Build Partition
                 PARTITION( make_regions(
                            imageparts::rgb_image_type::Cast(_src)->Gen(),
                            imageparts::diff_compare(static_cast<dm_float>(CCI_P(distance))),
                            _connect) );

                 _Region.OffsetRoi();  // offset ROI
                 ImagePart_SetZeroRegion(_This,_Region,_Region.Rectangle());

                 _This->_Built = dm_true;

                 //=========================
                 // Compute averaged values
                 //=========================
                 if((CCI_P(modes) & CCIV_ImageParts_Average) != 0 && _ColorSpace.CanMerge()) 
                 {
                    dmPoint ptOffset = _Region.Rectangle().TopLeft();
                    // Compute averaged values in each map

                    for(int i=0;i<3;++i) 
                      ImagePart_ComputeAverage(_This,*imgs[i],_Region,ptOffset);

                    // 1. The image in params is the original image
                    // 2. Colorspace restore data from the original roi
                    // 3. Averaging is restricted to the roi
                    // => Merge in place
                    _ColorSpace.Merge(_IParams);
                 }

                 CCI_RETURN_OK()

               } // NumChannels()<3
            } 
            //========================
            // Apply to one channel
            //========================
            else {
              img = _ColorSpace.GetBuffer(channel-1); // Extract the channel
              // Warning : we
            }
          } // Failed to split
        } // No colorspace available

        CCI_RETURN_FAIL()
      }

      // ======================
      // Image is scalar
      // ======================
   
      // Create a floating point image
      dmLink<dmImage> _src = dmCreateImage<dmPixelFormat32bppFloat>(
                                          _Region.Rectangle().Width(),
                                          _Region.Rectangle().Height());

      _src->GetCopy(*img,_Region);
      PARTITION( make_regions(
                 imageparts::scalar_image_type::Cast(_src)->Gen(),
                 imageparts::diff_compare(static_cast<dm_float>(CCI_P(distance))),
                 _connect) );

       _Region.OffsetRoi();  // offset ROI
       ImagePart_SetZeroRegion(_This,_Region,_Region.Rectangle());

       _This->_Built = dm_true;

       if((CCI_P(modes) & CCIV_ImageParts_Average)) {
         ImagePart_ComputeAverage(_This,*img,_Region,_This->_RoiRect.TopLeft());
       }

       CCI_RETURN_OK()

    } // No image
  } 
  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method: CompactPartition
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageParts, CompactPartition )
{
  CCI_INSTANCE(ImageParts)

  IMAGEPARTS_ENSURE_BUILT()
  CCI_RETVAL_P(count) = PARTITION( compact_partition() );

  CCI_RETURN_OK() 
}
//---------------------------------------------------------------------
// Method: RemoveBorderRgns
//---------------------------------------------------------------------
static CCI_IMPL_METHOD_NP( ImageParts, RemoveBorderRgns )
{
  CCI_INSTANCE(ImageParts)

  IMAGEPARTS_ENSURE_BUILT()
  PARTITION( remove_border_regions() );

  CCI_RETURN_OK() 
}
//---------------------------------------------------------------------
// Method: ResolveOverlappingRgn
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageParts, ResolveOverlappingRgn )
{
  CCI_INSTANCE(ImageParts)

  IMAGEPARTS_ENSURE_BUILT()

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmRegion* mask = _IParams.GetMask();
    if(mask)
    {
       dmRegion rgn = *mask;
       rgn.OffsetRoi(_This->_RoiRect.TopLeft());
       rgn.ClipToRect(PARTITION( rect() ));
       PARTITION( resolve_overlapping_regions( rgn ) );

       CCI_RETURN_OK()
    }
  }
  CCI_RETURN_FAIL() 
}
//---------------------------------------------------------------------
// Method: MergeRegions
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageParts, MergeRegions )
{
  CCI_INSTANCE(ImageParts)

  IMAGEPARTS_ENSURE_BUILT()
  IMAGEPARTS_VERIFY_LABEL( CCI_P(firstlabel)  )
  IMAGEPARTS_VERIFY_LABEL( CCI_P(secondlabel) )

  CCI_RETVAL_P( resultlabel ) =
    PARTITION( merge_regions( CCI_P(firstlabel), CCI_P(secondlabel) ) );

  CCI_RETURN_OK() 
}
//---------------------------------------------------------------------
// Method: GetLabelFromPoint
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageParts, GetLabelFromPoint )
{
  CCI_INSTANCE(ImageParts)

  IMAGEPARTS_ENSURE_BUILT()
  dmPoint p(CCI_P(x),CCI_P(y));

  // Apply offset in this point 
  p -= _This->_RoiRect.TopLeft();
  if(dmPointInRectangle(p, PARTITION(rect()) ) ) 
  {
    CCI_RETVAL_P(label) = PARTITION( get_label(p) );
    CCI_RETURN_OK() 
  }

  CCI_RETURN_FAIL() 
}
//---------------------------------------------------------------------
// Method: GetMaxRegion
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageParts, GetMaxRegion )
{
  CCI_INSTANCE(ImageParts)

  IMAGEPARTS_ENSURE_BUILT()

  size_t size;
  CCI_RETVAL_P(label) = PARTITION( get_max_region(size) );
  CCI_RETURN_OK() 
}
//---------------------------------------------------------------------
// Method: GetRegionSize
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageParts, GetRegionSize )
{
  CCI_INSTANCE(ImageParts)

  IMAGEPARTS_ENSURE_BUILT()
  IMAGEPARTS_VERIFY_LABEL( CCI_P(label) )

  CCI_RETVAL_P(size) = PARTITION( get_region_size(CCI_P(label)) );
  CCI_RETURN_OK() 
}
//---------------------------------------------------------------------
// Method: GetRegionIndex
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageParts, GetRegionIndex )
{
  CCI_INSTANCE(ImageParts)

  IMAGEPARTS_ENSURE_BUILT()
  IMAGEPARTS_VERIFY_LABEL( CCI_P(label) )

  CCI_RETVAL_P(index) = PARTITION( get_region_index(CCI_P(label)) );
  CCI_RETURN_OK() 
}
//---------------------------------------------------------------------
// Method: GetRegionLabels
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageParts, GetRegionLabels )
{
  CCI_INSTANCE(ImageParts)

  if(CCI_P(region)!=NULL)
  {
    IMAGEPARTS_ENSURE_BUILT()

    _This->_Labels.clear();

    dmRegion rgn = *CCI_P(region);
    rgn.OffsetRoi(_This->_RoiRect.TopLeft());
    rgn.ClipToRect( PARTITION( rect() ) );
    
    PARTITION( get_overlapping_regions_labels(rgn,_This->_Labels) );

    CCI_RETVAL_P(labels) = &_This->_Labels[0];
    CCI_RETVAL_P(size)   =  _This->_Labels.size();

    CCI_RETURN_OK()
  }
  CCI_RETURN_INVALID_PARAM() 
}
//---------------------------------------------------------------------
// Method: GetRegionMask
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageParts, GetRegionMask )
{
  CCI_INSTANCE(ImageParts)

  IMAGEPARTS_ENSURE_BUILT()

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
     dmRegion* _mask = _IParams.GetMask();

    if(CCI_P(label) != _NOINDEX_)
    {
      IMAGEPARTS_VERIFY_LABEL( CCI_P(label) )
      PARTITION( create_roi(*_mask,CCI_P(label)) );

      _mask->Translate( _This->_RoiRect.TopLeft() ); 
    }
    else
    {
      _This->_Labels.clear();

      dmRegion rgn = *_mask;
      rgn.OffsetRoi(_This->_RoiRect.TopLeft());
      rgn.ClipToRect( PARTITION(rect()) );

      PARTITION( get_overlapping_regions_labels(rgn,_This->_Labels) );
    
      dmRegion _tmprgn;

      _mask->KillRoi();

      daim::labels_array_type::iterator it   = _This->_Labels.begin();
      daim::labels_array_type::iterator last = _This->_Labels.end(); 
      for(;it!=last;++it) {
        PARTITION( create_roi(_tmprgn,*it) );
        _mask->AddRoi(_tmprgn);
      } 

      _mask->Translate( _This->_RoiRect.TopLeft() );
    }
    CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL() 
}
//---------------------------------------------------------------------
// Method: MergeLabels
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageParts, MergeLabels )
{
  CCI_INSTANCE(ImageParts)

  IMAGEPARTS_ENSURE_BUILT()
  IMAGEPARTS_VERIFY_LABEL( CCI_P(label) )

  if(CCI_P(labels) != NULL && CCI_P(size)>0)
  { 
    daim::labels_array_type& labels = _This->_Labels;
    dm_int max_label = static_cast<dm_int>(PARTITION(partition().size()));

    labels.reserve(CCI_P(size));
    labels.clear();

    dm_int* it   = CCI_P(labels);
    dm_int* last = CCI_P(labels) + CCI_P(size);

    for(dm_int lbl;it!=last;++it)
    {
      lbl = *it;
      if(lbl <= 0 || lbl >= max_label) {
         CCI_SET_ERROR_MSG(_TXT("Label is out of range"));
         CCI_RETURN_FAIL()
      }
      labels.push_back( lbl );
    }
    PARTITION( merge_labels( labels, CCI_P(label) ) );
    CCI_RETURN_OK()
  }

  CCI_RETURN_INVALID_PARAM() 
}
//---------------------------------------------------------------------
// Method: SelectRegionsBySize
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageParts, SelectRegionsBySize )
{
  CCI_INSTANCE(ImageParts)

  IMAGEPARTS_ENSURE_BUILT()

  _This->_Labels.clear();

  if(CCI_P(exclude))
    PARTITION( select_regions_size(_This->_Labels,
              daim::between_excl<dm_uint>(CCI_P(minsize),CCI_P(maxsize))) );
  else
    PARTITION( select_regions_size(_This->_Labels,
              daim::between_excl<dm_uint>(CCI_P(maxsize),CCI_P(minsize))) );

  CCI_RETVAL_P(labels) = &_This->_Labels[0];
  CCI_RETVAL_P(count)  =  _This->_Labels.size();

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: StoreRegion
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageParts, StoreRegion )
{
  CCI_INSTANCE(ImageParts)

  IMAGEPARTS_ENSURE_BUILT()

  dmIMaskBuffer _IMasks;
  if(_IMasks.QueryInterface(CCI_P(maskbuffer)))
  {
    dmRegion _Mask;

    IMAGEPARTS_VERIFY_LABEL( CCI_P(label) )
    PARTITION( create_roi(_Mask,CCI_P(label)) );

    _Mask.Translate( _This->_RoiRect.TopLeft() );

    if(_IMasks.StoreMask(&_Mask,CCI_P(index)))
       CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL() 
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( ImageParts )
  CCI_REGISTER_METHOD_PP( ImageParts, BuildPartition        )
  CCI_REGISTER_METHOD_PP( ImageParts, CompactPartition      )
  CCI_REGISTER_METHOD_NP( ImageParts, RemoveBorderRgns      )
  CCI_REGISTER_METHOD_PP( ImageParts, ResolveOverlappingRgn )
  CCI_REGISTER_METHOD_PP( ImageParts, MergeRegions          )
  CCI_REGISTER_METHOD_PP( ImageParts, GetLabelFromPoint     )
  CCI_REGISTER_METHOD_PP( ImageParts, GetMaxRegion          )
  CCI_REGISTER_METHOD_PP( ImageParts, GetRegionSize         )
  CCI_REGISTER_METHOD_PP( ImageParts, GetRegionIndex        )
  CCI_REGISTER_METHOD_PP( ImageParts, GetRegionLabels       )
  CCI_REGISTER_METHOD_PP( ImageParts, GetRegionMask         )
  CCI_REGISTER_METHOD_PP( ImageParts, MergeLabels           )
  CCI_REGISTER_METHOD_PP( ImageParts, SelectRegionsBySize   )
  CCI_REGISTER_METHOD_PP( ImageParts, StoreRegion           )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( ImageParts )
{
  CCI_INVOKE_FACTORY( ImageParts )
}
//---------------------------------------------------------------------
