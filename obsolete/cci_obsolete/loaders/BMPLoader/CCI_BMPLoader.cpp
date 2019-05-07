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

#define CCIC_BMPLoader _TXT("loaders/bmpldr.cci")
#define CCI_BMPLoader_Version 1L

#define CCIC_NAME  CCIC_BMPLoader
#define CCIC_SUPER CCIC_Loader
#define CCIC_SUPER_VERSION CCI_Loader_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_BMPLoader_Version
#define CCI_REVISION  3
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))

//-------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"
#include "daim_modules/CCI_Interface.h"

#include <stdio.h>
#include <errno.h>
//-----------------------------------------
#define DM_UNITS_NONE        0
#define DM_UNITS_METERS      1
#define DM_UNITS_CENTIMETERS 2
#define DM_UNITS_MILLIMETERS 3
#define DM_UNITS_KILOMETERS  4
#define DM_UNITS_INCH        5
//-----------------------------------------

CCI_DECL_DATA( BMPLoader )
{
  dmIInputStream  InputStream;
  dmIOutputStream OutputStream;

  dm_uint       BitsPerPixels;
  dm_uint       Width;
  dm_uint       Height;
  EPixelFormat  PixelFormat;
  dm_int        Status;
  dm_int        Mode;

  // Resolution unit is in pixel/meters (That's bmp !)
  dm_uint       Units;
  dm_uint       XResolution;
  dm_uint       YResolution;

  cci_Object*   StringList;

};

typedef CCI_INSTANCE_PTR(BMPLoader) _Instance_Ptr;

#define THIS_FILE  _This->File
//--------------------------------------------------------------------
// Helpers
//--------------------------------------------------------------------
#include "BMPLoader.cpp"
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( BMPLoader )
{
  CCI_CONSTRUCT_SUPER()
  CCI_INSTANCE( BMPLoader )

  memset(_This,0,sizeof(*_This));

  CCI_INIT_MEMBER(OutputStream);
  CCI_INIT_MEMBER(InputStream);

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( BMPLoader )
{
  CCI_INSTANCE( BMPLoader )

  BMPLoader_Clear(_This);

  if(_This->StringList != NULL)
     CCI_Release(_This->StringList);

  CCI_DESTROY_MEMBER(OutputStream);
  CCI_DESTROY_MEMBER(InputStream);

  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( BMPLoader )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( BMPLoader )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// CCIM_Loader_Close
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( Loader, Close )
{
  CCI_INSTANCE( BMPLoader )

  BMPLoader_Clear(_This);

  if(_This->StringList != NULL)
     CCI_Release(_This->StringList);

  _This->StringList = NULL;

  if(_This->OutputStream.IsValid())
     _This->OutputStream.Close();

  if(_This->InputStream.IsValid())
     _This->InputStream.Close();
     
  CCI_RETURN_INVOKE_SUPER()
} 
//---------------------------------------------------------------------
// Method:  Infos
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Loader, Infos )
{
  CCI_INSTANCE( BMPLoader )
 
  if( CCI_P(me)==CCI_ME_GET )
  {
    if(CCI_P(ioStream))
    {
      if(!dmReassignInterface(_This->InputStream,CCI_P(ioStream))) 
         CCI_RETURN_INVALID_PARAM()
    }
    
    if(!_This->InputStream.IsValid())
        CCI_RETURN_INVALID_PARAM()
        
    BMPLoader_Clear( _This );
    BMPLoader_LoadProperties( _This, CCI_ME_GET );

    //CCI_Try

    if(CCI_SUCCEEDED(BMPLoader_InitReadStruct(_This)) &&
       CCI_SUCCEEDED(BMPLoader_ReadHDR(_This)) &&
       CCI_SUCCEEDED(BMPLoader_GetFormat(_This)) )
    {     
      // Return information values
      CCI_RETVAL_P(format) = _This->PixelFormat;
      CCI_RETVAL_P(width ) = _This->Width; 
      CCI_RETVAL_P(height) = _This->Height;

      CCI_RETVAL_P(count) = 1;        // number of records
       
      //

      // TODO set others informations
       
    }

    //CCI_Catch(Loader,Infos)

    if(CCI_SUCCEEDED(_This->Status)) 
       CCI_RETURN_OK()

    BMPLoader_Clear( _This );
    dmLOG(_TXT("ERROR:BMP Failed to read image !\n"));    

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
    
    BMPLoader_Clear( _This );
    BMPLoader_LoadProperties( _This, CCI_ME_SET );

    _This->PixelFormat = CCI_P(format);
    _This->Width       = CCI_P(width);
    _This->Height      = CCI_P(height);

    //CCI_Try

    if(CCI_SUCCEEDED(BMPLoader_SetFormat(_This))) 
    {
      BMPLoader_InitWriteStruct(_This);
      BMPLoader_CreateText(_This);
    } else
      dmLOG(_TXT("ERROR:BMP Invalid pixel format !\n"));    

    //CCI_Catch(Loader,Infos)

    if(CCI_SUCCEEDED(_This->Status)) 
       CCI_RETURN_OK()

    BMPLoader_Clear( _This );
    dmLOG(_TXT("ERROR:BMP Failed to write image !\n"));    
  }
 
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method:  Data
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Loader, Data )
{
  CCI_INSTANCE( BMPLoader )

  if(_This->PixelFormat != CCI_P(format)  ||
     _This->Width       != CCI_P(width)   ||
     _This->Height      != CCI_P(height)) 
  {
     dmLOG(_TXT("ERROR:BMP: Invalid buffer format !\n"));
     CCI_RETURN_INVALID_PARAM()
  }

  //CCI_Try

  if(CCI_P(me)==CCI_ME_GET) 
  {
    if(!_This->InputStream.IsValid()) 
       CCI_RETURN_INVALID_OBJECT()
    
    BMPLoader_ReadData(_This,CCI_P(scan0),CCI_P(stride)); 
  } 
  else if(CCI_P(me)==CCI_ME_SET) 
  { 
    if(!_This->OutputStream.IsValid()) 
       CCI_RETURN_INVALID_OBJECT()
    
    BMPLoader_WriteData(_This,CCI_P(scan0),CCI_P(stride)); 
  } else
     _This->Status = CCI_ERR_INVALID_PARAM;

  //CCI_Catch(Loader,Data)

  if(CCI_SUCCEEDED(_This->Status)) 
     CCI_RETURN_OK()   

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
    _Strings.AddTail(_TXT("image/bmp"));
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
    CCI_INSTANCE( BMPLoader )

    if(_This->StringList != NULL)
      CCI_Release(_This->StringList);

    _Strings.AddRef();
    _This->StringList = _Strings;
    
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
     case dmPixelFormat24bppRGB       :
      CCI_RETURN_OK()

     default: break;
   }
   CCI_RETURN_NOT_IMPLEMENTED()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( BMPLoader )
  CCI_REGISTER_METHOD_NP( Loader, Close )
  CCI_REGISTER_METHOD_PP( Loader, Infos )
  CCI_REGISTER_METHOD_PP( Loader, Data  )
  CCI_REGISTER_METHOD_PP( Loader, GetMimeTypes  )
  CCI_REGISTER_METHOD_PP( Loader, SetStringList )
  CCI_REGISTER_METHOD_PP( Loader, IsSupportedFormat )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( BMPLoader )
{
  //DM_INIT_API()
  CCI_INVOKE_FACTORY(BMPLoader);
}
//---------------------------------------------------------------------
