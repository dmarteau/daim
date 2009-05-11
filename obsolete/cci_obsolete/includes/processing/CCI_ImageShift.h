#ifndef CCI_ImageShift_h
#define CCI_ImageShift_h

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
// File         : CCI_ImageShift.h
// Date         : 11/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//--------------------------------------------------------

#define CCIC_ImageShift _TXT("processing/imageshift.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( ImageShift, Notify )

  CCI_MODULE_UUID( ImageShift, f08654a9-acef-4072-9038-779dafc6d33c)

  CCI_MODULE_VERSION( ImageShift, 1 ) 

  /* Methods */
  CCI_DECL_METHOD_PP(ImageShift,1, Compute, 
       _in dm_cci(Parameters) params 
       _in dm_int             x
       _in dm_int             y
       _in dm_uint            index
       _in dm_uint            flags
  )

  CCI_DECL_METHOD_PP(ImageShift,2, CropImages,
       _in dm_cci(Parameters) params
       _in dm_cci(ImageList)  imagelist
       _in dm_rect*           testarea 
       _in dm_uint            index
       _in dm_uint            flags
  )
  /* Attributs */

  /* Special values */
  #define CCIV_ImageShift_ROI 1L
  
CCI_END_MODULE_DECL( ImageShift )

#endif //__doxygen

//----------------------------------------------------------------
#ifdef __cplusplus

#include "daim_modules/CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Compute shift between related images using correlations.
 *
 */
//------------------------------------------------------------------------------------
class dmIImageShift : public dmINotify
{ 
  public:
   dmDECLARE_GENERIC_IINTERFACE(ImageShift)

   enum Constants {
     flROI = CCIV_ImageShift_ROI,
   };

   //-------------------------------------------------------
   /*! \brief Compute shift                
    * 
    * Compute the shift between the image parameter and the buffer 
    * \a index. The rectangular area given by the ROI parameter will
    * be used as the test area. \a p is the top left coordinates 
    * of the test area for the image in buffer. Shift vector will be returned
    * in \a point.
    */
   //-------------------------------------------------------
   bool Compute( dm_cci(Parameters) params, 
                 dm_point& p,
                 dm_uint   index,
                 dm_uint   flags = 0 )
   { 
     IF_CCI_IMETHOD_RP(ImageShift,Compute, _in params 
                        _out(x) p.x _out(y) p.y _in index _in flags )
     THEN
       p.x = CCI_RETVAL(x);
       p.y = CCI_RETVAL(y);
     ENDIF_RETURN_BOOL()
   }

   //-------------------------------------------------------
   /*! \brief Crop images relatively to the computed image shift
    *         from a reference image.                
    *
    * This method will crop all images in \a  imagelist so as 
    * to match the reference image determined by the \a index 
    * parameter value. 
    * \param params     a dmIParameters object. 
    * \param imagelist  the imagelist to process (the whole list is processed)
    * \param testarea   the rectangle wich define the test area for the correlation
    *                   If null then the test will be located at center
    *                   of the reference image.
    * \param index      the buffer index that designate the referenceimage from 
    *                   the imagelist held by the \a param object.
    *                   If set to -1, the image is taken out from the default image 
    *                   held by the \a params object.
    * \param flags      flags options that modify processing
    *
    * \note The reference image can be an image from the \a imagelist parameter.
    * \sa dmIParameters for details about the \a params interface.
    */
   //-------------------------------------------------------
   bool CropImages( dm_cci(Parameters) params, 
                    dm_cci(ImageList) imagelist, 
                    dm_rect*    testarea,
                    dm_uint     index = -1,
                    dm_uint     flags = 0 )
   {
     CCI_RETURN_IMETHOD_PP_OK(ImageShift, CropImages,
          _in params
          _in imagelist
          _in testarea 
          _in index
          _in flags)
   }
   
};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_ImageShift_h */
