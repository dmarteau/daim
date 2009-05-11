
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
#include "templates/processing/dmKernelFamily.h"
#include "templates/processing/dmBinaryMorphology.h"

#include <limits>
#include <algorithm>

namespace daim {
//--------------------------------------------------------
void erode_region( dmRegion& _src, dmRegion& _dest,const dmStructuringElement& _elem )
{
  const dmPoint& p = _src.Rectangle().TopLeft();
  int xt=0,yt=0,pxt=0,pyt=0; //
  bool bAssign = true;

  const dmStructuringElement::value_type* kernel = _elem.Data();

  for(size_t ny=0;ny<_elem.Height();++ny) {
    for(size_t nx=0;nx<_elem.Width();++nx) {
      if( *kernel++ == 1) {   // Apply kernel for kernel values equal to 1
        xt = _elem.Ox() - nx; // Take the transposed of the kernel
        yt = _elem.Oy() - ny;
        _src.Translate(xt - pxt,yt - pyt);
        if(bAssign) { _dest = _src; bAssign = false; }
        else _dest.AndRoi(_src);  // Intersection of regions
        pxt = xt; pyt = yt;
      }
    }
  }
  // Restore region position
  _src.Translate( p - _src.Rectangle().TopLeft() );
}
//--------------------------------------------------------
void dilate_region( dmRegion& _src, dmRegion& _dest,const dmStructuringElement& _elem )
{
  const dmPoint& p = _src.Rectangle().TopLeft();
  int xt=0,yt=0,pxt=0,pyt=0; //
  bool bAssign = true;

  const dmStructuringElement::value_type* kernel = _elem.Data();
  for(size_t ny=0;ny<_elem.Height();++ny) {
    for(size_t nx=0;nx<_elem.Width();++nx) {
      if( *kernel++ == -1) {   // Apply kernel for kernel values equal to -1
        xt = nx - _elem.Ox();
        yt = ny - _elem.Oy();
        _src.Translate(xt - pxt,yt - pyt);
        if(bAssign) { _dest = _src; bAssign = false; }
        _dest.AddRoi(_src);  // Union of regions
        pxt = xt; pyt = yt;
      }
    }
  }
  // Restore region
  _src.Translate( p - _src.Rectangle().TopLeft() );
}
//--------------------------------------------------------
void binary_erosion(  const dmRegion& _src, dmRegion& _dest,const dmKernelFamily& _family,int nIter)
{
  bool idmp = nIter<=0;
  bool cond =!_dest.IsEmptyRoi();
  dmRegion rcond,P;
  if(cond) rcond = _dest;
  if(idmp) nIter = std::numeric_limits<int>::max();

  dmRegion  tmp = _src;
  dmRegion* pIn  = &tmp;
  dmRegion* pOut = &_dest;
  for(int i=0;i<nIter;++i) {
    if(idmp) P = *pIn;
    for(size_t k=0;k<_family.Size();++k) {
      erode_region(*pIn,*pOut,_family[k]);
      std::swap(pIn,pOut);
    }
    if(cond) pIn->AddRoi(rcond);
    if(idmp && *pIn==P) break;
  }
  if(pIn!=&_dest) _dest = *pIn;
}
//--------------------------------------------------------
void binary_dilation(  const dmRegion& _src, dmRegion& _dest,const dmKernelFamily& _family,int nIter)
{
  bool idmp = nIter<=0;
  bool cond =!_dest.IsEmptyRoi();
  dmRegion rcond,P;
  if(cond) rcond = _dest;
  if(idmp) nIter = std::numeric_limits<int>::max();

  _dest.KillRoi();
  const_cast<dmKernelFamily&>(_family).Complement();
  dmRegion tmp = _src;
  dmRegion* pIn  = &tmp;
  dmRegion* pOut = &_dest;
  for(int i=0;i<nIter;++i) {
    if(idmp) P = *pIn;
    for(size_t k=0;k<_family.Size();++k) {
      dilate_region(*pIn,*pOut,_family[k]);
      std::swap(pIn,pOut);
    }
    if(cond) pIn->AndRoi(rcond);
    if(idmp && *pIn==P) break;
  }
  if(pIn!=&_dest) _dest = *pIn;
  const_cast<dmKernelFamily&>(_family).Complement();
}
//--------------------------------------------------------
void binary_hitormiss(  const dmRegion& _src, dmRegion& _dest,const dmKernelFamily& _family,int nIter)
{
  bool idmp = nIter<=0;
  bool cond =!_dest.IsEmptyRoi();
  dmRegion rcond,P;
  if(cond) rcond = _dest;
  if(idmp) nIter = std::numeric_limits<int>::max();

  dmRegion B1,B2;
  dmRegion tmp = _src;

  dmRegion* pIn  = &tmp;
  dmRegion* pOut = &_dest;

  dmKernelFamily& rFamily =  const_cast<dmKernelFamily&>(_family);
  for(int i=0;i<nIter;++i) {
    pOut->KillRoi();
    if(idmp) P = *pIn;
    for(size_t k=0;k<rFamily.Size();++k)
    {
      dmStructuringElement& _elem = rFamily[k];
      erode_region(*pIn,B1,_elem);
      _elem.Transpose();
      dilate_region(*pIn,B2,_elem);
      _elem.Transpose();
      B1.SubRoi(B2);
      pOut->AddRoi(B1);
    }
    std::swap(pIn,pOut);
    if(cond) pIn->AndRoi(rcond);
    if(idmp && *pIn==P) break;
  }
  if(pIn!=&_dest) _dest = *pIn;
}
//--------------------------------------------------------
void binary_thinning(  const dmRegion& _src, dmRegion& _dest,const dmKernelFamily& _family, int nIter)
{
  bool bChange,idmp = nIter<=0;
  bool cond =!_dest.IsEmptyRoi();
  dmRegion rcond;
  if(cond) rcond = _dest;
  if(idmp) nIter = std::numeric_limits<int>::max();

  dmRegion B1,B2,P;
  _dest = _src;

  dmKernelFamily& rFamily =   const_cast<dmKernelFamily&>(_family);
  for(int i=0;i<nIter;++i)
  {
    bChange = false;
    if(cond && idmp) P = _dest;
    for(size_t k=0;k<rFamily.Size();++k)
    {
      dmStructuringElement& _elem = rFamily[k];
      erode_region(_dest,B1,_elem);
      _elem.Transpose();
      dilate_region(_dest,B2,_elem);
      _elem.Transpose();
      B1.SubRoi(B2);
      _dest.SubRoi(B1);
      bChange |= !B1.IsEmptyRoi();
    }
    if(cond) {
      _dest.AddRoi(rcond);
      if(idmp && P==_dest) break;
    } else if(idmp && !bChange) break;
  }
}
//--------------------------------------------------------
void binary_thickening(  const dmRegion& _src, dmRegion& _dest,const dmKernelFamily& _family,int nIter)
{
  bool idmp = nIter<=0;
  bool cond =!_dest.IsEmptyRoi();
  dmRegion rcond;
  if(cond) rcond = _dest;
  if(idmp) nIter = std::numeric_limits<int>::max();

  dmRegion B1,B2,P;
  _dest = _src;
  dmKernelFamily& rFamily = const_cast<dmKernelFamily&>(_family);
  rFamily.Complement();
  for(int i=0;i<nIter;++i)
  {
    if(idmp) P = _dest;
    for(size_t k=0;k<rFamily.Size();++k)
    {
      dmStructuringElement& _elem = rFamily[k];
      erode_region(_dest,B1,_elem);
      _elem.Transpose();
      dilate_region(_dest,B2,_elem);
      _elem.Transpose();
      B1.SubRoi(B2);
      _dest.AddRoi(B1);
    }
    if(cond) _dest.AndRoi(rcond);
    if(idmp && P==_dest) break;
  }
  rFamily.Complement();
}
//-------------------------------------------------------------
// We do not use successive iteration because
// opening and closing are idempotents operation
//-------------------------------------------------------------
void binary_opening(  const dmRegion& _src, dmRegion& _dest,const dmKernelFamily& _family, int nIter )
{
  _dest = _src;
  dmRegion tmp;
  dmKernelFamily& rFamily = const_cast<dmKernelFamily&>(_family);

  dmRegion* pIn  = &_dest;
  dmRegion* pOut = &tmp;

  int i;
  size_t k;
  if(nIter<=0) nIter = 1;

  for(i=0;i<nIter;++i) {
    for(k=0;k<rFamily.Size();++k) {
      dmStructuringElement& _elem = rFamily[k];
      erode_region(*pIn,*pOut,_elem);
      std::swap(pIn,pOut);
    }
  }
  rFamily.Transpose();
  rFamily.Complement();
  for(i=0;i<nIter;++i) {
    for(k=0;k<rFamily.Size();++k) {
      dmStructuringElement& _elem = rFamily[k];
      dilate_region(*pIn,*pOut,_elem);
      std::swap(pIn,pOut);
    }
  }
  rFamily.Complement();
  rFamily.Transpose();
  if(pIn!=&_dest) _dest = *pIn;
}
//--------------------------------------------------------
void binary_closing(  const dmRegion& _src, dmRegion& _dest,const dmKernelFamily& _family, int nIter )
{
  _dest = _src;
  dmRegion tmp;
  dmKernelFamily& rFamily = const_cast<dmKernelFamily&>(_family);

  dmRegion* pIn  = &_dest;
  dmRegion* pOut = &tmp;

  int i;
  size_t k;
  if(nIter<=0) nIter = 1;

  rFamily.Complement();
  for(i=0;i<nIter;++i) {
    for(k=0;k<rFamily.Size();++k) {
      dmStructuringElement& _elem = rFamily[k];
      dilate_region(*pIn,*pOut,_elem);
      std::swap(pIn,pOut);
    }
  }

  rFamily.Complement();
  rFamily.Transpose();

  for(i=0;i<nIter;++i) {
    for(k=0;k<rFamily.Size();++k) {
      dmStructuringElement& _elem = rFamily[k];
      erode_region(*pIn,*pOut,_elem);
      std::swap(pIn,pOut);
    }
  }

  rFamily.Transpose();

  if(pIn!=&_dest)
    _dest = *pIn;
}
//---------------------------------------------------------------------
static void __AddBorderRgn( dmRegion& rgn,
                            dmRegion& _dstBorder,
                            dmRegion& _objBorder,
                            const dmRect& _r, int _n, int _dx, int _dy )
{
   _objBorder = _dstBorder;
   _objBorder.ClipToRect( _r );
   if(!_objBorder.IsEmptyRoi()) {
     for(int i=_n;--i>=0;) {
       _objBorder.Translate(_dx,_dy);
       rgn.AddRoi(_objBorder);
     }
   }
}
//--------------------------------------------------------
void handle_region_borders( dmRegion& rgn,
                            const dmRect& r,
                            const dmKernelFamily& _family )
{
  dmRegion _objBorder(r);
  dmRegion _dstBorder;

  _objBorder.GetOutLine(_dstBorder); // image borders
  _dstBorder.AndRoi(rgn);            // object's regions adjacent to border

  if(!_dstBorder.IsEmptyRoi()) {     // we have intersection with borders
     dmMaskDescription _mask;
     _family.GetMaskDescription(_mask);

     __AddBorderRgn(rgn,_dstBorder,_objBorder, dmRect(0,0,r.Width(),1)          ,_mask.Height(),0,-1);  // Haut
     __AddBorderRgn(rgn,_dstBorder,_objBorder, dmRect(0,r.Bottom(),r.Width(),1) ,_mask.Height(),0, 1);  // Bas
     __AddBorderRgn(rgn,_dstBorder,_objBorder, dmRect(0,0,1,r.Height())         ,_mask.Width() ,-1,0);  // Gauche
     __AddBorderRgn(rgn,_dstBorder,_objBorder, dmRect(r.Right(),0,1,r.Height()) ,_mask.Width() , 1,0);  // Droite
  }
}
//--------------------------------------------------------


}; // namespace daim
