#ifndef CCI_Transforms_h
#define CCI_Transforms_h

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
// File         : CCI_Transforms.h
// Date         : 8/2005
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//--------------------------------------------------------
#define CCIC_Transforms _TXT("processing/transforms.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( Transforms, Notify )

  CCI_MODULE_UUID( Transforms, 4468bab2-3773-4a60-b604-c96305b69d63)

  CCI_MODULE_VERSION( Transforms, 2 ) 

  /* Methods */
  CCI_DECL_METHOD_PP(Transforms,1,  Stretch, 
       _in dm_cci(Parameters) params
       _in dm_uint            index 
       _in dm_int             mode
  )

  CCI_DECL_METHOD_PP(Transforms,2,  Rotate, 
       _in dm_cci(Parameters) params
       _in dm_uint            index 
       _in dm_real            angle
  )

  CCI_DECL_METHOD_PP(Transforms,3,  Crop, 
       _in dm_cci(Parameters) params
       _in dm_uint            index 
       _in dm_int             left
       _in dm_int             right
       _in dm_int             top
       _in dm_int             bottom
  )

  CCI_DECL_METHOD_PP(Transforms,4,  Motif, 
       _in dm_cci(Parameters) params
       _in dm_uint            index 
       _in dm_int             left
       _in dm_int             right
       _in dm_int             top
       _in dm_int             bottom
       _in dm_bool            periodic
  )

  CCI_DECL_METHOD_PP(Transforms,5,  Flip, 
       _in dm_cci(Parameters) params
       _in dm_uint            index 
       _in dm_uint            way
  )

  CCI_DECL_METHOD_PP(Transforms,6,  Rotate90, 
       _in dm_cci(Parameters) params
       _in dm_uint            index 
       _in dm_bool            clockwise
  )


  #define CCIV_Stretch_Decimate 1
  #define CCIV_Stretch_Bilinear 2
  #define CCIV_Stretch_Bicubic  3
 
CCI_END_MODULE_DECL( Transforms )


#endif //__doxygen

//----------------------------------------------------------------
#ifdef __cplusplus

#include "daim_modules/CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Scale and rotate transformations
 */
//------------------------------------------------------------------------------------
class dmITransforms : public dmINotify
{ 
  public:
   dmDECLARE_GENERIC_IINTERFACE(Transforms)

   //-------------------------------------------------------
   /*! \brief Apply a linear stretch on image.               
    * 
    * \param params parameter object ( see dmIParameters ) 
    * \param index  index to store the transformed image in the dmIImageList
    *               held by the parameter object.
    * \param mode   the type of stretching.
    *
    * If the parameter object has no image associated, the image buffer will be used.
    * The operation will fail is the image does not have the same pixel type.
    */
   //-------------------------------------------------------
   bool Strech( dm_cci(Parameters) params, dm_int index, dm_int mode ) {
     CCI_RETURN_IMETHOD_PP_OK(Transforms,Stretch, _in params _in index _in mode )
   }

   //-------------------------------------------------------
   /*! \brief Apply a rotation  on image.               
    * 
    * \param params parameter object ( see dmIParameters ) 
    * \param index  index to store the transformed image in the dmIImageList
    *               held by the parameter object.
    * \param angle  the angle in radians.
    *
    * The operation will fail is the image does not have the same pixel type.
    */
   //-------------------------------------------------------
   bool Rotate( dm_cci(Parameters) params, dm_int index, dm_real angle ) {
     CCI_RETURN_IMETHOD_PP_OK(Transforms,Rotate, _in params _in index _in angle )
   }
   
   //-------------------------------------------------------
   /*! \brief Crop image.               
    * 
    * \param params parameter object ( see dmIParameters ) 
    * \param index  index to store the transformed image in the dmIImageList
    *               held by the parameter object.
    * \param left   left offset in pixels.
    * \param right  rightoffset in pixels.
    * \param top    top offset in pixels.
    * \param bottom bottom offset in pixels.
    */
   //-------------------------------------------------------
   bool Crop( dm_cci(Parameters) params, dm_int index,
              dm_int left,dm_int right,
              dm_int top, dm_int bottom ) {
     CCI_RETURN_IMETHOD_PP_OK(Transforms,Crop, _in params _in index 
                  _in left 
                  _in right 
                  _in top 
                  _in bottom )
   }
   
   //-------------------------------------------------------
   /*! \brief Create a motif repeated over the image.               
    * 
    * his function create a regular or periodic representation of
    * the original rectangular area of the operand
    * 
    * \note : The position of the original area is left 
    *        unchanged in the image. 
    * 
    * \param params parameter object ( see dmIParameters ) 
    * \param left   left position of test rectangle
    * \param right  right position of source rectangle
    * \param top    top position of source rectangle   
    * \param bottom bottom position of source rectangle
    * \param periodic if set to dm_true then a periodic pattern is created
    */
   //-------------------------------------------------------
   bool Motif( dm_cci(Parameters) params,
               dm_int left,dm_int right,
               dm_int top, dm_int bottom,
               dm_bool periodic ) {
     CCI_RETURN_IMETHOD_PP_OK(Transforms,Motif, _in params 
                  _in left 
                  _in right 
                  _in top 
                  _in bottom
                  _in periodic )
   }

   //-------------------------------------------------------
   /*! \brief Flip image horizontally or vertically.               
    * 
    * \param params parameter object ( see dmIParameters ) 
    * \param index  index to store the transformed image
    * \param way    Horizontal flip = 1, Vertical flip = 2  
    */
   //-------------------------------------------------------
   bool Flip( dm_cci(Parameters) params, dm_uint index, dm_uint way ) {
     CCI_RETURN_IMETHOD_PP_OK(Transforms,Flip, _in params _in index _in way )
   }

   //-------------------------------------------------------
   /*! \brief Rotate image 90 degres Clockwise (or Counter clockwise)
    * 
    * \param params parameter object ( see dmIParameters ) 
    * \param index  index to store the transformed image
    * \param way    if true rotate clockwise 
    */
   //-------------------------------------------------------
   bool Rotate90( dm_cci(Parameters) params, dm_uint index, dm_bool clockwise ) {
     CCI_RETURN_IMETHOD_PP_OK(Transforms,Rotate90, _in params _in index _in clockwise )
   }

};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_Transforms_h */
