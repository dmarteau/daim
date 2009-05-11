
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

//---------------------------------------------------------------------
namespace imageparts {

  typedef dmIImage<dmPixelFormat32bppFloat>  scalar_image_type;
  typedef dmIImage<dmPixelFormat24bppRGB>    rgb_image_type;

  typedef scalar_image_type::value_type result_type;

  struct diff_compare 
  {
    result_type distance;
    diff_compare( result_type _dist ) : distance(_dist) {}

    bool operator()( const scalar_image_type::value_type& a,
                            const scalar_image_type::value_type& b) {
      return daim::absdiff( a, b ) <= distance;
    }

    bool operator()( const rgb_image_type::value_type& a,
                     const rgb_image_type::value_type& b) {

      return static_cast<result_type>(
               daim::absdiff(a.red  ,b.red  ) + 
               daim::absdiff(a.green,b.green) + 
               daim::absdiff(a.blue ,b.blue ) ) <= distance;
    }


  };
//---------------------------------------------------------------------
// Replace pixels values by the averaged value for each region partition
//---------------------------------------------------------------------

struct _average_data_set
{
  const daim::basic_partition&  _part;
  std::vector<dm_double>        _sum;
  std::vector<dm_uint>          _count;

  _average_data_set( const daim::basic_partition&   aPart )
  : _part(aPart)
  , _sum(aPart.size(),0)          
  , _count(aPart.size(),0) 
  {} 
};

struct _cumulate_functor
{
  const daim::basic_partition&  _part;
  std::vector<dm_double>&        _sum;
  std::vector<dm_uint>&         _count;

  _cumulate_functor( _average_data_set& aData )
  : _part(aData._part)
  , _sum(aData._sum)           
  , _count(aData._count) 
  {} 

  template<class In1,class In2>
  void operator()( In1 map, In2 _in, dm_int x1, dm_int x2 ) 
  {
    dm_int p; 
    for( dm_int x=x1;x<=x2;++x) 
    {
      p = _part[(*map)[x]];
      _sum[p] += (*_in)[x];
      ++_count[p];
    }
  }
};

template<class value_type>
struct _average_functor 
{
  const daim::basic_partition&  _part;
  std::vector<dm_double>&       _sum;
  std::vector<dm_uint>&         _count;

  _average_functor( _average_data_set& aData )
  : _part(aData._part)
  , _sum(aData._sum)           
  , _count(aData._count) 
  {} 

  template<class In1,class In2>
  void operator()( In1 map, In2 _in, dm_int x1, dm_int x2 ) 
  {
    dm_int   p; 

    for(dm_int x=x1;x<=x2;++x)
    {
      p = _part[(*map)[x]];
      (*_in)[x] = static_cast<value_type>(daim::round(
                        _sum[p] / static_cast<dm_double>(_count[p]) ));
    }
  }


};

struct compute_average
{
  dmPoint          _pt;
  const dmRegion&  _roi;

  const daim::basic_partition& _partition;
  const daim::map_type&        _map;

  compute_average(  CCI_INSTANCE_PTR( ImageParts ) _This, const dmRegion& _Roi, const dmPoint& _Pt )
  : _pt(_Pt)
  , _roi(_Roi) 
  , _partition( PARTITION(partition()) )
  , _map( PARTITION(map()) )
  {}

  template<EPixelFormat _PixelFormat>
  void operator()( dmIImage<_PixelFormat>& anImage ) 
  {
    typedef typename dmIImage<_PixelFormat>::value_type value_type;

    _average_data_set _data_set(_partition);

    daim::RoiOperation(_cumulate_functor(_data_set),_map,anImage.Gen(),_roi,_pt);
    daim::RoiOperation(_average_functor<value_type>(_data_set),
                       _map,anImage.Gen(),_roi,_pt);
  }

};
//---------------------------------------------------------------------
} // namespace imageparts
//---------------------------------------------------------------------
void ImagePart_ComputeAverage( CCI_INSTANCE_PTR( ImageParts ) _This, 
                               const dmImage&  _Img,
                               const dmRegion& _Roi,
                               const dmPoint&  _Pt )
{
  // Build an array of averaged values
  imageparts::compute_average _filter(_This,_Roi,_Pt);
  dmImplementScalarOperation(_filter,_Img);
 
}
//---------------------------------------------------------------------
void ImagePart_SetZeroRegion( CCI_INSTANCE_PTR( ImageParts ) _This,
                               const dmRegion&  _Region, 
                               const dmRect&    _Rect )
{
  dmRegion _rgncopy = _Region;
  _rgncopy.XorCoordinates(_Rect);
  if(!_rgncopy.IsEmptyRoi()) 
  {
    _This->_Labels.clear();
    PARTITION( get_overlapping_regions_labels(_rgncopy,_This->_Labels ) );
    PARTITION( merge_labels( _This->_Labels, 0 ) );
  }  
}
//---------------------------------------------------------------------
