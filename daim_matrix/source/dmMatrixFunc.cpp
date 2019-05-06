
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

#define dmUseUtilitiesExtra // Include dmVector
#include "dmMatrixFunc.h"
#include "dmMatrixLU.h"
#include "dmMatrixBase.h"


#include <math.h>
#include <algorithm>
#include <vector>

BEGIN_MATRIX()
//----------------------------------------------------------------
dmMatrixBase* dmMatrix_SwapRows( dmMatrixBase* _Matrix, int i, int j )
{
  int nr = _Matrix->NRows();
  if(i>nr || j>nr)
     MatrixError( "dmMatrixSwapRows : Bad row index" );
  
  dm_matrix_t *eli = _Matrix->GetData() + (i-1)*nr; 
  dm_matrix_t *elj = _Matrix->GetData() + (j-1)*nr;
  dm_matrix_t swp;
  for(int k=_Matrix->NCols();--k>=0;) { 
    swp = *elj; *elj++ = *eli; *eli++ = swp;
  }
  return _Matrix;
}
//----------------------------------------------------------------
dmMatrixBase* dmMatrix_SwapCols( dmMatrixBase* _Matrix, int i, int j )
{
  int nc = _Matrix->NCols();
  if(i>nc || j>nc)
    MatrixError( "dmMatrixSwapCols : Bad column index" );

  dm_matrix_t *eli = _Matrix->GetData() + i-1; 
  dm_matrix_t *elj = _Matrix->GetData() + j-1;
  dm_matrix_t swp;
  for(int k=_Matrix->NRows();--k>=0;) { 
    swp = *eli; *elj = *eli; *eli = swp;
    eli += nc;
    elj += nc;
  }

  return _Matrix;
}
//------------------------------------------------------------------------------
dmMatrixBase* dmMatrix_Identity( dmMatrixBase* _Matrix )
{
  if(_Matrix->NRows()!=_Matrix->NCols()) 
    MatrixError( "Square matrix required for identity" );

  _Matrix->Set(0.0);

  dmMatrixRow MRow(_Matrix); 
  for(int i=1; i<=_Matrix->NCols();++i) {
    MRow[i] = 1.0;
    MRow.Next();
  }
  return _Matrix;
}
//------------------------------------------------------------------------------
dmMatrixBase* dmMatrix_Diagonal( dmMatrixBase* _Matrix , const dmMatrixRowCol& _RowCol )
{
  if(_Matrix->NRows()!=_Matrix->NCols()) 
     MatrixError( "Square matrix required for diagonal matrix" );

  if(_Matrix->NRows()!=_RowCol.Length()) 
     MatrixError( "Invalid vector size in diagonal matrix construction" );

  _Matrix->Set(0.0);
  
  dmMatrixRow MRow(_Matrix); 
  for(int i=1; i<=_Matrix->NCols();++i) {
    MRow[i] = _RowCol[i];
    MRow.Next();
  }
  return _Matrix;  
}
//------------------------------------------------------------------------------
dm_matrix_t dmMatrix_BilinearProduct( const dmMatrix& _Matrix, const dmMatrixRowCol& _RowCol )
{
  dmMatrixBase* Matrix = const_cast<dmMatrix&>(_Matrix).Evaluate();
  if(Matrix->NRows()!=Matrix->NCols())  MatrixError( "Square matrix required for bilinear operator" );
  if(_RowCol.Length()!=Matrix->NRows()) MatrixError( "Invalid vector size in bilinear operator" );

  dm_matrix_t Sum = 0.0;
  
  dmMatrixRow MRow(Matrix); 
  for(int i=_RowCol.Length();i>0;--i) {
    Sum += _RowCol[i] * (MRow*_RowCol);
    MRow.Next();	
  }  
  if(Matrix->IsEval()) delete Matrix;
  return Sum;
}
//------------------------------------------------------------------------------
void dmMatrix_Apply( const dmMatrix& _Matrix, const dmMatrixRowCol& _RowCol, dmMatrixRowCol& _Result )
{
  dmMatrixBase* MLhs = const_cast<dmMatrix&>(_Matrix).Evaluate();
  if(MLhs->NCols()!=_RowCol.Length())  MatrixError( "Invalid rhs vector size in matrix operation" );
  if(MLhs->NRows()!=_Result.Length())  MatrixError( "Invalid lhs vector size in matrix operation" );
   
  int nr=MLhs->NRows();
  dmMatrixRow LhsRow(MLhs); 
  for(int i = 1; i <= nr; ++i) {
    _Result[i] = LhsRow * _RowCol;
    LhsRow.Next();
  }
  if(MLhs->IsEval()) delete MLhs;
}
//------------------------------------------------------------------------------
// Multiply Matrix
//------------------------------------------------------------------------------
dmMatrixBase* dmMatrix_Multiply( const dmMatrix& Lhs,const dmMatrix& Rhs, dmMatrixType _Type )
{
   dmMatrixBase* MLhs = const_cast<dmMatrix&>(Lhs).Evaluate();
   dmMatrixBase* MRhs = const_cast<dmMatrix&>(Rhs).Evaluate();

   if(MLhs->NCols() != MRhs->NRows())
	  MatrixError( "Bad Matrix Dimensions In '*' Operation." );

   int nr=MLhs->NRows(), nc = MRhs->NCols();
   dmMatrixBase* Result = _Type.New(nr,nc);
   Result->SetStatus(dmMatrixBase::Temporary);

   dmMatrixRow ResRow(Result);
   dmMatrixRow LhsRow(MLhs);
   for(;--nr>=0;)
   {
	   dmMatrixCol RhsCol(MRhs);
	   dm_matrix_t* el= ResRow();
	   for(int n = ResRow.Length();--n>=0;) 
		 { *(el++) = LhsRow * RhsCol; RhsCol.Next(); }
	   LhsRow.Next(); ResRow.Next();
   }

   if(MLhs->IsEval()) delete MLhs;
   if(MRhs->IsEval()) delete MRhs;
   return Result;
}
//---------------------------------------------------------------------------
// Add matrix
//------------------------------------------------------------------------------
dmMatrixBase* dmMatrix_Add( const dmMatrix& Lhs,const dmMatrix& Rhs, dmMatrixType _Type)
{
   dmMatrixBase* MLhs = const_cast<dmMatrix&>(Lhs).Evaluate();
   dmMatrixBase* MRhs = const_cast<dmMatrix&>(Rhs).Evaluate();

   int nr = MLhs->NRows(), nc = MLhs->NCols();
   if( nc != MRhs->NCols() || nr !=MRhs->NRows())
	 MatrixError( "Bad Matrix Dimensions In '+' Operation." );

   // Try to reuse data blocs
   bool UseRhs = ((MRhs->Reuse()||MRhs->IsEval())&&(MRhs->Type()>=_Type));
   bool UseLhs = ((MLhs->Reuse()||MLhs->IsEval())&&(MLhs->Type()>=_Type));
   if(UseLhs && UseRhs && MRhs->Reuse()) UseLhs = false;

   dmMatrixBase* Result;
   dm_matrix_t *pR = MRhs->GetData()-1;
   dm_matrix_t *pL = MRhs->GetData()-1;
   if(UseLhs) {
       Result = MLhs;
       for(int i=Result->NStore();--i>=0;) *++pL += *++pR;
   } else if(UseRhs) {
       Result = MRhs;
       for(int i=Result->NStore();--i>=0;) *++pR += *++pL;
   } else {
       Result = _Type.New(nr,nc);
       Result->SetStatus(dmMatrixBase::Temporary); // IsEval()
       dm_matrix_t* pRes = Result->GetData()-1;
       for(int i=Result->NStore();--i>=0;) *++pRes = *++pL + *++pR;
   }
   if(MRhs != Result && MRhs->IsEval()) delete MRhs;
   if(MLhs != Result && MLhs->IsEval()) delete MLhs;
   return Result;
}
//---------------------------------------------------------------------------
// Substract matrix
//---------------------------------------------------------------------------
dmMatrixBase* dmMatrix_Substract( const dmMatrix& Lhs,const dmMatrix& Rhs, dmMatrixType _Type)
{
   dmMatrixBase* MLhs = const_cast<dmMatrix&>(Lhs).Evaluate();
   dmMatrixBase* MRhs = const_cast<dmMatrix&>(Rhs).Evaluate();

   int nr = MLhs->NRows(), nc = MRhs->NCols();
   if( nc != MRhs->NCols() && nr !=MRhs->NRows())
	 MatrixError( "Bad Matrix Dimensions In '-' Operation." );

   // Try to reuse data blocs
   bool UseRhs = ((MRhs->Reuse()||MRhs->IsEval())&&(MRhs->Type()>=_Type));
   bool UseLhs = ((MLhs->Reuse()||MLhs->IsEval())&&(MLhs->Type()>=_Type));
   if(UseLhs && UseRhs && MRhs->Reuse()) UseLhs = false;

   dmMatrixBase* Result;
   dm_matrix_t *pR = MRhs->GetData()-1;
   dm_matrix_t *pL = MLhs->GetData()-1;
   if(UseLhs) {
       Result = MLhs;
       for(int i=Result->NStore();--i>=0;) *++pL -= *++pR;
   } else if(UseRhs) {
       Result = MRhs;
       for(int i=Result->NStore();--i>=0;) { ++pR; *pR = *++pL - *pR; }
   } else {
       Result = _Type.New(nr,nc);
       Result->SetStatus(dmMatrixBase::Temporary);  // IsEval()
       dm_matrix_t* pRes = Result->GetData()-1;
       for(int i=Result->NStore();--i>=0;) *++pRes = *++pL - *++pR;
   }
   if(MRhs != Result && MRhs->IsEval()) delete MRhs;
   if(MLhs != Result && MLhs->IsEval()) delete MLhs;
   return Result;
}
//---------------------------------------------------------------------------
// Solve linear system
// NOTE : Lhs is the input vector/matrix 
//        Rhs is the system matrix 
// i.e solve Rhs * X = Lhs
//---------------------------------------------------------------------------
dmMatrixBase* dmMatrix_Solve( const dmMatrix& Lhs,const dmMatrix& Rhs, dmMatrixType _Type )
{
   dmMatrixBase* MLhs = const_cast<dmMatrix&>(Lhs).Evaluate();
   dmMatrixBase* MRhs = const_cast<dmMatrix&>(Rhs).Evaluate();

   if(MRhs->NCols() != MLhs->NRows())
	  MatrixError( "Bad Matrix Dimensions In 'Solve'" );

   dmMatrixBase* LU;
   dmMatrixBase* Result;

   // Try to reuse data blocs for rhs
   if( MRhs->Reuse()||MRhs->IsEval()) LU = MRhs;
   else LU = MRhs->Duplicate();

   // Try to reuse data blocs for lhs
   if(MLhs->Reuse()||MLhs->IsEval()) Result = MLhs;
   else {
     Result = MLhs->Duplicate();
     Result->SetStatus(dmMatrixBase::Temporary);
   }

   // Perform LU decomposition on rhs
   dmVector<int> _Perm(MRhs->NRows());     
   dmMatrix_LU(*LU,_Perm.Begin());

   // Solve the system for each columns of Lhs
   dmMatrixCol RCol(Result);
   for(int i=Result->NCols();--i>=0;) {
     dmMatrix_LU_Solve(*LU,RCol,_Perm.Begin());
     RCol.Next();
   }  

   if(LU!=MRhs) delete LU;
  
   if(MRhs->IsEval()) delete MRhs;
   if(MLhs->IsEval() && MLhs!=Result) delete MLhs;

   return Result;
}
//---------------------------------------------------------------------------
// Shift Matrix
//---------------------------------------------------------------------------
dmMatrixBase* dmMatrix_Shift( const dmMatrix& Op, dm_matrix_t Value, dmMatrixType _Type)
{
   dmMatrixBase* MLhs = const_cast<dmMatrix&>(Op).Evaluate();

   int nr = MLhs->NRows(), nc = MLhs->NCols();

   dmMatrixBase* Result;
   dm_matrix_t* pL = MLhs->GetData() -1;
   dm_matrix_t  v = Value;
   if((MLhs->Reuse()||MLhs->IsEval())) {
       Result = MLhs;
       for(int i=Result->NStore();--i>=0;) *++pL += v;
   } else {
       Result = _Type.New(nr,nc);
       Result->SetStatus(dmMatrixBase::Temporary);  // IsEval()
       dm_matrix_t* pRes = Result->GetData()-1;
       for(int i=Result->NStore();--i>=0;) *++pRes = *++pL + v;
   }

   if(MLhs != Result && MLhs->IsEval() ) 
     delete MLhs;

   return Result;
}
//---------------------------------------------------------------------------
// Scale Matrix
//---------------------------------------------------------------------------
dmMatrixBase* dmMatrix_Scale( const dmMatrix& Op, dm_matrix_t Value, dmMatrixType _Type)
{
   dmMatrixBase* MLhs = const_cast<dmMatrix&>(Op).Evaluate();
   int nr = MLhs->NRows(), nc = MLhs->NCols();

   dmMatrixBase* Result;
   dm_matrix_t* pL = MLhs->GetData()-1;
   dm_matrix_t  v  = Value;
   if((MLhs->Reuse()||MLhs->IsEval())) {
	   Result = MLhs;
	   for(int i=Result->NStore();--i>=0;) *++pL *= v;
   } else {
  	 Result = _Type.New(nr,nc); 
	   Result->SetStatus(dmMatrixBase::Temporary);  // IsEval()
	   dm_matrix_t* pRes = Result->GetData()-1;
	   for(int i=Result->NStore();--i>=0;) *++pRes = *++pL * v;
   }

   if(MLhs != Result && MLhs->IsEval() ) 
     delete MLhs;

   return Result;
}
//---------------------------------------------------------------------------
// Transpose Matrix
//---------------------------------------------------------------------------
dmMatrixBase* dmMatrix_Transpose( const dmMatrix& Op, dmMatrixType _Type)
{
  dmMatrixBase* Matrix = const_cast<dmMatrix&>(Op).Evaluate();

  dmMatrixBase* Result;
  if(Matrix->Type()!=dmMatrixType::Sym) {
    int nc = Matrix->NRows(), nr = Matrix->NCols(); // Invert Rows/Columns
    Result = _Type.New(nr,nc);
    Result->SetStatus(dmMatrixBase::Temporary);  // IsEval()
    dmMatrixRow DstRow(Result);
    dmMatrixCol SrcCol(Matrix);
    for(;--nr>=0;) { DstRow.Copy(SrcCol); DstRow.Next(); SrcCol.Next(); }
  } else Result = Matrix; 

  if( (Result!=Matrix) &&  Matrix->IsEval() )
    delete Matrix;

  return Result;  
}
//---------------------------------------------------------------------------
// Invert Matrix
//---------------------------------------------------------------------------
dmMatrixBase* dmMatrix_Invert( const dmMatrix& Op, dmMatrixType _Type)
{
  dmMatrixBase* Matrix = const_cast<dmMatrix&>(Op).Evaluate();
  dmMatrixBase *Result,*LU;
  if((Matrix->Reuse()||Matrix->IsEval()))  Result = Matrix;
  else {
    Result = _Type.New(Matrix->NRows(),Matrix->NCols());
    Result->SetStatus(dmMatrixBase::Temporary);
  }

  LU = Matrix->Duplicate();
  int nr = LU->NRows();

  dmVector<int> _Perm(nr);
     
  dmMatrix_Identity(Result);
  dmMatrix_LU(*LU,_Perm.Begin());

  dmMatrixCol _MCol(Result);
  for(int i=nr;--i>=0;) {
     dmMatrix_LU_Solve(*LU,_MCol,_Perm.Begin());
     _MCol.Next();
  }

  delete LU;
 
  if( (Result!=Matrix) && Matrix->IsEval() ) 
    delete Matrix;

  return Result;  
}
//------------------------------------------------------------------------
//  Normalize matrix
//------------------------------------------------------------------------
dmMatrixBase* dmMatrix_Normalize( dmMatrix& Op, dm_matrix_t& aMinValue, dm_matrix_t& aMaxValue ) 
{
  dmMatrixBase* Result = Op.Evaluate();
   
  dm_matrix_t* _Start = Result->BlkStart();
  dm_matrix_t* _End   = Result->BlkEnd();
  
  dm_matrix_t _Min,_Max,u;
  
  _Min = _Max = *_Start++;
  for(;_Start!=_End;++_Start) 
  {
    u = *_Start;
    if(u <= _Min ) _Min = u; else
    if(u >= _Max ) _Max = u; 
  }

  // If factor is > 0 then really normalize values 

  if(aMinValue < aMaxValue ) 
  {
    if( _Max <= _Min ) 
    {
      Result->Set(_Max);
    } 
    else 
    {
      dm_matrix_t d,delta;
      
      d     = _Max - _Min;
      delta = aMaxValue - aMinValue;
      
      _Start = Result->BlkStart();
      for(;_Start!=_End;++_Start) 
      {
        u   = (*_Start - _Min)/d; // Normalize between 0 and 1.
        u  *= delta; 
        u  += aMinValue;
        
        *_Start = u;
      }   
    }    
  }
  
  aMaxValue = _Max;
  aMinValue = _Min;      
  
  return Result;  
}
//---------------------------------------------------------------------------
END_MATRIX()
