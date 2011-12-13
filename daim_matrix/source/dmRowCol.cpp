
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
#include "dmRowCol.h"
#include <math.h>

#ifndef _NO_ROWCOL_INLINES
#define INLINE inline
#else
#define INLINE
#endif

#define EPS 1.2e-7f

#ifndef ROWCOL_INCLUDED

BEGIN_MATRIX()
//---------------------------------------------------------------------------
INLINE  void dmMatrixRowCol::Init( dm_matrix_t* _Data, dm_int _Length, dm_int _Skip )
{
  Ptr      = _Data;
  RCLength = _Length;
  RCSkip   = _Skip;
}
//---------------------------------------------------------------------------
INLINE  void dmMatrixRowCol::Copy(dm_matrix_t x)
{
  dm_matrix_t* p = Ptr;
  int s=RCSkip;
  for(dm_int i=RCLength;--i>=0;) { *p = x; p+=s; }
}
//----------------------------------------------------------
INLINE  void dmMatrixRowCol::Copy(const dm_matrix_t* _p)
{
  dm_matrix_t* p = Ptr;
  long s=RCSkip;
  for(dm_int i=RCLength;--i>=0;) { *p = *_p++; p+=s; }
}
//----------------------------------------------------------
INLINE  void dmMatrixRowCol::Copy(const dmMatrixRowCol& _Rhs)
{
  dm_matrix_t *Rp=_Rhs.Ptr,*Lp=Ptr;
  int Rs=_Rhs.RCSkip,Ls = RCSkip;
  for(dm_int i=RCLength;--i>=0;) { *Lp = *Rp; Lp+=Ls; Rp+=Rs; }
}
//-----------------------------------------------------------
INLINE  void dmMatrixRowCol::Copy(const dmMatrixRowCol& _Rhs, dm_matrix_t x)
{
  dm_matrix_t *Rp=_Rhs.Ptr,*Lp=Ptr;
  int Rs=_Rhs.RCSkip,Ls = RCSkip;
  for(dm_int i=RCLength;--i>=0;) { *Lp = *Rp * x; Lp+=Ls; Rp+=Rs; }
}
//--------------------------------------------------------------------------
INLINE  void dmMatrixRowCol::Neg(const dmMatrixRowCol& _Rhs)
{
  dm_matrix_t *Rp=_Rhs.Ptr,*Lp = Ptr;
  int Rs=_Rhs.RCSkip,Ls = RCSkip;
  for(dm_int i=RCLength;--i>=0;) { *Lp = -*Rp; Lp+=Ls; Rp+=Rs; }
}
//-----------------------------------------------------------
INLINE  dm_matrix_t dmMatrixRowCol::operator*(const dmMatrixRowCol& _Rhs)
{
  dm_matrix_t *Rp = _Rhs.Ptr,*Lp=Ptr;
  int Rs=_Rhs.RCSkip,Ls=RCSkip;
  dm_matrix_t Sum = 0.0;
  for(dm_int i=RCLength;--i>=0;) { Sum+= *Lp * *Rp; Lp+=Ls; Rp+=Rs; }
  return Sum;
}
//-----------------------------------------------------------
INLINE  void dmMatrixRowCol::Mult(dm_matrix_t x)
{
  dm_matrix_t* p = Ptr;
  int s = RCSkip;
  for(dm_int i=RCLength; --i>=0;) { *p *= x; p+=s; }
}
//-----------------------------------------------------------
INLINE  void dmMatrixRowCol::Div(dm_matrix_t x)
{
  dm_matrix_t* p = Ptr;
  int s = RCSkip;
  for(dm_int i=RCLength; --i>=0;) { *p /= x; p+=s; }
}
//-----------------------------------------------------------
INLINE  void dmMatrixRowCol::Add(dm_matrix_t x)
{
  dm_matrix_t* p = Ptr;
  int s = RCSkip;
  for(dm_int i=RCLength; --i>=0;) { *p += x; p+=s; }
}
//-----------------------------------------------------------
INLINE  void dmMatrixRowCol::Sub(dm_matrix_t x)
{
  dm_matrix_t* p = Ptr;
  int s = RCSkip;
  for(dm_int i=RCLength; --i>=0;) { *p -= x; p+=s; }
}
//-----------------------------------------------------------
INLINE  void dmMatrixRowCol::Neg()
{
  dm_matrix_t* p = Ptr;
  int s = RCSkip;
  for(dm_int i=RCLength; --i>=0;) { *p = - *p; p+=s; }
}
//-----------------------------------------------------------
INLINE  void dmMatrixRowCol::Add( const dmMatrixRowCol& _Rhs )
{
  dm_matrix_t *Rp=_Rhs.Ptr,*Lp=Ptr;
  int Rs=_Rhs.RCSkip,Ls=RCSkip;
  for(dm_int i=_Rhs.RCLength;--i>0;) { *Lp += *Rp; Rp+=Rs; Lp+=Ls; }
}
//-----------------------------------------------------------
INLINE  void dmMatrixRowCol::Sub( const dmMatrixRowCol& _Rhs)
{
  dm_matrix_t *Rp=_Rhs.Ptr,*Lp = Ptr;
  int Rs=_Rhs.RCSkip,Ls=RCSkip;
  for(dm_int i=RCLength;--i>=0;) { *Lp -= *Rp; Rp+=Rs; Lp+=Ls; }
}
//-----------------------------------------------------------
INLINE  void dmMatrixRowCol::SubR( const dmMatrixRowCol& _Rhs)
{
  dm_matrix_t *Rp=_Rhs.Ptr,*Lp = Ptr;
  int Rs=_Rhs.RCSkip,Ls=RCSkip;
  for(dm_int i=RCLength;--i>=0;) { *Lp = *Rp - *Lp ; Rp+=Rs; Lp+=Ls; }
}
//-----------------------------------------------------------
INLINE  void dmMatrixRowCol::Add( const dmMatrixRowCol& _Rhs, dm_matrix_t x)
{
  dm_matrix_t *Rp=_Rhs.Ptr,*Lp=Ptr;
  int Rs=_Rhs.RCSkip,Ls=RCSkip;
  for(dm_int i=RCLength; --i>=0;) { *Lp += *Rp * x; Rp+=Rs; Lp+=Ls; }
}
//-----------------------------------------------------------
INLINE  void dmMatrixRowCol::Add(const dmMatrixRowCol& _Lhs,const dmMatrixRowCol& _Rhs)
{
  dm_matrix_t *Rp=_Rhs.Ptr,*Lp=_Lhs.Ptr,*p=Ptr;
  int Rs=_Rhs.RCSkip,Ls=_Lhs.RCSkip,s=RCSkip;
  for(dm_int i=RCLength;--i>=0;) { *p = *Lp + *Rp; p+=s; Lp+=Ls; Rp+=Rs; }
}
//-----------------------------------------------------------
INLINE  void dmMatrixRowCol::Sub(const dmMatrixRowCol& _Lhs,const dmMatrixRowCol& _Rhs)
{
  dm_matrix_t *Rp=_Rhs.Ptr,*Lp=_Lhs.Ptr, *p=Ptr;
  int Rs=_Rhs.RCSkip, Ls=_Lhs.RCSkip, s=RCSkip;
  for(dm_int i=RCLength; --i>=0;) { *p = *Lp - *Rp; p+=s; Lp+=Ls; Rp+=Rs; }
}
//-----------------------------------------------------------
INLINE void dmMatrixRowCol::Swap( dmMatrixRowCol& _Rhs )
{
  dm_matrix_t *Rp=_Rhs.Ptr,*Lp=Ptr;
  int Rs=_Rhs.RCSkip,Ls=RCSkip;
  dm_matrix_t swp;
  for(dm_int i=RCLength; --i>=0;) { 
    swp = *Lp; *Rp = *Lp; *Lp = swp;
    Rp+=Rs; Lp+=Ls; 
  }
}
//--------------------------------------------------------------------------
INLINE void dmMatrixRowCol::Sqr( const dmMatrixRowCol& _Rhs )
{
  dm_matrix_t x,*Rp=_Rhs.Ptr,*Lp=Ptr;
  int Rs=_Rhs.RCSkip,Ls=RCSkip;
  for(dm_int i=_Rhs.RCLength;--i>=0;) { x = *Rp; *Lp = x * x; Rp+=Rs; Lp+=Ls; }
}
//--------------------------------------------------------------------------
INLINE void dmMatrixRowCol::Log( const dmMatrixRowCol& _Rhs )
{
  dm_matrix_t *Rp=_Rhs.Ptr,*Lp=Ptr;
  int Rs=_Rhs.RCSkip,Ls=RCSkip;

  double v;
  for(dm_int i=_Rhs.RCLength;--i>=0;) {
    if((v = *Rp) < EPS) v = EPS;
    *Lp = static_cast<dm_matrix_t>(log(v)); 
    Rp+=Rs; Lp+=Ls; 
  }
}
//--------------------------------------------------------------------------
INLINE void dmMatrixRowCol::Pow( const dmMatrixRowCol& _Rhs, dm_matrix_t y )
{
  dm_matrix_t *Rp=_Rhs.Ptr,*Lp=Ptr;
  int Rs=_Rhs.RCSkip,Ls=RCSkip;
  for(dm_int i=_Rhs.RCLength;--i>=0;) {
    *Lp = static_cast<dm_matrix_t>(pow(*Rp,y)); 
    Rp+=Rs; Lp+=Ls; 
  }
}
//--------------------------------------------------------------------------
INLINE void dmMatrixRowCol::Exp( const dmMatrixRowCol& _Rhs )
{
  dm_matrix_t *Rp=_Rhs.Ptr,*Lp=Ptr;
  int Rs=_Rhs.RCSkip,Ls=RCSkip;
  for(dm_int i=_Rhs.RCLength;--i>=0;) {
    *Lp = static_cast<dm_matrix_t>(exp(*Rp)); 
    Rp+=Rs; Lp+=Ls; 
  }
}
//--------------------------------------------------------------------------
END_MATRIX()
#endif // ROWCOL_INCLUDED
