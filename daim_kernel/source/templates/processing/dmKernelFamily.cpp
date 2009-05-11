
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

#include "daim_kernel.h"
#include "templates/processing/dmKernelFamily.h"

#include <algorithm>

//-----------------------------------------------------------------
// dmMaskDescription
//-----------------------------------------------------------------
void dmMaskDescription::Init( dm_int _ox,dm_int _oy,size_t _width,size_t _height)
{
  dmDEBUG_ASSERT((dm_uint)_ox < _width && (dm_uint)_oy < _height);
  mOx     = _ox;
  mOy     = _oy;
  mWidth  = _width;
  mHeight = _height;
}
//-----------------------------------------------------------------
dmMaskDescription::dmMaskDescription()
: mOx(0)
 ,mOy(0)
 ,mWidth(1)
 ,mHeight(1)
{}
//-----------------------------------------------------------------
dmMaskDescription::dmMaskDescription( const dmMaskDescription& _aMask )
: mOx(_aMask.mOx)
 ,mOy(_aMask.mOy)
 ,mWidth(_aMask.mWidth)
 ,mHeight(_aMask.mHeight)
{}
//-----------------------------------------------------------------
dmMaskDescription& dmMaskDescription::operator=( const dmMaskDescription& _aMask )
{
  mOx = _aMask.mOx;
  mOy = _aMask.mOy;
  mWidth  = _aMask.mWidth;
  mHeight = _aMask.mHeight;
  return *this;
}
//-----------------------------------------------------------------
void dmMaskDescription::SetOrigin(dm_int _ox,dm_int _oy)
{
  dmDEBUG_ASSERT((dm_uint)_ox < mWidth && (dm_uint)_oy < mHeight);
  mOx = _ox;
  mOy = _oy;
}
//-----------------------------------------------------------------
dmRect dmMaskDescription::GetMask( const dmRect& _rect ) const
{
  return dmRect( _rect.Left()+mOx,
                 _rect.Top() +mOy,
                 _rect.Width()  - (mWidth -1),
	               _rect.Height() - (mHeight-1)
	             );
}
//-----------------------------------------------------------------
dmRect  dmMaskDescription::GetRect( const dmRect& _rect ) const
{
  return dmRect(_rect.Left()- mOx,
                _rect.Top() - mOy,
                _rect.Width()  + (mWidth -1),
	              _rect.Height() + (mHeight-1)
	             );
}
//------------------------------------------------------------------
// dmKernelDescription
//-----------------------------------------------------------------
dmKernelDescription::dmKernelDescription() : dmMaskDescription()
{
  mNorm = 0;
  mData = new value_type[Size()];
  std::fill(mData,mData + Size(),1);
}
//-----------------------------------------------------------------
dmKernelDescription::dmKernelDescription(
     dm_int _Ox,
     dm_int _Oy,
	   size_t _width,
	   size_t _height,
	   const value_type* _data)
: dmMaskDescription(_Ox,_Oy,_width,_height)
{
	mNorm = 0;
  mData   = new value_type[Size()];
  std::copy(_data,_data + Size(), mData);
}
//-----------------------------------------------------------------
dmKernelDescription::dmKernelDescription( const dmKernelDescription& _desc )
: dmMaskDescription(_desc)
{
  mNorm = _desc.mNorm;;
  mData = new value_type[Size()];
  std::copy(_desc.Data(),_desc.Data() + Size(), mData);
}
//-----------------------------------------------------------------
dmKernelDescription& dmKernelDescription::operator=( const dmKernelDescription& _desc )
{
  if(&_desc!=this)
  {
    size_t sz = Size();
    dmMaskDescription::operator=(_desc);
    if(sz!=Size()) {
      delete mData;
      mData = new value_type[Size()];
    }
    std::copy(_desc.Data(),_desc.Data() + Size(), mData);
    mNorm = _desc.mNorm;
  }
  return *this;
}
//-----------------------------------------------------------------
dmKernelDescription::~dmKernelDescription()
{
  delete mData;
}
//-----------------------------------------------------------------
void dmKernelDescription::SetDescription( dm_int _ox, dm_int _oy,size_t _width,size_t _height,
                                          const value_type* _data)
{
  dmASSERT( _data!=mData );
  size_t sz = Size();
  dmMaskDescription::Init(_ox,_oy,_width,_height);
  if(Size()!=sz) {
    delete mData;
    mData = new value_type[Size()];
  }
  if(_data) std::copy(_data,_data + Size(), mData);
  else
    std::fill(mData,mData + Size(), 0 );
}
//-----------------------------------------------------------------
int dmKernelDescription::Norm() const
{
  if(mNorm==0)
  {
    value_type val = 0;
    for(int i=Size();--i>=0;) val += Data()[i];
    return val;
  } else
    return mNorm;
}
//-----------------------------------------------------------------
void dmKernelDescription::Complement()
{
  for(size_t i=0;i<Size();++i)
     mData[i] = -mData[i];
}
//-----------------------------------------------------------------
void dmKernelDescription::Transpose()
{
  SetOrigin(Width()-Ox()-1,Height()-Oy()-1);
  size_t n = Size()/2;
  for(size_t k=0;k<n;++k)
    std::swap( mData[k], mData[Size()-k-1] );
}
//-----------------------------------------------------------------
// dmKernelFamily
//-----------------------------------------------------------------
dmKernelFamily::dmKernelFamily()
: _mode(0)
{}
//-----------------------------------------------------------------
dmKernelFamily::dmKernelFamily( const dmKernelFamily& _kf )
: _mode(0)
 ,_family(_kf._family)
{}
//-----------------------------------------------------------------
void dmKernelFamily::Complement()
{
  iterator F = Begin();
  iterator L = End();
  for(;F!=L;++F)
    (*F).Complement();
}
//-----------------------------------------------------------------
void dmKernelFamily::Transpose()
{
  iterator F = Begin();
  iterator L = End();
  for(;F!=L;++F)
   (*F).Transpose();
}
//-----------------------------------------------------------------
void dmKernelFamily::GetMaskDescription( dmMaskDescription& mask ) const
{
  // Il faut construire un masque � partir des diffrents masques
  // qui composent la famille
  if(!_family.Empty())
  {
    dmRect rt;
    for(size_t i=0;i<Size();++i) {
      const dmMaskDescription& msk = _family[i];
      rt.Add( dmRect(- static_cast<long>(msk.Ox()),
                     - static_cast<long>(msk.Oy()),
                       msk.Width(),msk.Height()) );
    }
    mask = dmMaskDescription(-rt.Left(),-rt.Top(),rt.Width(),rt.Height());
  }
}
//-----------------------------------------------------------------
dmKernelFamily& dmKernelFamily::operator=( const dmKernelFamily& _kf )
{
  if(&_kf!=this)
    _family = _kf._family;

  return *this;
}
//-----------------------------------------------------------------
