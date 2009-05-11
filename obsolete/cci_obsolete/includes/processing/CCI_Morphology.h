#ifndef CCI_Morphology_h
#define CCI_Morphology_h

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
// File         : CCI_Morphology.h
// Date         : 7/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//--------------------------------------------------------

#define CCIC_Morphology _TXT("processing/morphology.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( Morphology, Notify )

  CCI_MODULE_UUID( Morphology, 1a79231c-073d-4960-a65e-69134e456053)

  CCI_MODULE_VERSION( Morphology, 3 ) 

  /* Methods */
  CCI_DECL_METHOD_PP(Morphology,1,  ApplyKernel, 
       _in dm_cci(Parameters) params
       _in CONST char_t*      kernel
       _in dm_int             action 
       _in dm_int             iter
       _in dm_int             operation
  )
 
CCI_END_MODULE_DECL( Morphology )

#endif //__doxygen

//----------------------------------------------------------------
#ifdef __cplusplus

#include "daim_modules/CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Apply kernel-based morphological operations on image.
 */
//------------------------------------------------------------------------------------
class dmIMorphology : public dmINotify
{ 
  public:
   dmDECLARE_GENERIC_IINTERFACE(Morphology)

   //-------------------------------------------------------
   /*! \brief Apply a morphogical transformation.               
    * 
    * \param params parameter object ( see dmIParameters ) 
    * \param kernel the kernel family string id.
    * \param action the morphological operation to apply. 
    * \param iter the number of iteration.
    * \param operation an arithmetic operation to perform between the original
    * image and the result of the filter (see \ref daim_operations for details)
    */
  //-------------------------------------------------------
   bool ApplyKernel( dm_cci(Parameters) params, CONST char_t* kernel, 
                     int action, int iter, int operation ) 
   {
     CCI_RETURN_IMETHOD_PP_OK(Morphology,ApplyKernel,
                             _in params
                             _in kernel
                             _in action 
                             _in iter
                             _in operation )
   }

};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_Morphology_h */
