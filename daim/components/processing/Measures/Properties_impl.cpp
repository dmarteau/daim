
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

#include "math.h"
#include "cciMeasures.h"

//---------------------------------------------------------------------
// Compute sum of luminance
//--------------------------------------------------------------
struct Prop_CumulateDensity
{
  cciMeasurements& _This;

  Prop_CumulateDensity( cciMeasurements& _Instance ) 
  :_This(_Instance)
  {}

  void operator()(int x, const dm_float& v ) { 
    _This._PARTITION_NODE(x).ri_fvalue += v;
  }
};
//---------------------------------------------------------------------
// Compute sum of luminance of order 2
//--------------------------------------------------------------
struct Prop_CumulateDensityOrder2
{
  cciMeasurements& _This;

  Prop_CumulateDensityOrder2( cciMeasurements& _Instance ) 
  :_This(_Instance)
  {}

  void operator()( int x, const dm_float& v ) { 
    _This._PARTITION_NODE(x).ri_fvalue += v * v;
  }
};
//---------------------------------------------------------------------
// Compute sum of luminance of order N
//--------------------------------------------------------------
struct Prop_CumulateDensityOrderN
{
  cciMeasurements& _This;
  dm_real                    n;

  Prop_CumulateDensityOrderN( cciMeasurements& _Instance, dm_real _Order ) 
  :_This(_Instance),n(_Order)
  {}

  void operator()(int x, const dm_float& v ) { 
    _This._PARTITION_NODE(x).ri_fvalue += pow(static_cast<dm_real>(v),n);
  }
};
//--------------------------------------------------------------
bool cciMeasurements::ComputeDensity( const dmIImage<dmPixelFormat32bppFloat>& _Image,
                                      dm_uint  _Order,
                                      dm_real* _Results, const dmPoint& p  )
{
  UpdateRoi();

  info_list_type::iterator it   = mNodeList.Begin();
  info_list_type::iterator last = mNodeList.End();

  for(;it != last; ++it)
     (*it).ri_fvalue = 0;

  dmIImage<dmPixelFormat32bppFloat>* pImg = 
    const_cast<dmIImage<dmPixelFormat32bppFloat>*>(&_Image);

  if(_Order == 1) {
    daim::evaluate(mRoi,p,mMap,pImg->Gen(),Prop_CumulateDensity(*this));
  } else
  if(_Order == 2) {
    daim::evaluate(mRoi,p,mMap,pImg->Gen(),Prop_CumulateDensityOrder2(*this));
  } else {    
    daim::evaluate(mRoi,p,mMap,pImg->Gen(),Prop_CumulateDensityOrderN(*this,_Order));
  } 

  XNODE pNode;

  for(it = mNodeList.Begin();it!= last;++it) {
    pNode = &(*it);
    if(pNode->ri_Status==RI_REGION)
       _Results[mIndexTable[pNode->ri_Part]] = pNode->ri_fvalue; 
  } 
  return true;
}
//--------------------------------------------------------------
bool cciMeasurements::ComputeMeanDensity( const dmIImage<dmPixelFormat32bppFloat>& _Image,
                                          dm_real* _Results, const dmPoint& p  )
{
  UpdateRoi();
  M_UpdateCount();

  info_list_type::iterator it   = mNodeList.Begin();
  info_list_type::iterator last = mNodeList.End();

  for(;it != last; ++it)
     (*it).ri_fvalue = 0;

  dmIImage<dmPixelFormat32bppFloat>* pImg = 
    const_cast<dmIImage<dmPixelFormat32bppFloat>*>(&_Image);

  daim::evaluate(mRoi,p,mMap,pImg->Gen(),Prop_CumulateDensity(*this));

  XNODE pNode;

  for(it = mNodeList.Begin();it!= last;++it) {
    pNode = &(*it);
    if(pNode->ri_Status==RI_REGION)
       _Results[mIndexTable[pNode->ri_Part]] = (pNode->ri_fvalue/pNode->ri_Count); 
  } 
  return true;
}
//---------------------------------------------------------------------
// Compute min of luminance
//--------------------------------------------------------------
struct Prop_MinDensity
{
  cciMeasurements& _This;
  XNODE            _Node;

  Prop_MinDensity( cciMeasurements& _Instance ) 
  :_This(_Instance)
  {}

  void operator()( int x, const dm_float& v ) { 
    _Node = &_This._PARTITION_NODE(x);
    _Node->ri_fvalue = dm_min(_Node->ri_fvalue,static_cast<dm_real>(v));
  }
};
//--------------------------------------------------------------
bool cciMeasurements::ComputeMinDensity( const dmIImage<dmPixelFormat32bppFloat>& _Image,
                                         dm_real* _Results, const dmPoint& p  ) 
{
  UpdateRoi();

  info_list_type::iterator it   = mNodeList.Begin();
  info_list_type::iterator last = mNodeList.End();

  for(;it != last; ++it)
     (*it).ri_fvalue = dmIImage<dmPixelFormat32bppFloat>::traits_type::max();

  dmIImage<dmPixelFormat32bppFloat>* pImg = 
    const_cast<dmIImage<dmPixelFormat32bppFloat>*>(&_Image);

  daim::evaluate(mRoi,p,mMap,pImg->Gen(),Prop_MinDensity(*this));

  XNODE pNode;

  for(it = mNodeList.Begin();it!= last;++it) {
    pNode = &(*it);
    if(pNode->ri_Status==RI_REGION)
       _Results[mIndexTable[pNode->ri_Part]] = pNode->ri_fvalue; 
  } 
  return true;
}
//---------------------------------------------------------------------
// Compute max of luminance
//--------------------------------------------------------------
struct Prop_MaxDensity
{
  cciMeasurements& _This;
  XNODE            _Node;

  Prop_MaxDensity( cciMeasurements& _Instance ) 
  :_This(_Instance)
  {}

  void operator()( int x, const dm_float& v ) { 
    _Node = &_This._PARTITION_NODE(x);
    _Node->ri_fvalue = dm_max(_Node->ri_fvalue,static_cast<dm_real>(v));
  }
};
//--------------------------------------------------------------
bool cciMeasurements::ComputeMaxDensity( const dmIImage<dmPixelFormat32bppFloat>& _Image,
                                         dm_real* _Results, const dmPoint& p  ) 
{
  UpdateRoi();

  info_list_type::iterator it   = mNodeList.Begin();
  info_list_type::iterator last = mNodeList.End();

  for(;it != last; ++it)
     (*it).ri_fvalue = dmIImage<dmPixelFormat32bppFloat>::traits_type::min();

  dmIImage<dmPixelFormat32bppFloat>* pImg = 
    const_cast<dmIImage<dmPixelFormat32bppFloat>*>(&_Image);

  daim::evaluate(mRoi,p,mMap,pImg->Gen(),Prop_MaxDensity(*this));

  XNODE pNode;

  for(it = mNodeList.Begin();it!= last;++it) {
    pNode = &(*it);
    if(pNode->ri_Status==RI_REGION)
       _Results[mIndexTable[pNode->ri_Part]] = pNode->ri_fvalue; 
  } 
  return true;
}
//---------------------------------------------------------------------
