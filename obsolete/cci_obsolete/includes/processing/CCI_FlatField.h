#ifndef CCI_FlatField_h
#define CCI_FlatField_h

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
// File         : CCI_FlatField.h
// Date         : 11/2005
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//
//--------------------------------------------------------

#define CCIC_FlatField _TXT("processing/flatfield.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( FlatField, Notify )
  
  CCI_MODULE_UUID( FlatField, 7587997d-21cf-420a-8aae-f36b6aaa0d68)

  CCI_MODULE_VERSION( FlatField, 1 ) 
 
  /* Methods   */


  CCI_DECL_METHOD_PP(FlatField,  1,  SetFlatField,
     _in dm_cci(Parameters)  params
     _in dm_real             offset
     _in dm_real             norm
  )

  CCI_DECL_METHOD_PP(FlatField,  3,  Apply,
     _in dm_cci(Parameters)  params
     _in dm_param            flag
  )

  CCI_DECL_METHOD_NP(FlatField,  4,  Clear )
  
  /* Attributs */
  CCI_DECL_ATTR(FlatField,1,  Format , dm_uint  , [..G.] )
  

CCI_END_MODULE_DECL( FlatField )

#endif // __doxygen
//----------------------------------------------------------------
#ifdef __cplusplus

#include "daim_modules/CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Implement Flatfield correction
 */
//------------------------------------------------------------------------------------
class dmIFlatField : public dmINotify
{ 
  public:
   dmDECLARE_GENERIC_IINTERFACE(FlatField)

  //-------------------------------------------------------
  /*! \brief Set flat field image */
  //-------------------------------------------------------
  bool SetFlatField( dm_cci(Parameters) params, 
                     dm_real offset, dm_real norm ) {
    CCI_RETURN_IMETHOD_PP_OK(FlatField,SetFlatField,
            _in params _in offset _in norm);
  }
   
  //-------------------------------------------------------
  /*! \brief Apply flatfield correction to image */
  //-------------------------------------------------------
  bool Apply( dm_cci(Parameters) params, dm_param flags) {
    CCI_RETURN_IMETHOD_PP_OK(FlatField,Apply,_in params _in flags);
  }

  //-------------------------------------------------------
  /*! \brief Clear the flat field image */
  //-------------------------------------------------------
  bool Clear() {
    CCI_RETURN_IMETHOD_NP_OK(FlatField,Clear);
  }

};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_FlatField_h */
