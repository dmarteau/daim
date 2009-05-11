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

#include "tiffio.h"

#define CCIC_TIFFLoader _TXT("loaders/tiffldr.cci")
#define CCI_TIFFLoader_Version 1L

#define CCIC_NAME  CCIC_TIFFLoader
#define CCIC_SUPER CCIC_Loader
#define CCIC_SUPER_VERSION CCI_Loader_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_TIFFLoader_Version
#define CCI_REVISION  2
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME _TXT("(Using libtiff)"),AUTHOR,_TXT("(C) 2004"))

//-------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"
#include "daim_modules/CCI_Interface.h"
//-----------------------------------------
typedef void (*TIFFWarningHandler)(const char* module, const char* fmt, va_list ap);
typedef void (*TIFFErrorHandler  )(const char* module, const char* fmt, va_list ap);

CCI_DECL_DATA( TIFFLoader )
{
  dmIFileInputStream  InputStream;
  dmIFileOutputStream OutputStream;

  TIFF         *tif_ptr; 
  size_t        dircount;
  EPixelFormat  PixelFormat;
  dm_param      Status;
  dm_param      Mode;

  int           nrecords;
  tdir_t        record;

  uint32        width;
  uint32        height;
  uint16        bitspersample;
  uint16        samplesperpixel;
  uint16        sampleformat;
  uint16        photometric;

  uint16        resolutionunit;
  float         xresolution;
  float         yresolution;

  uint16        compression;
  uint16        predictor;
  uint16        jpegcolormode;
  int           jpegquality;

  TIFFWarningHandler warning_handler;
  TIFFErrorHandler   error_handler;

  cci_Object*   StringList;
};

typedef CCI_INSTANCE_PTR(TIFFLoader) _Instance_Ptr;

//--------------------------------------------------------------------
// Helpers
//--------------------------------------------------------------------
#include "TIFFLoader.cpp"
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( TIFFLoader )
{
  CCI_CONSTRUCT_SUPER()
  CCI_INSTANCE( TIFFLoader )

  memset(_This,0,sizeof(*_This));

  _This->predictor     = 0;
  _This->jpegquality   = 75;
  _This->jpegcolormode = JPEGCOLORMODE_RAW;

  _This->warning_handler = TIFFSetWarningHandler(TIFFLoader_Warning_fn);
  _This->error_handler   = TIFFSetErrorHandler  (TIFFLoader_Error_fn  );

  CCI_INIT_MEMBER(OutputStream);
  CCI_INIT_MEMBER(InputStream);

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( TIFFLoader )
{
  CCI_INSTANCE( TIFFLoader )

  TIFFLoader_Clear(_This);

  TIFFSetWarningHandler(_This->warning_handler);
  TIFFSetErrorHandler  (_This->error_handler);

  if(_This->StringList != NULL)
     CCI_Release(_This->StringList);

  CCI_DESTROY_MEMBER(OutputStream);
  CCI_DESTROY_MEMBER(InputStream);

  CCI_DESTROY_SUPER()
}
/*
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
/*
static inline CCI_METHOD_GET( TIFFLoader )
{
  CCI_RETURN_INVOKE_SUPER()
}
*/
CCI_IMPL_GETTER( TIFFLoader )
  CCI_ATTR_GET( Loader, HasStreamSupport, CCIA_RETVAL = dm_false )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( TIFFLoader )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// CCIM_Loader_Close
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( Loader, Close )
{
  CCI_INSTANCE( TIFFLoader )

  TIFFLoader_Clear(_This);

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
  CCI_INSTANCE( TIFFLoader )
 
  if( CCI_P(me)==CCI_ME_GET )
  {
    if(CCI_P(ioStream))
    {
      if(!dmReassignInterface(_This->InputStream,CCI_P(ioStream))) 
         CCI_RETURN_INVALID_PARAM()
    }
    
    if(!_This->InputStream.IsValid())
        CCI_RETURN_INVALID_PARAM()

    TIFFLoader_Clear( _This );
    TIFFLoader_LoadProperties( _This, CCI_ME_GET );

    //CCI_Try

    if(CCI_SUCCEEDED(TIFFLoader_InitReadStruct(_This)) &&
       CCI_SUCCEEDED(TIFFLoader_ReadHDR(_This)) &&
       CCI_SUCCEEDED(TIFFLoader_GetFormat(_This)) )
    {     
      // Return information values
      CCI_RETVAL_P(format) = _This->PixelFormat;
      CCI_RETVAL_P(width ) = _This->width; 
      CCI_RETVAL_P(height) = _This->height;

      CCI_RETVAL_P(count) = 1;        // number of records
       
      // TODO set others informations
       
    }

    //CCI_Catch(Loader,Infos)

    if(CCI_SUCCEEDED(_This->Status)) 
       CCI_RETURN_OK()

    TIFFLoader_Clear( _This );
    dmLOG(_TXT("ERROR:TIFF Failed to read image !\n"));    

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

    TIFFLoader_Clear( _This );
    TIFFLoader_LoadProperties( _This, CCI_ME_SET );

    _This->PixelFormat = CCI_P(format);
    _This->width       = CCI_P(width);
    _This->height      = CCI_P(height);

    //CCI_Try

    if(CCI_SUCCEEDED(TIFFLoader_SetFormat(_This))) 
    {
      TIFFLoader_InitWriteStruct(_This);
      TIFFLoader_CreateText(_This);
    } else
      dmLOG(_TXT("ERROR:TIFF Invalid pixel format !\n"));    

    //CCI_Catch(Loader,Infos)

    if(CCI_SUCCEEDED(_This->Status)) 
       CCI_RETURN_OK()

    TIFFLoader_Clear( _This );
    dmLOG(_TXT("ERROR:TIFF Failed to write image !\n"));    
  }
 
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method:  Data
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Loader, Data )
{
  CCI_INSTANCE( TIFFLoader )

  if(_This->tif_ptr != NULL) 
  {
    if(_This->PixelFormat != CCI_P(format)  ||
       _This->width       != CCI_P(width)   ||
       _This->height      != CCI_P(height)) 
    {
       dmLOG(_TXT("ERROR:TIFF: Invalid buffer format !\n"));
       CCI_RETURN_INVALID_PARAM()
    }

    //CCI_Try

    if(CCI_P(me)==CCI_ME_GET)  
      TIFFLoader_ReadData(_This,CCI_P(scan0),CCI_P(stride)); 
    else
     if(CCI_P(me)==CCI_ME_SET) 
       TIFFLoader_WriteData(_This,CCI_P(scan0),CCI_P(stride)); 
    else
       _This->Status = CCI_ERR_INVALID_PARAM;

    //CCI_Catch(Loader,Data)

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
    _Strings.AddTail(_TXT("image/tiff"));
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
    CCI_INSTANCE( TIFFLoader )

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
     case dmPixelFormat16bppGrayScale :
     case dmPixelFormat32bppGrayScale :
     case dmPixelFormat24bppRGB       :
     case dmPixelFormat32bppFloat     :
      CCI_RETURN_OK()

     default: break;
   }
   CCI_RETURN_NOT_IMPLEMENTED()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( TIFFLoader )
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
CCI_ENTRY_POINT( TIFFLoader )
{
  //DM_INIT_API()
  CCI_INVOKE_FACTORY(TIFFLoader);
}
//---------------------------------------------------------------------
