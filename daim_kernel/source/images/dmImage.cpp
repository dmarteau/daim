
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

#define dmUseKernelImage
#include "daim_kernel.h"

#include "templates/images/dmImageErrors.h"

//-------------------------------------------------------------
// class dmImage
//-------------------------------------------------------------
dmImage::dmImage(size_t _Width,size_t _Height)
: _Rect(0,0,_Width,_Height)
{
}
//--------------------------------------------------------------
dmImage::~dmImage() 
{}
//---------------------------------------------------
dmLink<dmImage> dmImage::CreateCopy( const dmImageDescriptor& _desc, const dmRect& r ) const
{
  if(Rect() < r  ) dmTHROW( EInvalidImageSize() );
  dmLink<dmImage> image = _desc.CreateImage(r.Width(),r.Height());
  image->GetCopy( *this, r, dmPoint(0,0) );
  image->ApplyUnits( *this );
  return image;
}
//---------------------------------------
/*
dmLink<dmImage> dmImage::CreateCopy( dmImageDescriptor& _desc ) const {
  return CreateCopy(_desc, Rect() );
}
//---------------------------------------
dmLink<dmImage> dmImage::CreateCopy( const dmRect& _rect ) const {
  return CreateCopy( TypeDescriptor(), _rect );
}
//---------------------------------------
dmLink<dmImage> dmImage::CreateCopy() const { 
  return CreateCopy( TypeDescriptor() ); 
}
*/
//---------------------------------------
void dmImage::SetUnits(const dmSpatialUnits& _calibr)
{
  _Units = dmAutoLink<dmSpatialUnits>(new dmSpatialUnits(_calibr));
}
//---------------------------------------
bool dmImage::GetUnits( dmSpatialUnits& _u ) const
{
  if(HasUnits()) { _u = *_Units; }
  return HasUnits();
}
//---------------------------------------
void dmImage::ApplyUnits(const dmImage& im)
{
  _Units = im._Units;
}
//---------------------------------------
