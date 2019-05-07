
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
#define dmUseKernelImageTemplates
#include "daim_kernel.h"
#include "templates/processing/dmArithmetics.h"
#include "templates/processing/dmHistogram.h"

#include <numeric>
#include <math.h>

#define REAL( x ) static_cast<dm_real>( x )
//---------------------------------------------------------
dmHistogram::dmHistogram() : h_slice(0,hmax)
{
  std::fill(h_data,h_data+hsize,0);
};
//---------------------------------------------------------
void dmHistogram::GetHistogram( const daim::image<dm_uint8>& src,const dmRegion& rgn )
{
  GetHistogram(src,rgn,daim::identity<dm_uint8>());
}
//---------------------------------------------------------
void dmHistogram::GetHistogram( const daim::image<float>& src,const dmRegion& rgn )
{
  #ifdef DM_CONFIG_MSVC_BUILD
    src.begin(); // VC6 COMPILER BUG !!!
  #endif
  GetHistogram(src,rgn,daim::_extract_range<float>(daim::minmax(rgn,src)));
}
//---------------------------------------------------------
void dmHistogram::GetHistogram( const daim::image<dm_rgb24>& src,const dmRegion& rgn )
{
  GetHistogram(src,rgn,daim::pixel_traits<dm_rgb24>::to_scalar());
}
//---------------------------------------------------------
void dmHistogram::UpdateDensitySlice()
{
  SetDensitySlice( MinDensityValue(),MaxDensityValue() );
}
//---------------------------------------------------------
void dmHistogram::SetDensitySlice( dm_uint8 lower,dm_uint8 upper )
{
  h_slice = daim::gap<dm_uint8>( lower, upper );
}
//---------------------------------------------------------
void dmHistogram::ClearData()
{
  std::fill(h_data,h_data+hsize,0);
  SetDensitySlice(0,hmax);
}
//---------------------------------------------------------
unsigned int dmHistogram::MinDensityValue() const { int i=0;    while(i<hmax && h_data[i]==0 ) { ++i; } return i; }  
unsigned int dmHistogram::MaxDensityValue() const { int i=hmax; while(i>0    && h_data[i]==0 ) { --i; } return i; } 
unsigned int dmHistogram::MaxContrib()      const { return std::max_element(Begin(),End()) - _start(); }
unsigned int dmHistogram::MinContrib()      const { return std::min_element(Begin(),End()) - _start(); }
//---------------------------------------------------------
int dmHistogram::Sum() const 
{ 
   return std::accumulate(Begin(),End(),0); 
}
//---------------------------------------------------------
dm_real dmHistogram::Mean() const 
{
  double r=0;
  for(int i=h_slice.lower;i<=h_slice.upper;++i) r+=i*h_data[i];
  return REAL(r/Sum());
}    
//----------------------------------------------------------
dm_real dmHistogram::Rms() const
{
  double r=0;
  for(int i=h_slice.lower;i<=h_slice.upper;++i) { r+=h_data[i]*i*i; }
  return REAL(sqrt(r/Sum()));
}
//----------------------------------------------------------
dm_real dmHistogram::Variance() const
{
  double r=0,t,m = Mean();
  for(int i=h_slice.lower;i<=h_slice.upper;++i) {
    t = i - m; 
    r+=h_data[i]*t*t;
  }
  return REAL(r/(Sum()-1)); 
}
//---------------------------------------------------------
