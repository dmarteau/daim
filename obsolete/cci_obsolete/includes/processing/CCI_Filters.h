#ifndef CCI_Filters_h
#define CCI_Filters_h

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
// File         : CCI_Filters.h
// Date         : 7/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//
// THIS FILE IS SUBJECT TO CHANGE WITHOUT NOTICE !!
//--------------------------------------------------------

#define CCIC_Filters _TXT("processing/filters.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( Filters, Notify )

  CCI_MODULE_UUID( Filters, ad4670b9-9031-4c15-ab4c-b285299522c9)

  CCI_MODULE_VERSION( Filters, 1 ) 

  /* Methods */
  CCI_DECL_METHOD_PP(Filters,1,  ApplyKernel, 
       _in dm_cci(Parameters) params
       _in CONST char_t*      kernel
       _in dm_int             iter
       _in dm_int             oper
  )

  CCI_DECL_METHOD_PP(Filters,2,  MaskSize, 
       _in dm_uint width 
       _in dm_uint height
       _in dm_uint flag
  ) 

  CCI_DECL_METHOD_PP(Filters,3,  MaskRef, 
       _in dm_uint ox 
       _in dm_uint oy 
       _in dm_uint flag
  ) 

  CCI_DECL_METHOD_PP(Filters,4,  MFilter, 
     _in dm_cci(Parameters) params
     _in dm_uint            type 
  )

  CCI_DECL_METHOD_PP(Filters,5,  RFilter, 
     _in dm_cci(Parameters) params
     _in dm_uint            rank 
  )

  CCI_DECL_METHOD_PP(Filters,6,  LFilter, 
     _in dm_cci(Parameters)      params
     _in dm_array(dm_real,count) coeffs 
     _in dm_uint                 count 
  )


  /* Attributs */
  CCI_DECL_ATTR(Filters,1,  MaskLength , dm_uint  , [..G.] )

  /* special values */

  #define CCIV_Filters_Min    1L
  #define CCIV_Filters_Max    2L
  #define CCIV_Filters_Middle 3L
  #define CCIV_Filters_Mean   4L
  #define CCIV_Filters_Median 5L

CCI_END_MODULE_DECL( Filters )

#endif //__doxygen

//----------------------------------------------------------------
#ifdef __cplusplus

#include "daim_modules/CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Apply linear filters on image.
 */
//------------------------------------------------------------------------------------
class dmIFilters : public dmINotify
{ 
  public:
   dmDECLARE_GENERIC_IINTERFACE(Filters)

   //-------------------------------------------------------
   /*! \brief Apply a convolution operator
    *
    * \param params parameter object ( see dmIParameters ) 
    * \param kernel the kernel family string id. 
    * \param iter the Number of iteration.
    * \param oper an arithmetic operation to perform between the original
    * image and the result of the filter (see \ref daim_operations for details)
    */
   //-------------------------------------------------------
   bool ApplyKernel( dm_cci(Parameters) params, CONST char_t* kernel, int iter, int oper ) {
     CCI_RETURN_IMETHOD_PP_OK(Filters,ApplyKernel,
                             _in params
                             _in kernel 
                             _in iter 
                             _in oper )
   }

   //-------------------------------------------------------
   /*! \brief Set the size of the rectangular filter mask  */
   //-------------------------------------------------------
   bool SetMaskSize ( dm_uint width, dm_uint height ) {
    CCI_RETURN_IMETHOD_PP_OK(Filters,MaskSize, _in width _in height _in CCI_ME_SET)
   }

   //-------------------------------------------------------
   /*! \brief Set the location of the central pixel for the 
    *         filter mask  */
   //-------------------------------------------------------
   bool SetMaskRef ( dm_uint ox, dm_uint oy ) {
     CCI_RETURN_IMETHOD_PP_OK(Filters,MaskRef, _in ox _in oy _in CCI_ME_SET)
   }

   //-------------------------------------------------------
   /*! \brief Apply a filter \a type using the rectangular mask
    *  defined by  SetMaskSize() and SetMaskRef()
    *   
    *  \a type can be set to the following values:
    *   - \c CCIV_Filters_Min    for a \<min\>    filter
    *   - \c CCIV_Filters_Max    for a \<max\>    filter
    *   - \c CCIV_Filters_Middle for a \<middle\> filter
    *   - \c CCIV_Filters_Mean   for a \<mean\>   filter
    *   - \c CCIV_Filters_Median for a \<median\> filter
    */
   //-------------------------------------------------------
   bool MFilter( dm_cci(Parameters) params, dm_uint type ) {
     CCI_RETURN_IMETHOD_PP_OK(Filters,MFilter, _in params _in type )
   }

   //-------------------------------------------------------
   /*! \brief Apply a rank filter of rank \a rank using the rectangular mask
    *  defined by  SetMaskSize() and SetMaskRef()
    *
    * Note that \a rank cannot be larger that the number of pixels
    * defined by the size \a N of the the rectangular mask.
    */
   //-------------------------------------------------------
   bool RFilter( dm_cci(Parameters) params, dm_uint rank ) {
     CCI_RETURN_IMETHOD_PP_OK(Filters,RFilter, _in params _in rank )
   }

   //-------------------------------------------------------
   /*! \brief Apply linear 'L' filter using the rectangular mask
    *  defined by  SetMaskSize() and SetMaskRef() and the array
    *  of coefficient \a coeffs.
    *
    * IMPORTANT: \a coeffs must at least as large as the size \a N 
    * of the the rectangular mask.
    *
    * The result of a 'L' filter is a linear combination of the
    * ordered sequence of pixels value with the input coefficents.
    */
   //-------------------------------------------------------
   bool LFilter( dm_cci(Parameters) params, dm_real* coeffs, dm_uint count ) {
     CCI_RETURN_IMETHOD_PP_OK(Filters,LFilter, _in params _in coeffs _in count )
   }
};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_Filters_h.h */
