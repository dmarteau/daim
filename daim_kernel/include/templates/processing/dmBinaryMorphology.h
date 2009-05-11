#ifndef dmBinaryMorphology_h
#define dmBinaryMorphology_h

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
// File         : dmBinaryMorphology.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

// Binary mathematical morphologie from region objects

class dmKernelDescription;
class dmKernelFamily;

namespace daim {
//--------------------------------------------------------
void erode_region ( dmRegion& _src, dmRegion& _dest,const dmKernelDescription&);
void dilate_region( dmRegion& _src, dmRegion& _dest,const dmKernelDescription&);
//--------------------------------------------------------
// 1 - If nIter = 0, the operation is done up to idempotence
// 2 - Id _dest nis not empty, the operation is performed
// conditionnaly relative to _dest.
// BEWARE : to perform an ultimate dilation/thickening it needs
// to do it conditionnaly to the size of the total working region
//--------------------------------------------------------
void binary_erosion   ( const dmRegion& _src, dmRegion& _dest,const dmKernelFamily&,int nIter=1 );
void binary_dilation  ( const dmRegion& _src, dmRegion& _dest,const dmKernelFamily&,int nIter=1 );
void binary_hitormiss ( const dmRegion& _src, dmRegion& _dest,const dmKernelFamily&,int nIter=1 );
void binary_thinning  ( const dmRegion& _src, dmRegion& _dest,const dmKernelFamily&,int nIter=1 );
void binary_thickening( const dmRegion& _src, dmRegion& _dest,const dmKernelFamily&,int nIter=1 );
//--------------------------------------------------------
void binary_opening   ( const dmRegion& _src, dmRegion& _dest,const dmKernelFamily&,int nIter=1 );
void binary_closing   ( const dmRegion& _src, dmRegion& _dest,const dmKernelFamily&,int nIter=1 );
//--------------------------------------------------------

// Compute the region borders for thinning/erosion/closing
// You will then need to substract the rect given from the computed region
// then pass the computed region as _src and a copy of this region, from wich 
// the rectangle parameter has been substracted, as _dest.
// This will ensure that object borders are computed correctly.

void handle_region_borders( dmRegion& , const dmRect&, const dmKernelFamily& ); 

//--------------------------------------------------------
int distance_map       ( const dmRegion& _rgn,const dmKernelFamily&, image<dm_uint16>& );  
int ultimate_dilation  ( const dmRegion& _rgn, dmRegion& _dest, const dmKernelFamily&, const dmRegion* _cond );
int ultimate_erosion   ( const dmRegion& _rgn, dmRegion& _dest, const dmKernelFamily& );
int reconstruct        ( const dmRegion&, dmRegion&, connectivity  );
int separate_particules( const dmRegion& _rgn, dmRegion& _dest, 
                         const dmKernelFamily& , 
                         const dmRegion* _seeds = NULL,
                         const dmKernelFamily*  = NULL );

//--------------------------------------------------------
// Simple watershed implementation for 8 bits images.
// Regions where pixels are == 0 are discarded.
//--------------------------------------------------------
int watershed_simple( const image<dm_uint8>& _image, dmRegion& _dest, 
                      const dmRegion* _seeds = NULL );

//////////////////////////////////////////////////////////////////////
// Looks for local maxima in a image in 8 connectivity
// store the results as binary image
//////////////////////////////////////////////////////////////////////
template<class X,class Y> 
inline bool _check_local_max( X v0,X v1, Y ismax)
{
  return (v0 == v1 ? ismax!=1 : (v0 >= v1 ));
}
//--------------------------------------------------------------------
template<class T1,class T2>
struct local_max8_pass1
{
  local_max8_pass1() {}
  
  template<class In,class Out>
  void operator()( In _in, Out  _out, long x1, long x2 )
  {    
    typename image<T2>::value_type vmax = image<T2>::traits_type::object();
    typename image<T1>::value_type c;
    for(int x=x1; x<=x2;++x) {
      c = (*_in)[x];
      if(
         ((*_out)[x] != 1) && 
         _check_local_max( c , _in[-1][x+1], _out[-1][x+1]) &&
         _check_local_max( c , _in[-1][x]  , _out[-1][x])   &&
         _check_local_max( c , _in[-1][x-1], _out[-1][x-1]) &&
         _check_local_max( c , _in[0][x-1] , _out[0][x-1])  &&
         _check_local_max( c , _in[0][x+1] , _out[0][x+1])  &&
         _check_local_max( c , _in[1][x-1] , _out[1][x-1])  &&
         _check_local_max( c , _in[1][x]   , _out[1][x])    &&
         _check_local_max( c , _in[1][x+1] , _out[1][x+1])
      ) { 
          (*_out)[x] = vmax; 
        } else (*_out)[x] = 1; // Rejected
    }
  }
};

///////////////////////////////////////////////////////
template<class T1,class T2>
struct local_max8_pass2
{
  local_max8_pass2() {}
   
  template<class In,class Out>
  void operator()( In _in, Out  _out, long x1, long x2 )
  {
    typename image<T2>::value_type vmax = image<T2>::traits_type::object();
    typename image<T1>::value_type c;
    for(int x=x2; x>=x1;--x) {
      c = (*_in)[x];
      if((*_out)[x]==vmax) { 
         if(!( 
           _check_local_max( c , _in[-1][x+1], _out[-1][x+1]) &&
           _check_local_max( c , _in[-1][x]  , _out[-1][x])   &&
           _check_local_max( c , _in[-1][x-1], _out[-1][x-1]) &&
           _check_local_max( c , _in[0][x-1] , _out[0][x-1])  &&
           _check_local_max( c , _in[0][x+1] , _out[0][x+1])  &&
           _check_local_max( c , _in[1][x-1] , _out[1][x-1])  &&
           _check_local_max( c , _in[1][x]   , _out[1][x])    &&
           _check_local_max( c , _in[1][x+1] , _out[1][x+1]))
         ) { 
           (*_out)[x] = 1; 
         }
      }
    }
  }
};
//--------------------------------------------------------------
template<class T1,class T2>
void local_maxima8( const dmRegion& _roi, const dmPoint p, 
                    image<T1>& _src, 
                    image<T2>& _dest )
{
    _TwoPassesRoiOperation( _roi,p,_src,_dest,
                           local_max8_pass1<T1,T2>(),
                           local_max8_pass2<T1,T2>());
}
//--------------------------------------------------------

}; // namespace daim

#endif // dmBinaryMorphology_h
