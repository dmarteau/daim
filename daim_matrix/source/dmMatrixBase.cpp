
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

 //----------------------------------------------------------------
// Version 1.00
//----------------------------------------------------------------
#include "dmMatrixBase.h"

BEGIN_MATRIX()
//----------------------------------------------------------------
// class dmMatrixBase
//----------------------------------------------------------------
dmMatrixBase::dmMatrixBase() 
: Status(NoStatus)
, NbCol(0)
, NbRow(0)
, Storage(0)
, Store(NULL) 
{}
//----------------------------------------------------------------
bool dmMatrixBase::Construct( int _NbRow, int _NbCol, int _Storage)
{
  if(Store) { delete Store; Store = NULL; };
  NbCol = _NbCol;
  NbRow = _NbRow;
  if(_Storage) { Store = new dm_matrix_t[Storage=_Storage]; }
  return (Storage!=0);
}
//----------------------------------------------------------------
dmMatrixBase::~dmMatrixBase()
{
  if(Storage) 
    delete Store;
}
//----------------------------------------------------------------
void dmMatrixBase::GetMatrix(const dmMatrix& _M)
{
  if(&_M!=this) 
  {
    // check if we are referenced in expression
    if(_M.Search(this)) SetStatus(ToReuse);
    dmMatrixBase* Matrix = ((dmMatrix&)_M).Evaluate(Type());
    if(Matrix!=this) {
      if(!(Type()>=Matrix->Type())) MatrixError("Bad Matrix Type in Copy Operation");
      if(Matrix->IsEval()) {
        Construct(Matrix->NbRow,Matrix->NbCol);
        Storage = Matrix->Storage;
        Store   = Matrix->Store;
        Matrix->Storage = 0;
        delete Matrix;
      } else {
        Resize(Matrix->NbRow,Matrix->NbCol);
        dm_matrix_t* pR = Matrix->Store-1;
        dm_matrix_t* pL = Store-1;
        for(int i=Storage;--i>=0;) *++pL = *++pR;
      }
    }
  }
}
//----------------------------------------------------------------
dmMatrixBase& dmMatrixBase::Set(dm_matrix_t f)
{
  dm_matrix_t* p = Store-1;
  for(int i=Storage;--i>=0;) *++p = f;
  return *this;
}
//----------------------------------------------------------------
void dmMatrixBase::Free()
{
  if(Storage) {
    delete Store;
    Storage = NbRow = NbCol = 0;
  }
}
//----------------------------------------------------------------
dmMatrixBase& dmMatrixBase::operator>>(dm_matrix_t *_To)
{
  dm_matrix_t* p = Store-1;
  dm_matrix_t* t = _To-1;
  for(int i=Storage;--i>=0;) *++t = *++p;
  return *this;
}
//----------------------------------------------------------------
dmMatrixBase& dmMatrixBase::operator<<(dm_matrix_t *_From)
{
  dm_matrix_t* p = Store-1;
  dm_matrix_t* f = _From-1;
  for(int i=Storage;--i>=0;) *++p = *++f;
  return *this;
}
//----------------------------------------------------------------
dmMatrixBase* dmMatrixBase::Evaluate(dmMatrixType _Type)
{
   if(!_Type) _Type = Type();
   else if(!(Type()>=_Type)) MatrixError("Bad Type Conversion in Matrix Evaluation");
   return this;
}
//----------------------------------------------------------------
/*
void dmMatrixBase::GetCol (dmMatrixRowCol& _Col) const
{
  if(_Col.RCRowCol>NCols()) MatrixError("GetCol() : Bad column index");
  _Col.Ptr      = &Store[_Col.RCRowCol-1];
  _Col.RCSkip   = NCols();
  _Col.RCLength = NRows();
}
//----------------------------------------------------------------
void dmMatrixBase::GetRow (dmMatrixRowCol& _Row) const
{
  if(_Row.RCRowCol>NRows()) MatrixError(_TXT("GetRow() : Bad row index"));
  _Row.Ptr      = &Store[(_Row.RCRowCol-1)*NRows()];
  _Row.RCSkip   = 1;
  _Row.RCLength = NCols();
}
*/
//----------------------------------------------------------------
void dmMatrixBase::GetCol ( dmMatrixRowCol& _Col, int _NCol ) const
{
  if(_NCol>NbCol) MatrixError("GetCol() : Bad column index");
  _Col.Ptr      = &Store[_NCol-1];
  _Col.RCRowCol = _NCol;
  _Col.RCSkip   = NbCol;
  _Col.RCLength = NbRow;

}
//----------------------------------------------------------------
void dmMatrixBase::GetRow ( dmMatrixRowCol& _Row, int _NRow ) const
{
  if(_NRow>NbRow) MatrixError("GetRow() : Bad row index");
  _Row.Ptr      = &Store[(_NRow-1)*NbRow];
  _Row.RCRowCol = _NRow;
  _Row.RCSkip   = 1;
  _Row.RCLength = NbCol;
}
//----------------------------------------------------------------
int dmMatrixBase::Search(const dmMatrixBase* M) const
{ return (M==this ? 1 : 0); }
//----------------------------------------------------------------
// class dmRectMatrix
//----------------------------------------------------------------
dmRectMatrix::dmRectMatrix() 
{
}
//----------------------------------------------------------------
dmRectMatrix::dmRectMatrix( int _NbRow, int _NbCol )
{ 
  Construct(_NbRow,_NbCol,_NbCol*_NbRow); 
}
//----------------------------------------------------------------
dmRectMatrix::dmRectMatrix( const dmMatrix& _Matrix)
{
   Construct();
   GetMatrix(_Matrix);
}
//----------------------------------------------------------------
dmRectMatrix& dmRectMatrix::operator=(const dmMatrix& _Matrix)
{ 
	GetMatrix(_Matrix); 
	return *this; 
}
//----------------------------------------------------------------
void dmRectMatrix::Resize(int _Row,int _Col)
{ 
  if(NbRow!=_Row || NbCol!=_Col) Construct(_Row,_Col,_Col*_Row); 
}
//----------------------------------------------------------------
dm_matrix_t&  dmRectMatrix::operator()( int row, int col ) const
{
   if((row<=0)||(row>NbRow)||(col<=0)||(col>NbCol))
	 MatrixError("Bad rect matrix indexes");
   return Store[(col-1)+(row-1)*NCols()];
}
//----------------------------------------------------------------
dmMatrixBase* dmRectMatrix::Duplicate()
{ 
  // Curiously , this is needed at least with VC6 in order
  // to get a correct call to the copy contructeur.
  return new dmRectMatrix(static_cast<const dmMatrix&>(*this));
}
//----------------------------------------------------------------
// Matrice Symmetrique
//----------------------------------------------------------------
dmSymMatrix::dmSymMatrix() {}
//----------------------------------------------------------------
dmSymMatrix::dmSymMatrix( int _Size ) : dmRectMatrix(_Size,_Size) {}
//----------------------------------------------------------------
dmSymMatrix::dmSymMatrix( const dmMatrix& _Matrix) : dmRectMatrix(_Matrix) {}
//----------------------------------------------------------------
dmSymMatrix& dmSymMatrix::operator=(const dmMatrix& _Matrix)
{    
  GetMatrix(_Matrix); return *this; 
}
//----------------------------------------------------------------
void dmSymMatrix::Resize(int _Row,int _Col)
{
  if(_Row!=_Col) MatrixError("Bad Size for Sym Matrix");
  dmRectMatrix::Resize(_Row,_Col);
}
//----------------------------------------------------------------
dmMatrixBase* dmSymMatrix::Duplicate()
{ 
  // Curiously , this is needed at least with VC6 in order
  // to get a correct call to the copy contructeur.
  return new dmSymMatrix(static_cast<const dmMatrix&>(*this));
}
//----------------------------------------------------------------
// matrice colonne
//----------------------------------------------------------------
dmColumnVector::dmColumnVector( int _Size )
{ 
	Construct(_Size,1,_Size); 
}
//----------------------------------------------------------------
dmColumnVector::dmColumnVector( const dmMatrix& _Matrix )
{
   Construct();
   GetMatrix(_Matrix);
}
//----------------------------------------------------------------
dmColumnVector& dmColumnVector::operator=(const dmMatrix& _Matrix)
{ 
   GetMatrix(_Matrix); 
   return *this; 
}
//----------------------------------------------------------------
void dmColumnVector::Resize(int _Row,int _Col)
{
  if(_Col !=1) MatrixError("Invalid Column for vector");
  if(NbRow!=_Row || NbCol!=1) Construct(_Row,1,_Row);
}
//----------------------------------------------------------------
dm_matrix_t& dmColumnVector::operator()( int row, int col ) const
{
   if((col!=1)||(row<=0)||(row>NbRow)) 
     MatrixError("Matrix : Bad Column-Vector indexes");

   return Store[row-1];
}
//----------------------------------------------------------------
dmMatrixBase* dmColumnVector::Duplicate()
{ 
  // Curiously , this is needed at least with VC6 in order
  // to get a correct call to the copy contructeur.
  return new dmColumnVector(static_cast<const dmMatrix&>(*this));
}
//------------------------------------------------------------------
// matrice ligne
//------------------------------------------------------------------
dmRowVector::dmRowVector( int _Size )
{ 
	Construct(1,_Size,_Size); 
}
//----------------------------------------------------------------
dmRowVector::dmRowVector( const dmMatrix& _Matrix )
{
  Construct();
  GetMatrix(_Matrix);
}
//----------------------------------------------------------------
dmRowVector& dmRowVector::operator=(const dmMatrix& _Matrix)
{ 
  GetMatrix(_Matrix); 
  return *this; 
}
//----------------------------------------------------------------
void dmRowVector::Resize(int _Row,int _Col)
{
  if(_Row !=1) MatrixError("Matrix :Invalid Row for vector");
  if(NbCol!=_Col || NbRow!=1) Construct(1,_Col,_Col);
}
//----------------------------------------------------------------
dm_matrix_t& dmRowVector::operator()( int row, int col ) const
{
   if(row!=1||(col<=0)||(col>NCols()))
	 MatrixError("Matrix : Bad Row-Vector indexes");
   return Store[col-1];
}
//----------------------------------------------------------------
dmMatrixBase* dmRowVector::Duplicate()
{ 
  // Curiously , this is needed at least with VC6 in order
  // to get a correct call to the copy contructeur.
  return new dmRowVector(static_cast<const dmMatrix&>(*this));
}
//------------------------------------------------------------------
END_MATRIX()
