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

#define CCIC_JPEGLoader _TXT("loaders/jpegldr.cci")
#define CCI_JPEGLoader_Version 1L

#define CCIC_NAME  CCIC_JPEGLoader
#define CCIC_SUPER CCIC_Loader
#define CCIC_SUPER_VERSION CCI_Loader_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_JPEGLoader_Version
#define CCI_REVISION  2
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME _TXT("(Using libjpeg)"),AUTHOR,_TXT("(C) 2004"))

//-------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"
#include "daim_modules/CCI_Interface.h"

#include <errno.h>
//-----------------------------------------
// Forward declarations
//-----------------------------------------
CCI_DECL_DATA( JPEGLoader );
typedef CCI_INSTANCE_PTR(JPEGLoader) _Instance_Ptr;
//-----------------------------------------
extern "C" {

#define INT32 jpeg_int32
#undef  FAR

#include "jpeglib.h"

union jpeg_struct {
  jpeg_compress_struct    cinfo;
  jpeg_decompress_struct  dinfo;
};

struct jpeg_err_struct {
  jpeg_error_mgr  jpeg_mngr;
  _Instance_Ptr   instance;
};

}; // extern "C"
//-----------------------------------------
#include <stdio.h>

CCI_DECL_DATA( JPEGLoader )
{
  dmIInputStream  InputStream;
  dmIOutputStream OutputStream;

  jpeg_struct     jpeg;
  jpeg_err_struct jpeg_err;
  size_t          jpegquality;
  J_COLOR_SPACE   jpegoutputformat;

  EPixelFormat  PixelFormat;
  dm_param      Status;
  dm_param      Mode;

  cci_Object*   StringList;
};

//-----------------------------------------

#include "jerror.h"

#define DJPEG_PTR( _This ) (&_This->jpeg.dinfo)
#define CJPEG_PTR( _This ) (&_This->jpeg.cinfo)

#define JINPUTSTREAM  dmIInputStream
#define JOUTPUTSTREAM dmIOutputStream

#define SIZEOF(object)  ((size_t) sizeof(object))

#define JFREAD(input,buf,sizeofbuf)  \
  (input->Read((buf) , (sizeofbuf)))

#define JFWRITE(output,buf,sizeofbuf)  \
  (output->Write((buf),(size_t) (sizeofbuf)))

//--------------------------------------------------------------------
// Helpers
//--------------------------------------------------------------------
#include "JPEGDatasrc.cpp"
#include "JPEGDatadest.cpp"
#include "JPEGLoader.cpp"
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( JPEGLoader )
{
  CCI_CONSTRUCT_SUPER()
  CCI_INSTANCE( JPEGLoader )

  memset(_This,0,sizeof(*_This));

  _This->jpeg_err.instance = _This;
  _This->jpegquality       = 75;

  CCI_INIT_MEMBER(OutputStream);
  CCI_INIT_MEMBER(InputStream);
  
  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( JPEGLoader )
{
  CCI_INSTANCE( JPEGLoader )

  JPEGLoader_Clear(_This);

  if(_This->StringList != NULL)
     CCI_Release(_This->StringList);

  CCI_DESTROY_MEMBER(OutputStream);
  CCI_DESTROY_MEMBER(InputStream);
  
  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( JPEGLoader )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( JPEGLoader )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// CCIM_Loader_Close
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( Loader, Close )
{
  CCI_INSTANCE( JPEGLoader )

  JPEGLoader_Clear(_This);

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
  CCI_INSTANCE( JPEGLoader )
 
  if( CCI_P(me)==CCI_ME_GET )
  {
    if(CCI_P(ioStream))
    {
      if(!dmReassignInterface(_This->InputStream,CCI_P(ioStream))) 
         CCI_RETURN_INVALID_PARAM()
    }
    
    if(!_This->InputStream.IsValid())
        CCI_RETURN_INVALID_PARAM()

    JPEGLoader_Clear( _This );
    //JPEGLoader_LoadProperties( _This, CCI_ME_GET );

    CCI_Try

    if(CCI_SUCCEEDED(JPEGLoader_InitReadStruct(_This)) &&
       CCI_SUCCEEDED(JPEGLoader_ReadHDR(_This))        &&
       CCI_SUCCEEDED(JPEGLoader_GetFormat(_This)))
    {     
      // Return information values
      CCI_RETVAL_P(format) = _This->PixelFormat;
      CCI_RETVAL_P(width ) = DJPEG_PTR(_This)->image_width ; 
      CCI_RETVAL_P(height) = DJPEG_PTR(_This)->image_height;

      CCI_RETVAL_P(count) = 1;        // number of records
       
        // TODO set others informations 
    }

    CCI_Catch(Loader,Infos)

    if(CCI_SUCCEEDED(_This->Status)) 
       CCI_RETURN_OK()

    JPEGLoader_Clear( _This );
    dmLOG(_TXT("ERROR:JPEG Failed to read image !\n"));    

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

    JPEGLoader_Clear( _This );
    JPEGLoader_LoadProperties( _This, CCI_ME_SET );

    _This->PixelFormat = CCI_P(format);

    CCI_Try

    if(CCI_SUCCEEDED(JPEGLoader_SetFormat(_This))) {
      JPEGLoader_InitWriteStruct(_This,CCI_P(width), CCI_P(height));
      JPEGLoader_CreateText(_This);
    } else
      dmLOG(_TXT("ERROR:JPEG Invalid pixel format !\n"));    

    CCI_Catch(Loader,Infos)

    if(CCI_SUCCEEDED(_This->Status)) 
       CCI_RETURN_OK()

    JPEGLoader_Clear( _This );
    dmLOG(_TXT("ERROR:JPEG Failed to write image !\n"));    

  }
 
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method:  Data
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Loader, Data )
{
  CCI_INSTANCE( JPEGLoader )

  if(_This->PixelFormat             != CCI_P(format)  ||
     DJPEG_PTR(_This)->image_width  != CCI_P(width)   ||
     DJPEG_PTR(_This)->image_height != CCI_P(height)) 
  {
     dmLOG(_TXT("ERROR:JPEG: Invalid buffer format !\n"));
     CCI_RETURN_INVALID_PARAM()
  }

  CCI_Try

  if(CCI_P(me)==CCI_ME_GET)  
    JPEGLoader_ReadData(_This,CCI_P(scan0),CCI_P(stride)); 
  else
   if(CCI_P(me)==CCI_ME_SET) 
     JPEGLoader_WriteData(_This,CCI_P(scan0),CCI_P(stride)); 

  else
     _This->Status = CCI_ERR_INVALID_PARAM;

  CCI_Catch(Loader,Data)

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
    _Strings.AddTail(_TXT("image/jpeg"));
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
    CCI_INSTANCE( JPEGLoader )

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
CCI_BEGIN_DISPATCH_MAP( JPEGLoader )
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
CCI_ENTRY_POINT( JPEGLoader )
{
  //DM_INIT_API()
  CCI_INVOKE_FACTORY(JPEGLoader);
}
//---------------------------------------------------------------------
