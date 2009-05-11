#ifndef CCI_ImageHandle_h
#define CCI_ImageHandle_h

/* ::: BEGIN LICENSE BLOCK:::
 *
 * Copyright (c) 2006 David Marteau
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
// File         : CCI_ImageHandle.h
// Date         : 12/2006
// Author       : David Marteau
//--------------------------------------------------------
#define CCIC_ImageHandle  _TXT("imagehandle.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( ImageHandle, Notify )
  
  CCI_MODULE_UUID( ImageHandle, 63ed4f84-7862-4d86-8e64-ba47fc51d469)

  CCI_MODULE_VERSION( ImageHandle, 1 ) 

  /* Methods */
  CCI_DECL_METHOD_PP(ImageHandle,1,  Init, 
      _in dm_cci(Parameters) params
  )

  CCI_DECL_METHOD_PP(ImageHandle,2,  GetImageData, 
      _out dm_int  format 
      _out dm_uint width  
      _out dm_uint height 
      _out dm_int  stride 
      _out dm_ptr  scan0
  )

  #ifdef DAIM_UTILITIES
  /* Use shared link for efficiency */
  CCI_DECL_METHOD_PP(ImageHandle,3,  GetImageA, 
     _in dm_param         me
     _in dm_uint          index 
     _in dmLink<dmImage>* link
  )
  #endif // DAIM_UTILITIES

  /* Attributs */
  CCI_DECL_ATTR(ImageHandle, 1,  Width      , dm_uint ,[..G.] )
  CCI_DECL_ATTR(ImageHandle, 2,  Height     , dm_uint ,[..G.] )
  CCI_DECL_ATTR(ImageHandle, 4,  Bands      , dm_uint ,[..G.] )
  CCI_DECL_ATTR(ImageHandle, 5,  Format     , dm_uint ,[..G.] )

CCI_END_MODULE_DECL( ImageHandle )

#endif // __doxygen
//----------------------------------------------------------------

#ifdef __cplusplus

#include "CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Handle to image buffer
 */
//------------------------------------------------------------------------------------
class dmIImageHandle : public dmINotify
{    
  public:
   dmDECLARE_GENERIC_IINTERFACE(ImageHandle)

   bool Init( dm_cci(Parameters) params ) {
      CCI_RETURN_IMETHOD_PP_OK(ImageHandle,Init,_in params )
   }

   //--------------------------------------------------------------
   /*! \brief Get image pixels data
    *  \param data  A dmImageData object that will hold pixels buffer infos. 
    *                      
    *  \note Note that the buffer is valid as long as the handle exists
    */ 
   //--------------------------------------------------------------
   bool GetImageData( dmImageData& data ) {
     IF_CCI_IMETHOD_RP(ImageHandle,GetImageData,_out(format) 0 _out(width) 0 _out(height) 0 _out(stride) 0 _out(scan0) 0 )
     THEN
       data.Width       = CCI_RETVAL(width);
       data.Height      = CCI_RETVAL(height);
       data.Stride      = CCI_RETVAL(stride);
       data.Scan0       = CCI_RETVAL(scan0); 
       data.PixelFormat = static_cast<EPixelFormat>(CCI_RETVAL(format));
     ENDIF_RETURN_BOOL()
   }

   //--------------------------------------------------------------
   /*! \brief Return the width of the image */
   //--------------------------------------------------------------
   dm_uint GetWidth() { return CCI_IGET(ImageHandle,Width,(dm_uint)0); }
   
   //--------------------------------------------------------------
   /*! \brief Return the height of the image */
   //--------------------------------------------------------------
   dm_uint GetHeight() { return CCI_IGET(ImageHandle,Height,(dm_uint)0); }

   //--------------------------------------------------------------
   /*! \brief Return the number of band the image */
   //--------------------------------------------------------------
   dm_uint GetBands() { return CCI_IGET(ImageHandle,Bands,(dm_uint)0); }

   //--------------------------------------------------------------
   /*! \brief Return the format of the image */
   //--------------------------------------------------------------
   dm_uint GetFormat() { return CCI_IGET(ImageHandle,Format,(dm_uint)0); }

   
   #ifdef DAIM_UTILITIES

   bool GetImageA(  dm_uint index, dmLink<dmImage>& link ) {
     CCI_RETURN_IMETHOD_PP_OK(ImageHandle,GetImageA  ,_in CCI_ME_GET  _in index _in &link ) 
   }

   #endif // DAIM_UTILITIES

};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_ImageList.h */
