
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
#include "templates/processing/dmPartition.h"


namespace daim {
////////////////////////////////////////////////////////////////
region_partition::region_partition()
:__pcount(NULL)
,__sharedmap(false)
,__build_index_table(true)
,__build_count_table(true)
{}
//--------------------------------------------------------------
region_partition::region_partition( const region_partition& _rgnmap )
:__pcount(NULL)
,__sharedmap(true)
,__build_index_table(true)
,__build_count_table(true) 
,__regions(_rgnmap.__regions)
,__partition(_rgnmap.__partition)
{}
//--------------------------------------------------------------
region_partition::region_partition( const basic_partition& _part, const dmLink<map_type>& _map )
:__pcount(NULL)
,__sharedmap(true)
,__build_index_table(true)
,__build_count_table(true) 
,__regions(_map)
,__partition(_part)
{}
//--------------------------------------------------------------
region_partition::~region_partition() 
{ 
  if(__pcount) delete __pcount; 
}
//--------------------------------------------------------------
void region_partition::relabel() 
{
  unsigned count = rgn_count();
  if(count>1) {
    relabel_image_map(__partition,*__regions); 
  }
}
//--------------------------------------------------------------
int region_partition::compact_partition()
{
  int sz = 0;
  if(rgn_count()>1) {
    basic_partition::index_table itable;
    sz = compact_partition_map(__partition,*__regions,itable);
    __partition.reserve(sz);
  }
  return sz;
}
//--------------------------------------------------------------
void region_partition::remove_border_regions()
{
  assign_map_borders( __partition, *__regions,0 );
  __build_index_table = true; // need to rebuild the index table
  __build_count_table = true;
}
//--------------------------------------------------------------
int region_partition::merge_regions( int k, int l )
{
  __partition.resolve( __partition[k] , __partition[l] );
  __partition.update();
  __build_index_table = true; // need to rebuild the index table
  __build_count_table = true;
  return __partition[k];
}
//--------------------------------------------------------------
int region_partition::get_overlapping_regions_labels(  const dmRegion& rgn, labels_array_type& _labels ) const
{
  return daim::get_overlapping_regions_labels( rgn,__partition,*__regions,_labels );
}
//--------------------------------------------------------------
int region_partition::get_border_regions_labels( const dmRegion& rgn, labels_array_type& _labels ) const
{
  return daim::get_border_regions_labels( rgn,__partition,*__regions,_labels );
}
//--------------------------------------------------------------
int region_partition::merge_labels( labels_array_type& _labels, int index )
{
  __build_index_table = true;
  __build_count_table = true;
  return daim::merge_labels(__partition,_labels, index );
}
//--------------------------------------------------------------
int region_partition::get_label( const dmPoint& p )
{
  return ( rgn_count()>0 ?  __partition[(*__regions)[p]] : 0);
}
//--------------------------------------------------------------
int region_partition::get_region_index( size_t _index )
{
	if(__build_index_table && __partition.size()>0) {
		__partition.get_index_table(__index_table);
		__build_index_table = false;
	}
	return  (_index < __index_table.size() ? __index_table[_index] : 0);
}
//--------------------------------------------------------------
int region_partition::get_region_size( size_t _index )
{
  unsigned count = rgn_count();
  if(__build_count_table && count>0) 
  {
    if(!__pcount) __pcount = new int[count+1];
    std::fill(__pcount,__pcount+count+1,0);

    map_type::line_type line = __regions->begin();
    map_type::line_type end  = __regions->end();
    for(;line!=end;++line) {
      map_type::pointer_type rgn = *line;
      for(int x=__regions->width();--x>=0;++rgn) ++__pcount[__partition[*rgn]]; 
    }
    __build_count_table = false;
  } 
  return (count ? __pcount[__partition[_index]] : 0);
}
//--------------------------------------------------------------
int region_partition::get_max_region( size_t& size )
{
  unsigned count = rgn_count();
  if(count) {
	get_region_size(0);
    int* loc = std::max_element(__pcount+1,__pcount+count); 
    size = *loc;
    int region_index = loc - __pcount;
    return region_index;
  }
  return 0;
}
//--------------------------------------------------------------
void region_partition::create_roi( dmRegion& _rgn, int k )
{
  daim::create_roi(*__regions,__partition.bind(
               std::bind2nd(std::equal_to<int>(),k)),_rgn); 
} 
//--------------------------------------------------------------
int region_partition::resolve_overlapping_regions( const dmRegion& rgn )
{
  __build_index_table = true;
  __build_count_table = true;
 
  labels_array_type _labels;
  int idx = daim::get_overlapping_regions_labels(rgn,__partition,*__regions,_labels);
  return daim::merge_labels( __partition , _labels, idx );
}
//--------------------------------------------------------------
void region_partition::__set_up( const dmRect& _r )
{
  if(__pcount) {
    delete __pcount; 
    __pcount = NULL; 
  }

  __build_index_table = true;
  __build_count_table = true;

  if( __sharedmap || __regions.IsNull() || !(_r==__regions->rect())) {
    __sharedmap = false;
    __regions = dmAutoLink<map_type>( new cont_image<dm_int>(_r) );
  }
}
//--------------------------------------------------------------

}; //namespace daim

