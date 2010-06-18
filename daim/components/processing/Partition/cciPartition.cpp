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

//--------------------------------------------------------
// File         : cciPartition.cpp
// Date         : 15 juin 2010
// Author       : David Marteau
//--------------------------------------------------------

#include "cciCOMPtr.h"
#include "cciInterfaceRequestorUtils.h"
#include "cciIRegionContainer.h"
#include "cciIImageContainer.h"
#include "cciIKernelFamilyContainer.h"
#include "cciIPartition.h"
#include "cciIResultColumn.h"

#define dmUseKernelImageTemplates
#include "daim_kernel.h"
#include "common/dmUserLib.h"

#include "templates/processing/dmPartition.h"

template<EPixelFormat _PixelFormat>
struct diff_compare
{
  typedef typename dmIImage<_PixelFormat>::traits_type  traits_type;
  typedef typename traits_type::value_type value_type;

  value_type distance;
  diff_compare( value_type _dist ) : distance(_dist) {}

  bool operator()( const value_type& a, const value_type& b) {
    return daim::absdiff( a, b ) <= distance;
  }
};
  
template<>
struct diff_compare<dmPixelFormat24bppRGB>
{
  typedef dmIImage<dmPixelFormat24bppRGB> image_type;
  typedef image_type::value_type          value_type;

  int distance;
  diff_compare( int _dist ) : distance(_dist) {}

  bool operator()( const value_type& a, const value_type& b) {               
    return  ( daim::absdiff(a.red  ,b.red  ) + 
              daim::absdiff(a.green,b.green) + 
              daim::absdiff(a.blue ,b.blue ) ) <= distance;
  }
};
 //------------------------------------------------------------------------
// partition operator
//----------------------------------------------------------------------
struct __dm_impl_partition
{
  daim::region_partition& Partition;
  dm_float                Distance;
  daim::connectivity      Connect;
    
  __dm_impl_partition(daim::region_partition& _Partition, dm_float _Distance,
                        daim::connectivity _Connect)
   :Partition(_Partition)
   ,Distance (_Distance)
   ,Connect  (_Connect)
   {}

  // Generic operation on scalar
  template<EPixelFormat _PixelFormat>
  void operator()( dmIImage<_PixelFormat>& _img )
  {
    typedef typename diff_compare<_PixelFormat>::value_type value_type;
       
    // Build Partition
    Partition.make_regions(_img.Gen(),diff_compare<_PixelFormat>(static_cast<value_type>(Distance)),
                           Connect);  
  }

  // Specialization
  void operator()( dmIImage<dmPixelFormat24bppRGB>& _img ) {      
    // Build Partition
    Partition.make_regions(_img.Gen(),diff_compare<dmPixelFormat24bppRGB>(static_cast<int>(Distance)),
                           Connect);  
  }

};

#define CCI_ENSURE_LABEL_RANGE( lbl ) \
  DM_BEGIN_MACRO \
  if(lbl < 0 || lbl >= static_cast<dm_int>(mPartionMap.partition().size())) \
    return CCI_ERROR_OUT_OF_RANGE; \
  DM_END_MACRO

  
class cciPartition : public cciIPartition
{
public:
  CCI_DECL_ISUPPORTS
  CCI_DECL_IPARTITION

  cciPartition();

private:
  ~cciPartition();

protected:
  daim::region_partition  mPartionMap;
  daim::labels_array_type mLabels;
  dmRect                  mRoiRect;
  dm_bool                 mBuilt;
  
  void SetZeroRegion(const dmRegion&, const dmRect& );

};

#define PARTITION( m ) mPartionMap.m


/* Implementation file */
CCI_IMPL_ISUPPORTS1(cciPartition, cciIPartition)

cciPartition::cciPartition()
: mBuilt(dm_false)
{
}

cciPartition::~cciPartition()
{
}

void cciPartition::SetZeroRegion( const dmRegion& _Region, const dmRect& _Rect )
{
  dmRegion _rgncopy = _Region;
  _rgncopy.XorCoordinates(_Rect);
  if(!_rgncopy.IsEmptyRoi()) 
  {
    mLabels.clear();
    mPartionMap.get_overlapping_regions_labels(_rgncopy,mLabels);
    mPartionMap.merge_labels( mLabels, 0);
  }  
}

/* void buildPartition (in cciImage image, in double distance, in cciRegion rgn, in unsigned long connectivity); */
CCI_IMETHODIMP cciPartition::BuildPartition(cciImage image, cciRegion rgn, dm_double distance, dm_uint32 connectivity)
{
  CCI_ENSURE_ARG_POINTER(image);
  
  dmImage* img     = CCI_NATIVE(image);
  dmRegion _Region = CCI_NATIVE_ROI(rgn,img->Rect());

  // Save ROI rectangle for offset
  mRoiRect = _Region.Rectangle();

  daim::connectivity _connect = (connectivity==8 ?daim::connect8 : daim::connect4);
  
  __dm_impl_partition filter(mPartionMap,distance,_connect);
  
  if(!dmImplementOperation(filter,*img))
     return CCI_ERROR_FAILURE;
  
  SetZeroRegion(_Region,_Region.Rectangle());
  mBuilt = dm_true;
  
  return CCI_OK;
}

/* long compactPartition (); */
CCI_IMETHODIMP cciPartition::CompactPartition(dm_int32 *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_TRUE(mBuilt,CCI_ERROR_NOT_INITIALIZED);
  
  *_retval = mPartionMap.compact_partition();
  
  return CCI_OK;
}

/* void removeBorderRgns (); */
CCI_IMETHODIMP cciPartition::RemoveBorderRgns()
{
  CCI_ENSURE_TRUE(mBuilt,CCI_ERROR_NOT_INITIALIZED);
  mPartionMap.remove_border_regions();
  
  return CCI_OK;
}

/* long resolveOverlappingRgn (in cciRegion rgn); */
CCI_IMETHODIMP cciPartition::ResolveOverlappingRgn(cciRegion rgn, dm_int32 *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_TRUE(mBuilt,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(rgn);
  
  dmRegion region = *CCI_NATIVE(rgn);
  region.ClipToRect(mPartionMap.rect());
  mPartionMap.resolve_overlapping_regions( region );
  
  return CCI_OK;
}

/* long mergeRegions (in long firstlabel, in long secondlabel); */
CCI_IMETHODIMP cciPartition::MergeRegions(dm_int32 firstlabel, dm_int32 secondlabel, dm_int32 *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_TRUE(mBuilt,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_LABEL_RANGE(firstlabel);
  CCI_ENSURE_LABEL_RANGE(secondlabel);
  
  *_retval = mPartionMap.merge_regions(firstlabel,secondlabel);
  
  return CCI_OK;
}

/* long getLabelFromPoint (in long x, in long y); */
CCI_IMETHODIMP cciPartition::GetLabelFromPoint(dm_int32 x, dm_int32 y, dm_int32 *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_TRUE(mBuilt,CCI_ERROR_NOT_INITIALIZED);
  
  dmPoint p(x,y);

  if(dmPointInRectangle(p,mPartionMap.rect()))
    *_retval = mPartionMap.get_label(p);
  else
    *_retval = 0;

  return CCI_OK; 
}

/* long getMaxRegion (); */
CCI_IMETHODIMP cciPartition::GetMaxRegion(dm_int32 *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_TRUE(mBuilt,CCI_ERROR_NOT_INITIALIZED);
  
  size_t size;
  *_retval = mPartionMap.get_max_region(size);

  return CCI_OK; 
}

/* unsigned long getRegionSize (in long label); */
CCI_IMETHODIMP cciPartition::GetRegionSize(dm_int32 label, dm_uint32 *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_TRUE(mBuilt,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_LABEL_RANGE(label);
  
  *_retval = mPartionMap.get_region_size(label);
  
  return CCI_OK;
}

/* long getRegionIndex (in long label); */
CCI_IMETHODIMP cciPartition::GetRegionIndex(dm_int32 label, dm_int32 *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_TRUE(mBuilt,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_LABEL_RANGE(label);
  
  *_retval = mPartionMap.get_region_index(label);
  
  return CCI_OK;
}

/* [noscript] void getRegionLabels (in cciRegion rgn, [shared] out labelArray labels, [retval] out dm_uint32 size); */
CCI_IMETHODIMP cciPartition::GetRegionLabels(cciRegion rgn, const dm_int32 **labels CCI_OUTPARAM, dm_uint32 *size CCI_OUTPARAM)
{
  CCI_ENSURE_TRUE(mBuilt,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(rgn); 
  
  mLabels.clear();
  
  dmRegion region = *CCI_NATIVE(rgn);
  region.ClipToRect( mPartionMap.rect() );
  
  mPartionMap.get_overlapping_regions_labels(region,mLabels);

  *labels = &mLabels[0];
  *size   = mLabels.size();
  
  return CCI_OK;
}

/* void getRegionMask (in cciRegion rgn, in long label); */
CCI_IMETHODIMP cciPartition::GetRegionMask(cciRegion rgn, dm_int32 label)
{
  CCI_ENSURE_TRUE(mBuilt,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(rgn); 
  CCI_ENSURE_LABEL_RANGE(label);
  
  mPartionMap.create_roi(*CCI_NATIVE(rgn),label);

  return CCI_OK;
}

/* void getOverlappingRegionMask (in cciRegion rgn); */
CCI_IMETHODIMP cciPartition::GetOverlappingRegionMask(cciRegion rgn)
{
  CCI_ENSURE_TRUE(mBuilt,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(rgn); 

  mLabels.clear();
    
  dmRegion* mask   = CCI_NATIVE(rgn);
  dmRegion  tmprgn = *mask;
  
  tmprgn.ClipToRect( mPartionMap.rect() );

  mPartionMap.get_overlapping_regions_labels(tmprgn,mLabels);
  
  mask->KillRoi();

  daim::labels_array_type::iterator it   = mLabels.begin();
  daim::labels_array_type::iterator last = mLabels.end(); 
  for(;it!=last;++it) {
    mPartionMap.create_roi(tmprgn,*it);
    mask->AddRoi(tmprgn);
  } 

  return CCI_OK;
}

/* void mergeLabels (in long label, [array, size_is (count)] in long labels, in unsigned long count); */
CCI_IMETHODIMP cciPartition::MergeLabels(dm_int32 label, dm_int32 *labels, dm_uint32 count)
{
  CCI_ENSURE_TRUE(mBuilt,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(labels);
  CCI_ENSURE_LABEL_RANGE(label);
  
  dm_int max_label = static_cast<dm_int>(mPartionMap.partition().size());

  mLabels.reserve(count);
  mLabels.clear();

  dm_int* it   = labels;
  dm_int* last = labels + count;

  for(dm_int lbl;it!=last;++it)
  {
    lbl = *it;
    if(lbl <= 0 || lbl >= max_label) {
       return CCI_ERROR_OUT_OF_RANGE;
    }
    mLabels.push_back( lbl );
  }
  mPartionMap.merge_labels(mLabels,label);
 
  return CCI_OK;
}

/* void selectRegionsBySize (in cciIResultColumn col, in unsigned long minsize, in unsigned long maxsize, in boolean exclude); */
CCI_IMETHODIMP cciPartition::SelectRegionsBySize(cciIResultColumn *col, 
                                                 dm_uint32 minsize, 
                                                 dm_uint32 maxsize, 
                                                 dm_bool exclude)
{
  CCI_ENSURE_TRUE(mBuilt,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(col);
  
  mLabels.clear();
  
  if(exclude)
    mPartionMap.select_regions_size(mLabels,
              daim::between_excl<dm_uint>(minsize,maxsize));
  else
    mPartionMap.select_regions_size(mLabels,
              daim::between_excl<dm_uint>(maxsize,minsize));

  //CCI_RETVAL_P(labels) = &_mLabels[0];
  //CCI_RETVAL_P(count)  =  mLabels.size();

  
  //FIXME
  return CCI_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute unsigned long count; */
CCI_IMETHODIMP cciPartition::GetCount(dm_uint32 *aCount)
{
  CCI_ENSURE_TRUE(mBuilt,CCI_ERROR_NOT_INITIALIZED);

  *aCount = mPartionMap.rgn_count();  
  return CCI_OK;
}



//=====================================
// Module
//=====================================

#include "cciIGenericFactory.h"

CCI_GENERIC_FACTORY_CONSTRUCTOR(cciPartition)

static const cciModuleComponentInfo components[] = {
    {  CCI_PARTITION_CLASSNAME,
       CCI_PARTITION_CID,
       CCI_PARTITION_CONTRACTID,
       cciPartitionConstructor
    },
};

CCI_IMPL_GETMODULE(PartitionModule, components)


