#ifndef dmMaskCopy_h
#define dmMaskCopy_h

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
// File         : dmMaskCopy.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------
namespace daim 
{
  namespace cpy 
  {
 
   typedef dm_uint8 dm_mask_t;

   // If Pr(x,y) is true then assign the mask value to the
   // mask and return first argument

   template<class Pr>
   struct cmp_mask
   {
     Pr        pr;
     dm_mask_t mask;

     cmp_mask(const Pr& _p, dm_mask_t v ) : pr(_p),mask(v) {}

     typedef typename Pr::first_argument_type  first_argument_type;
     typedef typename Pr::second_argument_type second_argument_type;

     first_argument_type
     operator()( dm_mask_t& m,  
                 const first_argument_type&  x, 
                 const second_argument_type& y )
     {
       if(pr(x,y)) { m = mask; return x; }
       return y;
     }
   };

   template<class Pr>
   cmp_mask<Pr> bind_cmp_mask( const Pr& pr, dm_mask_t x ) {
     return cmp_mask<Pr>(pr,x);
   }

   // If Pr(mask value) then return the first argument
   // else return the second

   template<class Pr,class T>
   struct mask_cond
   {
     Pr        pr;
     mask_cond(const Pr& _p) : pr(_p) {}

     T operator()( dm_mask_t& m, const T& x, const T& y ) {
       if(pr(m)) return x;
       return y;
     }
   };


   template<class Pr,class T>
   mask_cond<Pr,T> bind_mask_cond( const Pr& pr, type_of<T> ) {
     return mask_cond<Pr,T>(pr);
   }

  } // namespace copy

  // msk and src must have the same dimensions
  // \see daim::combine
  template<class Pr,class M,class T1,class T2,class X>
  inline void copy_mask_if( const dmRegion& rgn, const dmPoint& p,
                            image<M>&  _msk,
                            image<T1>& _src,
                            image<T2>& _dst,
                            X  x,
                            const Pr& pr )
  {
    combine(rgn,p,_msk,_src,_dst,cpy::bind_cmp_mask(pr,x));
  }

  // msk and src must have the same dimensions
  // \see daim::combine
  template<class Pr,class M,class T1,class T2>
  inline void copy_if_mask( const dmRegion& rgn, const dmPoint& p,
                            image<M>&  _msk,
                            image<T1>& _src,
                            image<T2>& _dst,
                            const Pr& pr )
  {

    typedef typename image<T1>::value_type value_type;
    combine(rgn,p,_msk,_src,_dst,cpy::bind_mask_cond(pr,type_of<value_type>())); 
  }

} // namespace daim


#endif // dmMaskCopy_h 
