#ifndef CCI_Fractals_h
#define CCI_Fractals_h

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
// File         : CCI_Fractals.h
// Date         : 7/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//--------------------------------------------------------

#define CCIC_Fractals _TXT("processing/fractals.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( Fractals, Notify )

  CCI_MODULE_UUID( Fractals, 6af58be5-ef91-40c3-9738-35d10194c1be)

  CCI_MODULE_VERSION( Fractals, 1 ) 
 
  /* Methods   */
  CCI_DECL_METHOD_PP(Fractals, 1,  Setup, 
     _in dm_cci(Parameters) params
     _in  dm_uint           flags
  )

  CCI_DECL_METHOD_PP(Fractals, 2,  Lacunarity, 
     _in  dm_uint    size 
     _in  dm_uint    flags
     _out dm_real    result
  )

  /* Special values */

  /* for Lacunarity flags */
  #define CCIV_Lacunarity_Fractional 0x1L
  

CCI_END_MODULE_DECL( Fractals )

#endif // __doxygen
//----------------------------------------------------------------
#ifdef __cplusplus

#include "daim_modules/CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Do arithmetics operation between images.
 */
//------------------------------------------------------------------------------------
class dmIFractals : public dmINotify
{ 
  public:
   dmDECLARE_GENERIC_IINTERFACE(Fractals)

   //-------------------------------------------------------
   /*! \brief Initialize image and region
    *
    * Call this method before any other methods.
    */
   //-------------------------------------------------------
   dm_real Setup( dm_cci(Parameters) params ,dm_uint flags = 0 ) {
     CCI_RETURN_IMETHOD_PP_OK(Fractals,Setup, _in params _in flags ) 
   }

   //-------------------------------------------------------
   /*! \brief Compute the lacunarity for the \a size (in pixels)
    *
    * The lacunarity is computed using the gliding box algorithm and
    * for the given roi (i.e the box will "glide" on pixels
    * belonging to the given roi, but taking into account pixels that lies outside
    * the roi).
    *
    * \param size  is the size (in pixels) of the gliding box
    * \param flags combination of flags that control the algorithm:
    *     - CCIV_Lacunarity_Fractional : 
    *         By setting this flag, ROI border correction is applied by taking into 
    *         account for the fractional part of the gliding box outside the ROI.
    * 
    * \return the lacunarity value or -1 if the method failed 
    */
   //-------------------------------------------------------
   dm_real Lacunarity( dm_uint size ,dm_uint flags = 0) 
   {
     IF_CCI_IMETHOD_RP(Fractals,Lacunarity, _in size _in flags _out(result) -1) 
     THEN return CCI_RETVAL(result);
     ELSE return  -1;
     ENDIF
   }

};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_Fractals_h */
