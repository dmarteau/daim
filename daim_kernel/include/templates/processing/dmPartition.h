#ifndef dmPartition_h
#define dmPartition_h

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
// File         : dmPartition.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------
#include "_dmPartition.h"

namespace daim {

class region_partition
{
  protected:
    int* __pcount;
    bool __sharedmap;
	bool __build_index_table;
    bool __build_count_table;

    dmLink<map_type> __regions;
    basic_partition  __partition;

    basic_partition::index_table __index_table;

    void __set_up( const dmRect& );

  public:
    region_partition();
    region_partition( const basic_partition&, const dmLink<map_type>& );
    region_partition( const region_partition& );
    virtual ~region_partition();

    dmRect rect() const { return __regions->rect(); }

    void relabel();
    int  compact_partition();
    void remove_border_regions();
    int  resolve_overlapping_regions( const dmRegion& rgn );
    int  merge_regions( int k, int l );
    int  get_overlapping_regions_labels( const dmRegion& rgn, labels_array_type&  ) const;
    int  get_border_regions_labels( const dmRegion& rgn, labels_array_type& ) const;
    int  merge_labels( labels_array_type&, int index );
    int  get_label( const dmPoint& p );

    template<class P> 
    void select_regions_size( labels_array_type& lbls, P pred ) {
      unsigned count = rgn_count();
      for( unsigned i=0;i<count;++i) {
        if( pred( get_region_size(i) )) lbls.push_back(__partition[i]);
      }
    }

    int  get_max_region  ( size_t& size );
    int  get_region_size ( size_t index );
    int  get_region_index( size_t index );
   
    unsigned rgn_count() const { return __partition.size(); }

   // Accessors
   const map_type& map()              const { return *__regions; }
   const basic_partition& partition() const { return __partition; }

   // Create ROI from region index
   void create_roi( dmRegion& _rgn, int _index );

   template<class P,class T> 
   unsigned int make_regions( const image<T>& src,P predicat,connectivity _connect)
   {
     __set_up( src.rect() );
     return create_map(*__regions,__partition,src,predicat,_connect);
   }    
};
//--------------------------------------------------------

}; //namespace daim


#endif // dmPartition_h

