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
// File         : cciDaimModule.cpp
// Date         : 15 déc. 2008
// Author       : David Marteau
//--------------------------------------------------------

#include "cciServiceManagerUtils.h"
#include "cciIComponentManager.h"
#include "cciIGenericFactory.h"

#include "cciImageList.h"
#include "cciFilterContext.h"
#include "cciDaimService.h"
#include "cciImageListImage.h"
#include "cciStorageImage.h"
#include "cciImageShell.h"
#include "cciScriptableRegion.h"
#include "cciImageContainerUtils.h"
#include "cciDaimModule.h"


CCI_GENERIC_FACTORY_CONSTRUCTOR(cciImageList)
CCI_GENERIC_FACTORY_CONSTRUCTOR(cciDaimService)
CCI_GENERIC_FACTORY_CONSTRUCTOR(cciImageShell)
CCI_GENERIC_FACTORY_CONSTRUCTOR(cciScriptableRegion)
CCI_GENERIC_FACTORY_CONSTRUCTOR(cciImageListImage)
CCI_GENERIC_FACTORY_CONSTRUCTOR(cciStorageImage)

static const cciModuleComponentInfo components[] = {
    { CCI_IMAGELIST_CLASSNAME,
      CCI_IMAGELIST_CID,
      CCI_IMAGELIST_CONTRACTID,
      cciImageListConstructor
    },
    {  CCI_DAIMSERVICE_CLASSNAME,
       CCI_DAIMSERVICE_CID,
       CCI_DAIMSERVICE_CONTRACTID,
       cciDaimServiceConstructor
    },
    {  CCI_IMAGESHELL_CLASSNAME,
       CCI_IMAGESHELL_CID,
       CCI_IMAGESHELL_CONTRACTID,
       cciImageShellConstructor
    },
    {  CCI_SCRIPTABLEREGION_CLASSNAME,
       CCI_SCRIPTABLEREGION_CID,
       CCI_SCRIPTABLEREGION_CONTRACTID,
       cciScriptableRegionConstructor
    },
    {  CCI_IMAGELISTIMAGE_CLASSNAME,
       CCI_IMAGELISTIMAGE_CID,
       CCI_IMAGELISTIMAGE_CONTRACTID,
       cciImageListImageConstructor
    },
    {  CCI_STORAGEIMAGE_CLASSNAME,
       CCI_STORAGEIMAGE_CID,
       CCI_STORAGEIMAGE_CONTRACTID,
       cciStorageImageConstructor
    },
};


CCI_IMPL_GETMODULE(DaimModule, components)



