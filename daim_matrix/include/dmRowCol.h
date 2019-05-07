#ifndef dmRowCol_h
#define dmRowCol_h

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
// File         : dmRowCol.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

#include "dmMatrixData.h"
//---------------------------------------------------------------------------
BEGIN_MATRIX()
//---------------------------------------------------------------------------
class dmMatrixBase;
class dmMatrixRowCol
{
 friend class dmMatrixBase;
 private:
   dm_matrix_t* Ptr;    // Pointer to first element of Row/Col
   dm_int    RCRowCol;  // Current Row or Column
   dm_int    RCLength;  // Length of Data
   dm_int    RCSkip;    // Distance to next element (==1 for Vector and rows)

 protected:
   dmMatrixRowCol(dm_int _RowCol=1) : Ptr(0), RCRowCol(_RowCol) {}
   
   void Init( dm_matrix_t* _Data, dm_int _Length, dm_int _Skip );
   
 public:
   virtual ~dmMatrixRowCol() {}

	 // First Index == 1
   dm_matrix_t& operator[](int i) const {
     return *(Ptr+((--i)*RCSkip));
   }

   dm_matrix_t* operator()() const { return Ptr; } // Deprecated
   dm_matrix_t* Data()       const { return Ptr; }
   
   dm_int   RowCol() const { return RCRowCol; }
   dm_int   Length() const { return RCLength; }
   dm_int   Skip()   const { return RCSkip;   }

   void Step( int n=1 ) { Ptr += n; }

   // Dot product 
   dm_matrix_t operator*(const dmMatrixRowCol&);

   // Scalar operations
   void Copy(const dm_matrix_t*);
   void Copy(dm_matrix_t);
   void Mult(dm_matrix_t);
   void Div(dm_matrix_t);
   void Add(dm_matrix_t);
   void Sub(dm_matrix_t);
   void Neg();

   // Line and columns must have the same length 
   void Copy( const dmMatrixRowCol& );         // RC = X
   void Add ( const dmMatrixRowCol& );         // RC = X+RC
   void Sub ( const dmMatrixRowCol& );         // RC = RC-X
   void SubR( const dmMatrixRowCol& );         // RC = X-RC
   void Neg ( const dmMatrixRowCol& );         // RC = -X
   void Copy( const dmMatrixRowCol&, dm_matrix_t);// RC = a*X
   void Add ( const dmMatrixRowCol&, dm_matrix_t);// RC = RC + a*X

   void Add( const dmMatrixRowCol&,const dmMatrixRowCol&); // RC = X1+X2
   void Sub( const dmMatrixRowCol&,const dmMatrixRowCol&); // RC = X1-X2

   void Sqr( const dmMatrixRowCol& );                // RC = sqr(RC)
   void Log( const dmMatrixRowCol& );                // RC = log(RC)
   void Pow( const dmMatrixRowCol&, dm_matrix_t y ); // RC = pow(RC,y)
   void Exp( const dmMatrixRowCol& );                // RC = exp(RC)

   void Swap( dmMatrixRowCol& );
};
//---------------------------------------------------------------------------
// Wrap external data into a dmMatrixRowCol object
//---------------------------------------------------------------------------
class dmRowColArray : public dmMatrixRowCol
{
  public:
   ~dmRowColArray() {}
    dmRowColArray( dm_matrix_t* _Data, dm_int _Length, dm_int _Skip ) { 
      Init(_Data,_Length,_Skip); 
    }
};

END_MATRIX()

#ifndef _NO_ROWCOL_INLINES
#include "../source/dmRowCol.cpp"
#define ROWCOL_INCLUDED
#endif

//----------------------------------------------------------------
#endif // dmRowCol_h

