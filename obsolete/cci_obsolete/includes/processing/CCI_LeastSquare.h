#ifndef CCI_LeastSquare_h
#define CCI_LeastSquare_h

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
// File         : CCI_LeastSquare.h
// Date         : 11/2005
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//
//--------------------------------------------------------

#define CCIC_LeastSquare _TXT("processing/lstsqr.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( LeastSquare, Notify )
  
  CCI_MODULE_UUID( LeastSquare, 3bc7536b-7c7b-49b0-9ebe-f9dcd1b1d2c2)

  CCI_MODULE_VERSION( LeastSquare, 1 ) 
 
  /* Methods   */

  CCI_DECL_METHOD_PP(LeastSquare,  1,  SetData,
     _in  dm_cci(StorageMap)  storage
     _in  CONST char_t*       xcolumn
     _in  CONST char_t*       ycolumn
  )

  CCI_DECL_METHOD_PP(LeastSquare,  2, Fit,
     _in  CONST char_t*  type   
     _in  dm_uint        iparam
     _in  dm_real        fparam1   
     _in  dm_real        fparam2        
     _out dm_real        chi2
  )
 
  CCI_DECL_METHOD_NP(LeastSquare,  5,  Update)

  CCI_DECL_METHOD_PP(LeastSquare,  6,  GetParam,
     _in  dm_uint index
     _out dm_real value
  )

  CCI_DECL_METHOD_PP(LeastSquare,  7,  SetDataRange,
     _in  dm_uint start
     _in  dm_uint end
  )

  CCI_DECL_METHOD_PP(LeastSquare,  8,  Predict,
     _in  CONST char_t*  ycolumn
  )
  
  /* Attributs */
  CCI_DECL_ATTR(LeastSquare, 1, XColumn, CONST char_t*      , [..G.] )
  CCI_DECL_ATTR(LeastSquare, 2, YColumn, CONST char_t*      , [..G.] )
  CCI_DECL_ATTR(LeastSquare, 3, Storage, dm_cci(StorageMap) , [..G.] )

  /* Special values */

CCI_END_MODULE_DECL( LeastSquare )

#endif // __doxygen
//----------------------------------------------------------------
#ifdef __cplusplus

#include "daim_modules/CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Implement methods for fitting data using generalized leastsquare methods
 */
//------------------------------------------------------------------------------------
class dmILeastSquare : public dmINotify
{ 
  public:
   dmDECLARE_GENERIC_IINTERFACE(LeastSquare)

  bool SetData( dm_cci(StorageMap)  storage,const char_t* xcolumn, const char_t* ycolumn ) 
  {
    CCI_RETURN_IMETHOD_PP_OK(LeastSquare,SetData,
        _in storage
        _in xcolumn 
        _in ycolumn)
  }

  dm_real Fit( const char_t*  type, dm_uint iparam, dm_real fparam1, dm_real fparam2 )
  {
    IF_CCI_IMETHOD_RP(LeastSquare,Fit,_in type _in iparam _in fparam1 _in fparam2)        
     THEN return CCI_RETVAL(chi2);
     ELSE return -1.0;
     ENDIF
  }
 
  bool Update() { CCI_RETURN_IMETHOD_NP_OK(LeastSquare,Update) }
  
  bool GetParam( dm_uint index, dm_real& value ) { 
    IF_CCI_IMETHOD_RP(LeastSquare,GetParam, _in index ) THEN 
      CCI_RETURN_P(value)
    ENDIF_RETURN_BOOL()
  }

  bool SetDataRange( dm_uint start, dm_uint end ) {
    CCI_RETURN_IMETHOD_PP_OK(LeastSquare,SetDataRange,_in start _in end)
  }

  bool Predict( const char_t* ycolumn ) {
    CCI_RETURN_IMETHOD_PP_OK(LeastSquare, Predict, _in  ycolumn)
  }
   
};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_LeastSquare_h */
