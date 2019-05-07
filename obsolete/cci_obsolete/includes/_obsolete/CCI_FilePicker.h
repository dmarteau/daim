#ifndef CCI_FilePicker_h
#define CCI_FilePicker_h
//--------------------------------------------------------
// File         : CCI_FilePicker.h
// Date         : 11/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//--------------------------------------------------------
#define CCIC_FilePicker _TXT("filepicker.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( FilePicker, Notify )
  
  CCI_MODULE_UUID( FilePicker, 8179e84d-360c-41f5-b89c-b35ede3b0d16)

  CCI_MODULE_VERSION( FilePicker, 1 ) 

  /* Methods */
  CCI_DECL_METHOD_PP(FilePicker,1, Show    , 
      _in CONST char_t*  title 
      _in dm_uint        mode
      _in CONST char_t*  dfltextension
      _in CONST char_t*  defaultFileName
      _in CONST char_t*  initialdir
  )

  CCI_DECL_METHOD_PP(FilePicker,2, GetFile , 
      _in  dm_uint       index 
      _out CONST char_t* path
  )

  CCI_DECL_METHOD_PP(FilePicker,3, GetFileList , 
      _in dm_cci(StringList) stringlist
  )

  CCI_DECL_METHOD_PP(FilePicker,4, AddFilter ,
      _in CONST char_t* title
      _in CONST char_t* filter
  )

  /* Attributs */
  CCI_DECL_ATTR(FilePicker,1, FileChanged     , dm_bool, [.SGN] )
  CCI_DECL_ATTR(FilePicker,2, DirChanged      , dm_bool, [.SGN] )
  CCI_DECL_ATTR(FilePicker,3, Parent          , dm_ptr , [I...] )
  CCI_DECL_ATTR(FilePicker,4, InitialDir      , char_t*, [ISGN] )
  CCI_DECL_ATTR(FilePicker,5, DefaultFileName , char_t*, [ISGN] )
  CCI_DECL_ATTR(FilePicker,6, SelectedType    , dm_uint, [ISGN] )
  CCI_DECL_ATTR(FilePicker,7, DefaultExt      , char_t*, [ISGN] )
  CCI_DECL_ATTR(FilePicker,8, Count           , dm_uint, [..G.] )

  /* special values */
  #define CCIV_FilePicker_ModeGetFolder     1L
  #define CCIV_FilePicker_ModeOpen          2L 
  #define CCIV_FilePicker_ModeOpenMultiple  3L
  #define CCIV_FilePicker_ModeSave          4L

CCI_END_MODULE_DECL( FilePicker )

#endif //__doxygen

//----------------------------------------------------------------
#ifdef __cplusplus

#include "daim_modules/CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 * Display a file/directory open/save dialog              
 * Notifications can be set when user change the selection
 */
//------------------------------------------------------------------------------------
class dmIFilePicker : public dmINotify
{ 
  public:
   dmDECLARE_GENERIC_IINTERFACE(FilePicker)

  enum Constants {
    ModeGetFolder    = CCIV_FilePicker_ModeGetFolder   ,
    ModeOpen         = CCIV_FilePicker_ModeOpen        ,
    ModeOpenMultiple = CCIV_FilePicker_ModeOpenMultiple,
    ModeSave         = CCIV_FilePicker_ModeSave
  };

  bool Show( 
      CONST char_t*  title, 
      dm_uint        mode,
      CONST char_t*  defaultFileName,
      CONST char_t*  dfltextension    = NULL,
      CONST char_t*  initialdir       = NULL
    )
  {
    CCI_RETURN_IMETHOD_PP_OK(FilePicker,Show, 
        _in title 
        _in mode
        _in dfltextension
        _in defaultFileName
        _in initialdir
    )
  }

  const char_t* GetFile( dm_uint index ) 
  {
    IF_CCI_IMETHOD_RP(FilePicker,GetFile, _in index _out(path) NULL )
    THEN return CCI_RETVAL(path); ENDIF
    return NULL;
  }

  bool GetFileList( dm_cci(StringList) stringlist ) {
    CCI_RETURN_IMETHOD_PP_OK(FilePicker,GetFileList, _in stringlist )
  }

  bool AddFilter( const char_t* title, const char_t* filter ) {
    CCI_RETURN_IMETHOD_PP_OK(FilePicker,AddFilter,_in title _in filter)
  }

  dm_uint Count() { return CCI_IGET(FilePicker,Count,dm_uint(0)); }

};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_FilePicker_h */
