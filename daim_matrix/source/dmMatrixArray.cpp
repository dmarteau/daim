
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
#include "dmMatrixArray.h"

BEGIN_MATRIX()
//----------------------------------------------------------------
void dmMatrixArray::Initialize( const dmMatrixBase* _MatrixBase )
{
  if(_MatrixBase)  {
     Matrix = _MatrixBase;
     Array  = new (dm_arena) dm_matrix_t*[Matrix->NRows()+1];
     Array[1]= Matrix->GetData() - 1;
     for(int i=2;i<=Matrix->NRows();i++)
         Array[i] = Array[i-1] + Matrix->NCols();    
  } else 
    MatrixError("Invalid Matrix in Matrix Array Constructor");
}
//----------------------------------------------------------------
dmMatrixArray::dmMatrixArray(const dmMatrixBase* _Matrix)
{
  Initialize(_Matrix);
}
//----------------------------------------------------------------
dmMatrixArray::dmMatrixArray(const dmRectMatrix& _Matrix)
{
  Initialize(&_Matrix);
}
//----------------------------------------------------------------
dmMatrixArray& dmMatrixArray::operator=(const dmMatrixArray& _Matrix)
{
  if(NRows()==_Matrix.NRows() && NCols() == _Matrix.NCols()) { 
	  for(int i=NRows();i>0;--i) {
	    dm_matrix_t* ai = Array[i];
	    dm_matrix_t* bi = _Matrix.Array[i]; 
	    for(int j=NCols();j>0;--j) ai[j] = bi[j];
	  }   
  } else MatrixError("Invalid Matrix in Matrix Array Copy");
  return *this;
}  
//----------------------------------------------------------------
dmMatrixArray::~dmMatrixArray()
{
   if(Array) 
     ::operator delete [] (Array,dm_arena);
}
//----------------------------------------------------------------
dmVectorArray::dmVectorArray(const dmRowVector& _Vector)
{
   Length = _Vector.NStore();
   Array  = _Vector.GetData()-1;
}
//----------------------------------------------------------------
dmVectorArray::dmVectorArray(const dmColumnVector& _Vector)
{
   Length = _Vector.NStore();
   Array  = _Vector.GetData()-1;
}
//----------------------------------------------------------------
dmVectorArray::dmVectorArray( dm_matrix_t * _Data, int _Length )
{
   Length = _Length;
   Array  = _Data - 1;
}
//----------------------------------------------------------------
dmVectorArray& dmVectorArray::operator=(const dmVectorArray& _Vector)
{
  if(Size()==_Vector.Size()) { 
    for(int i=Size();i>0;--i) { Array[i] = _Vector.Array[i]; }
  } else MatrixError("Invalid Matrix in Matrix Array Copy");
  return *this; 
}
//----------------------------------------------------------------
END_MATRIX()
