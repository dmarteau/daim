#ifndef dmHistogram_h
#define dmHistogram_h

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
// File         : dmHistogram.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------
#include "_dmHistogram.h"

class dmHistogram
{
  public:
    typedef const daim::histogram_value* const_iterator;
    typedef daim::image<dm_uint8> image_type;

    enum {
      hsize = daim::histogram_size,
      hmax  = daim::histogram_max,
    };

  private:
    daim::histogram_type h_data;
    daim::gap<dm_uint8>  h_slice;

    const_iterator _start()  const { return h_data; }
    const_iterator _last()   const { return h_data+hsize; }

  public:
    dmHistogram();

    template<class T,class E> 
    void GetHistogram( 
        const daim::image<T>& src, 
        const dmRegion& rgn,
        const E& extract )
    {
      std::fill(h_data,h_data+hsize,0);
      daim::extract_histogram(rgn,src,h_data,extract);
      SetDensitySlice( MinDensityValue(),MaxDensityValue() );
    } 

    void GetHistogram( const daim::image<dm_uint8>&,const dmRegion& );
    void GetHistogram( const daim::image<dm_rgb24>&,const dmRegion& );
    void GetHistogram( const daim::image<float>&   ,const dmRegion& );

    daim::histogram_type& Data() { return h_data; }
    void                  ClearData();

    int operator[](int i) const { return h_data[i]; } 
	  //
    void UpdateDensitySlice();
    void SetDensitySlice( dm_uint8 lower,dm_uint8 upper );
    void SetDensitySlice( const daim::gap<dm_uint8>& _range );

    const daim::gap<dm_uint8>& DensitySlice() const { return  h_slice; }

    unsigned int MinDensityValue() const; 
    unsigned int MaxDensityValue() const;

    // following return results according to density slice
    const_iterator Begin() const { return _start()+h_slice.lower;   }
    const_iterator End()   const { return _start()+h_slice.upper+1; }

    unsigned int MaxContrib() const;
    unsigned int MinContrib() const;

    int     Sum()      const;
    dm_real Mean()     const;    
    dm_real Rms()      const; 
    dm_real Variance() const;

    void Equalize   ( dmColorMapArray )       const; // Compute histogram equalization
    void Cumulate   ( daim::histogram_type& ) const; // Compute cumulated sum 
    void Extend     ( dmColorMapArray )       const; // Compute histogram extension
    void Specify    ( dmColorMapArray _output, dmColorMapArray _input   ) const; // Compute histogram specification
    void ExtendRange( dmColorMapArray _cmap, const daim::gap<dm_uint8>& ) const; 
    
    DM_DECL_ARENA_OPERATOR_NEW(dmHistogram)
};
//-------------------------------------------------------------
inline void  dmHistogram::SetDensitySlice( const daim::gap<dm_uint8>& _range ) 
{
   SetDensitySlice(_range.min(), _range.max() );
}
//-------------------------------------------------------------
inline void dmHistogram::Equalize( dmColorMapArray _cmap ) const
{
  daim::equalize_histogram(_cmap,h_data);
}
//---------------------------------------------------------
inline void dmHistogram::Cumulate( daim::histogram_type& _H ) const
{
  daim::cumulate_histogram(_H,h_data);
}
//---------------------------------------------------------
inline void dmHistogram::Extend( dmColorMapArray _cmap ) const
{ 
  ExtendRange(_cmap,daim::gap<dm_uint8>(0,hmax));
}
//---------------------------------------------------------
inline void dmHistogram::ExtendRange( dmColorMapArray _cmap, 
                               const daim::gap<dm_uint8>& _range ) const
{
  daim::extend_histogram_map( _cmap, DensitySlice(),_range );
}
//---------------------------------------------------------
inline void dmHistogram::Specify( dmColorMapArray _output, dmColorMapArray _input   ) const
{
  daim::specify_histogram(_output,h_data,_input);
}
//-------------------------------------------------------------
#endif
