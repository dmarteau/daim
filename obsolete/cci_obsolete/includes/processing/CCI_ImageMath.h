#ifndef CCI_ImageMath_h
#define CCI_ImageMath_h

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
// File         : CCI_ImageMath.h
// Date         : 7/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//
//--------------------------------------------------------

#define CCIC_ImageMath _TXT("processing/imagemath.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( ImageMath, Notify )

  CCI_MODULE_UUID( ImageMath, 2ec8ed7f-3a0f-42be-b55a-0d2d96c48164)

  CCI_MODULE_VERSION( ImageMath, 1 ) 
 
  /* Methods   */
  CCI_DECL_METHOD_PP(ImageMath, 1,  DoImageMath, 
      _in dm_cci(Parameters) params
      _in dm_int             operation 
      _in dm_uint            index 
  )

  CCI_DECL_METHOD_PP(ImageMath, 2, DoImageListMath, 
      _in dm_cci(Parameters) params
      _in dm_int             operation 
      _in dm_cci(ImageList)  imagelist 
  )

  CCI_DECL_METHOD_PP(ImageMath, 3, Accumulate,
      _in dm_cci(Parameters) params
      _in dm_bool            normalize 
  )

  CCI_DECL_METHOD_PP(ImageMath, 4, AddMul,
      _in dm_cci(Parameters) params
      _in dm_real            addval 
      _in dm_real            mulval
  )

  CCI_DECL_METHOD_PP(ImageMath, 5, Blend, 
      _in dm_cci(Parameters) params
      _in dm_real            percent 
      _in dm_uint            index 
      _in dm_int             x
      _in dm_int             y
  )

  CCI_DECL_METHOD_PP(ImageMath, 6, Truncate, 
      _in dm_cci(Parameters) params
      _in dm_uint            index 
      _in EPixelFormat       format 
  )

CCI_END_MODULE_DECL( ImageMath )

#endif // __doxygen
//----------------------------------------------------------------
#ifdef __cplusplus

#include "daim_modules/CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Do arithmetics operation between images.
 */
//------------------------------------------------------------------------------------
class dmIImageMath : public dmINotify
{ 
  public:
   dmDECLARE_GENERIC_IINTERFACE(ImageMath)

   //-------------------------------------------------------
   /*! \brief Perform a arithmetical operation
    *
    * The operation is performed between the lhs image associated
    * to the \a params object and a rhs image stored in buffer. 
    * \param params    the parameters object handle ( see dmIParameters )              
    * \param operation the operation  to be performed 
    *                  ( see \ref daim_operations for details )
    * \param index     the index of the buffer 
    *
    * If needed, the rhs image will be converted into the type of the lhs image.
    * The rhs image is left unchanged.
    * If \a index is set to -1 then the operation is performed between
    * the image associated to the \a params object as rhs operand and all valid images
    * stored in the imagelist passed in argument.
    *  Note that lhs operand always receives the result of the operation.
    *
    */
   //-------------------------------------------------------
   bool DoImageMath( dm_cci(Parameters) params, dm_int operation, dm_uint index ) {
      CCI_RETURN_IMETHOD_PP_OK(ImageMath,DoImageMath, 
                                _in params 
                                _in operation
                                _in index )
   }

   //-------------------------------------------------------
   /*! \brief Accumulate pixels values from imagelist
    *
    * This method will accumulate the pixels of all valid images 
    * in the imagelist held by the \a params object. The result
    * will be stored either in the current image parameter or in
    * the internal buffer if there is no current image available.
    */
   //-------------------------------------------------------
   bool Accumulate( dm_cci(Parameters) params, dm_bool normalize ) {
      CCI_RETURN_IMETHOD_PP_OK(ImageMath,Accumulate, _in params _in normalize )
   }

   //-------------------------------------------------------
   /*! \brief Apply arithmetical operation between two imagelists.
    *
    * This method will apply the given \a operation between  
    * images at index \a i for all buffers in the \a imagelist 
    * parameter and the buffer held by the \a params object.
    * Images in the \a imagelist passed in argument will be considered 
    * as the left hand side of the operation. If there is no valid
    * image in both imagelist for a given index, the first valid image
    * found will be copied into the lhs buffer. If no valid image
    * exists in both buffer then the corresponding buffer will be cleared.
    */
   //-------------------------------------------------------
   bool DoImageListMath( dm_cci(Parameters) params, dm_int operation, 
                         dm_cci(ImageList)  imagelist ) 
   {
      CCI_RETURN_IMETHOD_PP_OK(ImageMath,DoImageListMath, 
                              _in params 
                              _in operation
                              _in imagelist )
   }

   //-------------------------------------------------------
   /*! \brief Add/mutliply by constants */
   //-------------------------------------------------------
   bool AddMul( dm_cci(Parameters) params, dm_real addval, dm_real mulval ) {
      CCI_RETURN_IMETHOD_PP_OK(ImageMath,AddMul, _in params _in addval _in mulval )
   }

   //-------------------------------------------------------
   /*! \brief Blend images
    *
    * The operation copy the region specified by the current mask
    * from the specified buffer to the (\a x,\a y) location in the image held in
    * the \a params object.
    *
    * \param params    the parameters object handle ( see dmIParameters )              
    * \param percent   the value of the blending , must be a value beetween 0 and 1 
    *                  ( see \ref daim_operations for details )
    * \param index     the index of the image buffer 
    *                  if set to -1 then no mask is used.
    * \param x         The x destination of the region to be copied.
    * \param y         The y destination of the region to be copied.
    */
   //-------------------------------------------------------
   bool Blend( dm_cci(Parameters) params, dm_real percent, 
                      dm_uint index, dm_int x, dm_int y ) {
      CCI_RETURN_IMETHOD_PP_OK(ImageMath,Blend, 
                               _in params 
                               _in percent
                               _in index
                               _in x _in y )
   }

   //-------------------------------------------------------
   /*! \brief Offset image by a constants */
   //-------------------------------------------------------
   bool OffsetImage( dm_cci(Parameters) params, dm_real offset ) {
      CCI_RETURN_IMETHOD_PP_OK(ImageMath,AddMul, _in params _in offset _in 1.0 )
   }

   //-------------------------------------------------------
   /*! \brief Convert image by truncating pixels values */
   //-------------------------------------------------------
   bool Truncate( dm_cci(Parameters) params, dm_uint index, EPixelFormat format ) {
     CCI_RETURN_IMETHOD_PP_OK(ImageMath,Truncate, _in params _in index _in format )
   }


};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_ImageMath_h */
