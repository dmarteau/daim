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
// File         : cciDaimService.cpp
// Date         : 25 nov. 2008
// Author       : David Marteau
//--------------------------------------------------------

#include "cciDaimService.h"
#include "cciImageUtils.h"

/* Implementation file */
CCI_IMPL_ISUPPORTS1(cciDaimService, cciIDaimService)

cciDaimService::cciDaimService()
{
  /* member initializers and constructor code */
}

cciDaimService::~cciDaimService()
{
  /* destructor code */
}

/* cciIMatrix newMatrix ([array, size_is (count)] in dm_real coeffs, in unsigned long count, in unsigned long rows, in unsigned long columns); */
CCI_IMETHODIMP cciDaimService::NewMatrix(dm_real *coeffs, dm_uint32 count, dm_uint32 rows, dm_uint32 columns, cciIMatrix * *_retval CCI_OUTPARAM)
{
  if(count < rows*columns)
     return CCI_ERROR_INVALID_ARG;

  return CCI_NewMatrix(coeffs,rows,columns,_retval);
}


/* cciIFilterContext newFilterContext (); */
CCI_IMETHODIMP cciDaimService::NewFilterContext(cciIFilterContext * *_retval CCI_OUTPARAM)
{
  return CCI_NewFilterContext(_retval);
}

/* cciIColorSpace newColorSpace (in string clrSpaceName); */
CCI_IMETHODIMP cciDaimService::NewColorSpace(const char * clrSpaceName, cciIColorSpace * *_retval CCI_OUTPARAM)
{
  // Creeate a default RGB colorspace
  return CCI_NewColorSpace(clrSpaceName,_retval);
}
