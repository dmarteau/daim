#ifndef cciKernelServiceUtils_h
#define cciKernelServiceUtils_h
/* :::BEGIN LICENSE BLOCK:::
 *
 * Copyright (c) 2004-2010 David Marteau
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
// File         : cciKernelServiceUtils.h
// Date         : 21 janv. 2010
// Author       : David Marteau
//--------------------------------------------------------

#ifndef cciCOMPtr_h
#include "cciCOMPtr.h"
#endif

#ifndef cciServiceManagerUtils_h
#include "cciServiceManagerUtils.h"
#endif

#ifndef cciServiceManagerUtils_h
#include "cciServiceManagerUtils.h"
#endif

#include "cciIKernelService.h"

inline cci_result
CCI_GetKernelFamily( const char* name, dmKernelFamily& retval )
{
  cci_result rv;

  cci_Ptr<cciIKernelService> kernSrvc = cci::do_GetService(CCI_KERNELSERVICE_CONTRACTID,&rv);
  if(CCI_SUCCEEDED(rv))
     rv = kernSrvc->GetCopyOfNativeFamily(name,retval);

  return rv;
}

inline cci_result
CCI_NewKernelFamily( dmKernelFamily& aNativeFamily, cciKernelFamily* _retval )
{
  cci_result rv;

  cci_Ptr<cciIKernelService> kernSrvc = cci::do_GetService(CCI_KERNELSERVICE_CONTRACTID,&rv);
  if(CCI_SUCCEEDED(rv))
     rv = kernSrvc->NewFamilyFromNative(aNativeFamily,_retval);

  return rv;
}

#endif /* cciKernelServiceUtils_h */
