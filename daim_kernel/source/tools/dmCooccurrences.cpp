
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

#define dmUseKernelTemplates
#include "daim_kernel.h"
#include "tools/dmCooccurrences.h"

#include "dmMatrixArray.h"

#include <math.h>
//---------------------------------------------------------
double dmMatrix_Homogeneity( const dmRectMatrix& _data, double Nc )
{
  if(Nc) {
    double x= 0,a;
    float*  p=_data.GetData();
    for(int i=_data.NStore();--i>=0;) {
      a = *p++;
      x += a*a;
    }
    return x/(Nc*Nc);
  }
  return 0;
}
//---------------------------------------------------------
double dmMatrix_Contrast( const dmRectMatrix& _data, double Nc )
{
  if(Nc) {
    int sz,j,i;
    double x = 0;
    float* p=_data.GetData();
    for(sz=_data.NRows(),i=0;i<sz;++i) {
      for(j=0;j<sz;++j)  x += *p++ * ((double)(i-j)*(i-j));
    }
    return x/(Nc*(double)(sz-1)*(sz-1));
  }
  return 0;  
}
//---------------------------------------------------------
double dmMatrix_Entropy( const dmRectMatrix& _data, double Nc )
{
  if(Nc) {
    double x = 0,a;
    float*  p =_data.GetData();
    for(int i=_data.NStore();--i>=0;) {
      a = *p++;
      if(a) x += a * log(a);
    }
    return 1.0 - x/(Nc*log(Nc));
  }
  return 0;
}
//---------------------------------------------------------
double dmMatrix_Correlation( const dmRectMatrix& _data, double Nc )
{
  if(Nc) {
    double x,mx,my,mxy,mxx,myy;
    mx = my = mxy = mxx = myy = 0;
    float* p=_data.GetData();
    for(int sz=_data.NRows(),j,i=0;i<sz;++i) {
      for(j=0;j<sz;++j) {
        x  = *p++;
        mx  += i*x;
        my  += j*x;
        mxy += i*(j*x);
        mxx += i*(i*x);
        myy += j*(j*x);
      }
    }

    mx  /= Nc;
    my  /= Nc;
    mxx /= Nc;
    myy /= Nc;
    mxy /= Nc;

    mxx -= mx*mx;
    myy -= my*my;
    return (mxx>0 && myy>0 ? fabs( mxy - mx*my ) / sqrt(mxx*mxx) : 1);
  }
  return 0;
}
//---------------------------------------------------------
double dmMatrix_LocalHomogeneity( const dmRectMatrix& _data, double Nc )
{
  if(Nc) {
    int sz = _data.NRows();
    double x = 0;
    float* p=_data.GetData();
    for(int j,i=0;i<sz;++i) {
      for(j=0;j<sz;++j) {
        x += ( *p++ / (1.0 + (double)(i-j)*(i-j)) );
      }
    }
    return x/Nc;
  }
  return 0;
}
//---------------------------------------------------------
double dmMatrix_Directivity( const dmRectMatrix& _data, double Nc )
{
  if(Nc) {
    double x = 0;
    for(int i=_data.NRows();i>=1;--i) x += _data(i,i);
    return x/Nc;
  }
  return 0;
}
//---------------------------------------------------------
double dmMatrix_Uniformity( const dmRectMatrix& _data, double Nc )
{
  if(Nc) {
    double x = 0,a;
    for(int i=_data.NRows();i>=1;--i) {
      a = _data(i,i);
      x += a*a;
    }
    return x/(Nc*Nc);
  }
  return 0;
}
//---------------------------------------------------------
double dmMatrix_Clustering( const dmRectMatrix& _data, double Nc )
{/*
  if(Nc) {
    float x = 0; int cnt = 0;
    unsigned int sz = _data.NRows();
    for(int i=sz;i>=1;--i) {
      cnt += _data(i,i); 
      x   += _data(i,i)*(i-1);
    }
    return x/(cnt* (float)(sz-1));
  }
  return 0;
*/
 if(Nc) {
    int sz = _data.NRows();
    double x = 0;
    float* p=_data.GetData();
    for(dm_int j,i=0;i<sz;++i) {
      for(j=0;j<sz;++j) {
        x += *p++ * daim::min(i,j);
      }
    }
    return x/Nc;
  }
  return 0;
}
//---------------------------------------------------------
#define  NSIZE( b ) (size_t)(256>>b)

//---------------------------------------------------------
// helpers
//---------------------------------------------------------
static bool __compute_mask(  const std::vector<dmPoint>& _pts, dmRect& rect )
{ 
  dmRect r(0,0,1,1);
  for(int i=_pts.size(); --i>=0; ) {
    const dmPoint& p = _pts[i];
    r.top_left.x     = daim::min(r.top_left.x,p.x);
    r.top_left.y     = daim::min(r.top_left.y,p.y); 
    r.bottom_right.x = daim::max(r.bottom_right.x,p.x); 
    r.bottom_right.y = daim::max(r.bottom_right.y,p.y); 
  }
  rect.top_left.x  -= r.top_left.x;
  rect.top_left.y  -= r.top_left.y; 
  rect.bottom_right.x -= r.bottom_right.x; 
  rect.bottom_right.y -= r.bottom_right.y; 
  return (rect.top_left.x <= rect.bottom_right.x && rect.top_left.y <= rect.bottom_right.y);
}
//
//---------------------------------------------------------
dmCooccurrence::dmCooccurrence(  const dmCooccurrence::points_list& _points, EBinType nbin )
:c_data(NSIZE(nbin),NSIZE(nbin))
,c_pattern(_points)
,c_nbin(nbin)
{
  Clear();
}
//---------------------------------------------------------
dmCooccurrence::dmCooccurrence( const image_type& image,   const dmCooccurrence::points_list& _points, EBinType nbin )
:c_data(NSIZE(nbin),NSIZE(nbin))
,c_pattern(_points)
,c_nbin(nbin)
{
  Clear();
  operator()( image, image.rect(), c_pattern );
}
//---------------------------------------------------------
dmCooccurrence::dmCooccurrence( const image_type& image,const dmRegion& rgn, const dmCooccurrence::points_list& _points, EBinType nbin )
:c_data(NSIZE(nbin),NSIZE(nbin))
,c_pattern(_points)
,c_nbin(nbin)
{
  Clear();
  operator()( image,rgn,c_pattern );
}
//---------------------------------------------------------
void dmCooccurrence::SetTransform( const dmCooccurrence::points_list& _points )
{
  c_pattern = _points;
}
//---------------------------------------------------------
void dmCooccurrence::Clear()
{
  c_data.Set(0);
  Nc = 0;
}
//---------------------------------------------------------
bool dmCooccurrence::operator()( const image_type& image, const dmCooccurrence::points_list& _points )
{
  return operator()( image, image.rect(), _points );
}
//----------------------------------------------------------
#define  NBIN( v ) (v >> nbin)
//----------------------------------------------------------
struct __CO_COMPUTE {  
    dmCooccurrence&     _co;
    dmMatrixArray&      _m;
    __CO_COMPUTE(dmCooccurrence& co,dmMatrixArray& m) : _co(co),_m(m) {}

   void operator()( dmCooccurrence::image_type::const_line_type _in, 
                    long x1, long x2 )
   {
     dmCooccurrence::points_list::const_iterator it   = _co.GetTransform().begin();
     dmCooccurrence::points_list::const_iterator last = _co.GetTransform().end();
     for(int nbin=_co.GetBin();it!=last;++it) {
       const dm_uint8* p_in = *(_in + (*it).y) + (*it).x;
       for(int i=x1; i<=x2; ++i ) {
         ++_m[NBIN((*_in)[i])+1][NBIN(p_in[i])+1];
       }
     }
   }
};
//---------------------------------------------------------
bool dmCooccurrence::operator()( const image_type& image,const dmRegion& rgn, const dmCooccurrence::points_list& _points  )
{
  dmRect r = rgn.Rectangle();
  if(__compute_mask(c_pattern,r)) {
    dmRegion _rgn = rgn;
    _rgn.ClipToRect(r);
    if(!_rgn.IsEmptyRoi()) {
      dmMatrixArray _m(c_data);
      daim::apply(_rgn,image,__CO_COMPUTE(*this,_m));
      Nc = std::accumulate( 
          c_data.GetData(),
          c_data.GetData() + c_data.NStore(),0.0);
      return true;
    }
  }
  return false; 
}
//---------------------------------------------------------
bool dmCooccurrence::operator()( const image_type& image )
{
  return operator()( image, image.rect(), c_pattern );
}
//---------------------------------------------------------
bool dmCooccurrence::operator()( const image_type& image,const dmRegion& rgn )
{
  return operator()( image,rgn,c_pattern );
}
//---------------------------------------------------------
