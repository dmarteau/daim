
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

//---------------------------------------------------------------------------
// Version 1.00
//---------------------------------------------------------------------------
#include "dmMatrixBase.h"
#include "dmMatrixFunc.h"

#include <vector>

BEGIN_MATRIX()
//---------------------------------------------------------------------------
// dmReturnMatrix constructor
//---------------------------------------------------------------------------
// Return operator 
//---------------------------------------------------------------------------
dmReturnMatrix::dmReturnMatrix(const dmMatrix& _Matrix)
{
   dmMatrixBase* Matrix = const_cast<dmMatrix&>(_Matrix).Evaluate();
   if(Matrix->IsEval()) Returned = Matrix;
   else {
     Returned = Matrix->Duplicate();
     static_cast<dmMatrixBase*>(Returned)->SetStatus(dmMatrixBase::Temporary); // IsEval()
   }
}
//---------------------------------------------------------------------------
// Operation evaluators
//---------------------------------------------------------------------------
int dmMultipliedMatrix::Search(const dmMatrixBase* M) const
{ return Lhs->Search(M) + Rhs->Search(M); }
//----------------------------------------------------------------------------
dmMatrixBase* dmMultipliedMatrix::Evaluate(dmMatrixType _Type)
{
   if(!_Type) _Type = Type();
   else if(!(_Type >= Type())) 
     MatrixError( "Cannot store in specified matrix" );

   dmMatrixBase* Result = dmMatrix_Multiply(*Lhs,*Rhs,_Type);

   delete this;
   return Result;
}
//------------------------------------------------------------------------------
dmMatrixBase* dmAddedMatrix::Evaluate(dmMatrixType _Type)
{
   if(!_Type) _Type = Type();
   else if(!(_Type >= Type())) 
     MatrixError( "Cannot store in specified matrix" );

   dmMatrixBase* Result = dmMatrix_Add(*Lhs,*Rhs,_Type);

   delete this;
   return Result;
}
//---------------------------------------------------------------------------
dmMatrixBase* dmSubstractedMatrix::Evaluate(dmMatrixType _Type)
{
   if(!_Type) _Type = Type();
   else if(!(_Type >= Type())) MatrixError( "Cannot store in specified matrix" );

   dmMatrixBase* Result = dmMatrix_Substract(*Lhs,*Rhs,_Type);

   delete this;
   return Result;
}
//---------------------------------------------------------------------------
dmMatrixBase* dmDividedMatrix::Evaluate(dmMatrixType _Type)
{
   if(!_Type) _Type = Type();
   else if(!(_Type >= Type())) MatrixError( "Cannot store in specified matrix" );

   dmMatrixBase* Result = dmMatrix_Solve(*Lhs,*Rhs,_Type);

   delete this;
   return Result;
}
//---------------------------------------------------------------------------
// Operations with scalar
//---------------------------------------------------------------------------
int dmShiftedMatrix::Search(const dmMatrixBase* M) const { return Op->Search(M); }
//--------------------------------------------------------------------------
dmMatrixBase* dmShiftedMatrix::Evaluate(dmMatrixType _Type)
{
  if(!_Type) _Type = Type();
  else if(!(_Type >= Type())) MatrixError( "Cannot store in specified matrix" );

  dmMatrixBase* Result = dmMatrix_Shift(*Op,Value,_Type);

  delete this;
  return Result;
}
//---------------------------------------------------------------------------
dmMatrixBase* dmScaledMatrix::Evaluate(dmMatrixType _Type)
{
  if(!_Type) _Type = Type();
  else if(!(_Type >= Type())) MatrixError( "Cannot store in specified matrix" );

  dmMatrixBase* Result = dmMatrix_Scale(*Op,Value,_Type);

  delete this;
  return Result;
}
//---------------------------------------------------------------------------
// Transposed Matrix
//---------------------------------------------------------------------------
int dmTransposedMatrix::Search(const dmMatrixBase* M) const { return Op->Search(M); }
//---------------------------------------------------------------------------
dmMatrixBase* dmTransposedMatrix::Evaluate(dmMatrixType _Type)
{
  if(!_Type) _Type = Type();
  else if(!(_Type >= Type())) MatrixError( "Cannot store in specified matrix" );

  dmMatrixBase* Result = dmMatrix_Transpose(*Op,_Type);

  delete this;
  return Result;  
}
//---------------------------------------------------------------------------
// Inverted Matrix
//---------------------------------------------------------------------------
int dmInvertedMatrix::Search(const dmMatrixBase* M) const { return Op->Search(M); }
//---------------------------------------------------------------------------
dmMatrixBase* dmInvertedMatrix::Evaluate(dmMatrixType _Type)
{
  if(!_Type) _Type = Type();
  else if(!(_Type >= Type())) 
    MatrixError( "Cannot store in specified matrix" );

  dmMatrixBase* Result = dmMatrix_Invert(*Op,_Type);

  delete this;
  return Result;  
}
//---------------------------------------------------------------------------
END_MATRIX()
