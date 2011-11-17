
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
#include "templates/processing/_dmPartition.h"

using namespace std;

namespace daim {

//--------------------------------------------------------------
basic_partition::basic_partition( unsigned _cnt ) 
: __count(0), __partition(NULL) 
{ 
  reserve(_cnt); 
}
//-------------------------------------------------------
basic_partition::basic_partition( const basic_partition& toCopy )
: __count(0), __partition(NULL) 
{
  (void)operator=(toCopy);
}
//-------------------------------------------------------
basic_partition::~basic_partition()
{ 
  ::operator delete [] (__partition,dm_arena); 
}

//-------------------------------------------------------
basic_partition& basic_partition::operator=( const basic_partition& toCopy)
{
  if(&toCopy!=this) {
    reserve( toCopy.size() );
    std::copy(toCopy.begin(),toCopy.end(),__partition);
  }
  return *this;
}
//-------------------------------------------------------
void basic_partition::reserve( unsigned _cnt ) 
{
  if(__partition) 
    ::operator delete [] (__partition,dm_arena);
  
  __partition = new (dm_arena) dm_int[__count=_cnt]; 
  initialize();  
}
//-------------------------------------------------------
int basic_partition::resolve(dm_int k,dm_int j) 
{
   while(__partition[j]!=j) j=__partition[j];
   while(__partition[k]!=k) k=__partition[k];
   if(j!=k) return __partition[dm_max(j,k)]=dm_min(j,k);

   else return j;
}
//-------------------------------------------------------
int basic_partition::number_of_partitions() const 
{
  int n=1,m = __partition[0];
  for(int j=1;j<__count;++j)
    if(m<__partition[j]) { m = __partition[j]; ++n; }
  return n;
}
//-------------------------------------------------------
int basic_partition::get_index_table( index_table& _itable ) const 
{
  _itable.resize(__count); 
  dm_int n,m,j;

  m = __partition[0];
  for(n=0,j=0;j<__count;++j) 
  {
    if(m<__partition[j]) { 
      m = __partition[j]; ++n;
      _itable[j] = n;
    } 
    else if(m>__partition[j]) 
      _itable[j] = _itable[__partition[j]];
    else 
      _itable[j] = n;
  }
  return n+1;
}
//-------------------------------------------------------
int basic_partition::compact_partition( index_table& _itable )
{
  int _cnt = get_index_table(_itable);
  reserve(_cnt);
  return _cnt;
}
//-------------------------------------------------------   
void basic_partition::update() 
{
  for(dm_int j=0;j<__count;++j) 
     while(__partition[j]!=__partition[__partition[j]])
     __partition[j]=__partition[__partition[j]];
}
//-------------------------------------------------------
// partition api
//-------------------------------------------------------
void assign_map_borders( basic_partition& __partition, const map_type& _map, dm_int k )
{
  // set regions touching edges to background region <0>
  map_type::const_pointer_type top  = *_map.begin();
  map_type::const_pointer_type  bot = *_map.last();

  int pk = __partition[k];
  for(int i=_map.width();--i>=0;) {
    __partition.resolve( *top++, pk );
    __partition.resolve( *bot++, pk );
  }
  int r = _map.rect().Right();
  for(int j=_map.height();--j>=0;) {
    __partition.resolve( _map[j][0], pk );
    __partition.resolve( _map[j][r], pk );
  }
  __partition.update();
}
//--------------------------------------------------------------
void create_roi(dmRegion& _rgn, basic_partition& __partition, const map_type& _map, dm_int _index )
{
  create_roi(_map,__partition.bind(bind2nd(equal_to<dm_int>(),_index)),_rgn); 
}
//--------------------------------------------------------------
static int __push_labels( int x1, int x2, dm_int prev, 
                          const basic_partition& _partition, 
                          map_type::const_line_type _m, 
                          labels_array_type& _labels )
{
  dm_int v;
  for(int i=x1;i<=x2;++i) 
  {
    v = _partition[(*_m)[i]];
    if( v!=prev && v!=0 ) {  
      _labels.push_back( v );
      prev = v;
    }
  }
  return prev;
}
//--------------------------------------------------------------
int get_overlapping_regions_labels( const dmRegion& rgn, const basic_partition& _partition, const map_type& _map, labels_array_type& _labels )
{
  map_type::const_line_type _m = _map.begin(rgn.Rectangle());
  int prev = 0; // 0 == background
  long x1,x2;
  if(rgn.IsRectRoi())
  {
    const dmRect r = rgn.Rectangle();
    x1 = r.Left();
    x2 = r.Right();
    for(long h=r.BoxHeight();--h>=0;++_m) { 
      prev = __push_labels( x1,x2,prev,_partition,_m,_labels ); 
    }
  }
  else 
  {
    dmRgnHandle::iterator it  = rgn.Region().Begin();
    dmRgnHandle::iterator end = rgn.Region().End();
    for(dmRgnPair *p,*pend;it!=end; ++it,++_m) {
      for(p=it.begin(),pend=it.end();p!=pend;++p) {
        prev = __push_labels( (*p).x1,(*p).x2,prev,_partition,_m,_labels );
      }
    }
  }
  return _partition[prev];
}
//--------------------------------------------------------------------------
// get_overlapping_regions_labels2 : Same as get_overlapping_regions_labels
// but preserve order of region hits.
//--------------------------------------------------------------------------
static int __push_labels2( int x1, int x2, dm_int index, 
                           const basic_partition& _partition, 
                           map_type::const_line_type _m, 
                           labels_array_type& _hits )
{
  dm_int v,prev=0;
  for(int i=x1;i<=x2;++i) 
  {
    v = _partition[(*_m)[i]];
    if( v!=prev && _hits[v] == 0 ) { 
        _hits[v] = ++index; 
        prev = v;
    }
  }
  return index;
}
//--------------------------------------------------------------
int get_overlapping_regions_labels2( const dmRegion& rgn, 
                                      const basic_partition& _partition, 
                                      const map_type& _map, 
                                      labels_array_type& _labels )
{
  labels_array_type hits(_partition.size(),0);

  map_type::const_line_type _m = _map.begin(rgn.Rectangle());
  int index = 0;

  long x1,x2;
  if(rgn.IsRectRoi())
  {
    const dmRect r = rgn.Rectangle();
    x1 = r.Left();
    x2 = r.Right();
    for(long h=r.BoxHeight();--h>=0;++_m) { 
      index = __push_labels2( x1,x2,index,_partition,_m,hits ); 
    }
  }
  else 
  {
    dmRgnHandle::iterator it  = rgn.Region().Begin();
    dmRgnHandle::iterator end = rgn.Region().End();
    for(dmRgnPair *p,*pend;it!=end; ++it,++_m) {
      for(p=it.begin(),pend=it.end();p!=pend;++p) {
        index = __push_labels2( (*p).x1,(*p).x2,index,_partition,_m,hits );
      }
    }
  }
  
  _labels.resize(index);
  for(int i = hits.size();--i>=0;) {
       if(hits[i]) {
          _labels[hits[i]-1] = i;
       }
  }
  
  return _labels[0];
}
//--------------------------------------------------------------
int get_border_regions_labels( const dmRegion& _rgn, const basic_partition& _partition, const map_type& _map, labels_array_type& _labels )
{
  dmRegion rgn_outline;
  _rgn.GetOutLine(rgn_outline);
  return get_overlapping_regions_labels(_rgn,_partition,_map,_labels);
}
//--------------------------------------------------------------
int merge_labels( basic_partition& _partition , labels_array_type& _labels, dm_int k )
{
  labels_array_type::iterator it   = _labels.begin();
  labels_array_type::iterator last = _labels.end();
  int pk = _partition[k];
  for(;it!=last;++it) { _partition.resolve( *it, pk ); }
  _partition.update();
  return _partition[pk];
}
//--------------------------------------------------------------
labels_array_type::iterator compact_labels( labels_array_type& _labels )
{
  sort( _labels.begin(), _labels.end() );
  return unique( _labels.begin(), _labels.end() );
}
//--------------------------------------------------------------
unsigned create_region_partition( basic_partition& part, map_type& map, 
         const image<dm_uint8>& src,int bgvalue, connectivity _connect)
{
  typedef image<dm_uint8>::const_pointer_type lsrc;
  typedef map_type::pointer_type lmap;
 
  int left,top,right,bottom;
  left   = top = 0;         // start pixel
  right  = src.width()-1;   // end pixel
  bottom = src.height()-1;

  lsrc lj,ljp1;
  lmap mj,mjp1;

  map.fill(0);
  unsigned count = 0;

  //--------------------------------------------------------------
  // Si on est en connectivit� 8 le fond est en connectivit� 4
  // et vice versa
  //--------------------------------------------------------------
  connectivity _fgconnect = _connect;
  connectivity _bgconnect = (_connect==connect8 ? connect4 : connect8);

  int i,j,c,r;
  for( j=top;j<bottom;++j) 
  {
    lj = src[j]; ljp1 = src[j+1];
    mj = map[j]; mjp1 = map[j+1];
    for( i=left;i<right;++i) 
    {
      r = mj[i];
      if(r==0) { r = mj[i] = ++count; }

      if( (c = lj[i]) == bgvalue ) _connect = _bgconnect;
      else                         _connect = _fgconnect;

      if( lj[i+1]==c ) mj[i+1] = r; // E
      if( ljp1[i]==c ) mjp1[i] = r; // S
      if(_connect==connect8) {
        if( ljp1[i+1]==c )        mjp1[i+1] = r; // SE 
        if( i>0 && ljp1[i-1]==c ) mjp1[i-1] = r; // SW
      }
    } 
  }
  
  // last line 
  lj = src[bottom];
  mj = map[bottom];
  for( i=left;i<right;++i) { 
    if( (r=mj[i]) == 0 ) { r = mj[i] = ++count; }
    if( lj[i+1]==lj[i]) mj[i+1] = r; 
  }

  // last column 
  for( j=top;j<bottom;++j) {
   if( (r=map[j][right]) == 0 ) { r = map[j][right] = ++count; }
   if(src[j][right]==src[j+1][right]) map[j+1][right] = r; 
  }

  if(map[bottom][right]==0) map[bottom][right] = ++count;

  if(!count) return 0;
  part.reserve(++count);
  for( j=top;j<bottom;++j) 
  {
    lj = src[j]; ljp1 = src[j+1];
    mj = map[j]; mjp1 = map[j+1];
    for( i=left;i<right;++i) 
    {
      r = mj[i];
      if( (c = lj[i]) == bgvalue ) _connect = _bgconnect;
      else                         _connect = _fgconnect;

      if( lj[i+1]==c ) part.resolve( mj[i+1],r);  // E
      if( ljp1[i]==c ) part.resolve( mjp1[i],r);  // S
      if(_connect==connect8) {
        if( ljp1[i+1]==c )        part.resolve( mjp1[i+1],r); // SE 
        if( i>0 && ljp1[i-1]==c ) part.resolve( mjp1[i-1],r); // SW
      }
    } 
  }

  // last line 
  lj = src[bottom];
  mj = map[bottom];
  for( i=left;i<right;++i) {
    if(lj[i+1]==lj[i]) part.resolve( mj[i+1],mj[i] );
  }

  // last column 
  for( j=top;j<bottom;++j) {
    if(src[j][right]==src[j+1][right]) part.resolve( map[j][right],map[j+1][right] );   
  }

  part.update(); // update basic_partition
  return count;
}
//--------------------------------------------------------------
unsigned create_region_partition( basic_partition& _part, map_type& _map, 
                                  const dmRegion& _rgn, connectivity _connect)
{
  cont_image<dm_uint8> _mask(_map.rect(),0);
  fill(_rgn,_mask,pixel_traits<dm_uint8>::object());
  return create_region_partition(_part,_map,_mask,pixel_traits<dm_uint8>::zero(),_connect);
}
//--------------------------------------------------------------
int merge_regions( basic_partition& _partition, const map_type& _map, const dmRegion& _region )
{
  labels_array_type _labels;
  int k = get_overlapping_regions_labels( _region,_partition,_map,_labels );
  return merge_labels( _partition , _labels, k ); 
}
//--------------------------------------------------------------
void relabel_image_map( basic_partition& _part, map_type& __regions )
{
  map_type::line_type line = __regions.begin();
  map_type::line_type end  = __regions.end();
  for(;line!=end;++line) {
    map_type::pointer_type rgn = *line;
    for(int i=__regions.width();--i>=0;++rgn) *rgn = _part[*rgn];
  }
}
//--------------------------------------------------------------
int compact_partition_map(  basic_partition& _part, map_type& __regions, basic_partition::index_table& itable )
{
  int sz = _part.get_index_table( itable );
  map_type::line_type line = __regions.begin();
  map_type::line_type end  = __regions.end();
  for(map_type::pointer_type rgn;line!=end;++line) {
    rgn = *line;
    for(int i=__regions.width();--i>=0;++rgn) {  
      *rgn = itable[_part[*rgn]]; 
    }
  }
  _part.reserve(sz);
  return sz;
}
//--------------------------------------------------------------

}; // namespace daim
