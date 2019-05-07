#ifndef CCI_Pyramid_h
#define CCI_Pyramid_h

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
// File         : CCI_Pyramid.h
// Date         : 11/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//--------------------------------------------------------
#define CCIC_Pyramid _TXT("processing/pyramid.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( Pyramid, Notify )

  CCI_MODULE_UUID( Pyramid, b1f1b06b-96ac-44c3-bb1d-4fdeb386764d)

  CCI_MODULE_VERSION( Pyramid, 1 ) 

  /* Methods */
  CCI_DECL_METHOD_PP(Pyramid,1, Generate, 
       _in  dm_cci(Parameters) params 
       _in  dm_uint            limit
  )

  CCI_DECL_METHOD_PP(Pyramid,2, DownSample , 
     _in dm_cci(Parameters) params
     _in dm_uint            index
  )

  CCI_DECL_METHOD_PP(Pyramid,3, Reconstruct,
     _in dm_cci(Parameters) params
     _in dm_uint            from 
     _in dm_uint            to
     _in dm_cci(ImageList)  imagelist 
  )

  CCI_DECL_METHOD_PP(Pyramid,4, Laplacian, 
       _in  dm_cci(Parameters) params 
       _in  dm_uint            limit
  )

  /* Attributs */
  CCI_DECL_ATTR(Pyramid,1, Kernel , char_t*,    [.SGN] )
  CCI_DECL_ATTR(Pyramid,2, Factor , dm_uint,    [.SGN] )
 
CCI_END_MODULE_DECL( Pyramid )

#endif //__doxygen

//----------------------------------------------------------------
#ifdef __cplusplus

#include "daim_modules/CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Build a pyramid by downsampling with a convolution kernel
 *
 */
//------------------------------------------------------------------------------------
class dmIPyramid : public dmINotify
{ 
  public:
   dmDECLARE_GENERIC_IINTERFACE(Pyramid)

   //-------------------------------------------------------
   /*! \brief Construct the pyramid from source image in buffer 0.
    *
    * Pyramid images will be stored into the buffer list of the \a params
    * object starting from index 1. 
    * \sa dmIParameters for details about the \a params interface.
    */
   //-------------------------------------------------------
   bool Generate( dm_cci(Parameters) params, dm_uint limit = 0 ) { 
     CCI_RETURN_IMETHOD_PP_OK(Pyramid,Generate,_in params _in limit )
   }

   //-------------------------------------------------------
   /*! \brief Reconstruct an image from buffer index \a from
    *  to buffer index \a to.
    * 
    * The reconstruction will be applied \a from-\a to times 
    * on the source image.
    * If \a to \>= \a from : the source image is copied into 
    * the output buffer at index \a to.
    * \param params    an instance of dmIParameters class.  
    * \param imagelist the imagelist in which to store the result 
    *                  If imagelist is null the image will be stored
    *                  in the image member of the \a params object
    *                  or in the \param buffer if there is no image. 
    * \param from      the buffer index of the imagelist handled
                       by the \a params object.
    * \param to        the buffer index of the result image.
    * \param imagelist the imagelist in which result is stored.
    *
    * \sa dmIParameters for details about the \a params interface.
    */
   //------------------------------------------------------- 
   bool Reconstruct( dm_cci(Parameters) params, dm_uint from, dm_uint to,
                     dm_cci(ImageList)  imagelist )
   { 
      CCI_RETURN_IMETHOD_PP_OK(Pyramid,Reconstruct,
                         _in params
                         _in from 
                         _in to
                         _in imagelist ) 
   }

   //-------------------------------------------------------
   /*! \brief Construct a down sampling of the image parameter
    *
    * the resulting image will be stored int the
    * image buffer \a index  of the \a params object.
    * \sa dmIParameters for details about the \a params interface.
    */
   //-------------------------------------------------------
   bool DownSample( dm_cci(Parameters) params, dm_uint index ) { 
     CCI_RETURN_IMETHOD_PP_OK(Pyramid,DownSample,_in params _in index )
   }

    //-------------------------------------------------------
   /*! \brief Construct the Laplacien pyramid the from source image 
    *   in buffer 0. 
    */
   //-------------------------------------------------------
   bool Laplacian( dm_cci(Parameters) params, dm_uint limit = 0 ) { 
     CCI_RETURN_IMETHOD_PP_OK(Pyramid,Laplacian, _in params _in limit )
   }

  //-------------------------------------------------------------------
  /*! \brief Set the convolution kernel to be used for the construction 
   *         of the pyramid, if the kernel is set to a empty string then
   *         downsampling is done without filtering
   */
  //-------------------------------------------------------------------  
  bool          SetKernel( const char_t* kernel ) { return CCI_ISET(Pyramid,Kernel,kernel); }
  const char_t* GetKernel() { return CCI_IGET(Pyramid,Kernel,(char_t*)NULL); }

  bool          SetFactor( dm_uint factor ) { return CCI_ISET(Pyramid,Factor,factor); }
  const dm_uint GetFactor() { return CCI_IGET(Pyramid,Factor,(dm_uint)0); }

};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_Pyramid_h */
