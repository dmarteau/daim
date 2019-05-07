
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
#define dmUseKernelTemplates
#include "daim_kernel.h"
#include "templates/processing/dmHistogram.h"
#include "templates/processing/dmColorHistogram.h"

//---------------------------------------------------
void dmColorHistogram::realloc_histograms( dm_uint nchannels )
{
  dmASSERT( nchannels < max_channels );
  for(dm_uint i=0;i<nchannels;++i) {
   if(histograms[i] == NULL) 
     histograms[i] = new dmHistogram; 
  }
}
//---------------------------------------------------
void dmColorHistogram::clear_histograms()
{
  for(dm_uint i=0;i<max_channels;++i) {
   if(histograms[i] != NULL) 
     histograms[i]->ClearData();
   }
}
//---------------------------------------------------
void dmColorHistogram::init_histograms( dm_uint nchannels )
{
  dm_memset_handler(histograms,0,sizeof(histograms));
  realloc_histograms(nchannels);  
}
//---------------------------------------------------
dmColorHistogram::~dmColorHistogram()
{
  for(int i=0;i<max_channels;++i) 
   if(histograms[i] != NULL) 
      delete histograms[i];
}
//---------------------------------------------------
dmColorHistogram::dmColorHistogram( dm_uint nchannels )
{
  init_histograms(nchannels);
}
//---------------------------------------------------
dmHistogram* dmColorHistogram::operator[]( dm_uint nchannel )
{
  if(nchannel<max_channels)
    return histograms[nchannel];
  return NULL;
}
//---------------------------------------------------
const dmHistogram* dmColorHistogram::operator[]( dm_uint nchannel ) const
{
  if(nchannel<max_channels)
    return histograms[nchannel];
  return NULL;
}
//---------------------------------------------------
