#ifndef CCI_ImageStat_h
#define CCI_ImageStat_h

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
// File         : CCI_ImageStat.h
// Date         : 11/2005
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//
//--------------------------------------------------------

#define CCIC_ImageStat _TXT("processing/imgstat.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( ImageStat, Notify )
  
  CCI_MODULE_UUID( ImageStat, d24b4f3f-4984-4534-8453-7fb491ebd1fe)

  CCI_MODULE_VERSION( ImageStat, 1 ) 
 
  /* Methods   */

  CCI_DECL_METHOD_PP(ImageStat,  1,  Moment,
     _in  dm_cci(Parameters)  params
     _in  dm_uint             order
     _in  dm_real             central
     _out dm_real             value
  )

  CCI_DECL_METHOD_PP(ImageStat,  2,  GetStat,
     _in  dm_cci(Parameters)  params
     _in  CONST char_t*       type
     _out dm_real             value
  )

  
  /* Attributs */

  /* Special values */

CCI_END_MODULE_DECL( ImageStat )

#endif // __doxygen
//----------------------------------------------------------------
#ifdef __cplusplus

#include "daim_modules/CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Implement methods for computing statistical properties of an image
 */
//------------------------------------------------------------------------------------
class dmIImageStat : public dmINotify
{ 
  public:
   dmDECLARE_GENERIC_IINTERFACE(ImageStat)
   
  //-------------------------------------------------------
  /*! \brief Compute the moment of order \a order the input image
   *
   *  The returned value is not normalized.
   *  Using 0 order will return the size of the ROI
   */
  //-------------------------------------------------------
  dm_real Moment( dm_cci(Parameters) params, dm_uint order, dm_real central )
  {
     IF_CCI_IMETHOD_RP(ImageStat,Moment, _in params _in order _in central )
     THEN  
       return CCI_RETVAL(value);
     ENDIF
      return 0.; 
  }

  //-------------------------------------------------------
  /*! \brief Compute special statistical value from the input image */
  //-------------------------------------------------------
  dm_real GetStat( dm_cci(Parameters) params, const char_t* type )
  {
     IF_CCI_IMETHOD_RP(ImageStat,GetStat, _in params _in type )
     THEN  
       return CCI_RETVAL(value);
     ENDIF
       return 0.; 
  }  
};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_ImageStat_h */
