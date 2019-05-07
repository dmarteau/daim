#ifndef _daim_kernel_h
#define _daim_kernel_h

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
// File         : daim_kernel.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

#ifdef dmUseKernelBuffer

#ifndef dmUseKernelImage
#define dmUseKernelImage
#endif

#endif // dmUseKernelBuffer


#ifdef  dmUseKernelImageTemplates

#ifndef dmUseKernelImage
#define dmUseKernelImage
#endif

#ifndef dmUseKernelTemplates
#define dmUseKernelTemplates
#endif 

#endif // dmUseKernelImageTemplates

#define dmUseUtilitiesExtra
#include "../../daim_utilities/include/daim_utilities.h"

#include "dmKernelBase.h"

#include "templates/dmKernelConfig.h"
#include "templates/dmKernelTemplates.h"
#include "templates/dmConnectivity.h"

#include "dmCoordinatesTypes.h"
#include "dmRGBColor.h"
#include "dmPixelFormats.h"
#include "dmImageData.h"

#include "regions/dmCoordinateUtils.h"
#include "regions/dmCoordinates.h"

#include "regions/dmRegionLines.h"
#include "regions/dmRegions.h"
#include "regions/dmDigitalRegion.h"

#ifdef dmUseKernelTemplates

#include "templates/images/dmRoiOperators.h"
#include "templates/images/dmRoiAlgorithms.h"

#include "images/dmPixels.h"

#include "templates/images/dmImageErrors.h"
#include "templates/images/dmImageBase.h"
#include "templates/images/dmRoiOperations.h"
#include "templates/images/dmRoiAlgorithms.h"
#include "templates/images/dmImageAlgorithms.h"
#include "templates/images/dmImageScale.h"

#endif // dmUseKernelTemplates

#ifdef dmUseKernelImage
#include "common/dmLut.h"
#include "common/dmSpatialUnits.h"
#include "images/dmImage.h"
#include "images/dmImageBuffer.h"
#include "images/dmImageMath.h"
#endif // dmUseKernelImage

#ifdef dmUseKernelImageTemplates
#include "images/dmImagetypes.h"
#include "images/dmImagetypes.hxx"
#include "images/dmImageOperations.h"
#endif // dmUseKernelImageTemplates

#ifdef dmUseKernelTools
#include "tools/dmColorMaps.h"
#include "tools/dmIntensityProfile.h"
#include "tools/dmRegionTree.h"
#endif // dmUseKernelTools

#endif // _daim_kernel_h
