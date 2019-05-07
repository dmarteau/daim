#ifndef dmKernelBase_h
#define dmKernelBase_h

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
// File         : dmKernelBase.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

#ifndef daim_config_h
#include "daim_config.h"
#endif

#ifndef dmTypes_h
#include "dmTypes.h"
#endif

#ifdef DAIM_KERNEL_BUILD
  #ifndef DAIM_KERNEL
  #define DAIM_KERNEL
  #endif
#endif

#ifdef DAIM_KERNEL

  #ifdef DM_CONFIG_MSVC_BUILD  // Microsoft Visual C++ specific

    #ifdef DAIM_KERNEL_BUILD
      #pragma warning(disable: 4275)  // deriving exported class from non-exported
      #pragma warning(disable: 4251)  // using non-exported as public in exported
    #endif // DAIM_KERNEL_BUILD

  #endif // Microsoft Visual C++ specific

  #ifdef DAIM_KERNEL_BUILD

    #define __dmKernel        __DLL_EXPORT_TAG
    #define __dmKernelForward __DLL_EXPORT_FORWARD_TAG

  #else

    #define __dmKernel        __DLL_IMPORT_TAG
    #define __dmKernelForward __DLL_IMPORT_FORWARD_TAG

  #endif // DAIM_KERNEL_BUILD

#else  // DAIM_KERNEL

  #define __dmKernel
  #define __dmKernelForward

#endif // DAIM_KERNEL


// Forward declarations
class __dmKernelForward dmImage;
class __dmKernelForward dmLUT;
class __dmKernelForward dmImageBuffer;

class dmImageData;
class dmRegion;
class dmBufferParameters;
class dmKernelFamily;
class dmMaskDescription;
class dmHistogram;

#endif // dmKernelBase_h
