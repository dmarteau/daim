#ifndef dmOperators_h
#define dmOperators_h

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
// File         : dmOperators.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

namespace dmTk {

enum {
  None = 0
};

// Flip operations (may be combined)
enum {
   FlipHor = 0x1,
   FlipVer = 0x2
};

// Rotate operations
enum {
  RotateLeft  = 0,
  RotateRight = 1
};

namespace Math {
 // Binary math operations
 enum {
   AddPixels = 1, /*!< a+b            */ 
   SubPixels,     /*!< a-b            */ 
   MinPixels,     /*!< min(a,b)       */ 
   MaxPixels,     /*!< max(a,b)       */ 
   NSubPixels,    /*!< b-a            */ 
   OrPixels,      /*!< binary a OR  b */ 
   XorPixels,     /*!< binary a XOR b */ 
   AndPixels,     /*!< binary a AND b */ 
   DiffPixels,    /*!< abs( b - a )   */
   AbsPixels,     /*!< abs( b ) + abs( a ) )   */
   MulPixels,     /*!< b * a */
   DivPixels      /*!< b / a */
 };
  
 // Morphological operations
 enum {
   Erosion = 1,      
   Dilation,     
   Thinning,   
   Thickening, 
   HitOrMiss,  
   Opening,    
   Closing,    
   TopHat,
   BottomHat 
 };

 // Predicats
 enum { 
   Less = 1,      /*!< less  than          */
   Greater,       /*!< greater than        */
   EqualTo,       /*!< equal to            */
   GreaterEqual,  /*!< greater or equal to */
   LessEqual,     /*!< less or equal to    */
   NotEqual       /*!< not equal to        */
 };

} // namespace Math

} // namespace dmTk

#endif // dmOperators_h
