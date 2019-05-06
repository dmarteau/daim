#ifndef cciMeasures_h
#define cciMeasures_h
/* :::BEGIN LICENSE BLOCK:::
 *
 * Copyright (c) 2004-2010 David Marteau
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
// File         : cciMeasures.h
// Date         : 10 juin 2009
// Author       : David Marteau
//--------------------------------------------------------

#include "cciCOMPtr.h"
#include "cciIFilterContext.h"
#include "cciIRegionContainer.h"
#include "cciIImageContainer.h"

#define dmUseKernelImageTemplates
#include "daim_kernel.h"
#include "common/dmUserLib.h"

#include "cciIMeasures2.h"

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
  int ri_Status;  // RI_REGION|RI_EMPTY|RI_NONE
  int ri_Count;   // Size in pixels
  int ri_Childs;  // Number of child nodes
  int ri_Father;  // Father node partition index
  int ri_Depth;   // Invariant.
  int ri_Part;    // Invariant.

  dm_real ri_Border; // Length inner+outer borders
  dm_real ri_Theta;  // Invariant.
  dm_real ri_L1;     // Invariant.
  dm_real ri_L2;     // Invariant.

  dmPoly  ri_Boundary; // External boundary: Invariant.
  dm_real ri_Centroid_x;
  dm_real ri_Centroid_y;
  dm_real ri_Position_x;
  dm_real ri_Position_y;
  bool ri_pos;

  dm_real ri_fvalue;   // For intermediate storage

  _RegionLink ri_ChildNode; // Node linked to parent child list
  dmList      ri_ChildList; // List of child nodes

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


#ifdef _DEBUG
#define _DEBUG_IMAGEMAP
#endif

/* Header file */
class cciMeasurements final : public cciIMeasures2
{

friend class MapAdjacency;

public:
  CCI_DECL_ISUPPORTS
  CCI_DECL_IMEASUREMENTS
  CCI_DECL_IMEASURES2

  typedef dmIImage<dmPixelFormat32bppFloat> floatImage;
  
  cciMeasurements();

private:
  ~cciMeasurements();

protected:
  daim::cont_image<dm_int32> mMap;
  dmRegion                   mRoi;           // Roi representing all connexe regions elements
  info_table_type            mNodeTable;     // Indexed table of nodes
  index_table_type           mNodeIndexTable;
  daim::basic_partition      mNodePartition;
  daim::connectivity         mConnect;
  info_list_type             mNodeList;    // List of active nodes

  dmRegion                   mPartRoi;     // Roi for regions elements map (holes+regions)
  index_table_type           mIndexTable;  //
  daim::basic_partition      mPartition;
  daim::labels_array_type    mLabels;
  dmPoint                    mOffset;
  dm_uint                    mUpdate;
  dm_int                     mNumPartitions;
  dm_int                     mDepth;

  bool                       mBuilt;
  dmVector<dm_real>          mStorage;

  dmRegion  tmpRoi;

  bool  use_image_calibration;
  dm_real  aspect_ratio;
  dm_real  uppxls;

public:
  
  const daim::map_type&        Map()           const { return mMap;           }
  const index_table_type&      IndexTable()    const { return mIndexTable;    }
  const daim::basic_partition& NodePartition() const { return mNodePartition; }
  
  int          _INDEX( int part ) { return mNodeIndexTable[part];    }
  int          _PARTITION_INDEX( int part ) { return mNodeIndexTable[mNodePartition[part]]; }

  _RegionInfo& _NODE ( int part ) { return mNodeTable[_INDEX(part)]; }
  _RegionInfo& _PARTITION_NODE ( int part ) { return mNodeTable[_PARTITION_INDEX(part)]; }

  //dm_uint CopyLabelArray( dmIStorageMap& _IStorage, const char_t*  _Column );

protected:

  cci_result GetImage( dmImage* refImage, dmPoint& offs, dmLink<dmImage>& _retval );

  // Nodes manipulation
  void InsertNodes();
  bool CreateNodes();
  int  UpdatePartition(); 
  void UpdateRoi();
  void UpdatePartRoi();
  int  BuildIndexTable(); 
  void CollectChildLabels( XNODE node, bool include_holes );
  void MergeChilds( XNODE node );
  bool MergeNode( XNODE node );
  bool ClearNode( XNODE node );
  bool RemoveLabels();

  // Measures
  void M_UpdateCount();
  void M_UpdateCentroids(); 
  void M_UpdateEuler(); 
  void M_UpdateBoundary(); 
  bool M_ComputeArea(  dm_real* _Results );
  bool M_ComputeBoundary( dm_real* _Results );
  bool M_ComputeBorder( dm_real* _Results );
  bool M_ComputeMomentum( dm_real* _Results, int p, int q);
  bool M_ComputeLabels( dm_real* _Results );
  void M_UpdateDir(); 
  void M_UpdateRectProps(); 
  bool M_ComputeDir( dm_real* _Results );
  bool M_ComputeRectProps( dm_real* _Results, int l );
  bool M_ComputeEuler( dm_real* _Results  ); 
  bool M_ComputeDepth( dm_real* _Results  );
  bool M_ComputeHoles(  dm_real* _Results  ); 
  bool M_ComputeParentLabels(  dm_real* _Results  ); 
  bool M_ComputePosition( dm_real* _Results, int p );

  // Properties
  bool ComputeDensity( const floatImage& , dm_uint _Order, dm_real* _Results, const dmPoint& );
  bool ComputeMeanDensity( const floatImage& , dm_real* _Results, const dmPoint& );
  bool ComputeMinDensity( const floatImage&  , dm_real* _Results, const dmPoint& );
  bool ComputeMaxDensity( const floatImage&  , dm_real* _Results, const dmPoint& ); 
};

#endif /* cciMeasures_h */
