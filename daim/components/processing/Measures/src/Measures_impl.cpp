
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


#include <cciMeasures.h>
#include <math.h>

//---------------------------------------------------------------------
// Compute Pixels Area 
//--------------------------------------------------------------
struct MapCount
{
  cciMeasurements& _This;

  MapCount( cciMeasurements& _Instance ) 
  :_This(_Instance)
  {}

  void operator()( int x ) { 
    ++_This._PARTITION_NODE(x).ri_Count;
  }

};
//--------------------------------------------------------------
void cciMeasurements::M_UpdateCount() 
{
  if((mUpdate & IMAGEMAP_UPDATE_COUNT)==0) 
  {
    UpdatePartRoi();

    info_list_type::iterator it   = mNodeList.Begin();
    info_list_type::iterator last = mNodeList.End();

    for(;it != last; ++it)
      (*it).ri_Count = 0;

    daim::for_each(mPartRoi,mMap,MapCount(*this));
    mUpdate |= IMAGEMAP_UPDATE_COUNT;
  } 
}
//---------------------------------------------------------------------
// Compute centroids
//--------------------------------------------------------------
struct MapCentroids
{ 
  cciMeasurements& _This;

  MapCentroids( cciMeasurements& _Instance ) 
  :_This(_Instance)
  {}
  
  void operator()( daim::map_type::const_line_type _in , long x1, long x2 )
  {
    daim::map_type::const_line_type start = _This.Map().begin();
    XNODE pNode;
 
    for(long x=x1;x<=x2;++x) 
    {
      pNode = &_This._PARTITION_NODE((*_in)[x]);
      pNode->ri_Centroid_x += x;
      pNode->ri_Centroid_y += _in-start;
      
      // Compute region position
      if(!pNode->ri_pos) {
          pNode->ri_Position_x = x;
          pNode->ri_Position_y = _in-start;
          pNode->ri_pos = true;
      }
    }
  }
};
//---------------------------------------------------------------------
void cciMeasurements::M_UpdateCentroids() 
{
  if((mUpdate & IMAGEMAP_UPDATE_CENTROID)==0) 
  {
    UpdateRoi();
    M_UpdateCount();

    info_list_type::iterator it   = mNodeList.Begin();
    info_list_type::iterator last = mNodeList.End();
    XNODE pNode;

    for(;it != last; ++it) {
      (*it).ri_Centroid_x = 0;
      (*it).ri_Centroid_y = 0;
      (*it).ri_pos = false;
    }

    daim::apply(mRoi,mMap,MapCentroids(*this));
    
    dm_real count;
    for(it = mNodeList.Begin();it != last; ++it)  
    {
      pNode = &(*it);
      if(pNode->ri_Status==RI_REGION) {
        count = pNode->ri_Count;
        pNode->ri_Centroid_x /= count;
        pNode->ri_Centroid_y /= count;
      } 
    } 
    mUpdate |= IMAGEMAP_UPDATE_CENTROID;
  } 
}
//---------------------------------------------------------------------
// Compute boundary 
//---------------------------------------------------------------------
class ImageMap_BndrySearch
{
  public:
   int lbl;
   const daim::basic_partition& part;

   ImageMap_BndrySearch( const daim::basic_partition& _part ) 
   :lbl(0)
   ,part(_part) {}

   bool operator()(int _map) {
     return part[_map]>=lbl;  // This ensure that pixel is INSIDE object 
                              // See problems on boundary search algorithm
   }
};
//--------------------------------------------------------------
class MapBoundary
{ 
  cciMeasurements&     _This;
  ImageMap_BndrySearch _bndsrch;
  dmRegion&            _r;

public:
  MapBoundary( cciMeasurements& _Instance, dmRegion& _region ) 
  :_This(_Instance)
  ,_bndsrch(_Instance.NodePartition())
  ,_r(_region)
  {}
  
  void operator()( daim::map_type::const_line_type _in , long x1, long x2 )
  {
    daim::map_type::const_line_type start = _This.Map().begin();
    const daim::basic_partition& part     = _This.NodePartition(); 

    XNODE pNode;

    for(int lbl,x=x1; x<=x2;++x) 
    {
      lbl = part[ (*_in)[x] ];
      pNode = &_This._NODE(lbl);
      if(lbl && pNode->ri_Boundary.Size()==0) 
      {
         _bndsrch.lbl = lbl;
         daim::extract_boundary(_bndsrch,
                      dmPoint(x,_in-start),_r,
                      _This.Map(), 
                      pNode->ri_Boundary
                    );

         if(!pNode->ri_Boundary.Closed()) 
             pNode->ri_Boundary.Push_Back(pNode->ri_Boundary[0]);
      }
    }
  }
};
//--------------------------------------------------------------
// Update Euler-Poincare number
//--------------------------------------------------------------
static void M_UpdateEuler1( XNODE root  ) 
{
  dmList::iterator it   = root->ri_ChildList.Begin();
  dmList::iterator last = root->ri_ChildList.End();

  root->ri_Euler = root->ri_Status == RI_REGION ? 1 : -1;

  for(XNODE child;it!=last;++it) 
  {
    child = static_cast<_RegionLink*>(*it)->rl_Self;

    M_UpdateEuler1( child );
    root->ri_Euler += child->ri_Euler;
  }
}
//--------------------------------------------------------------
void cciMeasurements::M_UpdateEuler() 
{
  if((mUpdate & IMAGEMAP_UPDATE_EULER)==0) 
  {
    XNODE root = &mNodeTable[0];
    dmDEBUG_ASSERT( root->ri_Part == 0 );

    M_UpdateEuler1(root);
    mUpdate |= IMAGEMAP_UPDATE_EULER;
  }
}
//--------------------------------------------------------------
void cciMeasurements::M_UpdateBoundary( ) 
{
  if((mUpdate & IMAGEMAP_UPDATE_BOUNDARY)==0) 
  {
    dmRegion r;
    r.SetRectRoi(mMap.rect());

    info_list_type::iterator it   = mNodeList.Begin();
    info_list_type::iterator last = mNodeList.End();

    for(;it != last; ++it)
      (*it).ri_Boundary.Clear();

    daim::apply(r,mMap,MapBoundary(*this,r));
    mUpdate |= IMAGEMAP_UPDATE_BOUNDARY;
  } 
}
//--------------------------------------------------------------------
// Compute Area
//---------------------------------------------------------------------
bool cciMeasurements::M_ComputeArea(  dm_real* _Results )
{
  M_UpdateCount();

  dm_real _fac = uppxls * uppxls * aspect_ratio;

  info_list_type::iterator it   = mNodeList.Begin();
  info_list_type::iterator last = mNodeList.End();

  XNODE pNode;

  index_table_type& _itable = mIndexTable;
  
  for(;it != last; ++it) 
  {
    pNode = &(*it);
    if(pNode->ri_Status==RI_REGION) 
     _Results[_itable[pNode->ri_Part]] = _fac * pNode->ri_Count;
  }

  return true;
}
//---------------------------------------------------------------------
// Compute the length of the boundary of the object, i.e the length
// of the polygon enclosing the object 
//---------------------------------------------------------------------
bool cciMeasurements::M_ComputeBoundary( dm_real* _Results )
{
  M_UpdateBoundary();

  dm_real dx,dy,length;
  dm_real _xcal = uppxls;
  dm_real _ycal = aspect_ratio * _xcal;

  info_list_type::iterator it   = mNodeList.Begin();
  info_list_type::iterator last = mNodeList.End();

  XNODE pNode;

  index_table_type& _itable = mIndexTable;
  
  for(;it != last; ++it) 
  {
    pNode = &(*it);

    if(pNode->ri_Boundary.Size() > 1)
    {
      dmPoly::iterator pF = pNode->ri_Boundary.Begin();
      dmPoly::iterator pL = pNode->ri_Boundary.End() - 2;
      for(length = 0;pF!=pL;++pF) {
        dx = _xcal * ( (*pF).x - (*(pF+1)).x );
        dy = _ycal * ( (*pF).y - (*(pF+1)).y );
        length += sqrt( dx*dx + dy*dy );             
      }
      // take care of one pixel object
      // TODO : check what is the length of the object 
      if(length == 0) {
         length = _xcal*_ycal;

        #ifdef _DEBUG_IMAGEMAP
        if(pNode->ri_Boundary.Size()>2) {
          dmLOG("WARNING : computed object 0 length for object !\n");
        }
        #endif 
      }

      pNode->ri_Border = length;

      if(_Results && pNode->ri_Status==RI_REGION) 
         _Results[_itable[pNode->ri_Part]] = length; 
    }
  }  
  return true;
}
//---------------------------------------------------------------------
bool cciMeasurements::M_ComputeBorder( dm_real* _Results )
{
  M_ComputeBoundary(NULL);

  dm_real length;

  index_table_type& _itable = mIndexTable;

  info_list_type::iterator it   = mNodeList.Begin();
  info_list_type::iterator last = mNodeList.End();

  XNODE pNode;

  for(;it != last; ++it) 
  {
    pNode = &(*it);
    if(pNode->ri_Status==RI_REGION) 
    {
      length = 0;

      // Collect child borders length
      dmList::iterator cF = pNode->ri_ChildList.Begin();
      dmList::iterator cL = pNode->ri_ChildList.End();

      for(XNODE child;cF!=cL;++cF) {
        child = static_cast<_RegionLink*>(*cF)->rl_Self;
        length += child->ri_Border;
      }

      _Results[_itable[pNode->ri_Part]] = pNode->ri_Border + length; 
    }
  } 
  return true;
}
//--------------------------------------------------------------
// Momentum of order (p,q)
//--------------------------------------------------------------
struct MapMomentum
{
  cciMeasurements&         _This;
  const index_table_type&  _itable;
  dm_real* _Results;
  int      p,q;

  MapMomentum( cciMeasurements& _Instance, dm_real* _Res,int _p, int _q)
  :_This(_Instance) 
  ,_itable(_Instance.IndexTable())
  ,_Results(_Res)
  ,p(_p)
  ,q(_q)
  {}
 
  void operator()( daim::map_type::const_line_type _in , long x1, long x2 )
  {
    daim::map_type::const_line_type start = _This.Map().begin();
    XNODE pNode;

    dm_real dx,dy;

    for(long x=x1; x<=x2;++x) 
    { 
      pNode = &_This._PARTITION_NODE((*_in)[x]);

      if(pNode->ri_Status==RI_REGION) 
      {
        if(p) {
          dx = x - pNode->ri_Centroid_x;
          if(p==2) dx *= dx; else if(p>2) dx = pow(dx,p);
        } else dx = 1.0;

        if(q) {
          dy = (_in - start) - pNode->ri_Centroid_y;
          if(q==2) dy *= dy; else if(q>2) dy = pow(dy,q);
        } else dy = 1.0;

        _Results[_itable[pNode->ri_Part]] += dx*dy;  
      }    
    }
  }
};
//---------------------------------------------------------------------
bool cciMeasurements::M_ComputeMomentum(  dm_real* _Results, int p,int q )
{
  if(p<0 || q<0) return false;
  
  dm_real _xcal = uppxls;
  dm_real _ycal = aspect_ratio * _xcal;

  M_UpdateCentroids();

  // Momentum of order 1 => this is centroids
  if((p+q)==1) 
  {
    index_table_type& _itable = mIndexTable;

    info_list_type::iterator it   = mNodeList.Begin();
    info_list_type::iterator last = mNodeList.End();
    XNODE pNode;

    for(;it!= last;++it) {
      pNode = &(*it);
      if(pNode->ri_Status==RI_REGION) {
        if(p==1) _Results[_itable[pNode->ri_Part]] = _xcal * pNode->ri_Centroid_x; else
        if(q==1) _Results[_itable[pNode->ri_Part]] = _ycal * pNode->ri_Centroid_y;
      }
    }
    return true;
  } 
  else 
  {
    dm_size count = mNumPartitions+1;

    if(p==2) _xcal *= _xcal; else if(p>2) _xcal = pow(_xcal,p);
    if(q==2) _ycal *= _ycal; else if(q>2) _ycal = pow(_ycal,q);
    std::fill(_Results,_Results+count,0);
    daim::apply(mRoi,mMap,MapMomentum(*this,_Results,p,q));
    if(p && _xcal!=1.0) std::for_each(_Results,_Results + count,std::bind2nd(std::multiplies<dm_real>(),_xcal));
    if(q && _ycal!=1.0) std::for_each(_Results,_Results + count,std::bind2nd(std::multiplies<dm_real>(),_ycal)); 
    return true;
  }
  return false;
}
//---------------------------------------------------------------------
bool cciMeasurements::M_ComputeLabels( dm_real* _Results )
{
  info_list_type::iterator it   = mNodeList.Begin();
  info_list_type::iterator last = mNodeList.End();
  XNODE pNode;

  index_table_type& _itable = mIndexTable;
  
  for(;it!= last;++it) {
    pNode = &(*it);
    if(pNode->ri_Status==RI_REGION) 
     _Results[_itable[pNode->ri_Part]] = mPartition[pNode->ri_Part];
  }  
  return true;  
}
//--------------------------------------------------------------
// Compute direction using moments definitions
//--------------------------------------------------------------
void cciMeasurements::M_UpdateDir() 
{
  if( (mUpdate & IMAGEMAP_UPDATE_DIR)==0 )
  {
    BuildIndexTable();
 
    size_t nPart = mNumPartitions+1;

    dmVector<double> m11(nPart,0);
    dmVector<double> m02(nPart,0);
    dmVector<double> m20(nPart,0);
 
    //-----------------------
    // Compute momentums
    //-----------------------
    M_ComputeMomentum(m11.Begin(),1,1);    
    M_ComputeMomentum(m02.Begin(),0,2);    
    M_ComputeMomentum(m20.Begin(),2,0);    

    //-----------------------
    // Compute direction
    //-----------------------
    
    info_list_type::iterator it   = mNodeList.Begin();
    info_list_type::iterator last = mNodeList.End();
    XNODE pNode;

    int nindex;
 
    index_table_type& _itable = mIndexTable;
    
    for(;it!= last;++it) {
      pNode = &(*it);
      if(pNode->ri_Status==RI_REGION) 
      {
        nindex = _itable[pNode->ri_Part];
        pNode->ri_Theta = 0.5 * atan2(2.0*m11[nindex],m20[nindex]-m02[nindex]);
      }
    }  

    mUpdate |= IMAGEMAP_UPDATE_DIR;
  }
} 
//--------------------------------------------------------------
// Compute object circonscript rectangle
//--------------------------------------------------------------
static void M_ComputeRectProps1( 
   dm_real _xcal,dm_real _ycal,
   const dmPoly& p, 
   dm_real theta, 
   dm_real& l1, dm_real& l2 )
{
  dm_real a,amax,amin;
  dm_real b,bmax,bmin;
  dm_real c = cos(theta);
  dm_real s = sin(theta);

  dmPoly::const_iterator it   = p.Begin();
  dmPoly::const_iterator last = p.End();

  amin = amax =  (*it).x*_xcal*c + (*it).y*_ycal*s;
  bmin = bmax = -(*it).x*_xcal*s + (*it).y*_ycal*c;

  for(++it;it!=last;++it) {
    a=  (*it).x*_xcal*c + (*it).y*_ycal*s;
    b= -(*it).x*_xcal*s + (*it).y*_ycal*c;
    if( a < amin ) amin = a; else if( a > amax ) amax = a; 
    if( b < bmin ) bmin = b; else if( b > bmax ) bmax = b; 
  }
  l1 = amax-amin;
  l2 = bmax-bmin; 
} 
//--------------------------------------------------------------
void cciMeasurements::M_UpdateRectProps() 
{
  if( (mUpdate & IMAGEMAP_UPDATE_RECTPROPS)==0 ) 
  {
    M_UpdateBoundary();
    M_UpdateDir();

    double _xcal = uppxls;
    double _ycal = aspect_ratio * _xcal;

    info_list_type::iterator it   = mNodeList.Begin();
    info_list_type::iterator last = mNodeList.End();
    XNODE pNode;

    for(;it!= last;++it) {
      pNode = &(*it);
      if(pNode->ri_Status==RI_REGION) {
        M_ComputeRectProps1(
          _xcal,_ycal,
          pNode->ri_Boundary,
          pNode->ri_Theta,
          pNode->ri_L1,
          pNode->ri_L2
        );
      }
    }

    mUpdate |= IMAGEMAP_UPDATE_RECTPROPS;
  }
}
//---------------------------------------------------------------------
bool cciMeasurements::M_ComputeDir( dm_real* _Results )
{
  M_UpdateDir();

  info_list_type::iterator it   = mNodeList.Begin();
  info_list_type::iterator last = mNodeList.End();
  XNODE pNode;
  
  index_table_type& _itable = mIndexTable;
  
  for(;it!= last;++it) {
    pNode = &(*it);
    if(pNode->ri_Status==RI_REGION) 
     _Results[_itable[pNode->ri_Part]] = pNode->ri_Theta;
  }  
  return true;
}
//---------------------------------------------------------------------
bool cciMeasurements::M_ComputeRectProps( dm_real* _Results, int l )
{
  M_UpdateRectProps();

  info_list_type::iterator it   = mNodeList.Begin();
  info_list_type::iterator last = mNodeList.End();
  XNODE pNode;

  index_table_type& _itable = mIndexTable;
  
  for(;it!= last;++it) {
    pNode = &(*it);
    if(pNode->ri_Status==RI_REGION) {
      if(l>1) _Results[_itable[pNode->ri_Part]] = pNode->ri_L1; 
      else    _Results[_itable[pNode->ri_Part]] = pNode->ri_L2; 
    }
  } 
  return true;
}
//--------------------------------------------------------------
bool cciMeasurements::M_ComputeEuler( dm_real* _Results  ) 
{
  M_UpdateEuler();

  info_list_type::iterator it   = mNodeList.Begin();
  info_list_type::iterator last = mNodeList.End();
  XNODE pNode;

  index_table_type& _itable = mIndexTable;
  
  for(;it!= last;++it) {
    pNode = &(*it);
    if(pNode->ri_Status==RI_REGION)
      _Results[_itable[pNode->ri_Part]] = pNode->ri_Euler; 
  } 
  return true;

}
//--------------------------------------------------------------
bool cciMeasurements::M_ComputeDepth( dm_real* _Results  ) 
{
  info_list_type::iterator it   = mNodeList.Begin();
  info_list_type::iterator last = mNodeList.End();
  XNODE pNode;

  index_table_type& _itable = mIndexTable;
  
  for(;it!= last;++it) {
    pNode = &(*it);
    if(pNode->ri_Status==RI_REGION)
      _Results[_itable[pNode->ri_Part]] = pNode->ri_Depth; 
  } 
  return true;

}
//--------------------------------------------------------------
bool cciMeasurements::M_ComputeHoles( dm_real* _Results  ) 
{
  info_list_type::iterator it   = mNodeList.Begin();
  info_list_type::iterator last = mNodeList.End();
  XNODE pNode;

  index_table_type& _itable = mIndexTable;
  
  for(;it!= last;++it) {
    pNode = &(*it);
    if(pNode->ri_Status==RI_REGION)
      _Results[_itable[pNode->ri_Part]] = pNode->ri_Childs; 
  } 
  return true;

}
//---------------------------------------------------------------------
bool cciMeasurements::M_ComputeParentLabels( dm_real* _Results  ) 
{
  info_list_type::iterator it   = mNodeList.Begin();
  info_list_type::iterator last = mNodeList.End();
  XNODE pNode;

  index_table_type& _itable = mIndexTable;
  
  for(;it!= last;++it) {
    pNode = &(*it);
    if(pNode->ri_Status==RI_REGION)
      _Results[_itable[pNode->ri_Part]] = pNode->ri_Father; 
  } 
  return true;

}
//---------------------------------------------------------------------
bool cciMeasurements::M_ComputePosition( dm_real* _Results, int p )
{
  dm_real _xcal = uppxls;
  dm_real _ycal = aspect_ratio * _xcal;

  M_UpdateCentroids();

  info_list_type::iterator it   = mNodeList.Begin();
  info_list_type::iterator last = mNodeList.End();
  XNODE pNode;

  index_table_type& _itable = mIndexTable;

  for(;it!= last;++it) {
    pNode = &(*it);
    if(pNode->ri_Status==RI_REGION) {
      if(p==1) _Results[_itable[pNode->ri_Part]] = _xcal * pNode->ri_Position_x;
      else     _Results[_itable[pNode->ri_Part]] = _ycal * pNode->ri_Position_y;
    }
  }
  return true;
}
//---------------------------------------------------------------------
