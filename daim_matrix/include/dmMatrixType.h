#ifndef dmMatrixType_h
#define dmMatrixType_h

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
// File         : dmMatrixType.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

#include "dmMatrixError.h"

BEGIN_MATRIX()
//----------------------------------------------------------------------------
// Used for finding the type of a matrix resulting from the binary operations
// +, -, * and identifying what conversions are permissible.
//----------------------------------------------------------------------------


class dmMatrixBase;

class dmMatrixType
{
  public:
   enum MatrixType { UnSp,Rect,Sym,RowV,ColV };
  private:
   MatrixType Type;
  public:
   dmMatrixType(MatrixType _Type) : Type(_Type) {}
   dmMatrixType() : Type(UnSp) {}
  ~dmMatrixType() {}
   //-------------------------------------------
   bool operator>=(const dmMatrixType& ) const;
   //-------------------------------------------
   dmMatrixType operator+(const dmMatrixType&) const;
   dmMatrixType operator-(const dmMatrixType&) const;
   dmMatrixType operator*(const dmMatrixType&) const;
   dmMatrixType t() const; // type of transposed
   //--------------------------------------------
   bool operator==(const dmMatrixType& _T) const { return (Type == _T.Type); }
   bool operator!=(const dmMatrixType& _T) const { return !(*this == _T); }
   bool operator!() const { return Type == UnSp; }
   //--------------------------------------------------
   dmMatrixBase* New(int,int) const; // new matrix of given type   
   //--------------------------------------------------
   //operator int() const { return (int)Type; }
};

END_MATRIX()

#endif // dmMatrixType_h

