#ifndef CCI_Loader_h
#define CCI_Loader_h

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
// File         : CCI_Loader.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------
#include "daim_modules/CCI_Prolog.h"
#define CCIC_Loader _TXT("loader.cci")

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( Loader, Notify )
  
  CCI_MODULE_UUID( Loader, 2c1ce023-e9c5-46b3-a270-608a50ed2b5f)

  CCI_MODULE_VERSION( Loader, 2 ) 

  /* Methods */

  CCI_DECL_METHOD_NP(Loader,1,  Close )
  CCI_DECL_METHOD_PP(Loader,2,  Infos,
     _in dm_param       me
     _in dm_cci(Notify) ioStream  /* Input/Output Stream         */
     _in EPixelFormat   format    /* pixel type format id        */
     _in dm_uint        index     /* index of the current record */
     _in dm_uint        count     /* number of records           */
     _in dm_uint        width     /* width  of image             */
     _in dm_uint        height    /* height of image             */
     _in CONST char_t*  units     /* units name                  */
     _in dm_real        ar        /* pixel aspect ratio          */
     _in dm_real        uppx      /* units per pixels            */
     _in dm_uint        flags
  ) 

  CCI_DECL_METHOD_PP(Loader,3,  Data,
      _in  dm_param     me
      _in  EPixelFormat format 
      _in  dm_uint      width 
      _in  dm_uint      height 
      _in  dm_int       stride 
      _in  dm_ptr       scan0 
  )

  CCI_DECL_METHOD_PP(Loader,4, GetMimeTypes, 
       _in dm_cci(StringList)  stringlist
       _in dm_uint             flags 
  )

  CCI_DECL_METHOD_PP(Loader,5, SetStringList, 
       _in dm_cci(StringList) stringlist
  )

  CCI_DECL_METHOD_PP(Loader,6, IsSupportedFormat, 
       _in EPixelFormat pixelformat
  )

  CCI_DECL_METHOD_PP(Loader,7,  Initialize, 
       _in dm_cci(Properties)   properties 
       _in CONST char_t*        mime
       _in dm_uint              ioflags
  )

  CCI_DECL_METHOD_PP(Loader,8,Infos2,
     _in dm_param             me
     _in dm_cci(Notify)       ioStream   /* Input/Output Stream */
     _in dm_uint              flags
     _in dm_cci(PropertyList) properties 
     _in dm_param             key
     _in EPixelFormat         format     /* pixel type format id        */
     _in dm_uint              index      /* index of the current record */
     _in dm_uint              count      /* number of records           */
     _in dm_uint              width      /* width  of image             */
     _in dm_uint              height     /* height of image             */
  ) 

  /* Attributs */
  CCI_DECL_ATTR(Loader,1, HasStreamSupport , dm_bool , [..G.] )

  /* Special values */
  #define CCIV_Loader_Read   0x1L
  #define CCIV_Loader_Write  0x2L

  #define CCIV_Loader_TEXT          0x01
  #define CCIV_Loader_NOCOMPRESSION 0x02

CCI_END_MODULE_DECL( Loader )

#endif // __doxygen

//----------------------------------------------------------------
#ifdef __cplusplus

#include "CCI_Interface.h"

//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Daim loader base interface
 *
 *  Base interface for all daim loader implementations
 */
//------------------------------------------------------------------------------------

class dmILoader : public dmINotify
{ 
  public:
    dmDECLARE_GENERIC_IINTERFACE(Loader)

    enum Constants {
      fl_DECODE = CCIV_Loader_Read ,
      fl_ENCODE = CCIV_Loader_Write,
      fl_
    };

    //-------------------------------------------------------
    /*! Return true if this image format is supported (natively ?) by the loader
     */
    //-------------------------------------------------------   
    bool IsSupportedFormat( EPixelFormat pixelformat ) {
      CCI_RETURN_IMETHOD_PP_OK(Loader,IsSupportedFormat, _in pixelformat )
    } 

    //-------------------------------------------------------
    /*! Set the stringlist in which meta informations will be stored/retrieved
     */
    //-------------------------------------------------------
    bool SetStringList( dm_cci(StringList) stringlist ) {
      CCI_RETURN_IMETHOD_PP_OK(Loader,SetStringList, _in stringlist )
    } 

    //-------------------------------------------------------
    /*! Return the list of all mime types handled by this loader
     */
    //-------------------------------------------------------   
    bool GetMimeTypes( dm_cci(StringList) stringlist, dm_uint flags ) {
      CCI_RETURN_IMETHOD_PP_OK(Loader,GetMimeTypes, _in stringlist _in flags )
    } 

    //-------------------------------------------------------
    /*! Get Image informations - must be call before calling GetImageData() 
     */
    //-------------------------------------------------------   
    bool GetImageInfo( dm_cci(InputStream) inputStream, dm_uint flags, 
           EPixelFormat& format,
           dm_uint&      index ,
           dm_uint&      count ,
           dm_uint&      width ,
           dm_uint&      height,
           dm_cci(Properties) properties = dm_null,
           dm_param           key        = 0 )
    {
      IF_CCI_IMETHOD_RP(Loader,Infos2,_in CCI_ME_GET _in inputStream _in flags _in properties _in key )
      THEN
       CCI_RETURN_P(format)
       CCI_RETURN_P(index)
       CCI_RETURN_P(count)
       CCI_RETURN_P(width) 
       CCI_RETURN_P(height)
      ENDIF_RETURN_BOOL()
    }
    
    //-------------------------------------------------------
    /*! Set Image informations prior to call SetImageData() 
     */
    //-------------------------------------------------------   
    bool SetImageInfo( dm_cci(OutputStream) outputStream, dm_uint flags, 
           EPixelFormat format,
           dm_uint      index ,
           dm_uint      count ,
           dm_uint      width ,
           dm_uint      height,
           dm_cci(Properties) properties = dm_null, 
           dm_param           key        = 0 )
   {
     CCI_RETURN_IMETHOD_PP_OK(Loader,Infos2,_in CCI_ME_SET
       _in outputStream
       _in flags
       _in properties
       _in key
       _in format
       _in index
       _in count
       _in width
       _in height
     ) 
   }

   //-------------------------------------------------------
   /*! Get Image data from buffer 
    *  Informations about image size and format should have been obtained
    *  from the  GetImageInfo() method and a dmImageData must hold 
    *  valid memory pointer and data
    *  /note It is the responsability of the Loader to map the data onto the specified
    *  buffer: i.e 1 bit wide pixel will be mapped onto 8 bits buffer pixels
    */ 
   //--------------------------------------------------------------
   bool GetImageData( dmImageData& data ) 
   {
     CCI_RETURN_IMETHOD_PP_OK(Loader,Data,_in CCI_ME_GET
                _in data.PixelFormat 
                _in data.Width  
                _in data.Height 
                _in data.Stride 
                _in data.Scan0  
     )      
   }
   
   //-------------------------------------------------------
   /*! Set Image data to buffer 
    *  Informations about image size and format should have been set
    *  with the  SetImageInfo() method and a dmImageData must hold 
    *  valid memory pointer and data
    */
   //--------------------------------------------------------------   
   bool SetImageData( const dmImageData& data ) 
   {
     CCI_RETURN_IMETHOD_PP_OK(Loader,Data,_in CCI_ME_SET
                _in data.PixelFormat 
                _in data.Width  
                _in data.Height 
                _in data.Stride 
                _in data.Scan0  
     )
   }

   //-------------------------------------------------------
   /*! Close loader and free resources
    */
   //-------------------------------------------------------
   bool Close() { CCI_RETURN_IMETHOD_NP_OK(Loader,Close) }

   //-------------------------------------------------------
   /*! Set Image data to buffer 
    *  Informations about image size and format should have been set
    *  with the  SetImageInfo() method and a dmImageData must hold 
    *  valid memory pointer and data
    */
   //--------------------------------------------------------------   
   bool Initialize( dm_cci(Properties) properties, const char_t* mime,  dm_uint ioflags ) { 
     CCI_RETURN_IMETHOD_PP_OK(Loader,Initialize, _in properties _in mime _in ioflags ) 
   }

   //-------------------------------------------------------
   /*! Return true if the loader can support generic io stream.
    *  If not, the component should use only file I/O streams
    */
   //-------------------------------------------------------   
   bool HasStreamSupport() { 
     return CCI_IGET(Loader,HasStreamSupport,(dm_bool)dm_false)==dm_true; 
   }
};

//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

//-------------------------------------------------------------
#endif // CCI_Loader_h
