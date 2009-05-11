#ifndef dmImageMath_h
#define dmImageMath_h

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
// File         : dmImageMath.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

class dmImageMath
{
  private:
   dmImage& lhsImage; // Left hand side operand

  public:
   dmImageMath( dmImage& _Image ) : lhsImage(_Image) {}
  ~dmImageMath() {}

   bool Add( dm_float a );
   bool Add( dm_float a, const dmRegion& );

   bool Add( const dmImage& );
   bool Add( const dmImage& , const dmRegion&, const dmPoint& );

   bool Sub( const dmImage& );
   bool Sub( const dmImage& , const dmRegion&, const dmPoint& );

   bool SubNegate( const dmImage& );
   bool SubNegate( const dmImage& , const dmRegion&, const dmPoint& );

   bool AbsDiff( const dmImage& );
   bool AbsDiff( const dmImage& , const dmRegion&, const dmPoint& );

   bool XorImage( const dmImage& );
   bool XorImage( const dmImage& , const dmRegion&, const dmPoint& );

   bool OrImage( const dmImage& );
   bool OrImage( const dmImage& , const dmRegion&, const dmPoint& );

   bool AndImage( const dmImage& );
   bool AndImage( const dmImage& , const dmRegion&, const dmPoint& );

   bool MinImage( const dmImage& );
   bool MinImage( const dmImage& , const dmRegion&, const dmPoint& p );

   bool MaxImage( const dmImage& );
   bool MaxImage( const dmImage& , const dmRegion&, const dmPoint& p );

   bool AddMul( dm_real a, dm_real b );
   bool AddMul( dm_real a, dm_real b, const dmRegion& );

   bool Multiply( dm_real a );
   bool Multiply( dm_real a, const dmRegion& );


   // Cast operators

   operator       dmImage* ()       { return &lhsImage; }
   operator const dmImage* () const { return &lhsImage; }

         dmImage* operator->()       { return &lhsImage; }
   const dmImage* operator->() const { return &lhsImage; }
};

#endif // dmImageMath_h
