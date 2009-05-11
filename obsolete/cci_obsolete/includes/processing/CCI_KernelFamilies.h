#ifndef CCI_KernelFamilies_h
#define CCI_KernelFamilies_h

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
// File         : CCI_KernelFamilies.h
// Date         : 7/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//--------------------------------------------------------
#define CCIC_KernelFamilies  _TXT("processing/kernelfamilies.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( KernelFamilies, Notify )

  CCI_MODULE_UUID( KernelFamilies, f93728e1-32ce-47fe-ba4f-724cb7ed0797)

  CCI_MODULE_VERSION( KernelFamilies, 1 ) 

  /* Methods */
  CCI_DECL_METHOD_PP( KernelFamilies,1, AddKernels , _in CONST char_t* file )
  CCI_DECL_METHOD_NP( KernelFamilies,2, LoadConfig )
  CCI_DECL_METHOD_PP( KernelFamilies,3, KernelInfo , 
         _in  CONST char_t* family 
         _in  int index 
         _out int x 
         _out int y 
         _out int width 
         _out int height 
         _out CONST int* kernel 
  )
  CCI_DECL_METHOD_PP( KernelFamilies,4, GetFamilies, _in CONST char_t* set _out CONST char_t* data )
  CCI_DECL_METHOD_NP( KernelFamilies,5, GetInstance  )
  CCI_DECL_METHOD_PP( KernelFamilies,6, FindFamily , _in CONST char_t* name _out dmKernelFamily* family ) 

  /* Attributs */
  CCI_DECL_ATTR(KernelFamilies,1,Initialized, dm_bool, [.SGN] )


CCI_END_MODULE_DECL( KernelFamilies )

#endif // __doxygen

//----------------------------------------------------------------
#ifdef __cplusplus
#include "daim_modules/CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Kernel manager
 *
 *  Handle kernels for filtering and mathematical morphology.
 */
//------------------------------------------------------------------------------------
class dmIKernelFamilies : public dmINotify
{  
  public:  
   dmDECLARE_GENERIC_IINTERFACE(KernelFamilies)

   //-------------------------------------------------------
   /*! \brief Return the kernel family from its name       */
   //-------------------------------------------------------
   dmKernelFamily* FindFamily( const char_t* name ) { 
     IF_CCI_IMETHOD_RP(KernelFamilies,FindFamily, _in name _out(family) NULL) 
     THEN return CCI_RETVAL(family);
     ELSE return NULL;
     ENDIF
   }

   //-------------------------------------------------------
   /*! \brief Initialize the kernel manager.
    *
    * This function can be called at any time. When called for the
    * first time, the kernel configuration files will be parsed and stored
    * in memory for fast access with the FindFamily() method.
    */
   //-------------------------------------------------------
   bool LoadKernels() {
     if(!IsValid()) {
       dmIModuleClass _class(dmIKernelFamilies::CCI_CLSID());
       return (_class.IsValid() && this->QueryInterface(
               reinterpret_cast<cci_Object*>(
                 CCI_InvokeService_NP(_class,KernelFamilies,GetInstance))
              ));
     } 
     return true;
   }

   bool LoadConfig() { CCI_RETURN_IMETHOD_NP_OK(KernelFamilies,LoadConfig) }

   bool Initialized() { 
     return CCI_IGET(KernelFamilies,Initialized,dm_false) == dm_true;
   }

};
//------------------------------------------------------------------------------------
#endif // __cplusplus


#include "daim_modules/CCI_Epilog.h"


#endif /* CCI_KernelFamilies_h */
