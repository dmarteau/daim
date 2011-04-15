#ifndef _dmPartition_h
#define _dmPartition_h

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
// File         : _dmPartition.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

// Implementation of basic partitions 

#include <vector>

namespace daim {


class basic_partition 
{
 private:
   dm_int  __count;
   dm_int* __partition;

 public:
   typedef std::vector<dm_int> index_table;
   typedef const dm_int* iterator;

   basic_partition( unsigned _cnt=1);
   basic_partition( const basic_partition& );
  ~basic_partition();

   basic_partition& operator=( const basic_partition& );

 public:
   // Reset partition elements
   void initialize() { 
     for(dm_int j=0;j<__count;++j) 
      __partition[j] = j; 
   }

   // Specify the number of elements
   // and reinit the partition
   void reserve( unsigned _cnt );

   // return the number of elements  
   int size() const { return __count; }
   int operator[](int i) const { return __partition[i]; }     

   iterator begin() const { return __partition; }
   iterator end()   const { return __partition+__count; }

   // Return the number of partitions
   int number_of_partitions() const; 

   // Assign k and j index  to the same partition partition
   int resolve(dm_int k,dm_int j);

   // Update partition
   // This function has to be called after
   // multiples call to "resolve"
   void update(); 

   // Reduce the size of the index array
   // and create a mapping table
   int compact_partition( index_table&  );

   // Assign conditionnaly the partition elements
   // to the element partition k
   template<class P> 
   P& resolve_if( P& pred, dm_int k ) 
   { 
     for(dm_int j=0;j<__count;++j) { 
       if(pred(__partition[j])) resolve(__partition[j],k); 
     }
     update(); return pred;
   }

	 
   // Re-index partition elements and store the result
   // in the array given as parameter 
   // Return the number of partitions
   int get_index_table( index_table& ) const; 

   //-------------------------------------------------------------
   // fonction binding pour gerer l'indirection
   //-------------------------------------------------------------
   template<class Op> 
   struct binder : public std::unary_function<int,typename Op::result_type>
   {
     typedef typename Op::result_type result_type;

     const basic_partition& __part;
     Op op;
     binder( const basic_partition& _p, const Op& _op ) : __part(_p), op(_op) {}
     result_type operator()(dm_int x) { 
       return op(__part[x]); 
     }       
   };

   template<class Op> 
   binder<Op> bind( const Op& op ) const { 
      return binder<Op>(*this,op); 
    }
};



typedef image<dm_int>::image_type map_type;
typedef std::vector<dm_int> size_table_type;
typedef std::vector<dm_int> labels_array_type;

//-------------------------------------------------------------------
// partitionne une image par aggregation de points qui verifient le
// predicat P, la partition 0 n'est pas associee � une region 
//-------------------------------------------------------------------
template<class P,class T>
unsigned setup_regions( const image<T>& src,map_type& regions,P predicat, 
                        connectivity _connect )
{
  int left,top,right,bottom;
  left   = top = 0;       // start pixel
  right  = src.width()-1;  // end pixel
  bottom = src.height()-1;

  regions.fill(0);
  unsigned count = 0;

  int i,j,r;

  typename image<T>::value_type c;

  for( j=top;j<bottom;++j) {
    for( i=left;i<right;++i) {
      r = regions[j][i];
      if(r==0) { r = regions[j][i] = ++count; }
      c = src[j][i];
      if( predicat( src[j][i+1],c ) ) regions[j][i+1]   = r;   // E
      if( predicat( src[j+1][i],c ) ) regions[j+1][i]   = r;   // S
      if(_connect==connect8) {
        if( predicat( src[j+1][i+1],c) )        regions[j+1][i+1] = r; // SE 
        if( i>0 && predicat( src[j+1][i-1],c) ) regions[j+1][i-1] = r; // SW
      }
    } 
  }

  // last line 
  for( i=left;i<right;++i) 
  { 
    if( (r=regions[bottom][i]) == 0 ) {
      r = regions[bottom][i] = ++count;
    }
    if( predicat(src[bottom][i+1], src[bottom][i]) ) 
      regions[bottom][i+1] = r; 
  }

  // last column 
  for( j=top;j<bottom;++j)
  {
    if( (r=regions[j][right]) == 0 ) {
       r = regions[j][right] = ++count;
    }
    if( predicat(src[j][right], src[j+1][right]) ) 
       regions[j+1][right] = r; 
  }

  if(regions[bottom][right]==0) 
      regions[bottom][right] = ++count;

  return count;
} // setup_regions
//-------------------------------------------------------------------
// update a partition and set an image 
//-------------------------------------------------------------------
template<class P,class T> 
void update_partition_map(const image<T>& src,map_type& regions,unsigned count,
                     basic_partition& part,P predicat,connectivity _connect)
{ 
  if(!count) return;
  int left,top,right,bottom;
  left   = top = 0;        // start pixel
  right  = src.width()-1;  // end pixel
  bottom = src.height()-1;

  int i,j,r;

  typename image<T>::value_type c;

  part.reserve(++count);
  for( j=top;j<bottom;++j) {
    for( i=left;i<right;++i) {
	    r = regions[j][i];
	    c = src[j][i];
	    if( predicat( src[j][i+1],c ) ) part.resolve(regions[j][i+1],r);   // E
	    if( predicat( src[j+1][i],c ) ) part.resolve(regions[j+1][i], r);  // S
	    if(_connect==connect8) {
	      if( predicat( src[j+1][i+1],c ) ) part.resolve(regions[j+1][i+1],r); // SE 
	      if( i>0 && predicat( src[j+1][i-1],c ) ) part.resolve(regions[j+1][i-1],r); // SW
	    }
	  } 
  }

  // last line 
  for( i=left;i<right;++i) {
   if( predicat( src[bottom][i+1], src[bottom][i] ) ) 
     part.resolve( regions[bottom][i+1],regions[bottom][i] );
  }

  // last column 
  for( j=top;j<bottom;++j) {
   if( predicat( src[j][right], src[j+1][right]) ) 
    part.resolve( regions[j][right],regions[j+1][right] );   
  }

  part.update(); // update basic_partition
} 
//-------------------------------------------------------------------
// create_map<P,T> : Cree une partition et une carte � partir du
// predicat d'agregation de P appliqu� � l'image de type T
//-------------------------------------------------------------------
template<class P,class T> 
unsigned int create_map( map_type& _map, 
                         basic_partition& _part, 
                         const image<T>& src,
                         P predicat,
                         connectivity _connect )
{
  unsigned count = setup_regions(src,_map,predicat,_connect);
  update_partition_map(src,_map,count,_part,predicat,_connect);

  return count;
}
//--------------------------------------------------------
template<class T,class P>
P get_labels( const map_type& _map, basic_partition& _partition, const image<T>& _mask, P& pred, labels_array_type& _labels )
{
  typedef typename image<T>::const_line_type const_line_type;
  dmASSERT( _mask==_map );

  dm_int v;

  const_line_type line = _mask.begin();
  const_line_type lend = _mask.end();
  map_type::const_line_type rgn = _map.begin();
  for(int prev=0;line!=lend;++line,++rgn)  {
    for(int i=_mask.width();--i>=0;) {
      v = _partition[(*rgn)[i]];
      if( pred((*line)[i]) && v != prev && v!= 0 ) {
        _labels.push_back( v );
        prev = v;
      } 
    }
  }
  return pred;
}
//--------------------------------------------------------
unsigned create_region_partition( basic_partition& _part, map_type& _map, const dmRegion& _rgn, connectivity );
void     assign_map_borders( basic_partition& _part, const map_type& _map, dm_int _index );
void     create_roi( dmRegion& , basic_partition& , const map_type& , dm_int _index );
int      get_overlapping_regions_labels( const dmRegion& rgn, const basic_partition& _part, const map_type&, labels_array_type& _labels );
int      get_border_regions_labels     ( const dmRegion& rgn, const basic_partition& _part, const map_type&, labels_array_type& _labels );
int      merge_labels( basic_partition& , labels_array_type&, dm_int k );
int      merge_regions( basic_partition&, const map_type&, const dmRegion& );
void     relabel_image_map( basic_partition& , map_type& );
int      compact_partition_map( basic_partition& , map_type&, basic_partition::index_table& );

labels_array_type::iterator  compact_labels( labels_array_type& );

//--------------------------------------------------------------
int get_overlapping_regions_labels2( const dmRegion& rgn,
                                      const basic_partition& _partition,
                                      const map_type& _map, 
                                      labels_array_type& _labels );
//--------------------------------------------------------
} // namespace daim

#endif // _dmPartition_h 

