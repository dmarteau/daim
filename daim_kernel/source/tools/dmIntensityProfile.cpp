
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

 /*
  * Revised 7/06/2006 :
  * Added test for small radius ellipse 
  */

#define dmUseKernelImageTemplates
#define dmUseKernelBuffer
#include "daim_kernel.h"
#include "tools/dmIntensityProfile.h"

typedef std::vector<dm_point> point_vector_type;

//--------------------------------------------------------
dmIntensityProfile::dmIntensityProfile()
{}
//--------------------------------------------------------
dmIntensityProfile::~dmIntensityProfile()
{}
//--------------------------------------------------------
namespace  ns_IntensityProfile 
{
  struct AddPoint 
  {
    dmIntensityProfile::vector_type&  _data;
  
    AddPoint( dmIntensityProfile::vector_type& v ) : _data(v) {}
    void operator()(int x, int y) { 
      _data.push_back( dmIntensityProfile::value_type(x,y) ); 
    }
  };

  struct AddPointQuads
  {
    point_vector_type& _pv;
    dm_point           _pt;
  
    AddPointQuads( point_vector_type& _p ) : _pv(_p) {}
    void operator()(dm_int x, dm_int y, const dmPoint& c) {
       _pt.x = x;  
       _pt.y = y;
       _pv.push_back( _pt );
    }
  };

  struct Functor
  {
    dmIntensityProfile& _Impl;
    Functor( dmIntensityProfile& aImpl) : _Impl(aImpl) {}
 
    template<EPixelFormat _PixelFormat>
    void operator()( const dmIImage<_PixelFormat>& aSrc ) {
      _Impl.ComputeProfile(aSrc.Gen());
    }
  };
  
} // namespace ns_IntensityProfile
//--------------------------------------------------------
daim::gap<dm_real> dmIntensityProfile::GetRange() const
{
  dm_real f,fmin,fmax;

  if(Empty()) 
    return daim::gap<dm_real>(0,0);

  const_iterator it   = _Data.begin();
  const_iterator last = _Data.end();
  
  fmin = fmax = (*it).value;
  for(;it!=last;++it) {
    f = (*it).value;
    if(f>fmax) fmax = f; else
    if(f<fmin) fmin = f;
  }
  return daim::gap<dm_real>(fmin,fmax);
}
//--------------------------------------------------------
dmIntensityProfile& dmIntensityProfile::SetCoordinates( const dmLine& l )
{
  _Data.clear();
  
  ns_IntensityProfile::AddPoint op(_Data);
  dmDigitalLine( l, op  ); 
  return *this;
}
//--------------------------------------------------------
dmIntensityProfile& dmIntensityProfile::SetCoordinates( const dmPoly& p )
{
  _Data.clear();
     
  if(p.Size())
  {
  	ns_IntensityProfile::AddPoint op(_Data);
  	
    int cnt = p.Size()-1;
    for(int i=1;i<=cnt;++i) 
    {
      dmDigitalLine( p[i-1],p[i], op );
      _Data.pop_back(); // remove the last point
    }
    if(!p.Closed()) { 
      _Data.push_back( dm_point( p.Back() ) ); 
    }
  } 
  return *this;
}
//--------------------------------------------------------
// TODO: Need to debug that Bresenham ellipse algorithm !!!
//--------------------------------------------------------
dmIntensityProfile& dmIntensityProfile::SetCoordinates(const dmEllipse& e)
{
  _Data.clear();
  point_vector_type pv;
  
  ns_IntensityProfile::AddPointQuads op(pv);	
  dmDigitalEllipse(e,op);

  //-------------------------------------------------
  // Le Bresenham parcoure le quadrant d'abord dans
  // le sens directe, puis dans le sens indirect
  //-------------------------------------------------
  point_vector_type::iterator it =  pv.begin();
  while( it!= pv.end() && (*it).y!= 0) ++it;

  // BUG_BRESENHAM
  if(it== pv.end()) {
    dmASSERT((*pv.rbegin()).y==1);
    dm_point pt = { e.rx, 0 };
    pv.push_back( pt );
  } else 
    std::reverse(it,pv.end()-1);

  int n = pv.size()-1;

  if(n>2)
  {
    _Data.resize(4*n-3);

    int x,y,cx,cy;
    cx = e.center.x;
    cy = e.center.y;
  
    iterator q1 = _Data.begin();
    iterator q2 = _Data.begin()+2*n-1;
    iterator q3 = _Data.begin()+2*n-1;
    iterator q4 = _Data.begin()+4*n-4;
  
    for(it=pv.begin();q1<q2;++q1,--q2,++q3,--q4,++it) 
    {
      x=(*it).x; y=(*it).y; 
    
      (*q1).p.x = cx-x; (*q1).p.y = cy-y;   
      (*q2).p.x = cx-x; (*q2).p.y = cy+y;   
      (*q3).p.x = cx+x; (*q3).p.y = cy+y;   
      (*q4).p.x = cx+x; (*q4).p.y = cy-y;   
    }

    _Data[2*n-1].p.x = cx;
    _Data[2*n-1].p.y = cy + e.ry;
    _Data[3*n-1].p.x = cx + e.rx;
    _Data[3*n-1].p.y = cy;
  }
  else 
  {
    _Data.resize(1);
    _Data[0].p = e.center;
  }
  return *this;
}
//--------------------------------------------------------
dmIntensityProfile& dmIntensityProfile::SetCoordinates( const dmRect&  r )
{
  dmPoly poly(5);
  poly[0] = r.TopLeft();
  poly[1] = r.TopRight();
  poly[2] = r.BottomRight();
  poly[3] = r.BottomLeft();
  poly[4] = poly[0];
  SetCoordinates(poly);
  
  return *this;
}
//--------------------------------------------------------
dmIntensityProfile& dmIntensityProfile::SetCoordinates(const dmCircle& c )
{
  dmEllipse ellipse;
  ellipse.center = c.center;
  ellipse.rx = c.r;
  ellipse.ry = c.r;
  
  SetCoordinates(ellipse);

  return *this;
}
//--------------------------------------------------------
void dmIntensityProfile::Clear()
{
  _Data.clear();
}
//--------------------------------------------------------
bool dmIntensityProfile::ComputeProfile( const dmImage& aSource , bool bCopyUnits )
{
  if(bCopyUnits)
     aSource.GetUnits(_Units);
  
  ns_IntensityProfile::Functor _Functor(*this);
  return dmImplementOperation(_Functor,aSource);
}
//--------------------------------------------------------

