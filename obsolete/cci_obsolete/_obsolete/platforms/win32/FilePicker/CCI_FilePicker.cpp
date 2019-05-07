#define dmUseCCI
#define dmUseUtilitiesExtra
#include "daim_utilities.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_StringList.h"
#include "daim_modules/CCI_LoaderFactory.h"
#include "daim_modules/CCI_PropertyList.h"
#include "daim_modules/CCI_Loader.h"
#include "daim_modules/CCI_FilePicker.h"
//------------------------------------------------------------------
#define CCIC_NAME  CCIC_FilePicker
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_FilePicker_Version
#define CCI_REVISION  1
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))
//--------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"

#include "daim/daim_base.h"
#include "daim/daim_interfaces.h"

#include "shlobj.h"
#include <vector>

#define FILE_BUFFER_SIZE 4096 
#define wstring dmString
#define wchar   char_t

//#define _LPSTR  LPWSTR 
//#define _LPCSTR LPCWSTR 
#define _LPSTR  LPSTR 
#define _LPCSTR LPCSTR 

#define modeGetFolder    CCIV_FilePicker_ModeGetFolder
#define modeOpen         CCIV_FilePicker_ModeOpen         
#define modeOpenMultiple CCIV_FilePicker_ModeOpenMultiple 
#define modeSave         CCIV_FilePicker_ModeSave         

//--------------------------------------------------------------------
CCI_DECL_DATA( FilePicker )
{
  HWND       hParent;
  dmString   File;
  dmString   Urn;
  dmString   DefaultFileName;
  dmString   DisplayDirectory;
  dmString   DefaultExtension;
  dmString   FilterList;
  dmString   LastUsedDirectory;
  dm_uint    SelectedType;
  dm_bool    DirChanged;
  dm_bool    FileChanged;

  std::vector<dmString> Mimes; // For storing mimes types
  std::vector<dmString> Files; // Returned list of files

};
//--------------------------------------------------------------------
dm_uint  FilePicker_GetFileType(CCI_INSTANCE_PTR(FilePicker) _This, const char_t* _ext )
{
  dmIGetService<dmILoaderFactory> _Factory("daim://");
 
  const char_t* _mime = _Factory.GetMimeType((dmStrStream(".") << _ext).CStr());
  if(_mime) {
    for(int i=_This->Mimes.size();--i>=0;) {
      if(_This->Mimes[i] == _mime) {
         return i+1;
      }
    }
  }
  return _This->SelectedType;
}
//--------------------------------------------------------------------
// Append files filter
//--------------------------------------------------------------------
void  FilePicker_AddFilter(CCI_INSTANCE_PTR(FilePicker) _This, 
                           const char_t* _title, const char_t* _filter )
{
   _This->FilterList.Append(_title);
   _This->FilterList.Append(_TXT("|"));

   wstring filter(_filter);
   filter.Trim();
   filter.ReplaceString(_TXT("|"),_TXT(";*."));
   if(filter == _TXT("*")) 
     filter.Append(_TXT(".*"));
   else
    if(filter.Compare(0,2,_TXT("*.")) != 0) filter.Insert(0,_TXT("*."));

   _This->FilterList.Append(filter);
   _This->FilterList.Append(_TXT("|"));
}
//--------------------------------------------------------------------
// Add filters from daim mime infos
//--------------------------------------------------------------------
void FilePicker_AddMimes( CCI_INSTANCE_PTR(FilePicker) _This, dm_uint mode )
{
  dmIGetService<dmILoaderFactory> _Factory("daim://");
  dmIInterface<dmIPropertyList>   _Mimes;

  dm_uint flags = (mode == modeOpen || mode == modeOpenMultiple ?
                     CCIV_Loader_Read : CCIV_Loader_Write );

  _This->Mimes.clear();

  // Open a "local" propertylist ( i.e that is not associated 
  // to an external source )
  dm_param key = _Mimes.Open("local://",NULL);
  if(key) {
     // Retrieve mime informations
     if(_Factory.GetMimeInfos(NULL,key,_Mimes,flags))
     {
        wstring all;

        // Get the sequence
        if(_Mimes.GetSeq(key,NULL)) {
          while(_Mimes.NextSeq(key)) 
          {
             const char_t* _type  = _Mimes.GetPropertyStr(key,NULL,_TXT("media-types:type")       ,NULL);
             const char_t* _files = _Mimes.GetPropertyStr(key,NULL,_TXT("media-types:files")      ,NULL);
             const char_t* _title = _Mimes.GetPropertyStr(key,NULL,_TXT("media-types:description"),NULL);

             _This->Mimes.push_back(_type); // Store mime type

             FilePicker_AddFilter(_This,_title,_files);
             if(flags == CCIV_Loader_Read) {
                all.Append(_TXT("|"));
                all.Append(_files);
             }
          }
          _Mimes.EndSeq(key);
        }

        // Add "All images" filter
        if((flags == CCIV_Loader_Read) && !all.Empty()) 
        {
           all.ReplaceString(_TXT("|"),_TXT(";*."));
           all.TrimLeft(_TXT(";"));
           all.Insert(0,"Fichier images|");
           all.Append(_TXT("|"));
           _This->FilterList.Insert(0,all);
        }
     } 
  }
}
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( FilePicker )
{
  CCI_GENERIC_CONSTRUCT(FilePicker)

  _This->SelectedType = 1;
  _This->DirChanged   = dm_false;
  _This->FileChanged  = dm_false;
  _This->hParent      = NULL;
  _This->Urn          = _TXT("file://");

  CCI_BEGIN_INIT_MAP(FilePicker)
    CCI_ATTR_SET(FilePicker,SelectedType   , _This->SelectedType     = CCIA_DATA )
    CCI_ATTR_SET(FilePicker,Parent         , _This->hParent          = CCIA_TRANSLATE_DATA(HWND) )   
    CCI_ATTR_SET(FilePicker,InitialDir     , _This->DisplayDirectory = CCIA_TRANSLATE_DATA(char_t*) )
    CCI_ATTR_SET(FilePicker,DefaultFileName, _This->DefaultFileName  = CCIA_TRANSLATE_DATA(char_t*) )
    CCI_ATTR_SET(FilePicker,DefaultExt     , _This->DefaultExtension = CCIA_TRANSLATE_DATA(char_t*) )
  CCI_END_INIT_MAP()

  CCI_RETURN_THIS()
}
//--------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( FilePicker )
{
  CCI_GENERIC_DESTROY(FilePicker)
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( FilePicker )
  CCI_ATTR_GET(FilePicker,FileChanged    , CCIA_RETVAL = _This->FileChanged  )
  CCI_ATTR_GET(FilePicker,DirChanged     , CCIA_RETVAL = _This->DirChanged   )
  CCI_ATTR_GET(FilePicker,SelectedType   , CCIA_RETVAL = _This->SelectedType )
  CCI_ATTR_GET(FilePicker,Count          , CCIA_RETVAL = _This->Files.size() )
  CCI_ATTR_GET(FilePicker,Parent         , CCIA_RETVAL = CCIA_TRANSLATE_VAL(_This->hParent) )
  CCI_ATTR_GET(FilePicker,InitialDir     , CCIA_RETVAL = CCIA_TRANSLATE_VAL(_This->DisplayDirectory.CStr()) )
  CCI_ATTR_GET(FilePicker,DefaultFileName, CCIA_RETVAL = CCIA_TRANSLATE_VAL(_This->DefaultFileName.CStr() ) )
  CCI_ATTR_GET(FilePicker,DefaultExt     , CCIA_RETVAL = CCIA_TRANSLATE_VAL(_This->DefaultExtension.CStr()) )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( FilePicker )
  CCI_ATTR_SET(FilePicker,FileChanged    , _This->FileChanged      = CCIA_DATA )
  CCI_ATTR_SET(FilePicker,DirChanged     , _This->DirChanged       = CCIA_DATA )
  CCI_ATTR_SET(FilePicker,SelectedType   , _This->SelectedType     = CCIA_DATA )
  CCI_ATTR_SET(FilePicker,InitialDir     , _This->DisplayDirectory = CCIA_TRANSLATE_DATA(char_t*) )
  CCI_ATTR_SET(FilePicker,DefaultFileName, _This->DefaultFileName  = CCIA_TRANSLATE_DATA(char_t*) )
  CCI_ATTR_SET(FilePicker,DefaultExt     , _This->DefaultExtension = CCIA_TRANSLATE_DATA(char_t*) )
  CCIA_DISCARD(FilePicker,Parent) 
  CCIA_DISCARD(FilePicker,Count) 
CCI_END_SETTER()
//---------------------------------------------------------------------
// Method: Show
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( FilePicker, Show )
{
  CCI_INSTANCE( FilePicker )

  _This->Files.clear();
  _This->File.Clear();

  dm_uint mode          = CCI_P(mode);
  dm_bool result        = dm_false;
  dm_bool clear_filters = dm_false;

  wchar   fileBuffer[FILE_BUFFER_SIZE+1];
  const wchar* title  = CCI_P(title) ?  CCI_P(title) : "" ;

  if(CCI_P(defaultFileName)!=NULL) _This->DefaultFileName.Assign(CCI_P(defaultFileName));
  if(CCI_P(dfltextension)  !=NULL) _This->DefaultExtension.Assign(CCI_P(dfltextension));

  // UNICODE not supported up to now
  // wcsncpy(fileBuffer,  mDefault.get(), FILE_BUFFER_SIZE);
  _tcsncpy(fileBuffer,_This->DefaultFileName.CStr(),FILE_BUFFER_SIZE);

  wstring initialDir;
  initialDir = _This->DisplayDirectory;
  initialDir.TrimRight(_TXT(" \t\n\\/")); 

  // If no display directory, re-use the last one.
  if(initialDir.Empty()) {
    initialDir = dmPortability::GetCurrentDir();
  }

  initialDir.TrimRight(_TXT(" \t\n\\/")); 

  if (mode == modeGetFolder) 
  {
    wchar dirBuffer[MAX_PATH+1];
    // UNICODE not supported up to now
    //wcsncpy(dirBuffer, initialDir.get(), MAX_PATH);
    strncpy(dirBuffer,initialDir.CStr(),MAX_PATH);

    //BROWSEINFOW browserInfo;
    BROWSEINFO  browserInfo;
    browserInfo.hwndOwner      = _This->hParent;
    browserInfo.pidlRoot       = NULL;
    browserInfo.pszDisplayName = (_LPSTR)dirBuffer;
    browserInfo.lpszTitle      = title;
    browserInfo.ulFlags        = BIF_RETURNONLYFSDIRS;//|BIF_STATUSTEXT|BIF_RETURNONLYFSDIRS;
    browserInfo.lpfn           = NULL;
    browserInfo.iImage         = 0;
    browserInfo.lParam         = 0;

    // XXX UNICODE support is needed here --> DONE
    LPITEMIDLIST list = SHBrowseForFolder(&browserInfo);
    if (list != NULL) {
      result = SHGetPathFromIDList(list, (_LPSTR)fileBuffer);
      if (result) {
        _This->File   = fileBuffer;
        _This->Files.push_back(_This->File);
      }
  
      // free PIDL
      LPMALLOC pMalloc = NULL;
      ::SHGetMalloc(&pMalloc);
      if(pMalloc) {
         pMalloc->Free(list);
         pMalloc->Release();
      }
    }
  }
  else {

    //OPENFILENAMEW ofn;
    OPENFILENAME ofn;
    memset(&ofn, 0, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);

    if(_This->FilterList.Empty()) {
       FilePicker_AddMimes(_This,mode);
       clear_filters = dm_true;
    } else
      _This->Mimes.clear(); // Clear the mimes type;

    wchar* filterBuffer = strdup(_This->FilterList.CStr());
    LPTSTR pch  = filterBuffer;
    while ((pch = strchr(pch, '|')) != NULL) *pch++ = '\0';
         
    if(clear_filters) _This->FilterList.Clear();

    if (!initialDir.Empty()) {
      ofn.lpstrInitialDir = initialDir.CStr();
    }
    
    ofn.hwndOwner    = (HWND)_This->hParent;
    ofn.lpstrTitle   = (_LPCSTR)title;
    ofn.lpstrFilter  = (_LPCSTR)filterBuffer;
    ofn.nFilterIndex = FilePicker_GetFileType(_This,_This->DefaultExtension.CStr());
    ofn.lpstrFile    = fileBuffer;
    ofn.nMaxFile     = FILE_BUFFER_SIZE;

    ofn.Flags = OFN_NOCHANGEDIR | OFN_SHAREAWARE | OFN_LONGNAMES | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;

    if (!_This->DefaultExtension.Empty()) {
      ofn.lpstrDefExt = _This->DefaultExtension.CStr();
    }
    else {
      // Get file extension from suggested filename
      wstring ext = _This->DefaultFileName.FileExt();
      if ( !ext.Empty() ) 
      {
        // This is supposed to append ".htm" if user doesn't supply an extension
        //XXX Actually, behavior is sort of weird:
        //    often appends ".html" even if you have an extension
        //    It obeys your extension if you put quotes around name
        ofn.lpstrDefExt = ext.CStr();
      }
    }

    if (mode == modeOpen) {
      // FILE MUST EXIST!
      ofn.Flags |= OFN_FILEMUSTEXIST;
      result = ::GetOpenFileName(&ofn);
    }
    else if (mode == modeOpenMultiple) {
      ofn.Flags |= OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER;
      result = ::GetOpenFileName(&ofn);
    }
    else if (mode == modeSave) {
      ofn.Flags |= OFN_NOREADONLYRETURN;
      result = ::GetSaveFileName(&ofn);
      if (!result) {
        // Error, find out what kind.
        if (::GetLastError() == ERROR_INVALID_PARAMETER ||
            ::CommDlgExtendedError() == FNERR_INVALIDFILENAME) {
          // probably the default file name is too long or contains illegal characters!
          // Try again, without a starting file name.
          ofn.lpstrFile[0] = 0;
          result = ::GetSaveFileName(&ofn);
        }
      }
    }
    else 
      CCI_SET_ERROR_MSG(_TXT("Unsupported mode !"));
  
    // Remember what filter type the user selected
    _This->SelectedType = ofn.nFilterIndex;

    // Set user-selected location of file or directory
    if (result == TRUE) {
      if (mode == modeOpenMultiple) 
      {        
        // from msdn.microsoft.com, "Open and Save As Dialog Boxes" section:
        // If you specify OFN_EXPLORER,
        // The directory and file name strings are NULL separated, 
        // with an extra NULL character after the last file name. 
        // This format enables the Explorer-style dialog boxes
        // to return long file names that include spaces. 
        wchar *current = fileBuffer;
        
        wstring dirName(current);
        // sometimes dirName contains a trailing slash
        // and sometimes it doesn't.
        if (current[dirName.Size() - 1] != '\\')
          dirName.Append("\\");
        
        _This->LastUsedDirectory = dirName;

        while (current && *current && *(current + strlen(current) + 1)) {
          current = current + strlen(current) + 1;          
          _This->Files.push_back(dmStrStream(_This->Urn) << dirName << current);
        }
        
        // handle the case where the user selected just one
        // file.  according to msdn.microsoft.com:
        // If you specify OFN_ALLOWMULTISELECT and the user selects 
        // only one file, the lpstrFile string does not have 
        // a separator between the path and file name.
        if (current && *current && (current == fileBuffer)) {          
          _This->Files.push_back(dmStrStream(_This->Urn) << current);
        }
      }
      else 
      {
        _This->File = fileBuffer;
        if (mode == modeSave  && _This->FilterList.Empty())
        {
          dmIGetService<dmILoaderFactory> _Factory("daim://");
 
          const char_t* _file_mime   = _Factory.GetMimeType(_This->File.CStr());
          const char_t* _select_mime = _This->Mimes[_This->SelectedType - 1].CStr();

          if(_file_mime==NULL) {           
             const char_t* _ext = _Factory.GetMimeExt(_select_mime);
             // Add the extension matching the selected type
             _This->File.Append(".");
             _This->File.Append(_ext);
          } 
 
          _This->DefaultExtension = _This->File.FileExt();
        }
        _This->Files.push_back(dmStrStream(_This->Urn) << _This->File);
      }
    }

    free(filterBuffer);

  }


  if (result) 
  {
    // Remember last used directory.
    wstring newDir = _This->File.FilePath();
    if(!newDir.Empty())
      _This->LastUsedDirectory = newDir;
    // Update mDisplayDirectory with this directory, also.
    // Some callers rely on this.
    _This->DisplayDirectory = _This->LastUsedDirectory;

    if (mode == modeSave) {
      // Windows does not return resultReplace,
      // we should (must ?) check if file already exists
    }

    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: GetFile
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( FilePicker, GetFile )
{
  CCI_INSTANCE( FilePicker )

  if(CCI_P(index) < _This->Files.size() ) {
    CCI_RETVAL_P(path) = _This->Files[CCI_P(index)].CStr();
    CCI_RETURN_OK()
  }
  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method: GetFileList
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( FilePicker, GetFileList )
{
  CCI_INSTANCE( FilePicker )

  dmIStringList _IStr;
  if(_IStr.QueryInterface(CCI_P(stringlist))) {
    for(dm_uint i=0;i<_This->Files.size();++i)
      _IStr.AddTailA(_This->Files[i]);

    CCI_RETURN_OK()
  }
  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method: AddFilter
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( FilePicker, AddFilter )
{
  CCI_INSTANCE( FilePicker )

  if(EMPTY_STRING(CCI_P(title))||EMPTY_STRING(CCI_P(filter)))
     CCI_RETURN_INVALID_PARAM()

  FilePicker_AddFilter(_This,CCI_P(title),CCI_P(filter));
  CCI_RETURN_OK()
}

//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( FilePicker )
  CCI_REGISTER_METHOD_PP(FilePicker,Show        )
  CCI_REGISTER_METHOD_PP(FilePicker,GetFile     )
  CCI_REGISTER_METHOD_PP(FilePicker,GetFileList )
  CCI_REGISTER_METHOD_PP(FilePicker,AddFilter   )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( FilePicker )
{
  DM_INIT_API()
  CCI_INVOKE_FACTORY(FilePicker)
}
//---------------------------------------------------------
