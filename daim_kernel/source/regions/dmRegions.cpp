
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

#define dmUserIncludes
#include "daim_kernel.h"

/*
#define _CHECK_VALID \
   assert((RoiRgn.size()!=0?RoiRgn.begin()!=0:true));
*/
#define _CHECK_VALID

//----------------------------------------------------------
/// dmRgnHandle Operations (privates)
//----------------------------------------------------------
__dmKernel void dmRegion::AddRegion( const dmRgnHandle& rgn)
{
  if(RoiType!=NoRoi) {
    dmRgnHandle *pRegion = &RoiRgn, *tmprgn =NULL;
    if(RoiType==RectRoi) {
      tmprgn = dmRgnHandle::CreateRegion(&RoiRect);
      if(tmprgn) pRegion = tmprgn;
    }

    if(dmRgnHandle::AddRegionData(pRegion,&rgn,&RoiRgn)) 
      SetRgnRoi(RoiRgn);

    if(tmprgn) 
      dmRgnHandle::DisposeRegion(tmprgn);

  } else 
    SetRgnRoi(rgn);

  _CHECK_VALID
}
//----------------------------------------------------------
__dmKernel void dmRegion::SubRegion( const dmRgnHandle& rgn)
{
  if(RoiType!=NoRoi) 
  {
    dmRgnHandle *pRegion = &RoiRgn, *tmprgn=NULL;
    if(RoiType==RectRoi) {
      tmprgn = dmRgnHandle::CreateRegion(&RoiRect);
      if(tmprgn) pRegion = tmprgn;
    }

    if(dmRgnHandle::SubRegionData(pRegion,&rgn,&RoiRgn)) 
      SetRgnRoi(RoiRgn);

    if(tmprgn) 
      dmRgnHandle::DisposeRegion(tmprgn);
  }

  _CHECK_VALID
}
//----------------------------------------------------------
__dmKernel void dmRegion::XorRegion( const dmRgnHandle& rgn)
{
  if(RoiType!=NoRoi) 
  {
    dmRgnHandle *pRegion = &RoiRgn, *tmprgn=NULL;
    if(RoiType==RectRoi) {
      tmprgn = dmRgnHandle::CreateRegion(&RoiRect);
      if(tmprgn) pRegion = tmprgn;
    }

    if(dmRgnHandle::XorRegionData(pRegion,&rgn,&RoiRgn)) 
      SetRgnRoi(RoiRgn);

    if(tmprgn) 
      dmRgnHandle::DisposeRegion(tmprgn);

  } else
    SetRgnRoi(rgn);

  _CHECK_VALID
}
//----------------------------------------------------------
__dmKernel void dmRegion::AndRegion( const dmRgnHandle& rgn)
{
   if(RoiType!=NoRoi)
   {
     dmRgnHandle *pRegion = NULL;
     if(RoiType==RectRoi) {
       RoiRgn = rgn;
       pRegion = RoiRgn.ClipToRect(RoiRect);
     } else 
       pRegion = dmRgnHandle::AndRegionData(&RoiRgn,&rgn,&RoiRgn);

     if(pRegion) 
       SetRgnRoi(*pRegion);
   }

   _CHECK_VALID
}
//----------------------------------------------------------
__dmKernel dmRegion& dmRegion::GetOutLine( dmRegion& aDest, bool _inner ) const
{
  if(RoiType!=NoRoi) 
  {
    if(IsValidRoi()) 
    {
      const dmRgnHandle* rgn = &Region();
      dmRgnHandle* tmpRgn = NULL;
      if(IsRectRoi()) rgn = tmpRgn = dmRgnHandle::CreateRegion(&Rectangle());
      if(_inner) {
        dmRgnHandle::InnerBoundaryRgn( &aDest.Region(), rgn );
      } else { 
        dmRgnHandle::OuterBoundaryRgn( &aDest.Region(), rgn );
      }

      aDest.SetRgnRoi(aDest.Region());

      if(tmpRgn) 
        dmRgnHandle::DisposeRegion(tmpRgn);
	  }
  }
  return aDest;
}
//----------------------------------------------------------
__dmKernel bool dmRegion::operator==(const dmRegion& _toCompare) const
{
  if( IsValidRoi() && _toCompare.IsValidRoi() ) 
  {
    if(RoiType==NoRoi) return _toCompare.RoiType==NoRoi;
    else 
    {
      if(Rectangle()==_toCompare.Rectangle()) {      
        if(!IsRectRoi()) {
          if(!_toCompare.IsRectRoi()) return Region() == _toCompare.Region();
          else return Region().IsRectRgn(); // to Compare is rect
        } else return (_toCompare.IsRectRoi() ? true : _toCompare.Region().IsRectRgn());
      }
    }
  }
  return false;
}
//----------------------------------------------------------
__dmKernel dmRegion& dmRegion::FlipRoi( dmRegion& aDest, bool bHor,bool bVer ) const
{
  if(IsValidRoi()) {
    if( !IsRectRoi() ) {
      dmRgnHandle* rgn = NULL;
      dmRegion _roi;
      if(bVer && bHor) {
        dmRgnHandle tmprgn( aDest.Region() );
        rgn = dmRgnHandle::FlipVerRgn( &tmprgn, &Region() );
        rgn = dmRgnHandle::FlipHorRgn( &aDest.Region(), &tmprgn );
      } 
      else if(bVer) rgn = dmRgnHandle::FlipVerRgn( &aDest.Region(), &Region() );
      else if(bHor) rgn = dmRgnHandle::FlipHorRgn( &aDest.Region(), &Region() );
      if(rgn) aDest.SetRgnRoi(*rgn);
    } else  aDest.SetRectRoi(Rectangle());
  } else aDest.KillRoi();
  return aDest;
}
//----------------------------------------------------------
