#ifndef CCI_Edges_h
#define CCI_Edges_h

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
// File         : CCI_Edges.h
// Date         : 12/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//
//--------------------------------------------------------

#define CCIC_Edges _TXT("processing/edges.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( Edges, Notify )

  CCI_MODULE_UUID( Edges, 37fd11d8-20d6-4111-b219-c1acd6f0b5d2)

  CCI_MODULE_VERSION( Edges, 1 ) 

  /* Methods  */

  CCI_DECL_METHOD_PP(Edges,1,  Canny, 
     _in dm_cci(Parameters) params
     _in dm_bool            inner
  )

  CCI_DECL_METHOD_PP(Edges,2,  Gradient, 
     _in dm_cci(Parameters) params
  )

  CCI_DECL_METHOD_PP(Edges,3,  NonMaximalSuppression, 
     _in dm_cci(Parameters) params
     _in dm_bool            inner
  )

  CCI_DECL_METHOD_PP(Edges,4,  Nagao, 
     _in dm_cci(Parameters) params
  )

  CCI_DECL_METHOD_PP(Edges,5,  Phase, 
     _in dm_cci(Parameters) params
     _in CONST char_t*      xkernel
     _in CONST char_t*      ykernel
     _in dm_uint            flags
  )
  

  /* Attributs */


  /* Special values */

CCI_END_MODULE_DECL( Edges )

#endif // __doxygen
//----------------------------------------------------------------
#ifdef __cplusplus

#include "daim_modules/CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief 
 */
//------------------------------------------------------------------------------------
class dmIEdges : public dmINotify
{ 
  public:
   dmDECLARE_GENERIC_IINTERFACE(Edges)

   enum Constants {
   };

   bool Canny( dm_cci(Parameters) params, dm_bool inner ) { 
      CCI_RETURN_IMETHOD_PP_OK(Edges,Canny, _in params _in inner ) 
   }

   bool Gradient( dm_cci(Parameters) params ) {
     CCI_RETURN_IMETHOD_PP_OK(Edges,Gradient, _in params ) 
   }

   bool NonMaximalSuppression( dm_cci(Parameters) params, dm_bool inner ) {
     CCI_RETURN_IMETHOD_PP_OK(Edges,NonMaximalSuppression,
         _in params 
         _in inner
     ) 
   }

   bool Nagao( dm_cci(Parameters) params ) {
     CCI_RETURN_IMETHOD_PP_OK(Edges,Nagao, _in params ) 
   }
 
   bool Phase( dm_cci(Parameters) params, 
               const char_t*      xkernel, 
               const char_t*      ykernel, 
               dm_uint            flags = 0) 
   {
     CCI_RETURN_IMETHOD_PP_OK(Edges, Phase,_in params _in xkernel _in ykernel _in flags)
   }
};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_Edges_h */
