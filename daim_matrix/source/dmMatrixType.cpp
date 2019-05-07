
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

BEGIN_MATRIX()
//---------------------------------------------------------------------------
bool dmMatrixType::operator>=(const dmMatrixType& _Rhs) const
{
  MatrixType Rhs = _Rhs.Type;
  if((Type==Rhs)||(Type==Rect)) 
    return true;
  return false;
}
//---------------------------------------------------------------------------
dmMatrixType dmMatrixType::operator+(const dmMatrixType& _Rhs) const
{
   MatrixType Rhs=_Rhs.Type;

   if(Type==Rhs) 
     return Type;
   if( Type==UnSp || Rhs==UnSp)
     return UnSp;
   if(Type==Sym) 
     return Rhs;
   if(Rhs==Sym ) 
     return Type;

   return Rect;
}
//---------------------------------------------------------------------------
dmMatrixType dmMatrixType::operator-(const dmMatrixType& _Rhs) const
{
   return *this+_Rhs;
}
//---------------------------------------------------------------------------
dmMatrixType dmMatrixType::operator*(const dmMatrixType& _Rhs) const
{
   MatrixType Rhs=_Rhs.Type;

   if(Type==UnSp || Rhs==UnSp) 
     return UnSp;
   if(Rhs==ColV)    
     return ColV;  // On devrait retourner un scalaire si Rhs==RowV
   if(Type==RowV)   
     return RowV;
   if(Type==Sym ||  Rhs==Sym)  
     return Rect;
   if(Type==Rhs)  
     return Type;

   return Rect;
}
//------------------------------------------------------------------------------
dmMatrixType dmMatrixType::t() const
{
  switch(Type) 
  {
    case UnSp : return UnSp; break;
    case ColV : return RowV; break;
    case RowV : return ColV; break;
    case Sym  : return Sym;  break;
    default   : return Rect;
  }
}
//------------------------------------------------------------------------------
dmMatrixBase* dmMatrixType::New(int nr, int nc) const
{
   dmMatrixBase* Matrix=0;
   switch (Type)
   {
	 case UnSp:  MatrixError( "Can't build Unspecified Type" ); 
                 break;
	 case Rect:  Matrix = new dmRectMatrix(nr, nc); 
                 break;
	 case Sym:   Matrix = new dmSymMatrix(nr); 
                 break;
	 case RowV:  if (nr!=1) MatrixError( "Illegal Row Vector" );
		         Matrix = new dmRowVector(nc); 
                 break;
	 case ColV:  if (nc!=1) MatrixError( "Illegal Column Vector" );
		         Matrix = new dmColumnVector(nr); 
                 break;
	 default:    MatrixError( "Unknown matrix type specified" );
   }
   return Matrix;
}
//------------------------------------------------------------------------------
END_MATRIX()


