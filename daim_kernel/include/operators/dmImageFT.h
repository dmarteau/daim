#ifndef dmImageFT_Operator_h
#define dmImageFT_Operator_h

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

//--------------------------------------------------------
// File         : dmImageFT.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

#include "templates/images/dmImageFT.h"

typedef daim::ft_image dmImageFT;

bool dmCopyImageDataFT ( const dmImage& img, dmImageFT& ft, const dmRect& r );
bool dmComputeForwardFT( const dmImage& img, dmImageFT& ft, bool swblk, const dmRect r );  
bool dmComputeInverseFT( dmImage& img, dmImageFT& ft,dm_real range_min, dm_real range_max,
                         bool swblk, const dmPoint& p ); 

//===========================================================
// NOTE: dmComputeFTCorrelation will swap real data blocks
// you will need to call dmComputeInverseFT with swblk==true
// in order to get correct real data
//===========================================================
bool dmComputeFTConvolution  ( const dmImage&, dmImageFT&, int way, const dmPoint& = dmPoint(0,0));
bool dmComputeFTCorrelation  ( const dmImage&, dmImageFT&, const dmPoint& = dmPoint(0,0));

bool dmComputeFTPowerSpectrum( dmImage&, dmImageFT& );
bool dmComputeFTPhaseSpectrum( dmImage&, dmImageFT& );
//--------------------------------------------------------
#endif // dmImageFT_Operator_h
