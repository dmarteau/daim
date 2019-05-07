#include "daim_utilities.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_StringList.h"
#include "daim_modules/CCI_PropertyList.h"
#include "daim_modules/CCI_FrameGrabber.h"

#include "daim_modules/win32/FrameGrabbers/CCI_Twain.h"
//------------------------------------------------------------------
#define CCIC_NAME  CCIC_Twain
#define CCIC_SUPER CCIC_FrameGrabber
#define CCIC_SUPER_VERSION CCI_FrameGrabber_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_Twain_Version
#define CCI_REVISION  1
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2005"))

#define _MAKE_VERSION_STR( Ver, Rev )  #Ver _TXT(".") #Rev
#define CCI_VERSION_STR _MAKE_VERSION_STR(CCI_VERSION,CCI_REVISION)

#define _NOINDEX_ ~0L
//--------------------------------------------------------------------
#define CCI_CUSTOM_INIT_HANDLER

#include "daim_modules/CCI_Base.h"

#include "daim/daim_base.h"
#include "daim/daim_interfaces.h"

#include "twain.h"
#include "dibutils.h"

void DM_LogSystemMessage(const TCHAR *msg, DWORD dwError );

static LPCTSTR   TW_WNDCLASSNAME  = _TXT("DAIM_TWAIN_HIDDEN_WINDOW");
static LPCTSTR   TW_WNDNAME       = _TXT("");
static HINSTANCE TW_MODULE_HANDLE = NULL;

#define FrameGrabber_SetXferStatus( _This_object,status ) \
      CCI_Set(_This_object,CCIA_DECL_(FrameGrabber,XferStatus),CCIV_FrameGrabber_ ## status);

#define FrameGrabber_SetState( _This_object,st ) \
      CCI_Set(_This_object,CCIA_DECL_(FrameGrabber,State),st);

//--------------------------------------------------------------------
CCI_DECL_DATA( Twain )
{
  TW_IDENTITY       twIdentity; // Multiple identities should allow mutliple connections
  TW_IDENTITY       twDS;
  TW_USERINTERFACE  twUI;

  TW_UINT16         twLastRC;

  HWND              hWnd;
  HMODULE           hDSMDLL;
  DSMENTRYPROC      lpDSMEntry;

  BOOL              TWAcceptTransfert;
  BOOL              TWDSOpen;
  BOOL              TWDSMOpen;
  BOOL              TWDSEnabled;

  LPBITMAPINFOHEADER hLiveDIB;

  HANDLE   hThread;
  DWORD	   nThreadID;
  HANDLE   hThreadEvent;
 
  dm_param rv;

  cci_Object*  cci_Self;
};

typedef CCI_INSTANCE_PTR(Twain) _Instance_Ptr;

#include "twain_thread.h"




//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( Twain )
{
  CCI_GENERIC_CONSTRUCT(Twain)
  
  _This->cci_Self   = _THIS_OBJECT;

  _This->hDSMDLL    = NULL;
  _This->lpDSMEntry = NULL;
  _This->hLiveDIB   = NULL;

  _This->nThreadID    = 0;
  _This->hWnd         = NULL;
  _This->hThread      = NULL;
  _This->hThreadEvent = NULL;

  _This->TWDSOpen    = FALSE;
  _This->TWDSMOpen   = FALSE;
  _This->TWDSEnabled = FALSE;

  _This->TWAcceptTransfert = FALSE;

  TW_IDENTITY* ptwDS       = &_This->twDS;
  TW_IDENTITY* ptwIdentity = &_This->twIdentity;

  memset(ptwIdentity,0,sizeof(TW_IDENTITY));
  memset(ptwDS      ,0,sizeof(TW_IDENTITY));

  //======================
  // Set Application Identity
  //======================

  ptwIdentity->Id = 0; // Initialize to 0 (Source Manager will assign real value)
  ptwIdentity->Version.MajorNum = CCI_VERSION;
  ptwIdentity->Version.MinorNum = CCI_REVISION;
  ptwIdentity->Version.Language = TWLG_USA;
  ptwIdentity->Version.Country  = TWCY_USA;
  ptwIdentity->ProtocolMajor    = TWON_PROTOCOLMAJOR;
  ptwIdentity->ProtocolMinor    = TWON_PROTOCOLMINOR;
  ptwIdentity->SupportedGroups  = DG_IMAGE | DG_CONTROL;

  CPY_TWSTR32(ptwIdentity->Version.Info , CCI_VERSION_STR );
  CPY_TWSTR32(ptwIdentity->Manufacturer , "Daim.project" );
  CPY_TWSTR32(ptwIdentity->ProductFamily, "DAIM Library Extension" );
  CPY_TWSTR32(ptwIdentity->ProductName  , "DAIM CCI Twain Mmanager");

  //======================
  // Initialize DLL
  //======================

  HMODULE hDSMDLL  = ::LoadLibrary(DSMName);
  if( hDSMDLL != NULL)
  {
    _This->hDSMDLL = hDSMDLL;
    if((_This->lpDSMEntry = (DSMENTRYPROC)::GetProcAddress(hDSMDLL,MAKEINTRESOURCE(1))))
       CCI_RETURN_THIS()    
    else
     DM_LogSystemMessage("ERROR:TWAIN: <GetProcAddress> Error : ",::GetLastError());

  } else
     DM_LogSystemMessage(_TXT("ERROR:TWAIN: <LoadLibrary> Error: "),::GetLastError());

  CCI_RETURN_FAILED_TO_CONSTRUCT()
}
//--------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( Twain )
{
  CCI_INSTANCE( Twain )

  if(_This->hThread) 
  {
    if(_This->hWnd) {
      ::PostMessage(_This->hWnd,WM_DESTROY,0,0);
      ::WaitForSingleObject( _This->hThread, 25000 );
    }
    ::CloseHandle( _This->hThread );
  }

  if(_This->hThreadEvent) 
     ::CloseHandle(_This->hThreadEvent);
     
  if(_This->hDSMDLL)
     ::FreeLibrary(_This->hDSMDLL);

  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( Twain )
  CCI_ATTR_GET( FrameGrabber, Source , 
    CCIA_RETVAL = CCIA_TRANSLATE_VAL(_This->twDS.ProductName) ) 
CCI_END_GETTER()
/*
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( Twain )
CCI_END_SETTER()
*/
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( Twain )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// Method GetDeviceList
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( FrameGrabber , GetDeviceList )
{
  CCI_INSTANCE( Twain )

  if(TWCreateThread(_This))
  {
    dmIStringList _StringList;
 
    if(_StringList.QueryInterface(CCI_P(stringlist)))
    {
      TW_UINT16   twRC;
      TW_IDENTITY twDSIdentity;
 
      dm_uint count = 0;
 
      twRC = CallDSMEntry(_This,NULL,
                          DG_CONTROL,
                          DAT_IDENTITY,MSG_GETFIRST,
                          (TW_MEMREF)&twDSIdentity);

      while(twRC == TWRC_SUCCESS)
      {
        ++count;

        _StringList.AddTail( twDSIdentity.ProductName );

        twRC = CallDSMEntry(_This,NULL,
                            DG_CONTROL,
                            DAT_IDENTITY,MSG_GETNEXT,
                            (TW_MEMREF)&twDSIdentity);
       } 

       if(count>0) 
          CCI_RETURN_OK()
    
       CCI_RETURN_FAIL()
     }
     CCI_RETURN_INVALID_PARAM()
  } 
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method    : GetDeviceInfos
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( FrameGrabber, GetDeviceInfos )
{
  CCI_INSTANCE( Twain )

  if(TWCreateThread(_This))
  {
    dmIPropertyList _IPropList;
    if(_IPropList.QueryInterface(CCI_P(propertylist)))
    {
      TW_UINT16   twRC;
      TW_IDENTITY twDSIdentity;

      bool isSeq = EMPTY_STRING(CCI_P(device));

      twRC = CallDSMEntry(_This,NULL,
                          DG_CONTROL,
                          DAT_IDENTITY,MSG_GETFIRST,
                          (TW_MEMREF)&twDSIdentity);

      dm_param key = CCI_P(key);

      if(isSeq)
         _IPropList.NewSeq(key,NULL);  // Create a new sequence

      while(twRC == TWRC_SUCCESS)
      {
        if(isSeq || EQUAL_STRING(CCI_P(device),twDSIdentity.ProductName))
        {
          _IPropList.SetProperty(key,NULL,_TXT("device:productName"  ),twDSIdentity.ProductName);
          _IPropList.SetProperty(key,NULL,_TXT("device:productFamily"),twDSIdentity.ProductFamily);
          _IPropList.SetProperty(key,NULL,_TXT("device:manufacturer" ),twDSIdentity.Manufacturer);

          if(!isSeq) 
             CCI_RETURN_OK()
        }

        twRC = CallDSMEntry(_This,NULL,
                            DG_CONTROL,
                            DAT_IDENTITY,MSG_GETNEXT,
                            (TW_MEMREF)&twDSIdentity);
      } 

      if(isSeq)                     // Close sequence
         _IPropList.EndSeq(key);

      CCI_RETURN_OK()
    }
  }

  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method Initialize
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( FrameGrabber , Initialize )
{
  CCI_INSTANCE( Twain )

#ifdef _DEBUG
  #define INIT_TIMEOUT INFINITE
#else
  #define INIT_TIMEOUT 10000 
#endif
  
  if(TWCreateThread(_This))
  {
    if(_This->hWnd && !_This->TWDSOpen)
    {
      ::PostMessage(_This->hWnd,PM_INITIALIZE,0,(LPARAM)CCI_P(source));
      if(::WaitForSingleObject( _This->hThreadEvent,INIT_TIMEOUT )==WAIT_TIMEOUT) {
         DM_LogMessage("TWAIN:WARNING: TIMEOUT has occured for 'Initialize'\n");
         CCI_RETURN_FAIL()
      }
      ::ResetEvent(_This->hThreadEvent);

      if(CCI_SUCCEEDED(_This->rv))
         FrameGrabber_SetState(_This->cci_Self,1);

      return _This->rv;
    }
  }
  
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method ShutDown
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( FrameGrabber , ShutDown )
{
  CCI_INSTANCE( Twain )

  _This->rv = CCI_ERR_OK;
 
  ::PostMessage(_This->hWnd,PM_SHUTDOWN,0,0);
  ::WaitForSingleObject( _This->hThreadEvent,INIT_TIMEOUT );
  ::ResetEvent(_This->hThreadEvent);

  if(CCI_SUCCEEDED(_This->rv))
     FrameGrabber_SetState(_This->cci_Self,0); 

  // Always return OK
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method Suspend
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( FrameGrabber , Suspend )
{
  CCI_INSTANCE( Twain )

  if(_This->hWnd && _This->TWDSEnabled) 
  {
    ::PostMessage(_This->hWnd,PM_SUSPEND,0,0);
    if(::WaitForSingleObject( _This->hThreadEvent,5000 )==WAIT_TIMEOUT) {
       DM_LogMessage("TWAIN:WARNING: TIMEOUT has occured for 'Suspend'\n");
       CCI_RETURN_FAIL()
    }
    ::ResetEvent(_This->hThreadEvent);

    if(CCI_SUCCEEDED(_This->rv))
       FrameGrabber_SetState(_This->cci_Self,1); // Reset to state 1;

    return _This->rv;
  }
  
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method Resume
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( FrameGrabber , Resume )
{
  CCI_INSTANCE( Twain )

  if(_This->hWnd && !_This->TWDSEnabled) 
  {
    ::PostMessage(_This->hWnd,PM_RESUME,0,0);
    if(::WaitForSingleObject( _This->hThreadEvent,5000 )==WAIT_TIMEOUT) {
       DM_LogMessage("TWAIN:WARNING: TIMEOUT has occured for 'Resume'\n");
       CCI_RETURN_FAIL()
    }
    ::ResetEvent(_This->hThreadEvent);

    if(CCI_SUCCEEDED(_This->rv))
       FrameGrabber_SetState(_This->cci_Self,2);

    return _This->rv;
  }
  
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method Reset
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( FrameGrabber , Reset )
{
  CCI_UNUSUED_INSTANCE( Twain )
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method GetFrameBuffer
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( FrameGrabber , GetFrameBuffer )
{
  CCI_INSTANCE( Twain )

  LPBITMAPINFOHEADER lpbi = _This->hLiveDIB;

  if( lpbi != NULL)
  {
    size_t _RowBytes     =   DIB_BytesPerLine(lpbi);

    CCI_RETVAL_P(width)  = lpbi->biWidth;
    CCI_RETVAL_P(height) = lpbi->biHeight;

    CCI_RETVAL_P(format) =   DIB_Format(lpbi);
    CCI_RETVAL_P(scan0)  =   DIB_FindBits(lpbi) + _RowBytes * (lpbi->biHeight-1);
    CCI_RETVAL_P(stride) = - static_cast<dm_int>(DIB_BytesPerLine(lpbi));  

    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method Property
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( FrameGrabber , Property )
{
  CCI_UNUSUED_INSTANCE( Twain )

  switch(CCI_P(me))
  {
    case CCIV_FrameGrabber_PropGet : break;
    case CCIV_FrameGrabber_PropSet : break;
    case CCIV_FrameGrabber_PropMin : break;
    case CCIV_FrameGrabber_PropMax : break;
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method Value
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( FrameGrabber , Value )
{
  CCI_UNUSUED_INSTANCE( Twain )

  switch(CCI_P(me))
  {
    case CCIV_FrameGrabber_PropGet : break;
    case CCIV_FrameGrabber_PropSet : break;
    case CCIV_FrameGrabber_PropMin : break;
    case CCIV_FrameGrabber_PropMax : break;
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( Twain )
  CCI_REGISTER_METHOD_PP( FrameGrabber, GetFrameBuffer )
  CCI_REGISTER_METHOD_PP( FrameGrabber, Initialize     )
  CCI_REGISTER_METHOD_NP( FrameGrabber, ShutDown       )   
  CCI_REGISTER_METHOD_NP( FrameGrabber, Suspend        )
  CCI_REGISTER_METHOD_NP( FrameGrabber, Resume         )
  CCI_REGISTER_METHOD_NP( FrameGrabber, Reset          )
  CCI_REGISTER_METHOD_PP( FrameGrabber, GetDeviceList  )
  CCI_REGISTER_METHOD_PP( FrameGrabber, GetDeviceInfos )
  CCI_REGISTER_METHOD_PP( FrameGrabber, Property       )
  CCI_REGISTER_METHOD_PP( FrameGrabber, Value          )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// Twain_Initialize
//---------------------------------------------------------------------
dm_param Twain_Initialize(cci_Class* cl,void* _UserData)
{
   // Register our special window class

   WNDCLASS  myClass;
   myClass.hCursor       = NULL;
   myClass.hIcon         = NULL;
   myClass.lpszMenuName  = NULL;
   myClass.hbrBackground = NULL;
   myClass.lpszClassName = TW_WNDCLASSNAME;
   myClass.hInstance     = TW_MODULE_HANDLE;
   myClass.style         = 0;
   myClass.lpfnWndProc   = TWWndProc;
   myClass.cbClsExtra    = 0;  
   myClass.cbWndExtra    = 0;

   if (!::RegisterClass( &myClass ) )  
      CCI_RETURN_FAIL()     

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
void Twain_Finalize(cci_Class* cl,void* _UserData) 
{ 
 ::UnregisterClass(TW_WNDCLASSNAME, TW_MODULE_HANDLE);
}
//---------------------------------------------------------
// Special init handlers
//---------------------------------------------------------
CCI_METHOD CCI_Init(struct CCI_Custom_Hook* cci_Hook)
{
  if(CCI_INIT_API()) 
  {
    TW_MODULE_HANDLE = static_cast<HINSTANCE>(cci_Hook->cc_Module);
    return dm_true;
  }
  return dm_false;
}
//---------------------------------------------------------
void CCI_Close() 
{ 
  CCI_CLOSE_API(); 
}
//---------------------------------------------------------------------
CCI_ENTRY_POINT( Twain )
{
  DM_INIT_API()
  CCI_INVOKE_FACTORY_(Twain,NULL,Twain_Initialize,Twain_Finalize);
}
//---------------------------------------------------------------------
