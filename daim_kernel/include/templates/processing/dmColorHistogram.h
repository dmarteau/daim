#ifndef dmColorHistogram_h
#define dmColorHistogram_h

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
// File         : dmColorHistogram.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

class dmHistogram;
class dmRegion;
//-------------------------------------------------------------
class dmColorHistogram
{
  private:
    enum { max_channels = 255 };

    dm_uint      nchannels;
    dmHistogram* histograms[max_channels];

    void init_histograms   ( dm_uint channels );
    void realloc_histograms( dm_uint channels );
    void clear_histograms();

    typedef daim::image<dm_rgb24>  rgb_image;
    typedef daim::image<dm_uint8>  channel8;

  public:
    dmColorHistogram( dm_uint channels = 3 );
   ~dmColorHistogram();

    dmHistogram*       operator[]( dm_uint channel );
    const dmHistogram* operator[]( dm_uint channel ) const;

    dm_uint GetChannels() const             { return nchannels; }
    void    SetChannels( dm_uint channels ) { realloc_histograms(channels);  } 
};
//-------------------------------------------------------------

#endif // dmColorHistogram_h
