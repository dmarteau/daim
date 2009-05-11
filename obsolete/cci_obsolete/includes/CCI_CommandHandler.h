#ifndef CCI_CommandHandler_h
#define CCI_CommandHandler_h

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
// File         : CCI_CommandHandler.h
// Date         : 05/2006
// Author       : David Marteau
//--------------------------------------------------------
#define CCIC_CommandHandler  _TXT("cmdhdlr.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( CommandHandler, Notify )

  CCI_MODULE_UUID( CommandHandler, a773bbbd-1285-4fe0-92ff-fc3fbee41ce0)

  CCI_MODULE_VERSION( CommandHandler, 1 ) 

  /* Methods */

  CCI_DECL_METHOD_PP(CommandHandler,1,  ExecCommand,
    _in CONST char_t*  command
    _in CONST char_t*  argstr
    _in dm_param       argint
    _in cci_Object*    param
  )

  CCI_DECL_METHOD_PP(CommandHandler,2,  IsCommandSupported,
    _in CONST char_t*  command
  )

CCI_END_MODULE_DECL( CommandHandler )

#endif // __doxygen

#ifdef __cplusplus

#include "CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Command handler interface
 *
 *  Base interface for generic command handler object.
 */
//------------------------------------------------------------------------------------
class dmICommandHandler : public dmINotify
{
  public:
   dmDECLARE_GENERIC_IINTERFACE(CommandHandler)

   //-------------------------------------------------------
   /*! \brief Execute a command */
   //-------------------------------------------------------
   dm_bool ExecCommand( const char_t* command, const char_t* argstr, dm_param argint, cci_Object* param ) {
      CCI_RETURN_IMETHOD_PP_OK(CommandHandler,ExecCommand,_in command _in argstr _in argint _in param ) 
   }

   //-------------------------------------------------------
   /*! \brief Check if a command is supported */
   //-------------------------------------------------------
   dm_bool IsCommandSupported( const char_t* command ) {
      CCI_RETURN_IMETHOD_PP_OK(CommandHandler,IsCommandSupported,_in command) 
   }
};

#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

//-------------------------------------------------------------
#endif /* CCI_CommandHandler_h */
