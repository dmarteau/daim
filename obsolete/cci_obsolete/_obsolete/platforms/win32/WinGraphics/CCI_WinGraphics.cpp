#define  dmUseKernelImage
#include "daim_tools.h"
#include "dmWinGraphics.h"
#include "dmWinNativeImage.h"
#include "dmResources.h"

#include "documents/dmDocFactory.h"

#define DAIM_MEAN_AND_LEAN
#include "daim/daim_modules.h"
#include "daim_modules/CCI_Graphics.h"

#define CCIC_WINGraphics _TXT("wingraphics.cci")
#define CCI_WINGraphics_Version 1L

#define CCIC_NAME  CCIC_WINGraphics
#define CCIC_SUPER CCIC_Graphics
#define CCIC_SUPER_VERSION CCI_Graphics_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_WINGraphics_Version
#define CCI_REVISION  1
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))
//-------------------------------------------------------------------
#define CCI_CUSTOM_INIT_HANDLER

#include "daim_modules/CCI_Base.h"
#include "daim_modules/CCI_Interface.h"

#include "dmWinGraphics.h"

#define _SIZEDEFAULT TGZ_DEFAULT

//-----------------------------------------
CCI_DECL_DATA( WINGraphics )
{
  dmWinGraphics   _Graphics;
  BOOL            _DoubleBuffering;
  BOOL            _Initialized;
  POINT           _Screen;
};

typedef CCI_INSTANCE_PTR(WINGraphics) _Instance_Ptr;
//--------------------------------------------------------------------
// Helpers
//--------------------------------------------------------------------
static CCI_METHOD WINGraphics_SetOption( _Instance_Ptr _This, 
                                         const char_t* propertyName,
                                         const char_t* inputValue
                                       )
{
   if(EQUAL_NOCASE_STRING(propertyName,_TXT("DoubleBuffering"))) 
   {
     if(EQUAL_NOCASE_STRING(inputValue,"y")||EQUAL_NOCASE_STRING(inputValue,"yes")) {
       _This->_DoubleBuffering = TRUE;
       CCI_RETURN_OK()
     }      
     if(EQUAL_NOCASE_STRING(inputValue,"n")||EQUAL_NOCASE_STRING(inputValue,"no")) {
       _This->_DoubleBuffering = FALSE;
       CCI_RETURN_OK()
      }
   }

   CCI_RETURN_FAIL()
}
//--------------------------------------------------------------------
#define _GET_PROPERTY( optionname, valueName )                     \
   if(_This->_Graphics.Get##optionname() == optionname##valueName){\
     outputValue = _TXT(#valueName);                               \
     CCI_RETURN_OK()                                               \
   }
//--------------------------------------------------------------------
static CCI_METHOD WINGraphics_GetOption( _Instance_Ptr _This, 
                                         const char_t*  propertyName,
                                         const char_t*& outputValue
                                       )
{
   if(EQUAL_NOCASE_STRING(propertyName,_TXT("DoubleBuffering"))) 
   {
     if(_This->_DoubleBuffering)  outputValue = _TXT("yes");
     else                         outputValue = _TXT("no");
     CCI_RETURN_OK()     
   }

   CCI_RETURN_FAIL() 
}
//--------------------------------------------------------------------
static inline void WINGraphics_ReadOptionValue( _Instance_Ptr _This, 
                                                dmIProperties& _IProps, 
                                                const char_t* propertyKey,
                                                const char_t* propertyName ) 
{
  const char_t* propertyValue = _IProps.GetData(NULL,propertyKey);
  if(!EMPTY_STRING(propertyValue)) 
     WINGraphics_SetOption(_This,propertyName,propertyValue);
}
//--------------------------------------------------------------------
// TODO : look to an another way to specify 
// graphics options
//--------------------------------------------------------------------
#define READ_OPTION_VALUE( propertyName ) \
      GDIGraphics_ReadOptionValue(_This,_IProps,\
         _TXT("graphics.options.") propertyName,propertyName);
//--------------------------------------------------------------------
static void WINGraphics_ReadOptions( _Instance_Ptr _This )
{
  dmIInterface<dmIProperties> _IProps;
  if(_IProps.Open(_TXT("config://"),_TXT("wingraphics"))) 
  {
  }
}
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( WINGraphics )
{
  CCI_CONSTRUCT_SUPER()
  CCI_INSTANCE( WINGraphics )

  _This->_DoubleBuffering = FALSE;
  _This->_Initialized     = FALSE;
 
  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( WINGraphics )
{
  CCI_INSTANCE( WINGraphics )

  if(_This->_Initialized)
    CCI_DESTROY_MEMBER(_Graphics);

  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( WINGraphics )
  CCI_ATTR_GETVAL_P(Graphics,NativeWindow,_This->_Graphics.GetWindow()) 
  CCI_ATTR_GETVAL_L(Graphics,PageScale   ,_This->_Graphics.GetPageScale()) 
  CCI_ATTR_GETVAL_P(Graphics,Graphics    ,&_This->_Graphics)  
  
  CCI_ATTR_GETVAL_L(Graphics,ScreenX     ,_This->_Screen.x)  
  CCI_ATTR_GETVAL_L(Graphics,ScreenY     ,_This->_Screen.y)  
   
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( WINGraphics )
  CCI_ATTR_SET(Graphics,PageScale,
     CCIA_DATA = _This->_Graphics.SetPageScale(CCIA_DATA)
  ) 
  CCIA_DISCARD(Graphics,Graphics    )  
  CCIA_DISCARD(Graphics,NativeWindow) 
CCI_END_SETTER()
//---------------------------------------------------------------------
// Method: InitGraphics
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Graphics, InitGraphics  )
{
  CCI_INSTANCE( WINGraphics )
 
  HWND hWnd = static_cast<HWND>(CCI_P(nativeWindow));

  if(hWnd) 
  {
    CCI_INIT_MEMBER_1(_Graphics,hWnd);
    _This->_Initialized = TRUE;

    WINGraphics_ReadOptions(_This);

    // Enable UI
    
    _This->_Graphics.UIEnabled = true;

    // Pass our dmGraphic object to the parent
    
    CCI_P(nativeGraphics) = &_This->_Graphics;
    CCI_RETURN_INVOKE_SUPER()
  }
  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method: Draw
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Graphics, Draw ) 
{
  CCI_INSTANCE( WINGraphics )

  HDC hdc = static_cast<HDC>(CCI_P(nativeDeviceContext));

  if(hdc) 
  {
    CCI_METHOD Result;
   
    if(_This->_DoubleBuffering && CCI_P(boundRect)!=NULL) // Use double Buffering
    {
      dmRect rcBounds(*CCI_P(boundRect));    

      RECT    rc;
      HDC     hdcMem;
      HBITMAP hbmMem;
      HGDIOBJ hbmOld;

      // Get the size of the client rectangle.
      ::SetRect(&rc,rcBounds.Left(),rcBounds.Top(),rcBounds.Right(),rcBounds.Bottom());

      // Create a compatible DC.
      hdcMem = ::CreateCompatibleDC(hdc);

      // Create a bitmap big enough for our client rectangle.
      hbmMem = ::CreateCompatibleBitmap(hdc,
                                        rc.right-rc.left,
                                        rc.bottom-rc.top);

      // Select the bitmap into the off-screen DC.
      hbmOld = ::SelectObject(hdcMem, hbmMem);

      HDC savedGraphics = _This->_Graphics.SetGraphics(hdcMem);

      Result = CCI_INVOKE_SUPER();

      /* TODO: Handling correctly the background

      // Erase the background.
      HBRUSH hbrBkGnd = ::CreateSolidBrush(GetSysColor(COLOR_WINDOW));
      ::FillRect(hdcMem, &rc, hbrBkGnd);
      ::DeleteObject(hbrBkGnd);

      */

      // Blt the changes to the screen DC.
      ::BitBlt(hdc,
               rc.left, rc.top,
               rc.right-rc.left, rc.bottom-rc.top,
               hdcMem,
               0, 0,
               SRCCOPY);

      ::SelectObject(hdcMem, hbmOld);
      ::DeleteObject(hbmMem);
      ::DeleteDC(hdcMem);
           
      // Done with off-screen bitmap and DC.
      _This->_Graphics.Restore(savedGraphics);
    } 
    else // Draw directly into the DC
    {
      HDC savedGraphics = _This->_Graphics.SetGraphics(hdc);
      Result = CCI_INVOKE_SUPER();
      _This->_Graphics.Restore(savedGraphics);
    }
    return Result;
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method : SetDisplay
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Graphics, SetDisplay )
{
  CCI_INSTANCE( WINGraphics )

  if(CCI_SUCCEEDED(CCI_INVOKE_SUPER())) 
  {
    dmIGraphics _IGraphics;
    if(_IGraphics.QueryInterface(_THIS_OBJECT)) 
    {
      dm_int  left   = CCI_P(left);
      dm_int  top    = CCI_P(top);
      dm_uint width  = CCI_P(width);
      dm_uint height = CCI_P(height);

      dmIDocument _IDoc;
      _IDoc.QueryInterface(_IGraphics.GetDocument());

      dm_uint _PageScale = _This->_Graphics.GetPageScale();

      if(_IDoc.IsValid()) 
      {
        if(width  == _SIZEDEFAULT) { width  = _IDoc.Width() ; }
        if(height == _SIZEDEFAULT) { height = _IDoc.Height(); }

        _IDoc.Release(); // \see GetDocument()
      } 
      else
      {
        if(width  == _SIZEDEFAULT) { width  = 1; }
        if(height == _SIZEDEFAULT) { height = 1; }
      }

      _This->_Graphics.SetDisplay(left,top,width,height);

      if(width  > 1) CCI_RETVAL_P(width)  = (width  * _PageScale)/100;
      if(height > 1) CCI_RETVAL_P(height) = (height * _PageScale)/100;

      CCI_RETURN_OK()
    }
  }

  CCI_RETURN_FAIL();
}
//---------------------------------------------------------------------
// Method: PropertyValue
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Graphics, PropertyValue  )
{
  CCI_INSTANCE( WINGraphics )

  const char_t* propertyName = CCI_P(propertyName);
  if(EMPTY_STRING(propertyName)) 
    CCI_RETURN_FAIL()

  if(CCI_P(me)==CCI_ME_SET)
    return WINGraphics_SetOption(_This,propertyName,CCI_P(value));

  if(CCI_P(me)==CCI_ME_GET)
    return WINGraphics_GetOption(_This,propertyName,CCI_RETVAL_P(value));

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: GetPropertyValues
// Return all possible values for the given properties
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Graphics, GetPropertyValues  )
{
  CCI_UNUSUED_INSTANCE( WINGraphics )

  const char_t* propertyName = CCI_P(propertyName);
  if(EMPTY_STRING(propertyName)) 
    CCI_RETURN_FAIL()

  dmIStringList _Strings;
  if(!_Strings.QueryInterface(CCI_P(valueList)))
    CCI_RETURN_FAIL()

  _Strings.Clear();

  if(EQUAL_NOCASE_STRING(propertyName,_TXT("DoubleBuffering"))) 
  {
    _Strings.AddTail("no");
    _Strings.AddTail("yes");
    CCI_RETURN_OK()     
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method : ProcessSystemEvent (Overloaded)
//---------------------------------------------------------------------
#define EVT_TYPE(evtType) static_cast<dmEventType>(evtType)
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Graphics, ProcessSystemEvent )
{
  CCI_INSTANCE( WINGraphics )

  ::GetCursorPos(&_This->_Screen);

  if(CCI_P(strType)==evStruct_MouseEvent && 
     _This->_Graphics.GetPageScale() != 100)
  {
    HWND hwnd = _This->_Graphics.GetWindow();
    HDC  hdc  = ::GetWindowDC(hwnd);
    if(hdc!=NULL) 
    {
      if(_This->_Graphics.InitGraphics(hdc)) 
      {
        POINT p = { CCI_P(cursorX), CCI_P(cursorY) };

        ::DPtoLP(hdc,&p,1);

        CCI_P(cursorX) = p.x;
        CCI_P(cursorY) = p.y;
      }
      ::ReleaseDC(hwnd,hdc);
    }
  }

  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// Method : SetCursorType (Overloaded)
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Graphics, SetCursorType )
{
  CCI_INSTANCE( WINGraphics )
  
 ::GetCursorPos(&_This->_Screen);
    
 dmIGraphics _IGraphics;
 if(_IGraphics.QueryInterface(_THIS_OBJECT)) 
 {
   dm_param cursor = CCI_P(cursor); 
  
   dmIDocument _IDoc;
   if(_IDoc.QueryInterface(_IGraphics.GetDocument())) {
      _IDoc.SetCursorType(cursor);
      
      if(cursor == 0) 
      {
        dmTool* target = reinterpret_cast<dmTool*>(_IDoc.GetManager());
        if(target) {
          dmEvent theEvent;
          dmEventHandler::InitEvent(NULL,theEvent,evNoEvents,dmEVENT_FLAG_INIT);
          target->ProcessEvent(theEvent);
          cursor = theEvent.mCursorType;
        }
      }      
      _IDoc.Release();
   }
   
   if(cursor!=eCursor_undefined) 
   {
     // Update  only if cursor is located over the current window
     HWND hwnd = _This->_Graphics.GetWindow();
     if(hwnd) {
       POINT p;
       ::GetCursorPos(&p);
       if(::WindowFromPoint(p)==hwnd) {
         _This->_Graphics.SetCursor(static_cast<ECursorType>(cursor));
         _This->_Graphics.UpdateCursor();
       }
     }
   }
 }
 CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: PrintDocument
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Graphics, PrintDocument ) 
{
  CCI_INSTANCE( WINGraphics )

  if(EMPTY_STRING(CCI_P(output)))
     CCI_RETURN_INVALID_PARAM()

  CCI_METHOD _Result = CCI_ERR_METHOD_FAILED;

  dmIGraphics _IGraphics;
  if(_IGraphics.QueryInterface(_THIS_OBJECT)) 
  {
   dmIDocument _IDoc;
   if(_IDoc.QueryInterface(_IGraphics.GetDocument())) 
   {
     dmSurface gc(&_This->_Graphics);
  
     if(gc) 
     {
       RECT    rc;
       HDC     hdcMem,hdc;
       HGDIOBJ hbmOld;

       // Init the bounding box
       RECT rc2;

       rc2.left    = CCI_P(left);
       rc2.right   = CCI_P(right);
       rc2.top     = CCI_P(top);
       rc2.bottom  = CCI_P(bottom);
       
       //dm_uint docWidth  = _IDoc.Width();    
       //dm_uint docHeight = _IDoc.Height(); 

       if(rc2.left >= rc2.right) {
          rc2.left  = 0;
          rc2.right = _IDoc.Width() - 1;
       }

       if(rc2.top >= rc2.bottom) {
          rc2.top    = 0;
          rc2.bottom = _IDoc.Height() - 1;
       }
     
       // Get the size of the client rectangle.
       ::SetRect(&rc,rc2.left,rc2.top,rc2.right,rc2.bottom);

       // Disable zoom an UI
       dm_uint pageScale = _This->_Graphics.GetPageScale();
       bool    UIEnabled = _This->_Graphics.UIEnabled;
       
       _This->_Graphics.UIEnabled = false;
       _This->_Graphics.SetPageScale(100);

       // Create a compatible DC
       hdc    = _This->_Graphics.GetDC();
       hdcMem = ::CreateCompatibleDC(hdc);

       dmWinNativeImage _NativeImage;
       _NativeImage.CreateBitmap(rc.right-rc.left,rc.bottom-rc.top,
                                 dmPixelFormat24bppRGB);
                                       
       // Select the bitmap into the off-screen DC.
       hbmOld = ::SelectObject(hdcMem,_NativeImage.Bitmap());

       // Set off-screen DC as default.
       HDC savedGraphics = _This->_Graphics.SetGraphics(hdcMem);

        // TODO: Handling correctly the background \see Draw()       
       _IDoc.Draw(CCIV_Document_DrawBckgnd); // Draw background 
       _IDoc.Draw(CCIV_Document_DrawNormal); // Draw foreground

       // Done with off-screen bitmap and DC.
       _This->_Graphics.Restore(savedGraphics);

       // Restore initial settings
       _This->_Graphics.UIEnabled = UIEnabled;
       _This->_Graphics.SetPageScale(pageScale); 

       ::SelectObject(hdcMem, hbmOld);
       ::DeleteDC(hdcMem);           
      
        _IDoc.Release(); // XXX : IMPORTANT !!!!!
      
       //=========================
       // Send image to output
       //=========================

       dmLink<dmImage> _Img;
       int             _DocErr;

       // Image is compatible : share data
       dmImageData _ImageData;
       _NativeImage.LockBits(dmPixelFormat24bppRGB,_ImageData,ELockRead);
        
       dmImageDescriptor* _Desc = dmGetDescriptor(_ImageData.PixelFormat);
       if(_Desc!=NULL)
          _Img = _Desc->CreateImage(_ImageData);
       
       if(!_Img.IsNull())
       {
          // Output the image
         if((_DocErr=dmSaveDocument(CCI_P(output),_Img))==ERR_DOC_NOERROR) 
         {
           _Result = CCI_ERR_OK;
         } else
           dmLOG("PrintDocument Error: dmSaveDocument returned %d\n",_DocErr); 
       } else
         dmLOG("Cannot create image !\n");  
         
       _NativeImage.UnlockBits(_ImageData);
     } 
   }
  }  
  return _Result;
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( WINGraphics )
  CCI_REGISTER_METHOD_PP(Graphics, Draw               )
  CCI_REGISTER_METHOD_PP(Graphics, ProcessSystemEvent ) 
  CCI_REGISTER_METHOD_PP(Graphics, PropertyValue      ) 
  CCI_REGISTER_METHOD_PP(Graphics, GetPropertyValues  )
  CCI_REGISTER_METHOD_PP(Graphics, InitGraphics       )
  CCI_REGISTER_METHOD_PP(Graphics, SetDisplay         )
  CCI_REGISTER_METHOD_PP(Graphics, SetCursorType      )
  CCI_REGISTER_METHOD_PP(Graphics, PrintDocument      )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------
// Special init handlers
//---------------------------------------------------------
CCI_METHOD CCI_Init(struct CCI_Custom_Hook* cci_Hook)
{
  if(CCI_INIT_API()) 
  {
    // Get the actual module handle from CCI_Custom_Hook structure
    // Used to fetch resources

    dmWinGraphics::mhModuleDll = static_cast<HINSTANCE>(cci_Hook->cc_Module);
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
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( WINGraphics )
{
  CCI_INVOKE_FACTORY(WINGraphics)
}
//---------------------------------------------------------------------
