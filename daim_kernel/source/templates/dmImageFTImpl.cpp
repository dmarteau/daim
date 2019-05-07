
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

#include "dmMatrixBase.h"
#include "dmMatrixFT.h"

#include "templates/images/dmImageFT.h"

namespace daim {

//-------------------------------------------------------------------------------------
ft_image::ft_image()
: _is_ft(false)
{
  _ft.Data[0] = NULL;
  _ft.Data[1] = NULL;
  _ft.Speq    = NULL;
  dmFT_Initialize(_ft);
}
//-------------------------------------------------------------------------------------
ft_image::ft_image( size_t w, size_t h )
{
  dmFT_Initialize(_ft);
  initialize(w,h);
}
//-------------------------------------------------------------------------------------
ft_image::ft_image( const ft_image& _src  )
{
  dmFT_Initialize(_ft);
  copy_ft(_src,_src.is_ft());
}
//-------------------------------------------------------------------------------------
ft_image::ft_image( const ft_data_t& _src, bool _is_fourier_space )
{
  dmFT_Initialize(_ft);
  copy_ft(_src,_is_fourier_space);
}
//-------------------------------------------------------------------------------------
ft_image::~ft_image() 
{
  dmFT_Destroy(_ft);
}
//-------------------------------------------------------------------------------------
void ft_image::initialize( size_t w, size_t h )
{
  dmASSERT(is_power_of_2(w));
  dmASSERT(is_power_of_2(h));

  _is_ft = false;

  if(w!=width() && h!=height())
  {
    dmFT_Construct(_ft,h,w,NULL);
    construct(&(dmFT_DATA( _ft ))[1][1],_ft.Dim2,_ft.Dim1,
              sizeof(float)*_ft.Dim2);
  }
}
//-------------------------------------------------------------------------------------
ft_image&  ft_image::operator=( const ft_image& _src )
{
  if(&_src!=this) 
    copy_ft(_src,_src.is_ft());  

  return *this;
}
//-------------------------------------------------------------------------------------
void ft_image::copy_ft( const ft_data_t& _src , bool _is_fourier_space )
{
  dmFT_CopyFTData(_ft,_src);

  construct(&(dmFT_DATA( _ft ))[1][1],_ft.Dim2,_ft.Dim1,
              sizeof(float)*_ft.Dim2);

  _is_ft = _is_fourier_space;
}
//-------------------------------------------------------------------------------------
void ft_image::swap_blocks()
{
  dmASSERT(!is_ft());
  dmFT_SwapRealBlocks(*this);
}
//-------------------------------------------------------------------------------------
void ft_image::ft_convolution( const ft_data_t& _ft0, int way )
{
  dmASSERT(is_ft());
  dmFT_Convolution(*this,_ft0,*this,way );
}
//-------------------------------------------------------------------------------------
void ft_image::ft_correlation( const ft_data_t& _ft0 ) 
{
  dmASSERT(is_ft());
  dmFT_Correlation(*this,_ft0,*this);
}
//-------------------------------------------------------------------------------------
void ft_image::compute_ft( int way, bool bswap )
{
  if(way == dmFT_COMPUTE_FORWARD && !is_ft()) {
    if(bswap)  dmFT_SwapRealBlocks(*this);
    dmFT_Compute(*this); 
    _is_ft = true;
  } 
  
  else

  if(way == dmFT_COMPUTE_REVERSE && is_ft()) {
    dmFT_Compute_Inverse(*this); 
    if(bswap) dmFT_SwapRealBlocks(*this);
    _is_ft = false;
  }
}
//-------------------------------------------------------
}; // namespace daim 

