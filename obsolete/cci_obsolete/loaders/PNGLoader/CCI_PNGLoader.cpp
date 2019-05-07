
/* :::BEGIN LICENSE BLOCK:::
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
 *  :::END LICENSE BLOCK::: */

/* $ChangeLog  v1.4, 28 nov. 2006 19:39:23 , David
 *
 * Handle alpha channel with optionnal background color.
 * Bug fix : preferences are handled correctly.
 */ 

#define dmUseCCI
#define dmUseUtilitiesExtra
#include "daim_utilities.h"
#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_Loader.h"
#include "daim_modules/CCI_LoaderFactory.h"
#include "daim_modules/CCI_StringList.h"
#include "daim_modules/CCI_Properties.h"
#include "daim_modules/CCI_InputStream.h"
#include "daim_modules/CCI_OutputStream.h"
#include "daim_modules/CCI_FileInputStream.h"
#include "daim_modules/CCI_FileOutputStream.h"

#include "daim/daim_interfaces.h"
#include "daim/daim_base.h"

#include <errno.h>
#include "png.h"

#define CCIC_PNGLoader _TXT("loaders/pngldr.cci")
#define CCI_PNGLoader_Version 1L

#define CCIC_NAME  CCIC_PNGLoader
#define CCIC_SUPER CCIC_Loader
#define CCIC_SUPER_VERSION CCI_Loader_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_PNGLoader_Version
#define CCI_REVISION  4
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME _TXT("(Using libpng" PNG_LIBPNG_VER_STRING ")" ),AUTHOR,_TXT("(C) 2004"))

//-------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"
#include "daim_modules/CCI_Interface.h"
//-----------------------------------------

CCI_DECL_DATA( PNGLoader )
{
  dmIInputStream  InputStream;
  dmIOutputStream OutputStream;

  EPixelFormat  PixelFormat;
  dm_param      Status;
  dm_param      Mode;

  dm_param      Flags;
  cci_Object*   StringList;

  png_structp   png_ptr;
  png_infop     info_ptr;
  png_uint_32   width;
  png_uint_32   height;
  int           bit_depth;
  int           color_type;
  int           interlace_type;

  dmRGBColor    bckgnd_color;

  dm_uint  encodeFilter;
  dm_bool  encodeSwap16b;
  dm_bool  decodeSwap16b;
  
  dm_bool  expandPalette;
};

typedef CCI_INSTANCE_PTR(PNGLoader) _Instance_Ptr;

#define THIS_FILE  _This->File

//--------------------------------------------------------------------
// Helpers
//--------------------------------------------------------------------
#include "PNGLoader.cpp"
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( PNGLoader )
{
  CCI_CONSTRUCT_SUPER()
  CCI_INSTANCE( PNGLoader )

  memset(_This,0,sizeof(*_This));

  _This->encodeFilter = static_cast<dm_uint>(PNG_FILTER_DEFAULT);

  CCI_INIT_MEMBER(InputStream);
  CCI_INIT_MEMBER(OutputStream);
  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( PNGLoader )
{
  CCI_INSTANCE( PNGLoader )

  PNGLoader_Clear(_This);

  if(_This->StringList != NULL)
     CCI_Release(_This->StringList);
  
  _This->OutputStream.Release();
  _This->InputStream.Release();

  CCI_DESTROY_MEMBER(OutputStream);
  CCI_DESTROY_MEMBER(InputStream);
  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( PNGLoader )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( PNGLoader )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// CCIM_Loader_Close
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( Loader, Close )
{
  CCI_INSTANCE( PNGLoader )

  PNGLoader_Clear(_This);

  if(_This->OutputStream.IsValid())
     _This->OutputStream.Close();

  if(_This->InputStream.IsValid())
     _This->InputStream.Close();

  if(_This->StringList != NULL)
     CCI_Release(_This->StringList);

  _This->StringList = NULL;

  CCI_RETURN_INVOKE_SUPER()
} 
//---------------------------------------------------------------------
// Method:  Infos
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Loader, Infos )
{
  CCI_INSTANCE( PNGLoader )
 
  if( CCI_P(me)==CCI_ME_GET )
  {
    if(CCI_P(ioStream))
    {
      if(!dmReassignInterface(_This->InputStream,CCI_P(ioStream))) 
         CCI_RETURN_INVALID_PARAM()
    }
    
    if(!_This->InputStream.IsValid())
        CCI_RETURN_INVALID_PARAM()

    PNGLoader_Clear( _This );

    CCI_Try

    //_This->File = CCI_P(name);
    if(CCI_SUCCEEDED(PNGLoader_CheckIfPNG(_This)) &&
       CCI_SUCCEEDED(PNGLoader_InitReadStruct(_This,PNG_BYTES_TO_CHECK)) &&
       CCI_SUCCEEDED(PNGLoader_ReadHDR(_This)) &&
       CCI_SUCCEEDED(PNGLoader_GetFormat(_This)) )
    {            
      // Return information values
      CCI_RETVAL_P(count)  = 1;        // number of records  
      CCI_RETVAL_P(format) = _This->PixelFormat;
      CCI_RETVAL_P(width ) = _This->width;
      CCI_RETVAL_P(height) = _This->height;
      // TODO set others informations    
    }

    CCI_Catch(Loader,Infos)

    if(CCI_SUCCEEDED(_This->Status)) 
       CCI_RETURN_OK()

    PNGLoader_Clear( _This );
    dmLOG(_TXT("ERROR:PNG Failed to read  image !\n"));    

  }
  else 

  if( CCI_P(me)==CCI_ME_SET )
  {
    if(CCI_P(ioStream))
    {
      if(!dmReassignInterface(_This->OutputStream,CCI_P(ioStream))) 
         CCI_RETURN_INVALID_PARAM()
    }
    
    if(!_This->OutputStream.IsValid())
        CCI_RETURN_INVALID_PARAM()

    PNGLoader_Clear( _This );

    _This->PixelFormat = CCI_P(format);
    _This->Flags       = CCI_P(flags);

    _This->width       = CCI_P(width);
    _This->height      = CCI_P(height);

    CCI_Try

    if(CCI_SUCCEEDED(PNGLoader_SetFormat(_This)))
      PNGLoader_InitWriteStruct(_This);
    else
      dmLOG(_TXT("ERROR:PNG Invalid pixel format !\n"));    

    CCI_Catch(Loader,Infos)

    if(CCI_SUCCEEDED(_This->Status)) 
       CCI_RETURN_OK()

    PNGLoader_Clear( _This );
    dmLOG(_TXT("ERROR:PNG Failed to write image !\n"));    
  }
 
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method:  Data
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Loader, Data )
{
  CCI_INSTANCE( PNGLoader )

  {
    // Create text data
    int num_text = 0;
    png_textp text_ptr = PNGLoader_CreateText(_This,num_text);

    if(_This->PixelFormat != CCI_P(format)  ||
       _This->width       != CCI_P(width)   ||
       _This->height      != CCI_P(height)) 
    {
       dmLOG(_TXT("ERROR:PNG Invalid buffer format !\n"));
       CCI_RETURN_INVALID_PARAM()
    }

    CCI_Try

    if(CCI_P(me)==CCI_ME_GET)
    {
      if(!_This->InputStream.IsValid()) 
         CCI_RETURN_INVALID_OBJECT()
         
      PNGLoader_ReadData(_This,CCI_P(scan0),CCI_P(stride)); 
    } 
    else if(CCI_P(me)==CCI_ME_SET) 
    {
       if(!_This->OutputStream.IsValid()) 
          CCI_RETURN_INVALID_OBJECT()
          
       PNGLoader_WriteData(_This,CCI_P(scan0),CCI_P(stride),text_ptr,num_text); 
    }
    else
       _This->Status = CCI_ERR_INVALID_PARAM;

    CCI_Catch(Loader,Data)

    if(text_ptr)
      delete [] text_ptr;

    if(CCI_SUCCEEDED(_This->Status)) 
       CCI_RETURN_OK()   

  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method:  GetMimeTypes
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Loader, GetMimeTypes )
{
  dmIStringList _Strings;
  if(_Strings.QueryInterface(CCI_P(stringlist)))
  {
    _Strings.AddTail(_TXT("image/png"));
    CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method:  SetStringList
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Loader, SetStringList )
{
  dmIStringList _Strings;
  if(_Strings.QueryInterface(CCI_P(stringlist)))
  {
    CCI_INSTANCE( PNGLoader )

    cci_Object* _oldstr = _This->StringList;

    _Strings.AddRef();
    _This->StringList = _Strings;

    if(_oldstr != NULL)
      CCI_Release(_oldstr);
   
    CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method:  IsSupportedFormat
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Loader, IsSupportedFormat )
{
   switch(CCI_P(pixelformat)) {
     case dmPixelFormat8bppIndexed    : 
     case dmPixelFormat16bppGrayScale :
     case dmPixelFormat24bppRGB       :
      CCI_RETURN_OK()
  
     default: break;
   }
   CCI_RETURN_NOT_IMPLEMENTED()
}
//---------------------------------------------------------------------
// Method:  Initialize
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Loader, Initialize )
{
  CCI_INSTANCE( PNGLoader )

  if(CCI_P(properties)!=NULL) 
  {
    dmIProperties _IProps;
    if(!_IProps.QueryInterface(CCI_P(properties)))
      CCI_RETURN_INVALID_PARAM()

    PNGLoader_LoadProperties(_This,CCI_P(ioflags),_IProps);
  } else {
    dmIInterface<dmIProperties> _IProps;
    if(_IProps.Open(_TXT("daim://loaders/pngldr"),_TXT("image/png"))) 
      PNGLoader_LoadProperties(_This,CCI_P(ioflags),_IProps);

    CCI_RETURN_OK();
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( PNGLoader )
  CCI_REGISTER_METHOD_NP( Loader, Close )
  CCI_REGISTER_METHOD_PP( Loader, Infos )
  CCI_REGISTER_METHOD_PP( Loader, Data  )
  CCI_REGISTER_METHOD_PP( Loader, GetMimeTypes  )
  CCI_REGISTER_METHOD_PP( Loader, SetStringList )
  CCI_REGISTER_METHOD_PP( Loader, IsSupportedFormat )
  CCI_REGISTER_METHOD_PP( Loader, Initialize )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( PNGLoader )
{
  //DM_INIT_API()
  CCI_INVOKE_FACTORY(PNGLoader);
}
//---------------------------------------------------------------------
