#ifndef cciKernelService_h
#define cciKernelService_h
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
// File         : cciKernelService.h
// Date         : 19 nov. 2008
// Author       : David Marteau
//--------------------------------------------------------

#include "cciIKernelService.h"

#define dmUseUtilitiesExtra
#include "daim_kernel.h"
#include "templates/processing/dmKernelFamily.h"

struct kernelEntry;

class cciKernelService : public cciIKernelService
{

typedef daim::hashmap<dmCString,dmKernelFamily*> dmKernelDict;

public:
  CCI_DECL_ISUPPORTS
  CCI_DECL_IKERNELSERVICE

  cciKernelService();

private:
  ~cciKernelService();

  void AddFamilyEntry( const kernelEntry* family, size_t size,
                       const char* name,
                       const char* mode );

  void LoadStaticKernels();

  void Shutdown();

protected:
  dmKernelDict mFamilies;
};

#endif /* cciKernelService_h */
