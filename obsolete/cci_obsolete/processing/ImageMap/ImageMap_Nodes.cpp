
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
 
#define _DEBUG_IMAGEMAP
//---------------------------------------------------------
struct ImageMap_Adjacency
{
  CCI_INSTANCE_PTR(ImageMap) _This;

  ImageMap_Adjacency( 
       CCI_INSTANCE_PTR(ImageMap) _Instance
     ) 
  :_This(_Instance)
  {}
 
  //----------------------------------------------------------------
  // If pi and pj are partition labels and pi > pj then 
  // region corresponding to pi is included in pj, this is an effect
  // of the way that partition are built from the original region.
  //----------------------------------------------------------------

  bool insert( int pi, int pj ) 
  {
    if(pi>pj) 
    {
      // get child node
      _RegionInfo& _node = _NODE(_This,pi);
      if(_node.ri_Father==_NULL_NODE)
      {
        _node.ri_Father=pj;
        _node.ri_Part  =pi;
        return true;
      }
      #ifdef _DEBUG_IMAGEMAP
        else {
          dmASSERT( _node.ri_Father==pj );
          dmASSERT( _node.ri_Part  ==pi );
        }
      #endif // _DEBUG_IMAGEMAP
    }
    return false;
  }

  // In this particular case we have only to consider 4-connectivity

  void operator()( daim::map_type::const_line_type _in , long x1, long x2 )
  {
    const daim::map_type::value_type* p1 = _in[1]; // lower line
    const daim::map_type::value_type* p0 = _in[0];

    daim::basic_partition& _part = _This->NodePartition;

    int u;
    for(long x=x1; x<x2;++x)
    {
      u = _part[p0[x]];
      if(!insert(u,_part[p0[x+1]]))
          insert(u,_part[p1[x]]  );
    }

    // Right border
    u =  _part[p0[x2]];
    insert(u,0);
  }  
};
//------------------------------------------------------------
void ImageMap_InsertNodes( CCI_INSTANCE_PTR(ImageMap) _This )
{
   dmRect r = _This->Map.rect();
   r.Resize(0,0,0,-1);

   ImageMap_Adjacency functor(_This);

   daim::apply(r,_This->Map,functor);
   
   //------------------------
   // Handle the last line       
   //------------------------
   daim::basic_partition&        _part =  _This->NodePartition;
   const daim::map_type::value_type* p = *_This->Map.last();

   for(int u,w=r.Width(),x=0;x<w;++x) {
      u = _part[p[x]];
      functor.insert(u,0);
   }
}
//------------------------------------------------------------
bool ImageMap_CreateNodes( CCI_INSTANCE_PTR(ImageMap) _This )
{
   ImageMap_InsertNodes(_This);
   
   dmOList<_RegionInfo>& thisNodeList = _This->NodeList;

   thisNodeList.Clear();

   info_table_type::iterator it   = _This->NodeTable.Begin(); 
   info_table_type::iterator last = _This->NodeTable.End(); 
   
   // Ensure that node at partition 0 has no father
   #ifdef _DEBUG_IMAGEMAP
      dmASSERT( (*it).ri_Father == _NULL_NODE );
   #endif   
   
   (*it).ri_Status = RI_EMPTY;
   (*it).ri_Depth  = 0;
   (*it).ri_Part   = 0;
   (*it).ri_Childs = 0;

   _RegionInfo *_pfather,*_pnode;

   thisNodeList.Push_Back( &(*it) );

   for(++it;it!=last;++it) 
   {
     #ifdef _DEBUG_IMAGEMAP
      dmASSERT( (*it).ri_Part     != _NULL_NODE );
      dmASSERT( (*it).ri_Father   != _NULL_NODE );
     #endif   
   
     _pnode   = &(*it);
     _pfather = &_PARTITION_NODE(_This,_pnode->ri_Father);

     #ifdef _DEBUG_IMAGEMAP
       dmASSERT( _pfather->ri_Status != RI_NONE);
     #endif   

     _pnode->ri_Status = (_pfather->ri_Status == RI_REGION ? RI_EMPTY:RI_REGION);
     _pnode->ri_Depth  = _pfather->ri_Depth + 1;

     // Insert child node into father list 
     _pfather->ri_ChildList.Push_Back(&_pnode->ri_ChildNode);
     _pfather->ri_Childs++;

     thisNodeList.Push_Back( _pnode );
   }

   return true;
}
//--------------------------------------------------------------
// Update partition for objects
//--------------------------------------------------------------
static int ImageMap_UpdatePartition( CCI_INSTANCE_PTR(ImageMap) _This ) 
{
  if( (_This->Update & IMAGEMAP_UPDATE_PARTITION)==0 )
  {
    daim::basic_partition& part = _This->Partition;
    part = _This->NodePartition;

    info_list_type::iterator it   = _This->NodeList.Begin();
    info_list_type::iterator last = _This->NodeList.End();
    XNODE pNode;

    dm_int depth = 0;

    for(;it!= last;++it) 
    {
      pNode = &(*it);
      if(pNode->ri_Status==RI_EMPTY) 
         part.resolve(pNode->ri_Part,0);
      else {
         if(pNode->ri_Depth > depth)
            depth = pNode->ri_Depth; 
      }
    }
    
    part.update();
    
    _This->Depth   = depth;
    _This->Update |= IMAGEMAP_UPDATE_PARTITION;
  }
  return _This->Depth;
}
//--------------------------------------------------------------
// Update ROI for objects
//--------------------------------------------------------------
static void ImageMap_UpdateRoi( CCI_INSTANCE_PTR(ImageMap) _This )
{
  if( (_This->Update & IMAGEMAP_UPDATE_PARTITION)==0 ||
      (_This->Update & IMAGEMAP_UPDATE_ROI      )==0 ) 
  {
    ImageMap_UpdatePartition(_This);

    // If region for objects+holes is valid, use it as conditional region
    if((_This->Update & IMAGEMAP_UPDATE_PARTROI) !=0) 
    {  
      daim::create_rgnroi(
           _This->Map,
           _This->Partition.bind(std::bind2nd(std::not_equal_to<int>(),0)),
           _This->Roi,_This->PartRoi);
    } else 
      daim::create_roi(
           _This->Map,
           _This->Partition.bind(std::bind2nd(std::not_equal_to<int>(),0)),
           _This->Roi); 

    _This->Update|= IMAGEMAP_UPDATE_ROI;
  }
}
//--------------------------------------------------------------
// Update ROI for objects+holes
//--------------------------------------------------------------
static void ImageMap_UpdatePartRoi( CCI_INSTANCE_PTR(ImageMap) _This )
{
  if( (_This->Update & IMAGEMAP_UPDATE_PARTROI)==0) 
  {
    daim::create_roi(
      _This->Map,
      _This->NodePartition.bind(std::bind2nd(std::not_equal_to<int>(),0)),
      _This->PartRoi); 

      _This->Update |= IMAGEMAP_UPDATE_PARTROI;
  }
}

//--------------------------------------------------------------
// Update index table used for storing results
// results are indexed according to this table
//--------------------------------------------------------------
static int ImageMap_BuildIndexTable( CCI_INSTANCE_PTR(ImageMap) _This ) 
{
  if( (_This->Update & IMAGEMAP_UPDATE_ROI)        == 0 ||
      (_This->Update & IMAGEMAP_UPDATE_INDEXTABLE) == 0 )
 
  {
    ImageMap_UpdatePartition(_This);

    _This->NumPartitions = _This->Partition.get_index_table(_This->IndexTable) - 1;
    _This->Update       |= IMAGEMAP_UPDATE_INDEXTABLE;
  } 
  return _This->NumPartitions;
}
//--------------------------------------------------------------------------
// Recursively collect child labels
//--------------------------------------------------------------------------
static void ImageMap_CollectChildLabels( CCI_INSTANCE_PTR(ImageMap) _This, XNODE node, 
                                         bool include_holes )
{
  dmDEBUG_ASSERT( node != NULL );

  // Collect child borders length
  dmList::iterator it   = node->ri_ChildList.Begin();
  dmList::iterator last = node->ri_ChildList.End();

  for(XNODE child;it!=last;++it) 
  {
    child = static_cast<_RegionLink*>(*it)->rl_Self;

    if(include_holes || child->ri_Status==RI_REGION)
     _This->Labels.push_back(child->ri_Part);

    if(child->ri_Childs)
      ImageMap_CollectChildLabels(_This,child,include_holes);
  }
}
//--------------------------------------------------------------------------
// Recusively merge child nodes of a given node,
// Don't forget to update the partition !!
//--------------------------------------------------------------------------
static void ImageMap_MergeChilds(  CCI_INSTANCE_PTR(ImageMap) _This, XNODE node )
{
  dmDEBUG_ASSERT( node != NULL );

  while(!node->ri_ChildList.Empty())
  {
    XNODE child = static_cast<_RegionLink*>(node->ri_ChildList.Pop_Front())->rl_Self;

    if(child->ri_Childs)
       ImageMap_MergeChilds(_This,child);
   
    node->ri_Count  += child->ri_Count;
    node->ri_Childs -= 1;

    child->ri_Boundary.Clear();
    child->Remove(); // Remove from active list;

    _This->NodePartition.resolve(node->ri_Part,child->ri_Part);
  }

  #ifdef _DEBUG_IMAGEMAP
     dmASSERT( node->ri_Childs == 0 );
  #endif   
}
//--------------------------------------------------------------------------
// Merge a node with his father
//--------------------------------------------------------------------------
static bool ImageMap_MergeNode( CCI_INSTANCE_PTR(ImageMap) _This, XNODE node )
{
  dmDEBUG_ASSERT( node != NULL );

  XNODE father = &_PARTITION_NODE(_This,node->ri_Father);
  
  // Merge this node with father 
  
  father->ri_Count  += node->ri_Count;
  father->ri_ChildList.Remove( &node->ri_ChildNode );
  father->ri_Childs -= 1;

  node->ri_Boundary.Clear();
  node->Remove(); // Remove from active list;

  _This->NodePartition.resolve(father->ri_Part,node->ri_Part);

  // Reassign childs to new father
  while(!node->ri_ChildList.Empty())
  {
    XNODE child = reinterpret_cast<_RegionLink*>(node->ri_ChildList.Pop_Front())->rl_Self;
    father->ri_ChildList.Push_Back( &child->ri_ChildNode );
  }

  return true;
}
//--------------------------------------------------------------------------
// Clear a node :
// 1. Merge all its childs to this node
// 2. Merge this node to his father
//--------------------------------------------------------------------------
static bool ImageMap_ClearNode( CCI_INSTANCE_PTR(ImageMap) _This, XNODE node )
{
  dmDEBUG_ASSERT( node != NULL );

  if(node->ri_Childs) 
     ImageMap_MergeChilds(_This,node);

  return ImageMap_MergeNode(_This,node);
}
//--------------------------------------------------------------
static bool ImageMap_RemoveLabels( CCI_INSTANCE_PTR(ImageMap) _This )
{
  daim::labels_array_type::iterator it   = _This->Labels.begin();
  daim::labels_array_type::iterator last = _This->Labels.end(); 

  XNODE node;

  for(;it!=last;++it) 
  {
    node = &_PARTITION_NODE(_This,*it);
    ImageMap_ClearNode( _This, node );
  }

  _This->NodePartition.update();
  return true;
}
//--------------------------------------------------------------
