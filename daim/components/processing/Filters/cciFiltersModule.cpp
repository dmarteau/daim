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

#include "cciServiceManagerUtils.h"
#include "cciIComponentManager.h"
#include "cciIGenericFactory.h"

#include "cciConvolutionFilter.h"
#include "cciLFilter.h"
#include "cciRFilter.h"
#include "cciSharpenFilter.h"

CCI_GENERIC_FACTORY_CONSTRUCTOR(cciConvolutionFilter)
CCI_GENERIC_FACTORY_CONSTRUCTOR(cciRFilter)
CCI_GENERIC_FACTORY_CONSTRUCTOR(cciLFilter)
CCI_GENERIC_FACTORY_CONSTRUCTOR(cciSharpenFilter)

static const cciModuleComponentInfo components[] = {
    {  CCI_CONVOLUTIONFILTER_CLASSNAME,
       CCI_CONVOLUTIONFILTER_CID,
       CCI_CONVOLUTIONFILTER_CONTRACTID,
       cciConvolutionFilterConstructor
    },
    {  CCI_RFILTER_CLASSNAME,
       CCI_RFILTER_CID,
       CCI_RFILTER_CONTRACTID,
       cciRFilterConstructor
    },
    {  CCI_LFILTER_CLASSNAME,
       CCI_LFILTER_CID,
       CCI_LFILTER_CONTRACTID,
       cciLFilterConstructor
    },
    {  CCI_SHARPENFILTER_CLASSNAME,
       CCI_SHARPENFILTER_CID,
       CCI_SHARPENFILTER_CONTRACTID,
       cciSharpenFilterConstructor
    },
};

CCI_IMPL_GETMODULE(FiltersModule, components)







