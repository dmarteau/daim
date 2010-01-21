
#include "cciMeasures.h"

//====================================
// Nodes management
//====================================

class MapAdjacency
{
  cciMeasurements& _This;

public:
  MapAdjacency(cciMeasurements& _Instance) 
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
      _RegionInfo& _node = _This._NODE(pi);
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

    daim::basic_partition& _part = _This.mNodePartition;

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
void cciMeasurements::InsertNodes()
{
   dmRect r = mMap.rect();
   r.Resize(0,0,0,-1);

   MapAdjacency functor(*this);

   daim::apply(r,mMap,functor);
   
   //------------------------
   // Handle the last line       
   //------------------------
   daim::basic_partition&        _part =  mNodePartition;
   const daim::map_type::value_type* p = *mMap.last();

   for(int u,w=r.Width(),x=0;x<w;++x) {
      u = _part[p[x]];
      functor.insert(u,0);
   }
}
//------------------------------------------------------------
bool cciMeasurements::CreateNodes()
{
   InsertNodes();
   
   dmOList<_RegionInfo>& thisNodeList = mNodeList;

   thisNodeList.Clear();

   info_table_type::iterator it   = mNodeTable.Begin(); 
   info_table_type::iterator last = mNodeTable.End(); 
   
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
     _pfather = &_PARTITION_NODE(_pnode->ri_Father);

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
int cciMeasurements::UpdatePartition() 
{
  if( (mUpdate & IMAGEMAP_UPDATE_PARTITION)==0 )
  {
    daim::basic_partition& part = mPartition;
    part = mNodePartition;

    info_list_type::iterator it   = mNodeList.Begin();
    info_list_type::iterator last = mNodeList.End();
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
    
    mDepth   = depth;
    mUpdate |= IMAGEMAP_UPDATE_PARTITION;
  }
  return mDepth;
}
//--------------------------------------------------------------
// Update ROI for objects
//--------------------------------------------------------------
void cciMeasurements::UpdateRoi()
{
  if( (mUpdate & IMAGEMAP_UPDATE_PARTITION)==0 ||
      (mUpdate & IMAGEMAP_UPDATE_ROI      )==0 ) 
  {
    UpdatePartition();

    // If region for objects+holes is valid, use it as conditional region
    if((mUpdate & IMAGEMAP_UPDATE_PARTROI) !=0) 
    {  
      daim::create_rgnroi(
           mMap,
           mPartition.bind(std::bind2nd(std::not_equal_to<int>(),0)),
           mRoi,mPartRoi);
    } else 
      daim::create_roi(
           mMap,
           mPartition.bind(std::bind2nd(std::not_equal_to<int>(),0)),
           mRoi); 

    mUpdate|= IMAGEMAP_UPDATE_ROI;
  }
}
//--------------------------------------------------------------
// Update ROI for objects+holes
//--------------------------------------------------------------
void cciMeasurements::UpdatePartRoi()
{
  if( (mUpdate & IMAGEMAP_UPDATE_PARTROI)==0) 
  {
    daim::create_roi(
      mMap,
      mNodePartition.bind(std::bind2nd(std::not_equal_to<int>(),0)),
      mPartRoi); 

      mUpdate |= IMAGEMAP_UPDATE_PARTROI;
  }
}

//--------------------------------------------------------------
// Update index table used for storing results
// results are indexed according to this table
//--------------------------------------------------------------
int cciMeasurements::BuildIndexTable() 
{
  if( (mUpdate & IMAGEMAP_UPDATE_ROI)        == 0 ||
      (mUpdate & IMAGEMAP_UPDATE_INDEXTABLE) == 0 )
 
  {
    UpdatePartition();

    mNumPartitions = mPartition.get_index_table(mIndexTable) - 1;
    mUpdate       |= IMAGEMAP_UPDATE_INDEXTABLE;
  } 
  return mNumPartitions;
}
//--------------------------------------------------------------------------
// Recursively collect child labels
//--------------------------------------------------------------------------
void cciMeasurements::CollectChildLabels( XNODE node, 
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
     mLabels.push_back(child->ri_Part);

    if(child->ri_Childs)
      CollectChildLabels(child,include_holes);
  }
}
//--------------------------------------------------------------------------
// Recusively merge child nodes of a given node,
// Don't forget to update the partition !!
//--------------------------------------------------------------------------
void cciMeasurements::MergeChilds(  XNODE node )
{
  dmDEBUG_ASSERT( node != NULL );

  while(!node->ri_ChildList.Empty())
  {
    XNODE child = static_cast<_RegionLink*>(node->ri_ChildList.Pop_Front())->rl_Self;

    if(child->ri_Childs)
       MergeChilds(child);
   
    node->ri_Count  += child->ri_Count;
    node->ri_Childs -= 1;

    child->ri_Boundary.Clear();
    child->Remove(); // Remove from active list;

    mNodePartition.resolve(node->ri_Part,child->ri_Part);
  }

  #ifdef _DEBUG_IMAGEMAP
     dmASSERT( node->ri_Childs == 0 );
  #endif   
}
//--------------------------------------------------------------------------
// Merge a node with his father
//--------------------------------------------------------------------------
bool cciMeasurements::MergeNode( XNODE node )
{
  dmDEBUG_ASSERT( node != NULL );

  XNODE father = &_PARTITION_NODE(node->ri_Father);
  
  // Merge this node with father 
  
  father->ri_Count  += node->ri_Count;
  father->ri_ChildList.Remove( &node->ri_ChildNode );
  father->ri_Childs -= 1;

  node->ri_Boundary.Clear();
  node->Remove(); // Remove from active list;

  mNodePartition.resolve(father->ri_Part,node->ri_Part);

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
bool cciMeasurements::ClearNode( XNODE node )
{
  dmDEBUG_ASSERT( node != NULL );

  if(node->ri_Childs) 
     MergeChilds(node);

  return MergeNode(node);
}
//--------------------------------------------------------------
bool cciMeasurements::RemoveLabels()
{
  daim::labels_array_type::iterator it   = mLabels.begin();
  daim::labels_array_type::iterator last = mLabels.end(); 

  XNODE node;

  for(;it!=last;++it) 
  {
    node = &_PARTITION_NODE(*it);
    ClearNode(node);
  }

  mNodePartition.update();
  return true;
}
//--------------------------------------------------------------

