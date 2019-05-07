
//===============================================================
// Helpers
//===============================================================

#define DSMName _TXT("TWAIN_32.DLL")

#define PM_SHUTDOWN   WM_USER + 1
#define PM_INITIALIZE WM_USER + 2
#define PM_SUSPEND    WM_USER + 3
#define PM_RESUME     WM_USER + 4
#define PM_RESET      WM_USER + 5

const char_t* DS_Error( int _err )
{
  #define CASE_TWAIN_ERROR( __ERR__ ) case __ERR__ : _ErrStr = _TXT("TWAIN Error : ") #__ERR__; break
  if(_err) {
    const char_t *_ErrStr = _TXT("Unknown");
    switch( _err ) {
      CASE_TWAIN_ERROR(TWCC_BADCAP          );
      CASE_TWAIN_ERROR(TWCC_BADPROTOCOL     );	
      CASE_TWAIN_ERROR(TWCC_BADVALUE        );	
      CASE_TWAIN_ERROR(TWCC_OPERATIONERROR  );	
      CASE_TWAIN_ERROR(TWCC_LOWMEMORY       );	
      CASE_TWAIN_ERROR(TWCC_NODS            );	
      CASE_TWAIN_ERROR(TWCC_SEQERROR        );	
      CASE_TWAIN_ERROR(TWCC_MAXCONNECTIONS  );	
    }
    return _ErrStr;
  }
  return _TXT("Success");
}


void Dump_TWStatus(DSMENTRYPROC lpDSM_Entry,
                   pTW_IDENTITY pApp,
                   pTW_IDENTITY pSrc)
{
  TW_STATUS gGlobalStatus = {0, 0};

   DM_LogMessage("TWAIN:Status: "); 

   if((*lpDSM_Entry)(pApp, pSrc,DG_CONTROL,DAT_STATUS,MSG_GET, 
      (TW_MEMREF)&gGlobalStatus) == TWRC_SUCCESS) 
   {
       DM_LogMessage(DS_Error(gGlobalStatus.ConditionCode)); 
   }

   DM_LogMessage("\n"); 
}


#define CPY_TWSTR32( dest, src )   \
  strncpy(dest,src,sizeof(TW_STR32));

#define CallDSMEntry(_this,pSrc,DG,DAT,MSG,pData) \
   (_this->twLastRC = (*_this->lpDSMEntry)(&_this->twIdentity,pSrc,DG,DAT,MSG,pData))

inline void DSM_DumpStatus( const char_t* msg,_Instance_Ptr _this,
                            TW_IDENTITY* pSrc) 
{
   DM_LogMessage(msg);
   Dump_TWStatus(_this->lpDSMEntry,&_this->twIdentity,pSrc);
}

#define GET_INSTANCE( _This )  _Instance_Ptr _This \
    = reinterpret_cast<_Instance_Ptr>(::GetWindowLong(hWnd,GWL_USERDATA))

//===============================================================
// TWSuspend
//===============================================================
dm_param TWSuspend( _Instance_Ptr _This )
{
  // XXX Is this a good ID to enable/disable source here ?

  if(_This->TWDSOpen==TRUE)
  {
    if(_This->TWDSEnabled==TRUE) 
    {
       _This->TWAcceptTransfert = FALSE;

       _This->twUI.hParent = NULL;
       _This->twUI.ShowUI  = TWON_DONTCARE8;

       TW_UINT16 twRC;

       twRC = CallDSMEntry(_This,&_This->twDS,
                           DG_CONTROL,
                           DAT_USERINTERFACE,
                           MSG_DISABLEDS,
                           (TW_MEMREF)&_This->twUI);

       if(twRC==TWRC_SUCCESS) 
       {
         _This->TWDSEnabled = FALSE;
         CCI_RETURN_OK()
       }

       DSM_DumpStatus("ERROR:TWAIN: MSG_DISABLEDS failed: ",_This,&_This->twDS);
    }
  }
  CCI_RETURN_FAIL()
}
//===============================================================
// TWShutDown
//===============================================================
dm_param TWShutDown( _Instance_Ptr _This )
{         
  if(_This->TWDSOpen) 
  {
    TWSuspend(_This);

    //FIXME: got crash when not closing from twain ui.
    TW_UINT16 twRC;

    twRC = CallDSMEntry(_This,NULL,
                        DG_CONTROL,
                        DAT_IDENTITY,MSG_CLOSEDS,
                        (TW_MEMREF)&_This->twDS);

    if(twRC != TWRC_SUCCESS) 
      DSM_DumpStatus("ERROR:TWAIN: MSG_CLOSEDS failed: ",_This,NULL);
    
    _This->TWDSOpen = FALSE; 

    memset(&_This->twDS,0,sizeof(TW_IDENTITY));

    CCI_RETURN_OK();
  }

  CCI_RETURN_FAIL()
}      
//===============================================================
// TWInitialize
//===============================================================
dm_param TWInitialize( _Instance_Ptr _This, const char_t* source )
{
  if(!_This->TWDSOpen)
  {
    TW_UINT16   twRC;
    TW_IDENTITY twDSIdentity;

    twRC = CallDSMEntry(_This,NULL,
                        DG_CONTROL,
                        DAT_IDENTITY,MSG_GETFIRST,
                        &twDSIdentity);

    while(twRC == TWRC_SUCCESS)
    {
      if(EQUAL_STRING(twDSIdentity.ProductName,source))
      {
        memcpy(&_This->twDS,&twDSIdentity,sizeof(TW_IDENTITY));

        twRC = CallDSMEntry(_This,NULL,
                            DG_CONTROL,
                            DAT_IDENTITY,MSG_OPENDS,
                            (TW_MEMREF)&_This->twDS);      
                            
        if(twRC == TWRC_SUCCESS)
        {
          _This->TWDSOpen = TRUE;
          CCI_RETURN_OK()
        }
        else 
        {
          DSM_DumpStatus("ERROR:TWAIN: MSG_OPENDS failed: ",_This,NULL);
          // XXX: Need to force CLOSEDS
          //CallDSMEntry(_This,NULL,DG_CONTROL,DAT_IDENTITY,MSG_CLOSEDS,
          //             (TW_MEMREF)&_This->twDS);                       
        }
        break;
      }

      twRC = CallDSMEntry(_This,NULL,
                          DG_CONTROL,
                          DAT_IDENTITY,MSG_GETNEXT,
                          (TW_MEMREF)&twDSIdentity);
    }   
  }
  
  CCI_RETURN_FAIL()
}
//===============================================================
// TWResume
//===============================================================
dm_param TWResume( _Instance_Ptr _This )
{
  // XXX Is this a good ID to enable/disable source here ?

  if(_This->TWDSOpen==TRUE)
  {
    TW_UINT16 twRC;

    if(_This->TWDSEnabled==FALSE) 
    {
       memset(&_This->twUI, 0, sizeof(TW_USERINTERFACE));

       _This->twUI.hParent = NULL;
       _This->twUI.ShowUI  = TRUE;
       
       twRC = CallDSMEntry(_This,&_This->twDS,
                           DG_CONTROL,
                           DAT_USERINTERFACE,
                           MSG_ENABLEDS,
                           (TW_MEMREF)&_This->twUI);

       //XXX TWAIN programmers SUCKS !!!!
       //Some twain modules return TWRC_SUCCESS even if they fail !!!
       if(twRC==TWRC_SUCCESS) 
       {
          _This->TWDSEnabled       = TRUE;
          _This->TWAcceptTransfert = TRUE;

          CCI_RETURN_OK()
       }

       DSM_DumpStatus("ERROR:TWAIN: MSG_ENABLEDS failed: ",_This,&_This->twDS);
    }
  }

  CCI_RETURN_FAIL()
}
//===============================================================
// TWReset
//===============================================================
dm_param TWReset( _Instance_Ptr _This )
{
  CCI_RETURN_FAIL()
}
//===============================================================
// TWTransfertImage
//===============================================================
void TWTransfertImage( _Instance_Ptr _This  )
{
   // Handle Mutliple images
   TW_PENDINGXFERS     twPendingXfer;
   TW_UINT16           twRC    = TWRC_FAILURE;
   TW_UINT16           twRC2   = TWRC_FAILURE;
   HBITMAP             hBitMap = NULL;

   TW_BOOL             TWAbort = FALSE;

   memset(&twPendingXfer, 0, sizeof(TW_PENDINGXFERS));

   // Do until there are no more pending transfers
   twPendingXfer.Count = 0;
   do 
   {
     FrameGrabber_SetXferStatus(_This->cci_Self,XFERSTART);
     if(!_This->TWDSEnabled)
        return;

     // Initiate Native Transfer
     twRC = CallDSMEntry(_This,&_This->twDS,
                         DG_IMAGE,
                         DAT_IMAGENATIVEXFER, 
                         MSG_GET, 
                         (TW_MEMREF)&hBitMap);
     switch (twRC)
     {
       //===========================
       // TWRC_XFERDONE
       //===========================

       case TWRC_XFERDONE:          // Acknowledge the end of the transfer 

         twRC2 = CallDSMEntry(_This,&_This->twDS,
                              DG_CONTROL,
                              DAT_PENDINGXFERS, 
                              MSG_ENDXFER,
                             (TW_MEMREF)&twPendingXfer);
      
         // Notify acquisition here
    
         if((_This->hLiveDIB = (LPBITMAPINFOHEADER)::GlobalLock(hBitMap))!=NULL)
         {
            FrameGrabber_SetXferStatus(_This->cci_Self,XFERDONE );
            ::GlobalUnlock(_This->hLiveDIB); 

            _This->hLiveDIB = NULL;
         }
         break; // TWRC_XFERDONE

       //===========================
       // TWRC_CANCEL
       //===========================

       /*
        * the user canceled or wants to rescan the image
        * something wrong, abort the transfer
        */
       case TWRC_CANCEL: // Source (or User) Canceled Transfer
   
         twRC2 = CallDSMEntry(_This,&_This->twDS,
                              DG_CONTROL,
                              DAT_PENDINGXFERS, 
                              MSG_ENDXFER,
                              (TW_MEMREF)&twPendingXfer);

         TWAbort = TRUE;
         break;  // TWRC_CANCEL


       //===========================
       // TWRC_FAILURE
       //===========================

       case TWRC_FAILURE:  // The transfer failed
        /*
         * Abort the image
         * Enhancement: Check Condition Code and attempt recovery
         */
         DSM_DumpStatus(_TXT("ERROR:TWAIN: DAT_IMAGENATIVEXFER/MSG_GET failed: "),_This,&_This->twDS);
  
         twRC2 = CallDSMEntry(_This,&_This->twDS, 
                              DG_CONTROL,
                              DAT_PENDINGXFERS, 
                              MSG_ENDXFER,
                              (TW_MEMREF)&twPendingXfer); 

         FrameGrabber_SetXferStatus(_This->cci_Self,XFERERROR);
         TWAbort = TRUE;
         break;

       default:  // Sources should never return any other RC => Abort the image
         twRC2 = CallDSMEntry(_This,&_This->twDS, 
                              DG_CONTROL,
                              DAT_PENDINGXFERS, 
                              MSG_ENDXFER,
                              (TW_MEMREF)&twPendingXfer);
         TWAbort = TRUE;
         break;
      }   

      //==================
      // Tranfert aborted
      //==================
      if(TWAbort)
      {
        if(_This->hLiveDIB) {
          ::GlobalUnlock(_This->hLiveDIB);
          _This->hLiveDIB = NULL;
        }
        FrameGrabber_SetXferStatus(_This->cci_Self,XFERABORT);
      }

   } while (twPendingXfer.Count && _This->TWDSEnabled);
}
//===============================================================
// TWProcessMessage
//===============================================================
BOOL TWProcessMessage( _Instance_Ptr _This, LPMSG lpMsg )
{
  TW_UINT16  twRC = TWRC_NOTDSEVENT;
  TW_EVENT   twEvent;

  memset(&twEvent, 0, sizeof(TW_EVENT));

  if(_This->TWDSEnabled)
  {
     twEvent.pEvent = (TW_MEMREF)lpMsg;

     twRC = CallDSMEntry(_This,&_This->twDS,
                         DG_CONTROL,
                         DAT_EVENT,
                         MSG_PROCESSEVENT,
                         (TW_MEMREF)&twEvent);

    if(twRC != TWRC_NOTDSEVENT)  
    {
       switch(twEvent.TWMessage)
       {
         case MSG_XFERREADY:

         if(_This->TWAcceptTransfert) 
         {
           TWTransfertImage(_This);
         } 

         break; // MSG_XFERREADY

        case MSG_CLOSEDSREQ:
        case MSG_CLOSEDSOK :
          // XXX Here we need to nofify the new state
          if(CCI_SUCCEEDED(TWShutDown(_This)))
            FrameGrabber_SetState(_This->cci_Self,0);
          break;
      }
    }
  }

  // tell the caller what happened
  return (twRC==TWRC_DSEVENT);  // returns TRUE or FALSE

}
//===============================================================
// TWWindowProc
//===============================================================
static int TWPreprocessMsg( _Instance_Ptr _This,  LPMSG lpMsg  )
{
  BOOL bSignal = FALSE;

  switch (lpMsg->message) 
  {
    case WM_DESTROY: 
    {
      TWShutDown(_This);
      ::PostQuitMessage( 0 );
      return TRUE;
    } break;  

    case PM_SHUTDOWN: 
       _This->rv = TWShutDown(_This);
       bSignal = TRUE;
       break;

    case PM_INITIALIZE: 
       _This->rv = TWInitialize(_This,reinterpret_cast<char_t*>(lpMsg->lParam));
       bSignal = TRUE;
       break;

    case PM_SUSPEND   : 
       _This->rv = TWSuspend(_This);
       bSignal = TRUE;
       break;

    case PM_RESUME    : 
       _This->rv = TWResume(_This);
       bSignal = TRUE;
       break;

    case PM_RESET : 
       _This->rv = TWReset(_This);
       bSignal = TRUE;
       break;
  }

  if(bSignal) {
     ::SetEvent( _This->hThreadEvent );
     return TRUE;
  }
  
  return FALSE;
  
}
//===============================================================
// TWWindowProc
//===============================================================
static LRESULT CALLBACK TWWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  //GET_INSTANCE( _This );

  return DefWindowProc(hWnd,msg,wParam,lParam);
}
//===============================================================
// TWThread
//===============================================================
void WINAPI	TWThread( LPVOID pParam )
{ 
  cci_Object* _This_ob = static_cast<cci_Object*>(pParam);
  cci_Class*  _This_cl = CCI_FindPubClass(CCI_GET_CLSID(Twain),TGZ_DEFAULT);
 
  CCI_INSTANCE( Twain )

  HWND hWnd = ::CreateWindowEx(0,          // Create hidden Window 
                  TW_WNDCLASSNAME,
                  TW_WNDNAME,
                  WS_OVERLAPPEDWINDOW,
                  CW_USEDEFAULT,
                  CW_USEDEFAULT,
                  0,0,
                  (HWND)NULL,
                  (HMENU)NULL, 
                  TW_MODULE_HANDLE,
                  (LPSTR)NULL         
                );
  if (hWnd) 
  {
     _This->hWnd = hWnd;

     if(CallDSMEntry(_This,NULL,
                     DG_CONTROL,
                     DAT_PARENT,
                     MSG_OPENDSM,
                     (TW_MEMREF)&hWnd) == TWRC_SUCCESS)
      {
         _This->TWDSMOpen = TRUE;
      }    
      else 
      {
        DSM_DumpStatus(_TXT("ERROR:TWAIN: MSG_OPENDSM failed: "),_This,NULL);

         _This->hWnd = NULL;
        ::DestroyWindow(hWnd);     
      }

      ::SetEvent( _This->hThreadEvent );

      if(!_This->TWDSMOpen)
         ExitThread(0);         

      // Associate window with _This so we can access 
      // it in the window procedure
      ::SetWindowLong(hWnd, GWL_USERDATA, (LONG)_This);

     MSG   msg;     

     /* Polling messages from event queue */     
     while (::GetMessage((LPMSG)&msg, NULL, 0, 0)) 
     { 
        if(!TWPreprocessMsg(_This,(LPMSG)&msg)) 
        {
          if(!TWProcessMessage(_This,(LPMSG)&msg))
          {
            ::TranslateMessage((LPMSG)&msg); 
            ::DispatchMessage ((LPMSG)&msg); 
          }
        }
     }  

     // Close DSM 
     if(_This->TWDSMOpen) {
       //FIXME: got crash when not closing from twain ui.
       CallDSMEntry(_This,NULL,
                     DG_CONTROL,DAT_PARENT,
                     MSG_CLOSEDSM,(TW_MEMREF)&hWnd);

        _This->TWDSMOpen = FALSE;
     } 

      _This->hWnd = NULL;
     ::DestroyWindow(hWnd);  
   
  } else
    ::SetEvent( _This->hThreadEvent );

  CCI_UnLock(_This_cl);

  ExitThread(0);

}
//===============================================================
// TWCreateThread
//===============================================================
BOOL TWCreateThread(  _Instance_Ptr _This )
{
  if(!_This->TWDSMOpen)
  {
    if(_This->hThreadEvent) ::CloseHandle(_This->hThreadEvent);
    if(_This->hThread)      ::CloseHandle(_This->hThread);

    _This->hThread      = NULL;
    _This->nThreadID    = 0;
    
    _This->hThreadEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
     
    if( _This->hThreadEvent == NULL ) {
      DM_LogSystemMessage("ERROR:TWAIN: Error in system function <CreateEvent> :",::GetLastError());
    } else {
      // Create The Thread
      _This->hThread = ::CreateThread( NULL,0, (LPTHREAD_START_ROUTINE)TWThread, 
                                          _This->cci_Self,0,&_This->nThreadID);
    }
    
    if(_This->hThread == NULL ) {
       DM_LogSystemMessage("ERROR:TWAIN: Error in system function <CreateThread> :",::GetLastError());
    }

     // Wait for the thread to start
    ::WaitForSingleObject( _This->hThreadEvent,5000);
    ::ResetEvent(_This->hThreadEvent);
  }
  
  return _This->TWDSMOpen;
}

//===============================================================
