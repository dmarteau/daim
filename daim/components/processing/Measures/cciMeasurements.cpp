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
// File         : cciMeasurements.cpp
// Date         : 14 févr. 2009
// Author       : David Marteau
//--------------------------------------------------------

#include "cciCOMPtr.h"
#include "cciIFilterContext.h"
#include "cciIRegionContainer.h"
#include "cciIImageContainer.h"

#define dmUseKernelImageTemplates
#include "daim_kernel.h"
#include "common/dmUserLib.h"

#include "cciIMeasures.h"

#include "templates/processing/dmPartition.h"
#include "templates/processing/dmObjectDetection.h"


#define _NOINDEX_  ~0UL
#define _NULL_NODE -1

#define RI_REGION 0x02
#define RI_EMPTY  0x01
#define RI_NONE   0x00

#define SHRINK_RECT( r ) \
    if(r.Width() >=3) r.Resize(-1, 0,-1, 0); \
    if(r.Height()>=3) r.Resize( 0,-1, 0,-1);


#define IMAGEMAP_UPDATE_PARTITION  0x0001L
#define IMAGEMAP_UPDATE_ROI        0x0002L
#define IMAGEMAP_UPDATE_PARTROI    0x0004L
#define IMAGEMAP_UPDATE_INDEXTABLE 0x0008L
#define IMAGEMAP_UPDATE_COUNT      0x0010L
#define IMAGEMAP_UPDATE_CENTROID   0x0020L
#define IMAGEMAP_UPDATE_BOUNDARY   0x0040L
#define IMAGEMAP_UPDATE_DIR        0x0080L
#define IMAGEMAP_UPDATE_RECTPROPS  0x0100L
#define IMAGEMAP_UPDATE_EULER      0x0200L

#define IMAGEMAP_UPDATE_MAP (IMAGEMAP_UPDATE_PARTITION |\
                             IMAGEMAP_UPDATE_INDEXTABLE|\
                             IMAGEMAP_UPDATE_PARTROI|\
                             IMAGEMAP_UPDATE_ROI|\
                             IMAGEMAP_UPDATE_EULER)
//----------------------------
struct _RegionInfo;
struct _RegionLink : public dmNode {
   _RegionInfo* rl_Self;
};
//----------------------------
struct _RegionInfo : public dmNode
{
  int ri_Status;
  int ri_Count;
  int ri_Childs;
  int ri_Father;  // Father partition
  int ri_Depth;   // Invariant.
  int ri_Part;    // Invariant.

  dm_real ri_Border;
  dm_real ri_Theta;  // Invariant.
  dm_real ri_L1;     // Invariant.
  dm_real ri_L2;     // Invariant.

  dmPoly  ri_Boundary; // External boundary: Invariant.
  dm_real ri_Centroid_x;
  dm_real ri_Centroid_y;
  dm_real ri_Position_x;
  dm_real ri_Position_y;
  dm_bool ri_pos;

  dm_real ri_fvalue;

  _RegionLink ri_ChildNode;
  dmList      ri_ChildList;

  dm_int      ri_Euler;

 ~_RegionInfo() {}
  _RegionInfo()
  :ri_Status  (RI_NONE)
  ,ri_Count   (0)
  ,ri_Childs  (0)
  ,ri_Father  (_NULL_NODE)
  ,ri_Depth   (0)
  ,ri_Part    (_NULL_NODE)
  ,ri_Border  (0)
  ,ri_Theta   (0)
  ,ri_L1      (0)
  ,ri_L2      (0)
  ,ri_Boundary(0)
  ,ri_Euler   (0)
  {
    ri_ChildNode.rl_Self = this;
  }

  // Need to have a copy constructor
  // We do not need to do a real copy because
  // it is used only in dmVector initialisation/reallocation
  // and this done only when building the node table.

  _RegionInfo( const _RegionInfo& _ri )
  :ri_Status(RI_NONE)
  ,ri_Count   (0)
  ,ri_Childs  (0)
  ,ri_Father  (_NULL_NODE)
  ,ri_Depth   (0)
  ,ri_Part    (_NULL_NODE)
  ,ri_Border  (0)
  ,ri_Theta   (0)
  ,ri_L1      (0)
  ,ri_L2      (0)
  ,ri_Boundary(0)
  ,ri_Euler   (0)
  {
    ri_ChildNode.rl_Self = this;
  }

  void Remove() { dmNode::Remove(); }

};
typedef _RegionInfo *XNODE;

//--------------------------------------------------------------------

typedef dmVector<_RegionInfo>  info_table_type;
typedef dmOList<_RegionInfo>   info_list_type;

typedef daim::basic_partition::index_table index_table_type;

//--------------------------------------------------------------------



/* Header file */
class cciMeasurements : public cciIMeasurements
{
public:
  CCI_DECL_ISUPPORTS
  CCI_DECL_IMEASUREMENTS

  cciMeasurements();

private:
  ~cciMeasurements();

protected:
  /* additional members */
};

/* Implementation file */
CCI_IMPL_ISUPPORTS1(cciMeasurements, cciIMeasurements)

cciMeasurements::cciMeasurements()
{
  /* member initializers and constructor code */
}

cciMeasurements::~cciMeasurements()
{
  /* destructor code */
}

/* void build (in cciImage image, in cciRegion rgn, in dm_uint32 connect, in dm_uint32 flags); */
CCI_IMETHODIMP cciMeasurements::Build(cciImage image, cciRegion rgn, dm_uint32 connect, dm_uint32 flags)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* void createRegion (in cciRegion mask, in dm_int32 label, in boolean include_holes); */
CCI_IMETHODIMP cciMeasurements::CreateRegion(cciRegion mask, dm_int32 label, dm_bool include_holes)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* void clear (in dm_int32 label); */
CCI_IMETHODIMP cciMeasurements::Clear(dm_int32 label)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* [noscript,notxpcom] dm_uint32 getLabels (in cciRegion rgn, [array] out dm_int32 labels); */
CCI_IMETHODIMP_(dm_uint32) cciMeasurements::GetLabels(cciRegion rgn, dm_int32 **labels CCI_OUTPARAM)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* dm_int32 getLabelFromPoint (in dm_int32 x, in dm_int32 y); */
CCI_IMETHODIMP cciMeasurements::GetLabelFromPoint(dm_int32 x, dm_int32 y, dm_int32 *_retval CCI_OUTPARAM)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* void removeLabels ([array, size_is (count)] in dm_int32 labels, in dm_uint32 count); */
CCI_IMETHODIMP cciMeasurements::RemoveLabels(dm_int32 *labels, dm_uint32 count)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* void selectLabels (in cciRegion mask, [array, size_is (count)] in dm_int32 labels, in dm_uint32 count); */
CCI_IMETHODIMP cciMeasurements::SelectLabels(cciRegion mask, dm_int32 *labels, dm_uint32 count)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* void setCalibration (in dm_real unitsPerPixel, in dm_real aSpectRatio); */
CCI_IMETHODIMP cciMeasurements::SetCalibration(dm_real unitsPerPixel, dm_real aSpectRatio)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* [noscript,notxpcom] dm_uint32 getChildRegionLabels (in dm_int32 label, [array] out dm_int32 labels); */
CCI_IMETHODIMP_(dm_uint32) cciMeasurements::GetChildRegionLabels(dm_int32 label, dm_int32 **labels CCI_OUTPARAM)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* [noscript,notxpcom] dm_uint32 getIndexTable ([array] out dm_int32 table); */
CCI_IMETHODIMP_(dm_uint32) cciMeasurements::GetIndexTable(dm_int32 **table CCI_OUTPARAM)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* void fillHoles (); */
CCI_IMETHODIMP cciMeasurements::FillHoles()
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* void cleanBorders (); */
CCI_IMETHODIMP cciMeasurements::CleanBorders()
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] void getProperties (in dm_int32 label, in dm_uint32 flags, in cciRgnPropertiesRef props); */
CCI_IMETHODIMP cciMeasurements::GetProperties(dm_int32 label, dm_uint32 flags, cciRgnProperties & props)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* void reconstruct (in cciRegion mask); */
CCI_IMETHODIMP cciMeasurements::Reconstruct(cciRegion mask)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* void cleanRegionBorders (in cciRegion mask); */
CCI_IMETHODIMP cciMeasurements::CleanRegionBorders(cciRegion mask)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}


