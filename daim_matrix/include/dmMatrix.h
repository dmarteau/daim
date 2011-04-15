#ifndef dmMatrix_h
#define dmMatrix_h

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
// File         : dmMatrix_h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------
#include "dmMatrixData.h"
#include "dmMatrixType.h"

BEGIN_MATRIX()
//----------------------------------------------------------------------------
// General matrix handler
//----------------------------------------------------------------------------
class dmMatrixBase;
class dmReturnMatrix;
class dmMultipliedMatrix;
class dmAddedMatrix;
class dmSubstractedMatrix;
class dmDividedMatrix;
class dmShiftedMatrix;
class dmScaledMatrix;
class dmTransposedMatrix;
class dmInvertedMatrix;
//---------------------------------------------------------------------------
class dmMatrix
{
  public:
	  //---- Destructeur ---------------------------------
	  virtual ~dmMatrix() {}
	  //---------------------------------------
	  virtual dmMatrixBase* Evaluate(dmMatrixType = dmMatrixType::UnSp) = 0;
	  virtual dmMatrixType  Type() const = 0;
	  //-------------------------------------------------
	  virtual int Search(const dmMatrixBase*) const = 0;
	  //------ Operators ------------------------------------------------------
	  inline dmShiftedMatrix& operator+(dm_matrix_t);
	  inline dmShiftedMatrix& operator-(dm_matrix_t);
	  inline dmScaledMatrix&  operator*(dm_matrix_t);
	  inline dmScaledMatrix&  operator/(dm_matrix_t);
	  //-----------------------------------------------------------------------
	  inline dmAddedMatrix&       operator+(const dmMatrix&);
	  inline dmSubstractedMatrix& operator-(const dmMatrix&);
	  inline dmMultipliedMatrix&  operator*(const dmMatrix&);
	  inline dmDividedMatrix&     operator/(const dmMatrix&);
	  //-----------------------------------------------------------------------
	  inline dmTransposedMatrix& t();
	  inline dmInvertedMatrix&   i();
	  //-----------------------------------------------------------------------
	  
	  DM_DECL_ARENA_OPERATOR_NEW(dmMatrix)
};

END_MATRIX()
// ---- operator ------------------------------------------------------------

#include "dmMatrixOperators.h"

BEGIN_MATRIX()
//-----------------------------------------------------------------------
inline dmShiftedMatrix& dmMatrix::operator+(dm_matrix_t x) { return *(new dmShiftedMatrix(this,x)); }
inline dmShiftedMatrix& dmMatrix::operator-(dm_matrix_t x) { return *(new dmShiftedMatrix(this,-x));}
inline dmScaledMatrix&  dmMatrix::operator*(dm_matrix_t x) { return *(new dmScaledMatrix(this,x)); }
inline dmScaledMatrix&  dmMatrix::operator/(dm_matrix_t x) { return *(new dmScaledMatrix(this,(dm_matrix_t)1.0/x));}
//-----------------------------------------------------------------------
inline dmAddedMatrix&       dmMatrix::operator+(const dmMatrix& M) { return *(new dmAddedMatrix(this,&M));}
inline dmSubstractedMatrix& dmMatrix::operator-(const dmMatrix& M) { return *(new dmSubstractedMatrix(this,&M));}
inline dmMultipliedMatrix&  dmMatrix::operator*(const dmMatrix& M) { return *(new dmMultipliedMatrix(this,&M));}
// ---------------------------------------------------------------------------
// Formally, B / A is equivalent to A.i() * B
// but in the first case we use a LU decomposition to solve the linear problem
// ---------------------------------------------------------------------------
inline dmDividedMatrix&  dmMatrix::operator/(const dmMatrix& M) { return *(new dmDividedMatrix(this,&M));}
// ---------------------------------------------------------------------------
inline dmTransposedMatrix& dmMatrix::t() { return *(new dmTransposedMatrix(this)); }
inline dmInvertedMatrix&   dmMatrix::i() { return *(new dmInvertedMatrix(this));   }
//-----------------------------------------------------------------------
inline dmShiftedMatrix& operator+(dm_matrix_t x,const dmMatrix& M) { return *(new dmShiftedMatrix(&M,x)); }
inline dmShiftedMatrix& operator-(dm_matrix_t x,const dmMatrix& M) { return *(new dmShiftedMatrix(&M,-x));}
inline dmScaledMatrix&  operator*(dm_matrix_t x,const dmMatrix& M) { return *(new dmScaledMatrix(&M,x));  }
//----------------------------------------------------------------------------
END_MATRIX()

#endif // dmMatrix_h

