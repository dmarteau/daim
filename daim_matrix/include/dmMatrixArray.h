#ifndef dmMatrixArray_h
#define dmMatrixArray_h

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
// File         : dmMatrixArray.h
// Date         : 7/2004
// Author       : David Marteau
// Description  : Build an array-like accessor to matrix data
//                There is no validity checks for indices.
//                Data is offset so that first index is 1 (not 0!!!)
//--------------------------------------------------------
#include "dmMatrixBase.h"

BEGIN_MATRIX()
//---------------------------------------------------------------------
class dmMatrixArray
{
 private:
   const dmMatrixBase *Matrix;
   dm_matrix_t       **Array;
   
 protected:
   void Initialize( const dmMatrixBase* );
   
 public:
   dmMatrixArray( const dmMatrixBase* );
   dmMatrixArray( const dmRectMatrix& );
   
   virtual ~dmMatrixArray();

   dm_matrix_t** m() { return Array; }
   dm_matrix_t*  operator[](int row) const { return Array[row]; } // Access to line

   int NRows() const { return Matrix->NRows(); }
   int NCols() const { return Matrix->NCols(); }

   dmMatrixArray& operator=(const dmMatrixArray&);     
};
//---------------------------------------------------------------------
class dmVectorArray
{
  private:
    int          Length;
    dm_matrix_t *Array;
    
  public:
    dmVectorArray(const dmRowVector&);
    dmVectorArray(const dmColumnVector&);
    
    dmVectorArray( dm_matrix_t * _Data, int _Length );
    
    virtual ~dmVectorArray() {}

    dm_matrix_t& operator[](int i) { return Array[i]; } // Access element
    dm_matrix_t* m() { return Array; }

    const dm_matrix_t& operator[](int i) const { return Array[i]; } // Access element
    const dm_matrix_t* m()               const { return Array; }

    int Size() const { return Length; }

    dmVectorArray& operator=(const dmVectorArray&);  
};
//---------------------------------------------------------------------
END_MATRIX()

#endif // dmMatrixArray_h
