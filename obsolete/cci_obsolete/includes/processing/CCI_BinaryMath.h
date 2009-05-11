#ifndef CCI_BinaryMath_h
#define CCI_BinaryMath_h

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
// File         : CCI_BinaryMath.h
// Date         : 7/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//--------------------------------------------------------

#define CCIC_BinaryMath _TXT("processing/binarymath.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( BinaryMath, Notify )

  CCI_MODULE_UUID( BinaryMath, 85a57cc1-c7a2-4463-a393-9221e7b05398)

  CCI_MODULE_VERSION( BinaryMath, 3 )

  /* Methods   */
  CCI_DECL_METHOD_PP(BinaryMath,1,  ApplyKernel,
      _in dm_cci(Parameters) params
      _in CONST char_t*      kernel
      _in dm_int             action
      _in dm_int             iter
      _in dm_int             operation
      _in dm_uint            conditional
  )

  CCI_DECL_METHOD_PP(BinaryMath,2,  DoSeparation    , _in dm_cci(Parameters) params _in CONST char_t* kernel _in dm_uint seeds )
  CCI_DECL_METHOD_PP(BinaryMath,3,  DistanceFunc    , _in dm_cci(Parameters) params _in CONST char_t* kernel _in dm_uint index )
  CCI_DECL_METHOD_PP(BinaryMath,4,  UltimateErosion , _in dm_cci(Parameters) params _in CONST char_t* kernel )
  CCI_DECL_METHOD_PP(BinaryMath,5,  UltimateDilation, _in dm_cci(Parameters) params _in CONST char_t* kernel _in dm_uint conditional )
  CCI_DECL_METHOD_PP(BinaryMath,6,  LocalMax        , _in dm_cci(Parameters) params )
  CCI_DECL_METHOD_PP(BinaryMath,7,  Reconstruct     , _in dm_cci(Parameters) params  _in dm_int  connect  _in dm_uint seeds)
  CCI_DECL_METHOD_PP(BinaryMath,8,  Watershed       , _in dm_cci(Parameters) params  _in dm_uint seeds )

  /* Attributs */

CCI_END_MODULE_DECL( BinaryMath )

#endif // __doxygen

//----------------------------------------------------------------
#ifdef __cplusplus

#include "daim_modules/CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Apply morphological operation on binary masks.
 */
//------------------------------------------------------------------------------------
class dmIBinaryMath : public dmINotify
{
  public:
   dmDECLARE_GENERIC_IINTERFACE(BinaryMath)

   //-------------------------------------------------------
   /*! \brief Apply a morphogical transformation.
    *
    * \param params parameter object ( see dmIParameters )
    * \param kernel the kernel family string id.
    * \param action Type of operation ( see daim_operations )
    * \param iter Number of iteration, 0 is synonmous for idempotence
    * \param operation Arithmetic operation to perform
    * \param conditional Use a conditional mask, this mask must be located
    * in the buffer 0 of the Mask buffers accessible through the \a params object
    *
    * Note that applying a dilation-like operation will require a condtional mask.
    * (otherwise idempotence would not be reached).
    *
    * If conditional is required and no valid buffer is found then the \a roi member
    * of the parameter object will be used.
    *
    */
   //-------------------------------------------------------
   bool ApplyKernel(
      dm_cci(Parameters) params,
      CONST char_t*      kernel,
      dm_int             action,
      dm_int             iter,
      dm_int             operation,
      dm_uint            conditional)
   {
     CCI_RETURN_IMETHOD_PP_OK(BinaryMath,ApplyKernel,
       _in params
       _in kernel
       _in action
       _in iter
       _in operation
       _in conditional
     )
   }

   //-------------------------------------------------------
   /*! \brief Separate objects in a binary mask.
    *
    * \param params parameter object ( see dmIParameters )
    * \param kernel the kernel family string id.
    * \param seeds  Use a seeds mask, this mask must be located
    * in the buffer 0 of the Mask buffers accessible through the \a params object.
    *
    * If seeds are required and no valid buffer is found then the method
    * will fail.
    *
    */
   //-------------------------------------------------------
   bool DoSeparation( dm_cci(Parameters) params, CONST char_t* kernel, dm_uint seeds ) {
     CCI_RETURN_IMETHOD_PP_OK(BinaryMath,DoSeparation, _in params _in kernel _in seeds )
   }

   //-------------------------------------------------------
   /*! \brief Compute the distance function for a binary mask.
    *
    * \param params parameter object ( see dmIParameters )
    * \param kernel the kernel family string id.
    * \param index  Index of image buffer where the resulting
    *               16 bit grayscale image will be stored.
    */
   //-------------------------------------------------------
   bool DistanceFunc( dm_cci(Parameters) params, CONST char_t* kernel, dm_uint index ) {
     CCI_RETURN_IMETHOD_PP_OK(BinaryMath,DistanceFunc, _in params _in kernel _in index )
   }

   //-------------------------------------------------------
   /*! \brief Compute kernel-based ultimate erosion of binary mask.
    *
    * \param params parameter object ( see dmIParameters )
    * \param kernel the kernel family string id.
    */
   //-------------------------------------------------------
   bool UltimateErosion( dm_cci(Parameters) params, CONST char_t* kernel ) {
     CCI_RETURN_IMETHOD_PP_OK(BinaryMath,UltimateErosion, _in params _in kernel )
   }

   //-------------------------------------------------------
   /*! \brief Compute kernel-based ultimate dilation.
    *
    * \param params parameter object ( see dmIParameters )
    * \param kernel the kernel family string id.
    * \param conditional Use a conditional mask from mask buffer
    *
    * Note that applying a dilation-like operation will require a condtional mask.
    * (otherwise idempotence would not be reached).
    *
    * If conditional is required and no valid buffer is found then the \a roi member
    * of the parameter object will be used.
    * If roi is invalid then the method will fail.
    *
    */
   //-------------------------------------------------------
   bool UltimateDilation( dm_cci(Parameters) params, CONST char_t* kernel, dm_uint conditional ) {
     CCI_RETURN_IMETHOD_PP_OK(BinaryMath,UltimateDilation, _in params _in kernel _in conditional )
   }

   //-------------------------------------------------------
   /*! \brief Create a binary mask representing local max of image.
    *
    * \param params parameter object ( see dmIParameters )
    */
   //-------------------------------------------------------
   bool LocalMax( dm_cci(Parameters) params ) {
     CCI_RETURN_IMETHOD_PP_OK(BinaryMath,LocalMax, _in params )
   }

   //-------------------------------------------------------
   /*! \brief Create a binary mask by reconstruction.
    *
    * \param params  parameter object ( see dmIParameters )
    * \param connect connectivity used in the reconstruction algorithm.
    *
    * The reconstruction algorithm require seeds that must be located
    * in the buffer 0 of the Mask buffers accessible through the \a params object.
    */
   //-------------------------------------------------------
   bool Reconstruct( dm_cci(Parameters) params, dm_int connect, dm_uint seeds ) {
     CCI_RETURN_IMETHOD_PP_OK(BinaryMath,Reconstruct, _in params _in connect _in seeds )
   }

   //-------------------------------------------------------
   /*! \brief Simple watershed implementation             */
   //-------------------------------------------------------
   bool Watershed( dm_cci(Parameters) params, dm_uint seeds ) {
     CCI_RETURN_IMETHOD_PP_OK(BinaryMath,Watershed, _in params _in seeds )
   }

};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif // CCI_BinaryMath_h
