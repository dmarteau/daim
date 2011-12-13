
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

#include "dmMatrixBase.h"
#include "dmMatrixRandom.h"

#include <math.h>

#define IA 16807
#define IM 2147483647
#define AM (1.0f/IM)
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1+(IM-1)/NTAB)
#define EPS 1.2e-7f
#define RNMX (1.0f-EPS)

BEGIN_MATRIX()
//------------------------------------------------------------------------------
void dmInitRandomSequence( dm_random* _random, dm_int32 _seed )
{
  if(_seed >= 0) 
     _seed = -_seed;

  _random->idum = _seed;
  _random->iy   = 0;
  _random->iset = 0;
}
//------------------------------------------------------------------------------
// From "Numerical Recipes in C, 2nd Edition" (C),  Chapter �7.1, p 280
//------------------------------------------------------------------------------
dm_matrix_t dmUniformDeviate( dm_random* _random )
{
  int j;
  dm_int32  k;
  dm_matrix_t temp;

  dm_int32  idum = _random->idum;
  dm_int32  iy   = _random->iy;
  dm_int32* iv   = _random->iv;

  if (idum<= 0 || !iy)
  {
    if (-(idum) < 1) idum=1;
    else idum = -idum;
    for (j=NTAB+7;j>=0;j--) {
      k=idum/IQ;
      idum=IA*(idum-k*IQ)-IR*k;
      if (idum< 0) idum += IM;
      if (j < NTAB) iv[j] = idum;
    }
    iy=iv[0];
  }
  k=idum/IQ;
  idum=IA*(idum-k*IQ)-IR*k;
  if (idum < 0) idum += IM;
  j=iy/NDIV;
  iy=iv[j];
  iv[j] = idum;
  if ((temp=AM*iy) > RNMX) 
       temp = RNMX;

  _random->idum = idum;
  _random->iy   = iy;     
  return temp;
}
//------------------------------------------------------------------------------
dm_matrix_t dmExponentialDeviate( dm_random* _random )
{
  dm_matrix_t dum;
  do
    dum=dmUniformDeviate(_random);
  while (dum == 0.0);
  return static_cast<dm_matrix_t>( -log(dum) );  
}
//------------------------------------------------------------------------------
dm_matrix_t dmGaussianDeviate( dm_random* _random )
{
  dm_matrix_t  fac,v1,v2;
  double rsq;
  
  if(_random->iset == 0) 
  {
    do {
      v1=2.0f*dmUniformDeviate(_random)-1.0f;
      v2=2.0f*dmUniformDeviate(_random)-1.0f;
      rsq = v1*v1+v2*v2;
    } while (rsq >= 1.0 || rsq == 0.0);
    fac= static_cast<dm_matrix_t>( sqrt(-2.0*log(rsq)/rsq) );
    _random->gset=v1*fac;
    _random->iset=1;
    return v2*fac;
  } else {
    _random->iset=0;
    return _random->gset;
  }
}
//------------------------------------------------------------------------------
dmMatrixRowCol& dmMatrix_Random( dmMatrixRowCol& _RowCol, dm_random* _random, dm_matrix_t (*_DeviateFunc)(dm_random*))
{
  dm_matrix_t* p = _RowCol.Data();
  dm_int       s = _RowCol.Skip();

  for(int i=_RowCol.Length();--i>=0;p+=s)
      *p = _DeviateFunc(_random);
  
  return _RowCol;
}
//------------------------------------------------------------------------------
dmMatrixBase* dmMatrix_Random( dmMatrixBase* _Matrix, dm_random* _random, dm_matrix_t (*_DeviateFunc)(dm_random*))
{
  dm_matrix_t* it  = _Matrix->BlkStart();
  dm_matrix_t* end = _Matrix->BlkEnd();

  for(;it!=end;++it) 
      *it = _DeviateFunc(_random);

  return  _Matrix;
}
//------------------------------------------------------------------------------
END_MATRIX()
