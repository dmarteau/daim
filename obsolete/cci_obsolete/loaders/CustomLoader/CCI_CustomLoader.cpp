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

#define CCIC_CustomLoader _TXT("loaders/cstmldr.cci")
#define CCI_CustomLoader_Version 1L

#define CCIC_NAME  CCIC_CustomLoader
#define CCIC_SUPER CCIC_Loader
#define CCIC_SUPER_VERSION CCI_Loader_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_CustomLoader_Version
#define CCI_REVISION  2
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))

//-------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"
#include "daim_modules/CCI_Interface.h"

#include <stdio.h>
#include <errno.h>
//-----------------------------------------

CCI_DECL_DATA( CustomLoader )
{
  dmIInputStream  InputStream;
  dmIOutputStream OutputStream;
  
  dmString      MimeType;
  
  dm_uint       BitsPerPixels;
  dm_uint       HeaderSize;
  dm_uint       Alignement;
  dm_bool       ByteSwapped;
  dm_uint       Width;
  dm_uint       Height;
  dm_uint32     Format;
  EPixelFormat  PixelFormat;
};

typedef CCI_INSTANCE_PTR(CustomLoader) _Instance_Ptr;

#define THIS_FILE  _This->File

#define RAWFMT_UINT 0
#define RAWFMT_RGB  1
#define RAWFMT_FLT  2
#define RAWFMT_INT  3
//-------------------------------------------------------------
// Helpers
//-------------------------------------------------------------
#define _ALIGN_SIZE(sz,al)  (((((sz)-1) / al) + 1) * al)
//-------------------------------------------------------------
static EPixelFormat Loader_GetPixelFormat( size_t nb, int _Format )
{
  switch(nb) {
    case 1 : if(_Format==RAWFMT_INT) return dmPixelFormat8bppIndexed; break;
    case 2 : 
      switch(_Format) {
        case RAWFMT_UINT : return dmPixelFormat16bppGrayScale; 
      }
      break;
    case 3 : if(_Format==RAWFMT_RGB) return dmPixelFormat24bppRGB; break;
    case 4 :  
      switch(_Format) {
         case RAWFMT_UINT : return dmPixelFormat32bppGrayScale; 
         case RAWFMT_RGB  : return dmPixelFormat24bppRGB;
         case RAWFMT_FLT  : return dmPixelFormat32bppFloat;
     } 
     break;
  }

  return dmPixelFormatUndefined;
}
//-------------------------------------------------------------
static bool Loader_ReadLine( dm_byte* bline, dm_byte* l, size_t breadsize, size_t psz, size_t nb, 
                       int w, bool _bswap, dmIInputStream& _InputStream )
{
  int i;
  if(_InputStream.Read(bline,breadsize)==breadsize) 
  {
    size_t max_bytes = (nb<psz?nb:psz);

    dm_byte* pout = l;
    dm_byte* pin  = bline;
    if(_bswap) {
      for(int i=0;i<w;++i,pin+=nb,pout+=psz) 
        std::reverse_copy(pin,pin+max_bytes,pout);
    } else { 
      for(i=0;i<w;++i,pin+=nb,pout+=psz) 
        std::copy(pin,pin+max_bytes,pout); 
    }
    return true;
  }
  return false;
}
//-------------------------------------------------------------
bool Loader_ReadImageData( void* scan0, int stride, dm_byte* bline, size_t psz, size_t nb,
                          int w, int h, size_t breadsize,bool bswap,
                          dmIInputStream& _InputStream )
{
  dm_byte* _scan = static_cast<dm_byte*>(scan0);

  if(psz == 1) {
    for(int j=0;j<h;++j,_scan += stride) {
      if(_InputStream.Read(bline,breadsize)==breadsize) 
        dmMemory::Memcpy(_scan,bline,w);
      else
        return false; 
    }
  } else {
    for(int j=0;j<h;++j,_scan += stride) {
      if(!Loader_ReadLine(bline,_scan,breadsize,psz,nb,w,bswap,_InputStream))
        return false; 
    }
  }
  return true; 
}
//--------------------------------------------------------------------
// ME_NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( CustomLoader )
{
  CCI_CONSTRUCT_SUPER()
  CCI_INSTANCE( CustomLoader )

  memset(_This,0,sizeof(*_This));

  CCI_INIT_MEMBER(OutputStream);
  CCI_INIT_MEMBER(InputStream);
  CCI_INIT_MEMBER(MimeType);
  
  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// ME_DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( CustomLoader )
{
  CCI_INSTANCE( CustomLoader )

  _This->OutputStream.Release();
  _This->InputStream.Release();

  CCI_DESTROY_MEMBER(OutputStream);
  CCI_DESTROY_MEMBER(InputStream);
  CCI_DESTROY_MEMBER(MimeType);

  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
// ME_GET
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( CustomLoader )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// ME_SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( CustomLoader )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// CCIM_LoaderFactory_Register
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( Loader, Close )
{
  CCI_INSTANCE( CustomLoader )
 
  if(_This->OutputStream.IsValid())
     _This->OutputStream.Close();

  if(_This->InputStream.IsValid())
     _This->InputStream.Close();
  
  CCI_RETURN_OK()
} 
//---------------------------------------------------------------------
// CCIM_Loader_Infos
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Loader, Infos )
{
    CCI_INSTANCE( CustomLoader )

    if(CCI_P(me)!=CCI_ME_GET) // Can only read !!
      CCI_RETURN_FAIL()
 
    if(CCI_P(ioStream))
    {
      if(!dmReassignInterface(_This->InputStream,CCI_P(ioStream))) 
         CCI_RETURN_INVALID_PARAM()
    }
    
    if(!_This->InputStream.IsValid())
        CCI_RETURN_INVALID_PARAM()
 
    const char_t* mime = _This->MimeType.CStr();

    if(EMPTY_STRING(mime))
       CCI_RETURN_FAIL()

    dmIInterface<dmIProperties> _IProps;
    if(_IProps.Open(_TXT("daim://loaders/cstmldr"),mime)) 
    {
      _This->HeaderSize     = dmString(_IProps.GetData(NULL,_TXT("header"),_TXT("0"))).AsUnsigned();
      _This->BitsPerPixels  = dmString(_IProps.GetData(NULL,_TXT("pixel" ),_TXT("0"))).AsUnsigned();
      _This->Alignement     = dmString(_IProps.GetData(NULL,_TXT("align" ),_TXT("1"))).AsUnsigned();
      _This->ByteSwapped    = dmString(_IProps.GetData(NULL,_TXT("swap"  ),_TXT("0"))).AsUnsigned();
      _This->Width          = dmString(_IProps.GetData(NULL,_TXT("width" ),_TXT("0"))).AsUnsigned();
      _This->Height         = dmString(_IProps.GetData(NULL,_TXT("height"),_TXT("0"))).AsUnsigned();
      _This->Format         = dmString(_IProps.GetData(NULL,_TXT("format"),_TXT("0"))).AsUnsigned();
      
      _This->PixelFormat  = Loader_GetPixelFormat(_This->BitsPerPixels >> 3,_This->Format);

      if(_This->PixelFormat==dmPixelFormatUndefined) {
        dmLOG(_TXT("ERROR: Cannot handle custom pixel format from %s definition\n"),mime);
        CCI_SET_ERROR_MSG(_TXT("Invalid pixel format !"));
        CCI_RETURN_FAIL()
      }

      if(_This->Width==0||_This->Height==0) {
        CCI_SET_ERROR_MSG(_TXT("Invalid image size !"));
        CCI_RETURN_FAIL()
      }
       
      // Return information values

      CCI_RETVAL_P(format) = _This->PixelFormat;
      CCI_RETVAL_P(width ) = _This->Width; 
      CCI_RETVAL_P(height) = _This->Height;

      CCI_RETVAL_P(count) = 1;        // number of records
      /*
      CCI_RETVAL_P(index) = 0;        // index of the current record
      CCI_RETVAL_P(units) = "pixels"; // units name                 
      CCI_RETVAL_P(ar)    = 1.0;      // pixel aspect ratio         
      CCI_RETVAL_P(uppx)  = 1.0;      // units per pixels           
      CCI_RETVAL_P(text)  = NULL;     // text data                  
      */

     CCI_RETURN_OK()

    }

  // Get infos here !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// CCIM_Loader_Data
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Loader, Data )
{
  CCI_INSTANCE( CustomLoader )

  if(_This->InputStream.IsValid()) 
  {
    if(_This->PixelFormat != CCI_P(format)  ||
       _This->Width       != CCI_P(width)   ||
       _This->Height      != CCI_P(height)) 
    {
       dmLOG(_TXT("ERROR: Invalid buffer format !\n"));
       CCI_RETURN_INVALID_PARAM()
    }

    bool res = false;

    size_t nb        = _This->BitsPerPixels >> 3;
    size_t psz       = dmGetPixelFormatBits(_This->PixelFormat) >> 3;
    size_t breadsize = _ALIGN_SIZE(CCI_P(width)*nb,_This->Alignement);
      
    // Seek header
    if(_This->HeaderSize>0) 
    {
      size_t   _headersize = _This->HeaderSize;
      if(_This->InputStream.Read(NULL,_headersize)!=_headersize)
      {
        dmLOG(_TXT("ERROR: Cannot seek header  (header size = %lu) !\n"),_headersize);
        CCI_RETURN_FAIL()
      }
    }
    
    dm_byte* bline  = new dm_byte[breadsize];
    if(bline) 
    {
      bool bSwap = _This->ByteSwapped != 0;
      res = Loader_ReadImageData(CCI_P(scan0),CCI_P(stride),bline,psz,nb,
                                 CCI_P(width),CCI_P(height),breadsize,
                                 bSwap,_This->InputStream);

      delete [] bline;
      if(!res)
        dmLOG(_TXT("ERROR: Read error !\n"));
    } else
      CCI_RETURN_MEMORY_ERROR()

    CCI_RETURN_OK()

  } else
    dmLOG(_TXT("ERROR: Invalid stream !\n"));
    
  CCI_RETURN_FAIL()

}
//---------------------------------------------------------------------
// CCIM_Loader_GetMimeTypes
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Loader, GetMimeTypes )
{
  if(CCI_P(flags)==CCIV_Loader_Read)
  {
    dmIInterface<dmIProperties> _IProps;

    // Mimes types should be the only sections 
    // in this ressources
    if(_IProps.Open(_TXT("daim://formats.ini")))
       return _IProps.GetSections(CCI_P(stringlist));
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method:  Initialize
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Loader, Initialize )
{
  CCI_INSTANCE( CustomLoader )

  _This->MimeType = CCI_P(mime);
  CCI_RETURN_OK();
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( CustomLoader )
  CCI_REGISTER_METHOD_NP( Loader, Close )
  CCI_REGISTER_METHOD_PP( Loader, Infos )
  CCI_REGISTER_METHOD_PP( Loader, Data  )
  CCI_REGISTER_METHOD_PP( Loader, GetMimeTypes )
  CCI_REGISTER_METHOD_PP( Loader, Initialize )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( CustomLoader )
{
  DM_INIT_API()
  CCI_INVOKE_FACTORY(CustomLoader);
}
//---------------------------------------------------------------------
