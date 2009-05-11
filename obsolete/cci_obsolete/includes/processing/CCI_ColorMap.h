#ifndef CCI_ColorMap_h
#define CCI_ColorMap_h

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
// Copyright (c) 2005 David Marteau
// 
// File         : CCI_ColorMap.h
// Date         : 08/2006
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//--------------------------------------------------------

#define CCIC_ColorMap _TXT("processing/colormap.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( ColorMap, Notify )

  CCI_MODULE_UUID( ColorMap, bb80d998-9a02-4551-8ce6-3f42abf8e18d )

  CCI_MODULE_VERSION( ColorMap, 1 ) 

  /* Methods */
  CCI_DECL_METHOD_PP(ColorMap,1, SetLUT, 
    _in        dm_uint32     count
    _in  CONST dm_uint32*    colors
  )

  CCI_DECL_METHOD_PP(ColorMap,2, GetLUT, 
    _out       dm_uint32    count
    _out CONST dm_uint32*   colors
  )

  CCI_DECL_METHOD_PP(ColorMap,3, SetColor32, 
    _in  dm_uint32  index
    _in  dm_uint32  color
  )

  CCI_DECL_METHOD_PP(ColorMap,4, GetColor32, 
    _in   dm_uint32  index
    _out  dm_uint32  color
  )

  CCI_DECL_METHOD_PP(ColorMap,5, Remap, 
    _in  dm_uint32         count
    _in  CONST dm_uint32  *cmap
  )

  CCI_DECL_METHOD_PP(ColorMap,6, RGBToColorSpace, 
    _in  dm_cci(ColorSpace)  colorspace
  )

  CCI_DECL_METHOD_PP(ColorMap,7, ColorSpaceToRGB, 
    _in  dm_cci(ColorSpace)  colorspace
  )

  /* Attributs */
  CCI_DECL_ATTR( ColorMap, 1, Count      , dm_uint      , [..G.] )
  CCI_DECL_ATTR( ColorMap, 2, Description, CONST char_t*, [.SGN] )

CCI_END_MODULE_DECL( ColorMap )

#endif //__doxygen
//----------------------------------------------------------------
#ifdef __cplusplus

#include "daim_modules/CCI_Interface.h"
//----------------------------------------------------------
/*! \ingroup Interfaces
 * 
 * Manipulate colormap/palette entries
 */
//----------------------------------------------------------
class dmIColorMap : public dmINotify
{ 
  public:
   dmDECLARE_GENERIC_IINTERFACE(ColorMap)
   
   bool SetLUT( dm_uint32 count, const dm_uint32* colors ) { 
      CCI_RETURN_IMETHOD_PP_OK(ColorMap, SetLUT,  _in count _in colors)
   }

   bool GetLUT( dm_uint32& count, const dm_uint32*& colors ) { 
     IF_CCI_IMETHOD_RP(ColorMap,GetLUT, _out(count) 0 _out(colors) dm_null ) THEN 
       CCI_RETURN_P(count)
       CCI_RETURN_P(colors)
     ENDIF_RETURN_BOOL()
   }

   bool SetColor32( dm_uint32 index, dm_uint32  color ) { 
      CCI_RETURN_IMETHOD_PP_OK(ColorMap, SetColor32,  _in index _in color)
   }

   bool GetColor32( dm_uint32 index, dm_uint32& color ) { 
     IF_CCI_IMETHOD_RP(ColorMap,GetColor32, _in index ) THEN 
       CCI_RETURN_P(color)
     ENDIF_RETURN_BOOL()
   }

   bool Remap( dm_uint32 count, const dm_uint32* cmap ) { 
      CCI_RETURN_IMETHOD_PP_OK(ColorMap, Remap, _in count _in cmap )
   }
   
   dm_uint Count() { return CCI_IGET(ColorMap,Count,(dm_uint)0); }

   bool          SetDescription( const char_t* description ) { return CCI_ISET(ColorMap,Description,description); }
   const char_t* GetDescription() { return CCI_IGET(ColorMap,Description,(char_t*)NULL); }


   bool RGBToColorSpace( dm_cci(ColorSpace)  colorspace ) { 
      CCI_RETURN_IMETHOD_PP_OK(ColorMap, RGBToColorSpace,  _in colorspace)
   }

   bool ColorSpaceToRGB( dm_cci(ColorSpace)  colorspace ) { 
      CCI_RETURN_IMETHOD_PP_OK(ColorMap, ColorSpaceToRGB,  _in colorspace)
   }

};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_ColorMap_h */
