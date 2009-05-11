
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

/* $ChangeLog  v1.5, 9 d�c. 06 00:19:00 , David
 *
 * Added the "position primitive"
 */

#define dmUseCCI
#define dmUseUtilitiesExtra
#define dmUseKernelBuffer
#define dmUseKernelImageTemplates
#include "daim_kernel.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_Parameters.h"
#include "daim_modules/CCI_StorageMap.h"
#include "daim_modules/CCI_ImageList.h"
#include "daim_modules/CCI_PropertyList.h"

#include "daim_modules/processing/CCI_ImageMap.h"
#include "daim_modules/processing/CCI_Colorspace.h"

//------------------------------------------------------------------
#define CCIC_NAME CCIC_ImageMap
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_ImageMap_Version
#define CCI_REVISION  5
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2005"))
//--------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"
#include "common/dmUserLib.h"

#include "daim/daim_base.h"
#include "daim/daim_interfaces.h"

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

#define IMAGEMAP_UPDATE_MAP (IMAGEMAP_UPDATE_PARTITION|IMAGEMAP_UPDATE_INDEXTABLE|IMAGEMAP_UPDATE_PARTROI|IMAGEMAP_UPDATE_ROI|IMAGEMAP_UPDATE_EULER)
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
CCI_DECL_DATA(ImageMap)
{
  daim::cont_image<dm_int> Map;
  dmRegion                 Roi;           // Roi representing all connexe regions elements
  info_table_type          NodeTable;     // Indexed table of nodes
  index_table_type         NodeIndexTable;
  daim::basic_partition    NodePartition;
  daim::connectivity       Connect;
  info_list_type           NodeList;    // List of active nodes

  dmRegion                 PartRoi;     // Roi for regions elements map (holes+regions)
  index_table_type         IndexTable;  //
  daim::basic_partition    Partition;
  daim::labels_array_type  Labels;
  dmPoint                  Offset;
  dm_param                 Update;
  dm_int                   NumPartitions;
  dm_int                   Depth;

  dm_bool                  Built;
  dmVector<dm_real>        Storage;

  dmRegion  tmpRoi;

  dm_bool  use_image_calibration;
  dm_real  aspect_ratio;
  dm_real  uppxls;
};

#define _INDEX( _This, part )            _This->NodeIndexTable[part]
#define _NODE( _This, part )             _This->NodeTable[_INDEX(_This,part)]
#define _PARTITION_INDEX( _This, part )  _This->NodeIndexTable[_This->NodePartition[part]]
#define _PARTITION_NODE( _This, part )   _This->NodeTable[_PARTITION_INDEX(_This,part)]

#include "ImageMap_Nodes.cpp"
#include "ImageMap_Compute.cpp"
#include "ImageMap_ImgProps.cpp"
//--------------------------------------------------------------------

#define IMAGEMAP_ENSURE_BUILT() \
  if(!_This->Built) {\
    CCI_SET_ERROR_MSG(_TXT("Image map must be built before use"));\
    CCI_RETURN_FAIL()\
  }

#define IMAGEMAP_VERIFY_LABEL( lbl ) \
  if(lbl >= _This->NodePartition.size()) { \
    CCI_SET_ERROR_MSG(_TXT("Label is out of range"));\
    CCI_RETURN_FAIL()\
  }

//--------------------------------------------------------------------
// Utility functions
//--------------------------------------------------------------------
static dm_uint ImageMap_CopyLabelArray( CCI_INSTANCE_PTR(ImageMap) _This,
                                        dmIStorageMap& _IStorage,
                                        const char_t*  _Column )
{
  dm_real* data  = _IStorage.GetCol(_Column,0);
  dm_uint count  = 0;

  if(data)
  {
    count = static_cast<dm_uint>(data[0]);
    if(count > 0)
    {
      daim::labels_array_type& labels = _This->Labels;
      labels.resize(count);

      dm_real* it   = data+1;
      dm_real* last = it+count;

      dm_int   size = _This->NodePartition.size();

      daim::labels_array_type::iterator dst = labels.begin();
      for(dm_int lbl;it!=last;++it,++dst)
      {
        lbl = static_cast<dm_int>( *it );
        if(lbl >= size) {
           dmLOG("ERROR:ImageMap:Label out of range\n");
          return 0;
        }
        *dst = _This->Partition[lbl];
      }
    }
  }
  return count;
}
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( ImageMap )
{
  CCI_GENERIC_CONSTRUCT(ImageMap)

  _This->Built = dm_false;

  _This->use_image_calibration = dm_false;
  _This->aspect_ratio = 1.0;
  _This->uppxls       = 1.0;


  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( ImageMap )
{
  CCI_GENERIC_DESTROY(ImageMap)
}

//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( ImageMap )
  CCI_ATTR_GET( ImageMap,Connectivity , CCIA_RETVAL = _This->Connect )
  CCI_ATTR_GET( ImageMap,Count        , CCIA_RETVAL = (_This->Built ? ImageMap_BuildIndexTable(_This):0) )
  CCI_ATTR_GET( ImageMap,Depth        , CCIA_RETVAL = (_This->Built ?ImageMap_UpdatePartition(_This) :0) )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( ImageMap )
  CCIA_DISCARD( ImageMap,Connectivity)
  CCIA_DISCARD( ImageMap,Count       )
  CCIA_DISCARD( ImageMap,Depth       )
CCI_END_SETTER()
/*
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( ImageMap )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( ImageMap )
{
  CCI_RETURN_INVOKE_SUPER()
}
*/
//---------------------------------------------------------------------
// Method : Build
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageMap, Build )
{
  CCI_INSTANCE(ImageMap)

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*  img    = _IParams.GetImage();
    dmRegion* rgn    = _IParams.GetMask();

    if(rgn && !rgn->IsEmptyRoi())
    {
      _This->Roi           = *rgn;
      _This->Update        = 0;
      _This->NumPartitions = 0;
      _This->Depth         = 0;
      _This->Built         = dm_false;

      dmRect rect;

      if(img)
      {
        //---------------------------------
        // Get Image calibration if needed
        //---------------------------------
        if(_This->use_image_calibration && img->HasUnits())
        {
          dmConstLink<dmSpatialUnits> units = img->Units();
          _This->aspect_ratio = units->mAspectRatio;
          _This->uppxls       = units->mUnitsPerPixel;
        }

        //---------------------------------
        // Region will be clipped by the image rectangle
        //---------------------------------
        if((CCI_P(flags) & CCIV_ImageMap_NoClip)==0) {
          rect = img->Rect();
          _This->Roi.ClipToRect(rect);
        }
      }

      //---------------------------------
      // Otherwise region is clipped by
      // its own bounding box
      //---------------------------------
      if(rect.IsEmpty())
      {
        rect = _This->Roi.Rectangle();

        if((CCI_P(flags) & CCIV_ImageMap_OffsetRoi)!=0)
        {
          _This->Offset = rect.TopLeft();
          _This->Roi.Translate(-rect.Left(),-rect.Top());
          rect = _This->Roi.Rectangle();
        } else {
          _This->Offset = dmPoint(0,0);
          rect.Resize(rect.Left(),rect.Top(),0,0);
        }
      }

      //-----------------------------------
      // Build Partition Map
      //-----------------------------------
      _This->Map.reserve(rect);
      _This->Connect = (CCI_P(connect)==8?daim::connect8:daim::connect4);

      daim::create_region_partition(_This->NodePartition,
                                    _This->Map,
                                    _This->Roi,
                                    _This->Connect);

      //--------------------------------------
      // Assign empty borders regions to 0
      //--------------------------------------
      _This->tmpRoi.SetRectRoi(rect);
      _This->tmpRoi.XorRoi(_This->Roi); // Get Complementary regions

      SHRINK_RECT(rect)

      _This->tmpRoi.SubCoordinates(rect);
      _This->Labels.clear();

      if(!_This->tmpRoi.IsEmptyRoi()) {   // Merge borders regions to 0
        daim::get_overlapping_regions_labels(_This->tmpRoi,_This->NodePartition,_This->Map,_This->Labels);
        daim::merge_labels(_This->NodePartition,_This->Labels,0);
      }

      int nPartitions = _This->NodePartition.get_index_table(_This->NodeIndexTable);
      if( nPartitions>0 )
      {
        _This->NodeTable.Clear();
        _This->NodeTable.Resize( nPartitions );
        if(ImageMap_CreateNodes( _This )) {
          _This->Built = dm_true;
          CCI_RETURN_OK()
        }
      }

    } else
      CCI_SET_ERROR_MSG(_TXT("Empty region !"));
  } else
    CCI_SET_ERROR_MSG(_TXT("Invalid parameters !"));

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method : CreateRegion
//
// if label==-1  : Background region
// if label== 0  : Objects region
// if label== x  : Get region from its label <x>
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageMap, CreateRegion )
{
  CCI_INSTANCE( ImageMap )

  IMAGEMAP_ENSURE_BUILT()

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    if(CCI_P(label)==-1) {  // Get background region
      daim::create_roi(_This->Map,_This->NodePartition.bind(std::bind2nd(std::equal_to<dm_int>(),0)),
                       _This->tmpRoi);
    }
    else
    {
      ImageMap_UpdatePartition(_This);
      if(CCI_P(label)==0)
      {
        if(CCI_P(include_holes)) {
           ImageMap_UpdatePartRoi(_This);
           _This->tmpRoi = _This->PartRoi;
        }
        else {
          ImageMap_UpdateRoi(_This);
          _This->tmpRoi = _This->Roi;
        }
      }
      else
      {
        IMAGEMAP_VERIFY_LABEL(CCI_P(label))

        if(CCI_P(include_holes))
        {
          _This->Labels.clear();

          XNODE n = &_PARTITION_NODE(_This,CCI_P(label));
          ImageMap_CollectChildLabels(_This,n,true);

          daim::basic_partition _part = _This->Partition;
          int lbl = daim::merge_labels(_part,_This->Labels,CCI_P(label));
          daim::create_roi(
            _This->Map,_part.bind(std::bind2nd(std::equal_to<dm_int>(),lbl)),
            _This->tmpRoi);
        }
        else
        {
          daim::create_roi(
            _This->Map,
            _This->Partition.bind(std::bind2nd(std::equal_to<dm_int>(),CCI_P(label))),
            _This->tmpRoi);
        }
      }
    }

    _IParams.SetMask(&_This->tmpRoi);
    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method : Clear
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageMap, Clear )
{
  CCI_INSTANCE( ImageMap )

  if(CCI_P(label) > 0)
  {
    IMAGEMAP_ENSURE_BUILT()
    IMAGEMAP_VERIFY_LABEL(CCI_P(label))

    XNODE node = &_PARTITION_NODE(_This,CCI_P(label));
    ImageMap_ClearNode(_This,node);

    _This->NodePartition.update();
    _This->Update &= ~IMAGEMAP_UPDATE_MAP;
  }
  else
  {
    _This->Roi.KillRoi();

    _This->Update        = 0;
    _This->NumPartitions = 0;
    _This->Depth         = 0;

    _This->Labels.clear();
    _This->IndexTable.clear();

    _This->NodeTable.Clear();
    _This->NodeList.Clear();

    _This->Partition.reserve(1);
    _This->NodePartition.reserve(1);

    _This->Built = dm_false;

  }
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method : GetLabels
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageMap, GetLabels )
{
  CCI_INSTANCE( ImageMap )

  CCI_RETVAL_P(count)  = 0;
  CCI_RETVAL_P(labels) = NULL;

  IMAGEMAP_ENSURE_BUILT()

  ImageMap_UpdatePartition(_This);

  _This->Labels.clear();

  const dmRegion* rgn = CCI_P(rgn);

  if(rgn!=NULL)
  {
    if(rgn->IsEmptyRoi())
      CCI_RETURN_INVALID_PARAM()

    daim::get_overlapping_regions_labels2(*rgn,
                         _This->Partition,
                         _This->Map,
                         _This->Labels);

    //daim::labels_array_type::iterator it = daim::compact_labels( _This->Labels );
    //_This->Labels.resize( it - _This->Labels.begin() );
  }
  else
  {
    info_list_type::iterator it   = _This->NodeList.Begin();
    info_list_type::iterator last = _This->NodeList.End();

    for(;it!= last;++it) {
      if((*it).ri_Status==RI_REGION)
        _This->Labels.push_back(_This->Partition[(*it).ri_Part]);
    }
  }

  CCI_RETVAL_P(count)  =  _This->Labels.size();
  CCI_RETVAL_P(labels) = &_This->Labels[0];

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: GetLabelFromPoint
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageMap, GetLabelFromPoint )
{
  CCI_INSTANCE( ImageMap )

  IMAGEMAP_ENSURE_BUILT()

  dmPoint p(CCI_P(x),CCI_P(y));
  if(!dmPointInRectangle(p,_This->Map.rect()))
    CCI_RETURN_FAIL()

  ImageMap_UpdatePartition(_This);
  CCI_RETVAL_P(label) = _This->Partition[_This->Map[p]];

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: FillHoles
//---------------------------------------------------------------------
static CCI_IMPL_METHOD_NP( ImageMap, FillHoles )
{
  CCI_INSTANCE( ImageMap )

  IMAGEMAP_ENSURE_BUILT()

  XNODE root = &_This->NodeTable[0];

  dmDEBUG_ASSERT( root->ri_Part == 0 );

  dmList::iterator it   = root->ri_ChildList.Begin();
  dmList::iterator last = root->ri_ChildList.End();

  for(XNODE child;it!=last;++it)
  {
    child = static_cast<_RegionLink*>(*it)->rl_Self;
    dmDEBUG_ASSERT( child->ri_Father == 0 );
    ImageMap_MergeChilds( _This, child );
  }

  _This->NodePartition.update();
  _This->Update &= ~(IMAGEMAP_UPDATE_MAP|IMAGEMAP_UPDATE_CENTROID);

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: CleanBorders
//---------------------------------------------------------------------
static CCI_IMPL_METHOD_NP( ImageMap, CleanBorders )
{
  CCI_INSTANCE( ImageMap )

  IMAGEMAP_ENSURE_BUILT()

  dmRect r = _This->Map.rect();
  _This->tmpRoi.SetRectRoi(r);

  SHRINK_RECT(r);   // Take care of thin regions

  _This->tmpRoi.SubCoordinates(r);

  ImageMap_UpdatePartition(_This);

  _This->Labels.clear();

  //------------------------------
  // Get all objects
  //------------------------------
  daim::get_overlapping_regions_labels(
           _This->tmpRoi,
           _This->Partition,
           _This->Map,
           _This->Labels);

  daim::labels_array_type::iterator it = daim::compact_labels( _This->Labels );
  _This->Labels.resize( it - _This->Labels.begin() );

  ImageMap_RemoveLabels( _This );

  _This->Update &= ~(IMAGEMAP_UPDATE_MAP|IMAGEMAP_UPDATE_CENTROID);


  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: CleanRegionBorders
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageMap, CleanRegionBorders )
{
  CCI_INSTANCE( ImageMap )

  IMAGEMAP_ENSURE_BUILT()

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmRegion* roi = _IParams.GetRoi();
    if(roi && !roi->IsEmptyRoi())
    {
      dmRect   r = _This->Map.rect();

      dmRegion outline;

      _This->tmpRoi = *roi;
      _This->tmpRoi.GetOutLine(outline);
      _This->tmpRoi.SubRoi(outline);
      _This->tmpRoi.XorCoordinates(r);
  /*
      if( _This->tmpRoi.IsEmptyRoi())
      {
        _This->tmpRoi.SetRectRoi(r);

        SHRINK_RECT(r);   // Take care of thin regions
        _This->tmpRoi.SubCoordinates(r);
      }
   */
      ImageMap_UpdatePartition(_This);

      _This->Labels.clear();

      //------------------------------
      // Get all objects
      //------------------------------
      daim::get_overlapping_regions_labels(
               _This->tmpRoi,
               _This->Partition,
               _This->Map,
               _This->Labels);

      daim::labels_array_type::iterator it = daim::compact_labels( _This->Labels );
      _This->Labels.resize( it - _This->Labels.begin() );

      ImageMap_RemoveLabels( _This );

      _This->Update &= ~(IMAGEMAP_UPDATE_MAP|IMAGEMAP_UPDATE_CENTROID);
    }
    CCI_RETURN_OK()

  } else
    CCI_SET_ERROR_MSG(_TXT("Invalid parameters !"));

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: RemoveLabels
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageMap, RemoveLabels )
{
  CCI_INSTANCE( ImageMap )

  IMAGEMAP_ENSURE_BUILT()

  if(CCI_P(count) && CCI_P(labels)!=NULL)
  {
    daim::labels_array_type& labels = _This->Labels;

    if(CCI_P(labels) != &labels[0]) {
      labels.resize(CCI_P(count));
      std::copy(CCI_P(labels),CCI_P(labels)+CCI_P(count),labels.begin());
    }

    daim::labels_array_type::iterator it   = labels.begin();
    daim::labels_array_type::iterator last = labels.end();

    for(;it!=last;++it) *it = _This->Partition[*it];

    ImageMap_RemoveLabels( _This );

    _This->Update &= ~(IMAGEMAP_UPDATE_MAP|IMAGEMAP_UPDATE_CENTROID);

    CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: SelectLabels
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageMap, SelectLabels )
{
  CCI_INSTANCE( ImageMap )

  IMAGEMAP_ENSURE_BUILT()

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    if(CCI_P(count) && CCI_P(labels)!=NULL)
    {
      daim::labels_array_type& labels = _This->Labels;

      if(CCI_P(labels) != &labels[0]) {
        labels.resize(CCI_P(count));
        std::copy(CCI_P(labels),CCI_P(labels)+CCI_P(count),labels.begin());
      }

      daim::basic_partition _part = _This->Partition;
      int lbl = daim::merge_labels(_part,labels,labels[0]);

      daim::create_roi(
       _This->Map,_part.bind(std::bind2nd(std::equal_to<dm_int>(),lbl)),
       _This->tmpRoi);

       _IParams.SetMask(&_This->tmpRoi);

      CCI_RETURN_OK()
    }
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: GetProperties
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageMap, GetProperties )
{
  CCI_INSTANCE( ImageMap )

  IMAGEMAP_ENSURE_BUILT()
  IMAGEMAP_VERIFY_LABEL(CCI_P(label))

  CCI_RETVAL_P(area)   = 0;
  CCI_RETVAL_P(dir)    = 0;
  CCI_RETVAL_P(cx)     = 0;
  CCI_RETVAL_P(cy)     = 0;
  CCI_RETVAL_P(l1)     = 0;
  CCI_RETVAL_P(l2)     = 0;
  CCI_RETVAL_P(points) = NULL;
  CCI_RETVAL_P(count)  = 0;
  CCI_RETVAL_P(euler)  = 0;
  CCI_RETVAL_P(depth)  = 0;

  _RegionInfo& rInfo = _PARTITION_NODE(_This, CCI_P(label));

  if(rInfo.ri_Status==RI_REGION)
  {
    CCI_RETVAL_P(depth) = rInfo.ri_Depth;
    CCI_RETVAL_P(holes) = rInfo.ri_Childs;

    if(CCI_P(flags) & CCIV_ImageMap_Area) {
      ImageMap_UpdateCount(_This);
      CCI_RETVAL_P(area) = rInfo.ri_Count;
    }

    if(CCI_P(flags) & CCIV_ImageMap_Loc) {
      ImageMap_UpdateCentroids(_This);
      CCI_RETVAL_P(cx) = static_cast<int>(daim::round(rInfo.ri_Centroid_x));
      CCI_RETVAL_P(cy) = static_cast<int>(daim::round(rInfo.ri_Centroid_y));
    }

    if(CCI_P(flags) & CCIV_ImageMap_Shape)  {
      ImageMap_UpdateBoundary(_This);
      CCI_RETVAL_P(points) = rInfo.ri_Boundary.Begin();
      CCI_RETVAL_P(count)  = rInfo.ri_Boundary.Size();
    }

    if(CCI_P(flags) & CCIV_ImageMap_Dir)  {
      ImageMap_UpdateDir(_This);
      CCI_RETVAL_P(dir) = rInfo.ri_Theta;
    }

    if(CCI_P(flags) & CCIV_ImageMap_Rect) {
      ImageMap_UpdateRectProps(_This);
      CCI_RETVAL_P(l1) = rInfo.ri_L1;
      CCI_RETVAL_P(l2) = rInfo.ri_L2;
    }

    if(CCI_P(flags) & CCIV_ImageMap_Euler) {
      ImageMap_UpdateEuler(_This);
      CCI_RETVAL_P(euler) = rInfo.ri_Euler;
    }

    CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: GetChildRegions
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageMap, GetChildRegions )
{
  CCI_INSTANCE( ImageMap )

  IMAGEMAP_ENSURE_BUILT()
  IMAGEMAP_VERIFY_LABEL(CCI_P(label))

  _RegionInfo& rInfo = _PARTITION_NODE(_This, CCI_P(label));
  if(rInfo.ri_Status==RI_REGION)
  {
    daim::labels_array_type& labels = _This->Labels;
    labels.clear();

    ImageMap_CollectChildLabels(_This,&rInfo,false);

    CCI_RETVAL_P(labels) = &labels[0];
    CCI_RETVAL_P(count)  = labels.size();
    CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: GetIndexTable
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageMap, GetIndexTable )
{
  CCI_INSTANCE( ImageMap )

  IMAGEMAP_ENSURE_BUILT()

  ImageMap_BuildIndexTable(_This);

  CCI_RETVAL_P(table) = &_This->Labels[0];
  CCI_RETVAL_P(count) =  _This->Labels.size();

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: SetCalibration
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageMap, SetCalibration )
{
  CCI_INSTANCE( ImageMap )

  _This->aspect_ratio = CCI_P(ar);
  _This->uppxls       = CCI_P(uppx);

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: Reconstruct
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageMap, Reconstruct )
{
  CCI_INSTANCE( ImageMap )

  IMAGEMAP_ENSURE_BUILT()

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmRegion* rgn = _IParams.GetMask();
    if(rgn && !rgn->IsEmptyRoi())
    {
      ImageMap_UpdatePartition(_This);

      _This->Labels.clear();
      _This->tmpRoi.KillRoi();

      int index = daim::get_overlapping_regions_labels(*rgn,
                            _This->Partition,
                            _This->Map,
                            _This->Labels);

      daim::basic_partition _part = _This->Partition;

      int lbl = daim::merge_labels(_part,_This->Labels,index);
      daim::create_roi(_This->Map,_part.bind(std::bind2nd(std::equal_to<dm_int>(),lbl)),
                       _This->tmpRoi);

      _IParams.SetMask(&_This->tmpRoi);

      CCI_RETURN_OK()
    }
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: RemoveLabelArray
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageMap, RemoveLabelArray )
{
  CCI_INSTANCE( ImageMap )

  IMAGEMAP_ENSURE_BUILT()

  dmIStorageMap _IStorage;
  if(_IStorage.QueryInterface(CCI_P(storage)))
  {
    if(ImageMap_CopyLabelArray(_This,_IStorage,CCI_P(column))>0)
    {
       ImageMap_RemoveLabels( _This );
       _This->Update &= ~(IMAGEMAP_UPDATE_MAP|IMAGEMAP_UPDATE_CENTROID);

       CCI_RETURN_OK()
    }
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: SelectLabelArray
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageMap, SelectLabelArray )
{
  CCI_INSTANCE( ImageMap )

  IMAGEMAP_ENSURE_BUILT()

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmIStorageMap _IStorage;
    if(_IStorage.QueryInterface(CCI_P(storage)))
    {
      if(ImageMap_CopyLabelArray(_This,_IStorage,CCI_P(column))>0)
      {
        daim::basic_partition    _part  = _This->Partition;
        daim::labels_array_type& labels = _This->Labels;

        int lbl = daim::merge_labels(_part,labels,labels[0]);

        daim::create_roi(
          _This->Map,_part.bind(std::bind2nd(std::equal_to<dm_int>(),lbl)),
          _This->tmpRoi);
          _IParams.SetMask(&_This->tmpRoi);

        CCI_RETURN_OK()
      }
    }
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: RegionProps
//---------------------------------------------------------------------

  /* Attributs for Region descriptors to be used in Compute methods */

  #define CCIA_RgnDesc_Labels       CCI_BASE+1L
  #define CCIA_RgnDesc_Border       CCI_BASE+2L
  #define CCIA_RgnDesc_Boundary     CCI_BASE+3L
  #define CCIA_RgnDesc_Area         CCI_BASE+4L
  #define CCIA_RgnDesc_Dir          CCI_BASE+5L
  #define CCIA_RgnDesc_Euler        CCI_BASE+6L
  #define CCIA_RgnDesc_CX           CCI_BASE+7L
  #define CCIA_RgnDesc_CY           CCI_BASE+8L
  #define CCIA_RgnDesc_L1           CCI_BASE+9L
  #define CCIA_RgnDesc_L2           CCI_BASE+10L
  #define CCIA_RgnDesc_Moment       CCI_BASE+11L
  #define CCIA_RgnDesc_Depth        CCI_BASE+12L
  #define CCIA_RgnDesc_Holes        CCI_BASE+13L
  #define CCIA_RgnDesc_ParentLabels CCI_BASE+14L
  #define CCIA_RgnDesc_PosX         CCI_BASE+15L
  #define CCIA_RgnDesc_PosY         CCI_BASE+16L

  #define CCIA_ImgDesc_BASE         CCI_BASE+64L
  #define CCIA_ImgDesc_TotalDensity CCIA_ImgDesc_BASE+2L
  #define CCIA_ImgDesc_MinDensity   CCIA_ImgDesc_BASE+3L
  #define CCIA_ImgDesc_MaxDensity   CCIA_ImgDesc_BASE+4L
  #define CCIA_ImgDesc_MeanDensity  CCIA_ImgDesc_BASE+5L

  #define CCIA_ImgDesc_TotalDensity2 CCIA_ImgDesc_BASE+6L


#define IMAGEMAP_GET_PROPERTY( tg, args )                \
    item = _IProps.GetPropertyStr(key,NULL,args,NULL);   \
    if(item) {                                           \
       tagItems[count].ti_Tag  = tg;                     \
       tagItems[count].ti_Data = dm_ptr_to_long(item);   \
       ++count;                                          \
    }

static CCI_IMPL_METHOD( ImageMap, RegionProps )
{
  CCI_INSTANCE( ImageMap )


  IMAGEMAP_ENSURE_BUILT()

  //__dm_DebugBreak__

  dm_param key = CCI_P(key);

  dmIPropertyList _IProps;
  if(!_IProps.QueryInterface(CCI_P(props)))
    CCI_RETURN_INVALID_PARAM()

  dmIStorageMap _IStorage;
  if(!_IStorage.QueryInterface(CCI_P(store)))
    CCI_RETURN_INVALID_PARAM()

  ImageMap_UpdateRoi(_This);
  ImageMap_BuildIndexTable(_This);

  if(_This->NumPartitions<=0) // Numpartition is the number of objects
      return 0; // No objects

  //index_table_type&      _itable    = _This->IndexTable;
  //daim::basic_partition& _partition = _This->Partition;


  //==============================================================
  const char_t* item;
  tg_TagItem tagItems[20] = {0};
  dm_uint    count = 0;

  IMAGEMAP_GET_PROPERTY(CCIA_RgnDesc_Labels  ,"CCIA_RgnDesc_Labels");
  IMAGEMAP_GET_PROPERTY(CCIA_RgnDesc_Border  ,"CCIA_RgnDesc_Border");
  IMAGEMAP_GET_PROPERTY(CCIA_RgnDesc_Boundary,"CCIA_RgnDesc_Boundary");
  IMAGEMAP_GET_PROPERTY(CCIA_RgnDesc_Area    ,"CCIA_RgnDesc_Area");
  IMAGEMAP_GET_PROPERTY(CCIA_RgnDesc_Dir     ,"CCIA_RgnDesc_Dir");
  IMAGEMAP_GET_PROPERTY(CCIA_RgnDesc_Euler   ,"CCIA_RgnDesc_Euler");
  IMAGEMAP_GET_PROPERTY(CCIA_RgnDesc_CX      ,"CCIA_RgnDesc_CX");
  IMAGEMAP_GET_PROPERTY(CCIA_RgnDesc_CY      ,"CCIA_RgnDesc_CY");
  IMAGEMAP_GET_PROPERTY(CCIA_RgnDesc_L1      ,"CCIA_RgnDesc_L1");
  IMAGEMAP_GET_PROPERTY(CCIA_RgnDesc_L2      ,"CCIA_RgnDesc_L2");
  IMAGEMAP_GET_PROPERTY(CCIA_RgnDesc_Depth   ,"CCIA_RgnDesc_Depth");
  IMAGEMAP_GET_PROPERTY(CCIA_RgnDesc_Holes   ,"CCIA_RgnDesc_Holes");

  IMAGEMAP_GET_PROPERTY(CCIA_RgnDesc_ParentLabels,"CCIA_RgnDesc_ParentLabels");

//IMAGEMAP_GET_PROPERTY(CCIA_RgnDesc_Moment  ,"CCIA_RgnDesc_Moment");

  IMAGEMAP_GET_PROPERTY(CCIA_ImgDesc_MinDensity   ,"CCIA_ImgDesc_MinDensity");
  IMAGEMAP_GET_PROPERTY(CCIA_ImgDesc_MaxDensity   ,"CCIA_ImgDesc_MaxDensity");
  IMAGEMAP_GET_PROPERTY(CCIA_ImgDesc_MeanDensity  ,"CCIA_ImgDesc_MeanDensity");

  IMAGEMAP_GET_PROPERTY(CCIA_ImgDesc_TotalDensity ,"CCIA_ImgDesc_TotalDensity?n=1");
  IMAGEMAP_GET_PROPERTY(CCIA_ImgDesc_TotalDensity2,"CCIA_ImgDesc_TotalDensity?n=2");

  IMAGEMAP_GET_PROPERTY(CCIA_RgnDesc_PosX      ,"CCIA_RgnDesc_PosX");
  IMAGEMAP_GET_PROPERTY(CCIA_RgnDesc_PosY      ,"CCIA_RgnDesc_PosY");

  tagItems[count].ti_Tag  = 0;
  tagItems[count].ti_Data = 0;

  //==============================================================

  //--------------------------------------
  // Compute and add results in storage
  //--------------------------------------

  _This->Storage.Resize( _This->NumPartitions + 1 ); // XXX Numpartition is the number of objects
  dm_real* _results = _This->Storage.Begin();

  dmLink<dmImage>  _Image;
  dmIImage<dmPixelFormat32bppFloat>* _IImage = NULL;
  dmPoint offs(0,0);

  //-------------------------------------
  // Loop over tags
  //-------------------------------------

  cci_TagItem *tag,*tags = tagItems;

  while( (tag = TG_kNextItem(&tags)) )
  {
    bool bRes = false;

    item = reinterpret_cast<char*>(tag->ti_Data);

    if(tag->ti_Tag>CCIA_ImgDesc_BASE)
    {
      if(_IImage==NULL)
      {
        _IImage = ImageProp_PrepareImage(_This,CCI_P(params),_Image,offs);
        if(_IImage==NULL)
           CCI_RETURN_FAIL()
      }

      switch(tag->ti_Tag)
      {
        case CCIA_ImgDesc_MinDensity    : bRes = ImageProp_ComputeMinDensity (_This,*_IImage,_results,offs);break;
        case CCIA_ImgDesc_MaxDensity    : bRes = ImageProp_ComputeMaxDensity (_This,*_IImage,_results,offs);break;
        case CCIA_ImgDesc_MeanDensity   : bRes = ImageProp_ComputeMeanDensity(_This,*_IImage,_results,offs);break;

        case CCIA_ImgDesc_TotalDensity  : bRes = ImageProp_ComputeDensity(_This,*_IImage,1,_results,offs); break;
        case CCIA_ImgDesc_TotalDensity2 : bRes = ImageProp_ComputeDensity(_This,*_IImage,2,_results,offs); break;

        default : break;
      }
    }

    else

    switch(tag->ti_Tag)
    {
      case CCIA_RgnDesc_Labels      : bRes = ImageMap_ComputeLabels      (_This,_results)    ; break;
      case CCIA_RgnDesc_Border      : bRes = ImageMap_ComputeBorder      (_This,_results)    ; break;
      case CCIA_RgnDesc_Boundary    : bRes = ImageMap_ComputeBoundary    (_This,_results)    ; break;
      case CCIA_RgnDesc_Area        : bRes = ImageMap_ComputeArea        (_This,_results)    ; break;
      case CCIA_RgnDesc_Dir         : bRes = ImageMap_ComputeDir         (_This,_results)    ; break;
      case CCIA_RgnDesc_Euler       : bRes = ImageMap_ComputeEuler       (_This,_results)    ; break;
      case CCIA_RgnDesc_CX          : bRes = ImageMap_ComputeMomentum    (_This,_results,1,0); break;
      case CCIA_RgnDesc_CY          : bRes = ImageMap_ComputeMomentum    (_This,_results,0,1); break;
      case CCIA_RgnDesc_L1          : bRes = ImageMap_ComputeRectProps   (_This,_results,1)  ; break;
      case CCIA_RgnDesc_L2          : bRes = ImageMap_ComputeRectProps   (_This,_results,2)  ; break;
      case CCIA_RgnDesc_Depth       : bRes = ImageMap_ComputeDepth       (_This,_results)    ; break;
      case CCIA_RgnDesc_Holes       : bRes = ImageMap_ComputeHoles       (_This,_results)    ; break;
      case CCIA_RgnDesc_ParentLabels: bRes = ImageMap_ComputeParentLabels(_This,_results)    ; break;
      case CCIA_RgnDesc_PosX        : bRes = ImageMap_ComputePosition    (_This,_results,1)  ; break;
      case CCIA_RgnDesc_PosY        : bRes = ImageMap_ComputePosition    (_This,_results,2)  ; break;
      default : break;
    }

    // Store result in storage-map object
    if(bRes)
    {
       // XXX Numpartition is the number of objects
       dm_real* new_store = _IStorage.GetCol(item,_This->NumPartitions);
       if(new_store) {
          std::copy(_This->Storage.Begin()+1,_This->Storage.End(),new_store+1);
       } else {
         CCI_SET_ERROR_MSG(_TXT("Failed to allocate storage !"));
         CCI_RETURN_FAIL()
       }
     }
  }
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( ImageMap )
  CCI_REGISTER_METHOD_PP(ImageMap, Build              )
  CCI_REGISTER_METHOD_PP(ImageMap, CreateRegion       )
  CCI_REGISTER_METHOD_PP(ImageMap, Clear              )
  CCI_REGISTER_METHOD_PP(ImageMap, GetLabels          )
  CCI_REGISTER_METHOD_PP(ImageMap, GetLabelFromPoint  )
  CCI_REGISTER_METHOD_NP(ImageMap, FillHoles          )
  CCI_REGISTER_METHOD_NP(ImageMap, CleanBorders       )
  CCI_REGISTER_METHOD_PP(ImageMap, CleanRegionBorders )
  CCI_REGISTER_METHOD_PP(ImageMap, RemoveLabels       )
  CCI_REGISTER_METHOD_PP(ImageMap, SelectLabels       )
  CCI_REGISTER_METHOD_PP(ImageMap, GetProperties      )
  CCI_REGISTER_METHOD_PP(ImageMap, GetChildRegions    )
  CCI_REGISTER_METHOD_PP(ImageMap, GetIndexTable      )
  CCI_REGISTER_METHOD_PP(ImageMap, SetCalibration     )
  CCI_REGISTER_METHOD_PP(ImageMap, Reconstruct        )
  CCI_REGISTER_METHOD_PP(ImageMap, RegionProps        )
  CCI_REGISTER_METHOD_PP(ImageMap, RemoveLabelArray   )
  CCI_REGISTER_METHOD_PP(ImageMap, SelectLabelArray   )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( ImageMap )
{
  CCI_INVOKE_FACTORY( ImageMap )
}
//---------------------------------------------------------------------
