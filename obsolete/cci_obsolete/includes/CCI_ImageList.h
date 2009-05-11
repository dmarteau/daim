#ifndef CCI_ImageList_h
#define CCI_ImageList_h

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
// File         : CCI_ImageList.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------
#define CCIC_ImageList  _TXT("imagelist.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( ImageList, Notify )
  
  CCI_MODULE_UUID( ImageList, 203c8de1-1559-4ee6-b66b-dc180a44e3f7)

  CCI_MODULE_VERSION( ImageList, 1 ) 

  /* Methods */
  CCI_DECL_METHOD_PP(ImageList, 1,  StoreBuffer, 
     _in dm_uint         index 
     _in CONST dmImage*  image  
     _in CONST dm_rect*  rect  
     _in dm_int          format 
  ) 

  CCI_DECL_METHOD_PP(ImageList, 5,  IsEmpty        , _in dm_uint index _out dm_bool empty )
  CCI_DECL_METHOD_PP(ImageList, 6,  ReleaseBuffer  , _in dm_uint index )
  CCI_DECL_METHOD_PP(ImageList, 7,  GetBuffer      , _in dm_uint index _out dmImage* image )
  CCI_DECL_METHOD_PP(ImageList, 8,  Resize         , _in dm_uint size  )
  CCI_DECL_METHOD_PP(ImageList, 9,  CreateBuffer   , 
    _in dm_uint index
    _in dm_int  format 
    _in dm_uint width 
    _in dm_uint height
  )

  CCI_DECL_METHOD_NP(ImageList,10,  ReleaseAll )
  CCI_DECL_METHOD_PP(ImageList,11,  MoveBuffer     , _in dm_uint from  _in dm_uint to ) 
  CCI_DECL_METHOD_PP(ImageList,12,  SwapBuffers    , _in dm_uint first _in dm_uint second ) 
  CCI_DECL_METHOD_PP(ImageList,13,  CloneBuffer    , _in dm_uint src   _in dm_uint dst ) 

  CCI_DECL_METHOD_PP(ImageList,14,  SetBufferFormat, 
    _in dm_uint  index 
    _in dm_int   format   
  ) 

  CCI_DECL_METHOD_PP(ImageList,20,  GetBufferFormat, 
    _in  dm_uint  index 
    _out dm_int   format   
  ) 

  CCI_DECL_METHOD_PP(ImageList,15,  SpliceBuffers, 
    _in dm_uint           to
    _in dm_uint           from
    _in dm_cci(ImageList) source 
    _in dm_uint           count
  ) 

  CCI_DECL_METHOD_PP(ImageList,16,  CopyBuffers, 
    _in dm_uint           to
    _in dm_uint           from
    _in dm_cci(ImageList) source 
    _in dm_uint           count
  ) 

  CCI_DECL_METHOD_PP(ImageList,19, SetCapacity,
    _in dm_uint capacity
  )

  CCI_DECL_METHOD_PP(ImageList,21,  GetBufferData,
      _in  dm_uint index
      _out dm_int  format 
      _out dm_uint width  
      _out dm_uint height 
      _out dm_int  stride 
      _out dm_ptr  scan0
  )

  CCI_DECL_METHOD_PP(ImageList,22,  SetBufferData,
      _in dm_uint index
      _in dm_int  format 
      _in dm_uint width 
      _in dm_uint height 
      _in dm_int  stride 
      _in dm_ptr  scan0 
  )

  /* Attributs */
  CCI_DECL_ATTR(ImageList, 1,  Size , dm_uint ,[..G.] )

  #ifdef DAIM_UTILITIES
  /* Use shared link for efficiency */
  CCI_DECL_METHOD_PP(ImageList,23,  GetBufferA, 
     _in dm_param         me
     _in dm_uint          index 
     _in dmLink<dmImage>* link
  )
  #endif // DAIM_UTILITIES


CCI_END_MODULE_DECL( ImageList )

#endif // __doxygen
//----------------------------------------------------------------

#ifdef __cplusplus

#include "CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Maintain a array of images.
 * 
 *  This interface expose methods for a container for storing/retrieving
 *  indexed images. Indexation enable the ordering of the images as
 *  a sequence.
 */
//------------------------------------------------------------------------------------
class dmIImageList : public dmINotify
{    
  public:
   dmDECLARE_GENERIC_IINTERFACE(ImageList)

   //--------------------------------------------------------------
   /*! \brief Store a \e copy of the image at buffer index \a index. */
   //--------------------------------------------------------------
   bool StoreBuffer( dm_uint index, const dmImage* image, 
                     const dm_rect* rect = dm_null, dm_int format = 0) 
   { 
      CCI_RETURN_IMETHOD_PP_OK(ImageList,StoreBuffer, 
       _in index 
       _in image 
       _in rect 
       _in format
     ) 
   }

   //--------------------------------------------------------------
   /*! \brief Check if buffer \a index contains a valid image. */
   //--------------------------------------------------------------
   bool IsEmpty( dm_uint index ) { 
     IF_CCI_IMETHOD_RP(ImageList,IsEmpty, _in index _out(empty) dm_true )
     THEN return CCI_RETVAL(empty)==dm_true;
     ELSE return false;
     ENDIF
   }

   //--------------------------------------------------------------
   /*! \brief Delete the image located at buffer \a index. */
   //--------------------------------------------------------------
   bool ReleaseBuffer( dm_uint index ) { 
     CCI_RETURN_IMETHOD_PP_OK(ImageList,ReleaseBuffer, _in index ) 
   }

   //--------------------------------------------------------------
   /*! \brief Retrieve the image at buffer \a index. */
   //--------------------------------------------------------------
   dmImage* GetBuffer( dm_uint index ) { 
     IF_CCI_IMETHOD_RP(ImageList,GetBuffer, _in index _out(image) NULL )
     THEN return CCI_RETVAL(image);
     ELSE return NULL;
     ENDIF
   }

   //--------------------------------------------------------------
   /*! \brief Create a new image in buffer \a index */
   //--------------------------------------------------------------
   bool CreateBuffer( dm_uint index, dm_int format, dm_uint width, dm_uint height ) { 
     CCI_RETURN_IMETHOD_PP_OK(ImageList,CreateBuffer,
              _in index _in format _in width _in height) 
   }

   //--------------------------------------------------------------
   /*! \brief Set the size of the array. */
   //--------------------------------------------------------------
   bool Resize( dm_uint size ) { 
     CCI_RETURN_IMETHOD_PP_OK(ImageList,Resize, _in size ) 
   }

   //--------------------------------------------------------------
   /*! \brief Release all the images */
   //--------------------------------------------------------------
   bool ReleaseAll() { CCI_RETURN_IMETHOD_NP_OK(ImageList,ReleaseAll) }
   bool Clear()      { return ReleaseAll(); }

   //--------------------------------------------------------------
   /*! \brief Move the buffer from location \a from to location \a to. */
   //--------------------------------------------------------------
   bool MoveBuffer( dm_uint from, dm_uint to ) { 
     CCI_RETURN_IMETHOD_PP_OK(ImageList,MoveBuffer, _in from _in to )
   }

   //--------------------------------------------------------------
   /*! \brief Swap images between buffer \a first and buffer \a second. */
   //--------------------------------------------------------------
   bool SwapBuffers( dm_uint first, dm_uint second ) { 
     CCI_RETURN_IMETHOD_PP_OK(ImageList,SwapBuffers, _in first _in second )
   }

   //--------------------------------------------------------------
   /*! \brief Copy images at \a src to buffer \a dst. */
   //--------------------------------------------------------------
   bool CloneBuffer( dm_uint src, dm_uint dst ) { 
     CCI_RETURN_IMETHOD_PP_OK(ImageList, CloneBuffer, _in src _in dst );
   } 

   //--------------------------------------------------------------
   /*! \brief Change the format of the buffer \a index
    *
    * If \a index is equal to -1,  all images in the list
    * are converted to the given format. 
    */
   //--------------------------------------------------------------
   bool SetBufferFormat( dm_uint index,  dm_int format ) { 
     CCI_RETURN_IMETHOD_PP_OK(ImageList,SetBufferFormat, _in index _in format )
   }

   //--------------------------------------------------------------
   /*! \brief Retrieve the format of the image stored at \a index */
   //--------------------------------------------------------------
   dm_int GetBufferFormat( dm_uint index ) { 
     IF_CCI_IMETHOD_RP(ImageList,GetBufferFormat, _in index _out(format) 0 )
     THEN return CCI_RETVAL(format);
     ELSE return dmPixelFormatUndefined;
     ENDIF
   }

   //------------------------------------------------------------------
   /*! \brief Move \a count images from index \a from to \a destination
    *         starting at index \a to 
    */
   //--------------------------------------------------------------
   bool SpliceBuffers( dm_uint to, dm_uint from, dm_cci(ImageList) source, dm_uint count ) 
   { 
      CCI_RETURN_IMETHOD_PP_OK(ImageList,SpliceBuffers, 
                 _in to
                 _in from
                 _in source 
                 _in count
      ) 
   }

   //------------------------------------------------------------------
   /*! \brief Copy \a count images from index \a from to \a destination
    *         starting at index \a to 
    */
   //--------------------------------------------------------------
   bool CopyBuffers( dm_uint to, dm_uint from, dm_cci(ImageList) source, dm_uint count ) 
   { 
      CCI_RETURN_IMETHOD_PP_OK(ImageList,CopyBuffers, 
                 _in to
                 _in from
                 _in source 
                 _in count
      ) 
   }

   //--------------------------------------------------------------
   /*! \brief Return the size of the image list */
   //--------------------------------------------------------------
   dm_uint Size() { return CCI_IGET(ImageList,Size,(dm_uint)0 ); }


   //--------------------------------------------------------------
   /*! \brief Reserve a number of buffer slots in the list  */
   //--------------------------------------------------------------
   bool SetCapacity( dm_uint capacity ) {
     CCI_RETURN_IMETHOD_PP_OK(ImageList, SetCapacity, _in capacity )
   }

   //--------------------------------------------------------------
   /*! \brief Copy pixels data at buffer \a index
    *  \param data  A dmImageData object that holds pixels buffer infos. 
    *  \param index Buffer index
    */ 
   //--------------------------------------------------------------
   bool SetBufferData( const dmImageData& data , dm_uint index ) {
     CCI_RETURN_IMETHOD_PP_OK(ImageList,SetBufferData, 
       _in index
       _in static_cast<dm_int>(data.PixelFormat)
       _in data.Width 
       _in data.Height 
       _in data.Stride 
       _in data.Scan0 
     )
   }

   //--------------------------------------------------------------
   /*! \brief Get image pixels data from buffer \a index
    *  \param data  A dmImageData object that holds pixels buffer infos. 
    *  \param index Buffer index
    *  \note The returned data may be temporary.
    */ 
   //--------------------------------------------------------------
   bool GetBufferData( dmImageData& data, dm_uint index ) {
     IF_CCI_IMETHOD_RP(ImageList,GetBufferData,_in index _out(format) 0 _out(width) 0 _out(height) 0 _out(stride) 0 _out(scan0) 0 )
     THEN
       data.Width       = CCI_RETVAL(width);
       data.Height      = CCI_RETVAL(height);
       data.Stride      = CCI_RETVAL(stride);
       data.Scan0       = CCI_RETVAL(scan0); 
       data.PixelFormat = static_cast<EPixelFormat>(CCI_RETVAL(format));
     ENDIF_RETURN_BOOL()
   }

   #ifdef DAIM_UTILITIES

   bool GetBufferA(  dm_uint index, dmLink<dmImage>& link ) {
     CCI_RETURN_IMETHOD_PP_OK(ImageList,GetBufferA  ,_in CCI_ME_GET  _in index _in &link ) 
   }

   bool SetBufferA(  dm_uint index, dmLink<dmImage>& link ) {
     CCI_RETURN_IMETHOD_PP_OK(ImageList,GetBufferA  ,_in CCI_ME_SET  _in index _in &link ) 
   }

   #endif // DAIM_UTILITIES

};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_ImageList.h */
