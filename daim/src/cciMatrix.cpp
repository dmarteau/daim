/* :::BEGIN LICENSE BLOCK:::
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
 *  :::END LICENSE BLOCK::: */

//--------------------------------------------------------
// File         : cciMatrix.cpp
// Date         : 25 nov. 2008
// Author       : David Marteau
//--------------------------------------------------------

#define DM_MATRIX_DOUBLE

#include "cciIMatrix.h"
#include "dmMatrixBase.h"


/* Header file */
class cciMatrix : public cciIMatrix
{
public:
  CCI_DECL_ISUPPORTS
  CCI_DECL_IMATRIX

  cciMatrix(const dm_real* data, dm_uint32 rows, dm_uint32 columns );

private:
  ~cciMatrix();

protected:
  DP::dmRectMatrix mMatrix;
};

/* Implementation file */
CCI_IMPL_ISUPPORTS1(cciMatrix, cciIMatrix)

cciMatrix::cciMatrix(const dm_real* data, dm_uint32 rows, dm_uint32 columns)
{
  dmASSERT(data != NULL);
  mMatrix.Resize(rows,columns);
  mMatrix << const_cast<dm_real*>(data);
}

cciMatrix::~cciMatrix()
{
  /* destructor code */
}

//===============================
// cciIMatrix
//==============================

/* [noscript,notxpcom] dmRealPtr data (); */
CCI_IMETHODIMP_(dm_real *) cciMatrix::Data()
{
  return mMatrix.GetData();
}

/* [noscript,notxpcom] dm_uint32 size (); */
CCI_IMETHODIMP_(dm_uint32) cciMatrix::Size()
{
  return mMatrix.NStore();
}

/* [noscript,notxpcom] dm_uint32 rows (); */
CCI_IMETHODIMP_(dm_uint32) cciMatrix::Rows()
{
  return mMatrix.NRows();
}

/* [noscript,notxpcom] dm_uint32 columns (); */
CCI_IMETHODIMP_(dm_uint32) cciMatrix::Columns()
{
  return mMatrix.NCols();
}

/* cciIMatrix clone (); */
CCI_IMETHODIMP cciMatrix::Clone(cciIMatrix * *_retval CCI_OUTPARAM)
{
  cciIMatrix* cloned = new cciMatrix(Data(),Rows(),Columns());
  CCI_ADDREF( *_retval = cloned);
  return CCI_OK;
}


CCI_COM_GLUE
cci_result CCI_NewMatrix( const dm_real* data, dm_uint32 rows, dm_uint32 columns,
                          cciIMatrix** _result)
{
  cciIMatrix* matrix = new cciMatrix(data,rows,columns);
  CCI_ADDREF(*_result = matrix);
  return CCI_OK;
}
