#ifndef dmLinearFilters_h
#define dmLinearFilters_h

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
// File         : dmLinearFilters.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

#include "_dmFilterTemplates.h"
#include "_dmConvTemplates.h"
#include "_dmRankFilters.h"
#include "_dmMeanFilter.h"
#include "_dmMaxFilter.h"
#include "_dmHistogramFilters.h"

namespace daim {


   template<class K,class A,class T1,class T2>
   void applyGenericFilter( const image<T1>& in,
                            image<T2>& out,
                            const K& _k, 
                            const A& _a,
                            const dmRegion&  rgn,
                            const dmPoint    p )
   {

      RoiOperation(
        filter::getGenericFunctor(_k,_a),
        in,out,rgn,p);        

   }


   template<class K,class A,class T1,class T2>
   void applyHistogramFilter( const image<T1>& in,
                         image<T2>& out,
                         const K& _k, 
                         const A& _a, 
                         const dmRegion&  rgn,
                         const dmPoint    p )
   {
      RoiOperation(
        filter::getHistogramFunctor(_k,_a),
        in,out,rgn,p);        
   }


   template<class K,class A,class T1,class T2>
   void applyConvFilter( const image<T1>& in,
                         image<T2>& out,
                         const K& _k, 
                         const A& _a, 
                         const dmRegion&  rgn,
                         const dmPoint    p )
   {
      RoiOperation(
        conv::getFunctor(_k,_a),
        in,out,rgn,p);        
   }


} // namespace daim
//--------------------------------------------------------
#endif // dmLinearFilters_h
