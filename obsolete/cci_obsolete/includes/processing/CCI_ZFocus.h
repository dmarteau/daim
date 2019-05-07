#ifndef CCI_ZFocus_h
#define CCI_ZFocus_h

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
// File         : CCI_ZFocus.h
// Date         : 11/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//--------------------------------------------------------

#define CCIC_ZFocus _TXT("processing/zfocus.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( ZFocus, Notify )

  CCI_MODULE_UUID( ZFocus, b05905f9-4b1c-4502-ad07-c4fbf638420f)

  CCI_MODULE_VERSION( ZFocus, 1 ) 

  /* Methods */
  CCI_DECL_METHOD_PP(ZFocus,1, Initialize, 
         _in dm_uint flags
         _in dm_uint levels
  ) 

  CCI_DECL_METHOD_PP(ZFocus,2, Process, 
       _in dm_cci(Parameters) params 
       _in dm_cci(ImageList)  imagelist
       _in dm_uint            index
       _in dm_uint            flags
       _in dm_uint            mapindex
  )

  CCI_DECL_METHOD_PP(ZFocus,3, Reconstruct, 
       _in dm_cci(Parameters) params
       _in dm_uint            index
  )

  CCI_DECL_METHOD_NP(ZFocus,4, Reset )

  CCI_DECL_METHOD_PP(ZFocus,5, GetImage, 
      _in dm_uint   channel 
      _out dmImage* image 
  )

  CCI_DECL_METHOD_PP(ZFocus,6, GetResList, 
      _in  dm_uint           channel 
      _out dm_cci(ImageList) imagelist 
  )
  
  /* Attributs */
  CCI_DECL_ATTR(ZFocus,1, Kernel     , char_t*,           [.SGN] )
  CCI_DECL_ATTR(ZFocus,2, Count      , dm_uint,           [..G.] )
  CCI_DECL_ATTR(ZFocus,3, Options    , dm_uint,           [.SGN] )
  CCI_DECL_ATTR(ZFocus,4, Levels     , dm_uint,           [..G.] )
  CCI_DECL_ATTR(ZFocus,5, ColorSpace , dm_uint,           [..G.] )
  CCI_DECL_ATTR(ZFocus,6, MapList    , dm_cci(ImageList), [..G.] )
  CCI_DECL_ATTR(ZFocus,7, ResList    , dm_cci(ImageList), [..G.] )

  #define CCIV_ZFocus_DefaultsLevels -1L

  /* Initialize flags */
  #define CCIV_ZFocus_ColorSpaceGrey 0x00L
  #define CCIV_ZFocus_ColorSpaceRGB  0x01L

  /* Process flags */
  #define CCIV_ZFocus_UpdateMap  0x01L
  #define CCIV_ZFocus_CopyBack   0x02L
  #define CCIV_ZFocus_ProcessMap 0x04L
  #define CCIV_ZFocus_Finalize   0x08L
 
    
CCI_END_MODULE_DECL( ZFocus )

#endif //__doxygen

//----------------------------------------------------------------
#ifdef __cplusplus

#include "daim_modules/CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Reconstruct an image from list of images using
 *  a Laplacian pyramid fusion/reconstruction algorithm.
 */
//------------------------------------------------------------------------------------
class dmIZFocus : public dmINotify
{ 
  public:
   dmDECLARE_GENERIC_IINTERFACE(ZFocus)

   /*! Flags Constants */
   enum Constants {
     flColorSpaceGrey = CCIV_ZFocus_ColorSpaceGrey, /*!< Convert images to gray scale before processing images */ 
     flCopyBack       = CCIV_ZFocus_CopyBack,       /*!< Copy result image back to the processed image   */ 
     flFinalize       = CCIV_ZFocus_Finalize,       /*!< Reconstruct result image */
     flColorSpaceRGB  = CCIV_ZFocus_ColorSpaceRGB,  /*!< Use RGB decomposition before processing images   */ 
     flProcessMap     = CCIV_ZFocus_ProcessMap,
     flUpdateMap      = CCIV_ZFocus_UpdateMap,
   };

   //-------------------------------------------------------------------
   /*! \brief Initialize the object before processing.
    *
    * \param flags initialization flags, can be set to the following values:
    *         - \c flColorSpaceGrey
    *         - \c flColorSpaceRGB
    * \param levels the number of levels in the pyramid algorithm
    * This method must be called before any new processing.
    */
   //-------------------------------------------------------------------
   bool Initialize( dm_uint flags = 0, dm_uint levels = -1 ) { 
     CCI_RETURN_IMETHOD_PP_OK(ZFocus,Initialize, _in flags _in levels )
   }

   //-------------------------------------------------------
   /*! \brief Process images               
    * 
    * \param params     a dmIParameters object. 
    * \param imagelist  the imagelist to process (the whole list is processed)
    * \param index      the buffer index to get a single image from the imagelist 
    *                   held by the \a param object. If set to -1, the image is
    *                   taken out from the default image held by the \a params object.
    *                   The \a index parameter is ignored on input if \a imagelist
    *                   is not null.  
    * \param flags      options passed that modify processing and output
    *
    * If flags has \c flCopyBack and \c flFinalize set
    * then the result will be stored back into the input image  defined 
    * by the \a index parameter.
    *
    * \sa dmIParameters for details about the \a params interface.
    */
   //-------------------------------------------------------
   bool Process( dm_cci(Parameters) params, 
        dm_cci(ImageList) imagelist,
        dm_uint           index    = -1,
        dm_uint           flags    =  0,
        dm_uint           mapindex = -1 )
   { 
     CCI_RETURN_IMETHOD_PP_OK(ZFocus,Process,
          _in params _in imagelist _in index _in flags 
          _in mapindex )
   }

  //-------------------------------------------------------------------
  /*! \brief Reconstruct an image from the computed pyramid
   *  
   *  \param params a dmIParameters object. 
   *  \param index  the index of the buffer to store the result
   *                if set to -1, the default image held by \a params
   *                will be used if it exists. 
   */
  //-------------------------------------------------------------------  
  bool Reconstruct( dm_cci(Parameters)  params, dm_uint index = -1 ) {
     CCI_RETURN_IMETHOD_PP_OK(ZFocus,Reconstruct, _in params _in index )
  }

  //-------------------------------------------------------------------
  /*! \brief Reset results by clearing accumulated data and resetting
   *         image count to 0. Index mapping will not be modified
   *
   *  This method is called prior to processing images based on the map
   *  previously updated with the \c flUpdateMap process flag.
   */
  //-------------------------------------------------------------------
  bool Reset() { CCI_RETURN_IMETHOD_NP_OK(ZFocus,Reset) }

  //-------------------------------------------------------------------
  /*! \brief Just after Reconstruct, return the reconstructed image. 
   *         Otherwise return the last processed image.
   */
  //-------------------------------------------------------------------  
  dmImage* GetImage( dm_uint channel ) {
     IF_CCI_IMETHOD_RP(ZFocus,GetImage, _in channel _out(image) NULL )
     THEN return CCI_RETVAL(image); ENDIF
     return NULL;
  }

  dm_cci(ImageList) GetMapList() { return CCI_IGET(ZFocus,MapList, (cci_Object*)NULL );  }
  dm_cci(ImageList) GetResList( dm_uint channel ) { 
     IF_CCI_IMETHOD_RP(ZFocus,GetResList, _in channel _out(imagelist) NULL )
     THEN return CCI_RETVAL(imagelist); ENDIF
     return NULL;
  }

  //-------------------------------------------------------------------
  /*! \brief Return the number of images processed */
  //-------------------------------------------------------------------  
  dm_uint GetCount() { return CCI_IGET(ZFocus,Count, (dm_uint)0 );  }
 
};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_ZFocus_h */
