#ifndef CCI_Colorspaces_h
#define CCI_Colorspaces_h

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
// File         : CCI_Colorspaces.h
// Date         : 12/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//--------------------------------------------------------

#include "daim_modules/processing/CCI_Colorspace.h"

#define CCIC_ColorSpaceRGB   _TXT("processing/colorspaces/RGB.cci")
#define CCIC_ColorSpaceHSV   _TXT("processing/colorspaces/HSV.cci")
#define CCIC_ColorSpaceHLS   _TXT("processing/colorspaces/HLS.cci")
#define CCIC_ColorSpaceYUV   _TXT("processing/colorspaces/YUV.cci")
#define CCIC_ColorSpaceYIQ   _TXT("processing/colorspaces/YIQ.cci")
#define CCIC_ColorSpaceOCP   _TXT("processing/colorspaces/OCP.cci")
#define CCIC_ColorSpaceGSC   _TXT("processing/colorspaces/GSC.cci")
#define CCIC_ColorSpaceGSL   _TXT("processing/colorspaces/GSL.cci")

#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( ColorSpaceRGB, ColorSpace )
  CCI_MODULE_UUID( ColorSpaceRGB, 9eb15001-3647-4636-8559-e0e1592316df)
  CCI_MODULE_VERSION( ColorSpaceRGB, 1 ) 
CCI_END_MODULE_DECL( ColorSpaceRGB )

CCI_BEGIN_MODULE_DECL( ColorSpaceHSV, ColorSpace )
  CCI_MODULE_UUID( ColorSpaceHSV, c33ae4b3-b5a2-49d1-b364-6351b32fbff0)
  CCI_MODULE_VERSION( ColorSpaceHSV, 1 ) 
CCI_END_MODULE_DECL( ColorSpaceHSV )

CCI_BEGIN_MODULE_DECL( ColorSpaceHLS, ColorSpace )
  CCI_MODULE_UUID( ColorSpaceHLS, 2fdd7562-1d0f-48f8-82aa-bbbc9ef50009)
  CCI_MODULE_VERSION( ColorSpaceHLS, 1 ) 
CCI_END_MODULE_DECL( ColorSpaceHLS )

CCI_BEGIN_MODULE_DECL( ColorSpaceYUV, ColorSpace )
  CCI_MODULE_UUID( ColorSpaceYUV, 7e3263d4-0de3-4d49-a5fd-0b25232dd06f)
  CCI_MODULE_VERSION( ColorSpaceYUV, 1 ) 
CCI_END_MODULE_DECL( ColorSpaceYUV )

CCI_BEGIN_MODULE_DECL( ColorSpaceYIQ, ColorSpace )
  CCI_MODULE_UUID( ColorSpaceYIQ, defb001d-246a-442d-86a3-c2ae8d84dc37)
  CCI_MODULE_VERSION( ColorSpaceYIQ, 1 ) 
CCI_END_MODULE_DECL( ColorSpaceYIQ )

CCI_BEGIN_MODULE_DECL( ColorSpaceOCP, ColorSpace )
  CCI_MODULE_UUID( ColorSpaceOCP, 31e4f649-8424-424d-813a-808c8a039cbf)
  CCI_MODULE_VERSION( ColorSpaceOCP, 1 ) 
CCI_END_MODULE_DECL( ColorSpaceOCP )

CCI_BEGIN_MODULE_DECL( ColorSpaceGSC, ColorSpace )
  CCI_MODULE_UUID( ColorSpaceGSC, 705cc897-23bc-4de0-97b0-1bf7492c8f67)
  CCI_MODULE_VERSION( ColorSpaceGSC, 1 ) 
CCI_END_MODULE_DECL( ColorSpaceGSC )

CCI_BEGIN_MODULE_DECL( ColorSpaceGSL, ColorSpace )
  CCI_MODULE_UUID( ColorSpaceGSL, 0693514e-999e-4428-92ad-6c70e4c33e7c)
  CCI_MODULE_VERSION( ColorSpaceGSL, 1 ) 
CCI_END_MODULE_DECL( ColorSpaceGSL )

#endif //__doxygen

//----------------------------------------------------------------
#ifdef __cplusplus
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Interface for RGB decomposition
 */
//------------------------------------------------------------------------------------
class dmIColorSpaceRGB : public dmIColorSpace { 
  public:
   dmDECLARE_GENERIC_IINTERFACE(ColorSpaceRGB)
};
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Interface for HSV decomposition
 */
//------------------------------------------------------------------------------------
class dmIColorSpaceHSV : public dmIColorSpace { 
  public:
   dmDECLARE_GENERIC_IINTERFACE(ColorSpaceHSV)
};
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Interface for HLS decomposition
 */
//------------------------------------------------------------------------------------
class dmIColorSpaceHLS : public dmIColorSpace { 
  public:
   dmDECLARE_GENERIC_IINTERFACE(ColorSpaceHLS)
};

//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Interface for YUV decomposition
 */
//------------------------------------------------------------------------------------
class dmIColorSpaceYUV : public dmIColorSpace { 
  public:
   dmDECLARE_GENERIC_IINTERFACE(ColorSpaceYUV)
};
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Interface for YIQ decomposition
 */
//------------------------------------------------------------------------------------
class dmIColorSpaceYIQ : public dmIColorSpace { 
  public:
   dmDECLARE_GENERIC_IINTERFACE(ColorSpaceYIQ)
};
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Interface for OCP decomposition
 */
//------------------------------------------------------------------------------------
class dmIColorSpaceOCP : public dmIColorSpace { 
  public:
   dmDECLARE_GENERIC_IINTERFACE(ColorSpaceOCP)
};
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Interface for GSC decomposition
 */
//------------------------------------------------------------------------------------
class dmIColorSpaceGSC : public dmIColorSpace { 
  public:
   dmDECLARE_GENERIC_IINTERFACE(ColorSpaceGSC)
};
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Interface for GSL decomposition
 */
//------------------------------------------------------------------------------------
class dmIColorSpaceGSL : public dmIColorSpace { 
  public:
   dmDECLARE_GENERIC_IINTERFACE(ColorSpaceGSL)
};

//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_Colorspaces_h */
