#ifndef CCI_FrameGrabber_h
#define CCI_FrameGrabber_h

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
// File         : CCI_FrameGrabber.h
// Date         : 4/2005
// Author       : David Marteau
//--------------------------------------------------------
#define CCIC_FrameGrabber  _TXT("frmgrbr.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( FrameGrabber, Notify )

  CCI_MODULE_UUID( FrameGrabber, 3ae5aa91-63de-44d1-8871-0982e5bf6f99)

  CCI_MODULE_VERSION( FrameGrabber, 1 )

  /* Methods */

  CCI_DECL_METHOD_PP(FrameGrabber,1, Initialize, _in CONST char_t* source )
  CCI_DECL_METHOD_NP(FrameGrabber,2, ShutDown   )   
  CCI_DECL_METHOD_NP(FrameGrabber,3, Suspend    )
  CCI_DECL_METHOD_NP(FrameGrabber,4, Resume     )
  CCI_DECL_METHOD_NP(FrameGrabber,5, Reset      )

  CCI_DECL_METHOD_PP(FrameGrabber,6, GetFrameBuffer,
      _in  dm_int  flags 
      _out dm_int  format 
      _out dm_uint width  
      _out dm_uint height 
      _out dm_int  stride 
      _out dm_ptr  scan0
  )

  CCI_DECL_METHOD_PP(FrameGrabber,7, CopyFrameBuffer,
      _in dm_cci(ImageList)  imagelist
      _in dm_uint            index  
      _in dm_int             flags 
  )

  CCI_DECL_METHOD_PP(FrameGrabber,8,  Property, 
     _in dm_param       me
     _in CONST char_t*  name 
     _in CONST char_t*  value 
  )

  CCI_DECL_METHOD_PP(FrameGrabber,9,  Value, 
     _in dm_param       me
     _in CONST char_t*  name 
     _in dm_param       value 
  )

  CCI_DECL_METHOD_PP(FrameGrabber,10,  GetDeviceList, 
     _in dm_cci(StringList) stringlist
  )

  CCI_DECL_METHOD_PP(FrameGrabber,11,  GetDeviceInfos, 
     _in CONST char_t*        device
     _in dm_param             key
     _in dm_cci(PropertyList) propertylist
  )

  CCI_DECL_METHOD_PP(FrameGrabber,12,  ExecCommand,
    _in CONST char_t*  command
    _in CONST char_t*  arg
    _in cci_Object*    param
  )

  CCI_DECL_METHOD_PP(FrameGrabber,13,  IsCommandSupported,
    _in CONST char_t*  command
  )

  /* Not implemented : interface description only */

  CCI_DECL_METHOD_PP(FrameGrabber,21,  SetProperty, 
     _in CONST char_t* name 
     _in CONST char_t* value 
  )

  /* Not implemented : interface description only */

  CCI_DECL_METHOD_PP(FrameGrabber,22,  GetProperty, 
     _in  CONST char_t* name 
     _out CONST char_t* value 
  )

  CCI_DECL_METHOD_PP(FrameGrabber,23,  GetValue, 
     _in  CONST char_t* name 
     _out dm_int        value 
  )

  CCI_DECL_METHOD_PP(FrameGrabber,24,  SetValue, 
     _in  CONST char_t* name 
     _in  dm_int        value 
  )

  CCI_DECL_METHOD_PP(FrameGrabber,25,  GetMinValue, 
     _in  CONST char_t* name 
     _out dm_int        value 
  )

  CCI_DECL_METHOD_PP(FrameGrabber,26,  GetMaxValue, 
     _in  CONST char_t* name 
     _out dm_int        value 
  )

  CCI_DECL_METHOD_PP(FrameGrabber,27,  GetDefaultValue, 
     _in  CONST char_t* name 
     _out dm_int        value 
  )

  /* Attributs */
  
  CCI_DECL_ATTR(FrameGrabber,1  , State     , dm_int        , [..GN] )
  CCI_DECL_ATTR(FrameGrabber,2  , XferStatus, dm_int        , [..GN] )
  CCI_DECL_ATTR(FrameGrabber,3  , Source    , CONST char_t* , [..G.] )

  /* Special values */

  #define CCIV_FrameGrabber_XFERSTART 1L
  #define CCIV_FrameGrabber_XFERDONE  2L
  #define CCIV_FrameGrabber_XFERERROR 3L
  #define CCIV_FrameGrabber_XFERABORT 4L

  #define CCIV_FrameGrabber_LiveBuffer 0L
  #define CCIV_FrameGrabber_FullBuffer 1L

  #define CCIV_FrameGrabber_PropGet     0L
  #define CCIV_FrameGrabber_PropSet     1L
  #define CCIV_FrameGrabber_PropMin     2L
  #define CCIV_FrameGrabber_PropMax     3L
  #define CCIV_FrameGrabber_PropDefault 4L

CCI_END_MODULE_DECL( FrameGrabber )

#endif // __doxygen
//----------------------------------------------------------------
#ifdef __cplusplus

#include "CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief FrameGrabber base interface
 *
 */
//------------------------------------------------------------------------------------
class dmIFrameGrabber : public dmINotify
{
  public:
   dmDECLARE_GENERIC_IINTERFACE(FrameGrabber)

   bool Initialize( CONST char_t* source ) {
     CCI_RETURN_IMETHOD_PP_OK(FrameGrabber,Initialize, _in source )
   }
   
   bool ShutDown() { CCI_RETURN_IMETHOD_NP_OK(FrameGrabber,ShutDown) }
   bool Suspend()  { CCI_RETURN_IMETHOD_NP_OK(FrameGrabber,Suspend ) }
   bool Resume()   { CCI_RETURN_IMETHOD_NP_OK(FrameGrabber,Resume  ) }
   bool Reset()    { CCI_RETURN_IMETHOD_NP_OK(FrameGrabber,Reset   ) }

   bool SetProperty( CONST char_t* name, CONST char_t* value ) {
     CCI_RETURN_IMETHOD_PP_OK(FrameGrabber,Property,
      _in CCIV_FrameGrabber_PropSet _in name _in value );
   }

   const char_t* GetProperty( CONST char_t* name ) {
     IF_CCI_IMETHOD_RP(FrameGrabber,Property, _in CCIV_FrameGrabber_PropGet _in name _out(value) NULL )
     THEN return CCI_RETVAL(value);
     ELSE return NULL;
     ENDIF
   }

   bool SetValue( CONST char_t* name, dm_int value ) {
     CCI_RETURN_IMETHOD_PP_OK(FrameGrabber,Value, 
      _in CCIV_FrameGrabber_PropSet _in name _in value );
   }

   dm_int GetValue( CONST char_t* name ) {
     IF_CCI_IMETHOD_RP(FrameGrabber, Value, _in CCIV_FrameGrabber_PropGet _in name _out(value) 0 )
     THEN return CCI_RETVAL(value);
     ELSE return 0;
     ENDIF
   }

   dm_int GetMinValue( CONST char_t* name ) {
     IF_CCI_IMETHOD_RP(FrameGrabber, Value, _in CCIV_FrameGrabber_PropMin _in name _out(value) 0 )
     THEN return CCI_RETVAL(value);
     ELSE return 0;
     ENDIF
   }

   dm_int GetMaxValue( CONST char_t* name ) {
     IF_CCI_IMETHOD_RP(FrameGrabber, Value, _in CCIV_FrameGrabber_PropMax _in name _out(value) 0 )
     THEN return CCI_RETVAL(value);
     ELSE return 0;
     ENDIF
   }

   dm_int GetDefaultValue( CONST char_t* name ) {
     IF_CCI_IMETHOD_RP(FrameGrabber, Value, _in CCIV_FrameGrabber_PropDefault _in name _out(value) 0 )
     THEN return CCI_RETVAL(value);
     ELSE return 0;
     ENDIF
   }

   //-------------------------------------------------------
   /*! \brief Execute a command */
   //-------------------------------------------------------
   dm_bool ExecCommand( const char_t* command, const char_t* arg, cci_Object* param ) {
      CCI_RETURN_IMETHOD_PP_OK(FrameGrabber,ExecCommand,_in command _in arg _in param ) 
   }
   
   //-------------------------------------------------------
   /*! \brief Check if a command is supported */
   //-------------------------------------------------------
   dm_bool IsCommandSupported( const char_t* command ) {
      CCI_RETURN_IMETHOD_PP_OK(FrameGrabber,IsCommandSupported,_in command) 
   }

};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_FrameGrabber_h */
