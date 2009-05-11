#define dmUseCCI
#define dmUseUtilitiesExtra
#include "daim_utilities.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_StringList.h"
#include "daim_modules/CCI_PropertyList.h"

#include "daim_modules/CCI_Clipboard.h"
//------------------------------------------------------------------
#define CCIC_NAME  CCIC_Clipboard
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_Clipboard_Version
#define CCI_REVISION  1
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2005"))
//--------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"

#include "daim/daim_base.h"
#include "daim/daim_interfaces.h"

//#define _LPSTR  LPWSTR 
//#define _LPCSTR LPCWSTR 
#define _LPSTR  LPSTR 
#define _LPCSTR LPCSTR 

//--------------------------------------------------------------------
CCI_DECL_DATA( Clipboard )
{
  dm_uint _Dummy;
};
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( Clipboard )
{
  CCI_GENERIC_CONSTRUCT(Clipboard)
  CCI_RETURN_THIS()
}
//--------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( Clipboard )
{
  CCI_DESTROY_SUPER()
}
/*
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( Clipboard )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( Clipboard )
CCI_END_SETTER()
*/
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( Clipboard )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( Clipboard )
{
  CCI_RETURN_INVOKE_SUPER()
}
/*
//---------------------------------------------------------------------
// Method: GetFile
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Clipboard, SetImageData )
{
  CCI_INSTANCE( Clipboard )
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: GetFileList
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Clipboard, GetImageData )
{
  CCI_INSTANCE( Clipboard )
  CCI_RETURN_FAIL()
}
*/

static inline CCI_IMPL_METHOD_NN( Clipboard, IsDataAvailable )
{
  if(::IsClipboardFormatAvailable(CF_DIB))
    CCI_RETURN_OK()

  CCI_RETURN_FAIL()
}

//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( Clipboard )
  CCI_REGISTER_METHOD_NN(Clipboard,IsDataAvailable)
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------

// In dmClipboardHandler.cpp
void dmRegisterClipboardHandler();

dm_param Clipboard_Initialize(cci_Class* cl,void* _UserData)
{
  dmRegisterClipboardHandler();
  CCI_Flush(cl,~0);  // Make the class persistent (not flushable)

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
void Clipboard_Finalize(cci_Class* cl,void* _UserData) 
{ 
  // TODO: Should deregister handler 
}
//---------------------------------------------------------------------
CCI_ENTRY_POINT( Clipboard )
{
  DM_INIT_API()
  CCI_INVOKE_FACTORY_(Clipboard,NULL,Clipboard_Initialize,Clipboard_Finalize);
}
//---------------------------------------------------------
