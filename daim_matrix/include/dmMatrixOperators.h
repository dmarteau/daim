#ifndef dmMatrixOperators_h
#define dmMatrixOperators_h

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
// File         : dmMatrixOperators.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------
#include "dmMatrix.h"


BEGIN_MATRIX()
//---------------------------------------------------------------------------
// Type wich should be used in return statements, preventing copy
// of matrix data when returning results from evaluations functions
//---------------------------------------------------------------------------
class dmReturnMatrix : public dmMatrix
{
  protected:
	  dmMatrix* Returned; // temporary object
  public:
    dmReturnMatrix(const dmMatrix& _Returned);
    dmMatrixType Type() const { return Returned->Type(); }
    dmMatrixBase* Evaluate(dmMatrixType) { return (dmMatrixBase*)Returned; }
    //------------------------------------
    int Search(const dmMatrixBase* M) const { return 0; }
};
//---------------------------------------------------------------------------
// Temporary Types wich handle results from  binary matrix operations
//---------------------------------------------------------------------------
class dmMultipliedMatrix :  public dmMatrix
{
  protected:
    const dmMatrix* Lhs;
    const dmMatrix* Rhs;
  public:
    dmMultipliedMatrix(const dmMatrix* _Lhs,const dmMatrix* _Rhs) : Lhs(_Lhs),Rhs(_Rhs) {}
    dmMatrixType Type() const { return Lhs->Type()*Rhs->Type(); }
    dmMatrixBase* Evaluate(dmMatrixType);
    //------------------------------------
    int Search(const dmMatrixBase* ) const;
};
//---------------------------------------------------------------------------
class dmAddedMatrix : public dmMultipliedMatrix
{
  public:
    dmAddedMatrix(const dmMatrix* _Lhs,const dmMatrix* _Rhs) : dmMultipliedMatrix(_Lhs,_Rhs) {}
    dmMatrixType Type() const { return Lhs->Type()+Rhs->Type(); }
    dmMatrixBase* Evaluate(dmMatrixType);
};
//---------------------------------------------------------------------------
class dmSubstractedMatrix : public dmMultipliedMatrix
{
  public:
    dmSubstractedMatrix(const dmMatrix* _Lhs,const dmMatrix* _Rhs) : dmMultipliedMatrix(_Lhs,_Rhs) {}
    dmMatrixType Type() const { return Lhs->Type()-Rhs->Type(); }
    dmMatrixBase* Evaluate(dmMatrixType);
};
//---------------------------------------------------------------------------
class dmDividedMatrix : public dmMultipliedMatrix
{
  public:
    dmDividedMatrix(const dmMatrix* _Lhs,const dmMatrix* _Rhs) : dmMultipliedMatrix(_Lhs,_Rhs) {}
    dmMatrixBase* Evaluate(dmMatrixType);
};
//---------------------------------------------------------------------------
// Operations with scalar
//---------------------------------------------------------------------------
class dmShiftedMatrix : public dmMatrix
{
  protected:
    const dmMatrix* Op;
    dm_matrix_t        Value;
  public:
    dmShiftedMatrix(const dmMatrix* _Op,dm_matrix_t _Shift) : Op(_Op), Value(_Shift) {}
    dmMatrixType Type() const { return Op->Type(); }
    dmMatrixBase* Evaluate(dmMatrixType);
    //-----------------------------------------
    int Search(const dmMatrixBase* ) const;
};
//---------------------------------------------------------------------------
class dmScaledMatrix : public dmShiftedMatrix
{
  public:
    dmScaledMatrix(const dmMatrix* _Op,dm_matrix_t _Factor) : dmShiftedMatrix(_Op,_Factor) {}
    dmMatrixType Type() const { return Op->Type(); }
    dmMatrixBase* Evaluate(dmMatrixType);
};
//---------------------------------------------------------------------------
// Transposed Matrix
//---------------------------------------------------------------------------
class dmTransposedMatrix : public dmMatrix
{
  protected:
    const dmMatrix* Op;
  public:
    dmTransposedMatrix(const dmMatrix* _Op) : Op(_Op) {}
    dmMatrixType Type() const { return Op->Type().t(); }
    dmMatrixBase* Evaluate(dmMatrixType);
    //----------------------------------------------
    int Search(const dmMatrixBase* ) const;
};
//---------------------------------------------------------------------------
// Inverted Matrix
//---------------------------------------------------------------------------
class dmInvertedMatrix : public dmMatrix
{
  protected:
    const dmMatrix* Op;
  public:
    dmInvertedMatrix(const dmMatrix* _Op) : Op(_Op) {}
    dmMatrixType Type() const { return Op->Type();  }
    dmMatrixBase* Evaluate(dmMatrixType);
    //----------------------------------------------
    int Search(const dmMatrixBase* ) const;
};


END_MATRIX()
//---------------------------------------------------------------------------
#endif // dmMatrixOperators_h

