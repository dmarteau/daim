#ifndef dmCooccurrences_h
#define dmCooccurrences_h

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
// File         : dmCooccurrences.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

#include "dmMatrixBase.h"
#include <vector>

typedef enum {
  CO_BIN256 = 0,
  CO_BIN128 = 1,
  CO_BIN64  = 2,
  CO_BIN32  = 3,
  CO_BIN16  = 4,
  CO_BIN8   = 5
} EBinType;
//-------------------------------------------------------------
dm_double dmMatrix_Homogeneity      ( const dmRectMatrix&, dm_double );
dm_double dmMatrix_Contrast         ( const dmRectMatrix&, dm_double );
dm_double dmMatrix_Entropy          ( const dmRectMatrix&, dm_double );
dm_double dmMatrix_Correlation      ( const dmRectMatrix&, dm_double );
dm_double dmMatrix_LocalHomogeneity ( const dmRectMatrix&, dm_double );
dm_double dmMatrix_Directivity      ( const dmRectMatrix&, dm_double );
dm_double dmMatrix_Uniformity       ( const dmRectMatrix&, dm_double );
dm_double dmMatrix_Clustering       ( const dmRectMatrix&, dm_double );
//-------------------------------------------------------------
class dmCooccurrence
{
  public:
    typedef daim::image<dm_uint8> image_type;
    typedef std::vector<dmPoint>  points_list;

  private:
    dmRectMatrix  c_data; 
    points_list   c_pattern; // definition of the pattern
    EBinType      c_nbin;
    dm_double     Nc;

  public:
    dmCooccurrence( const points_list& , EBinType nbin = CO_BIN256 );
    dmCooccurrence( const image_type& image,const points_list& , EBinType nbin = CO_BIN256 );
    dmCooccurrence( const image_type& image,const dmRegion&, const points_list& , EBinType nbin = CO_BIN256 );
   ~dmCooccurrence() {}

    bool operator()( const image_type& image, const points_list&  );
    bool operator()( const image_type& image, const dmRegion&, const points_list&  );
    bool operator()( const image_type& image );
    bool operator()( const image_type& image, const dmRegion&  );

    void               SetTransform( const points_list& _pattern );
    const points_list& GetTransform() const { return c_pattern; }

    const dmRectMatrix& Data() const { return c_data; }
    EBinType GetBin()          const { return c_nbin; }

    void   Clear();
    size_t Size()  const { return c_data.NRows(); }

    // results 
    dm_double Sum()               const { return Nc; }
    dm_double Homogeneity()       const { return dmMatrix_Homogeneity      (c_data,Nc); }
    dm_double Contrast()          const { return dmMatrix_Contrast         (c_data,Nc); }
    dm_double Entropy()           const { return dmMatrix_Entropy          (c_data,Nc); }
    dm_double Correlation()       const { return dmMatrix_Correlation      (c_data,Nc); }
    dm_double Local_homogeneity() const { return dmMatrix_LocalHomogeneity (c_data,Nc); }
    dm_double Directivity()       const { return dmMatrix_Directivity      (c_data,Nc); }
    dm_double Uniformity()        const { return dmMatrix_Uniformity       (c_data,Nc); } 
};
//-------------------------------------------------------------

#endif // dmCooccurrences_h
