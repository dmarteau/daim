
/* ::: BEGIN LICENSE BLOCK:::
 *
 * Copyright (c) 2004-2005 David Marteau
 *
 * This file is part of the DAIM Image Processing Library (DAIM library)
 *
 * The DAIM library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License (LGPL)
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * The DAIM library is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the DAIM library; see the file LICENSE.  If
 * not, write to the Free Software Foundation, Inc., 59 Temple Place -
 * Suite 330, Boston, MA 02111-1307, USA.
 *
 *  ::: END LICENSE BLOCK::: */

#include "daim_info.h"
#include "documents/dmDocFactory.h"
#include "documents/dmDocHandler.h"

#include "../../include/daim/daim_decls.h"

#define THIS_DOC(me) if(mDocument) mDocument->me

//--------------------------------------------------------------------------
static dmString Resolve_File_URL( int _attributs, const char_t* _path )
{
  if(_attributs & FDOC_FILEURL) {
    dmString _R = _path;
    size_t pos = _R.Find(_TXT("file://"));
    if(pos==dmString::npos) pos = 0; else pos = 7;
    _R.Replace(pos,dmString::npos,dmPortability::FullPath(_R.Begin()+pos));
    return _R;
  }
  return _path;
}
//--------------------------------------------------------------------------
bool daim_info::SaveDocument( const dmString::E* _path, bool bReplace )
{
  if(IsValidView()) {
    if(EMPTY_STRING(_path)|| (mPath==_path)) 
    {
      if(mDocAttrs & FDOC_READONLY) return false;
      _path = mPath.CStr(); 
    }

    mLastDocError = dmSaveDocument(_path,mImage);
    if(mLastDocError!=ERR_DOC_NOERROR) {
       dmLOG(_TXT("ERROR:INFO: Cannot save as %s (error %d)\n"),_path,mLastDocError);
       return false;
    }   
    
    if(bReplace) {
      mDocAttrs = dmGetDocAttributs(_path);
      mPath     = Resolve_File_URL(mDocAttrs,_path);
      SetModified(false);
    }

    return true;

  } else
    dmLOG(_TXT("WARNING:INFO: Empty string passed to SaveDocument\n"));
 
  return false;
}
//--------------------------------------------------------------------------
bool  daim_info::OpenDocument( const dmString::E* _path, bool bReplace  )
{
  if(!EMPTY_STRING(_path))
  {
    int             _DocAttributs = 0;
    dmLink<dmImage> _image;

    mLastDocError = dmOpenDocument(_path,_image,_DocAttributs);
    if(mLastDocError!=ERR_DOC_NOERROR) {
       dmLOG(_TXT("ERROR:INFO: Cannot open %s (error %d)\n"),_path,mLastDocError);
       return false;
    }   

    if(bReplace) {
      mPath      = Resolve_File_URL(_DocAttributs,_path);
      mDocAttrs  = _DocAttributs;
      mbFromDisk = (mDocAttrs & FDOC_PERSISTENT);
    } 

    THIS_DOC(OnNewDocument)(_BOOL(bReplace));

    SetImage(_image);
    SetModified(false);

    THIS_DOC(OnNewDocument)(dm_false);

    return true;

  } else
    dmLOG(_TXT("WARNING:INFO: Empty string passed to OpenDocument\n"));
   
  return false;
}
//--------------------------------------------------------------------------
bool daim_info::RevertToSaved()
{
  if(mbFromDisk) 
  {
    dmASSERT(!mPath.Empty());
    return OpenDocument(  mPath.CStr(), true ); 
  }
  return false;
}
//----------------------------------------------------------------------------
bool daim_info::SaveRegion( const dmString::E* _path, const dmRegion& rgn )
{
  if(IsValidView() && !EMPTY_STRING(_path))
  {
    dmImage* pImage = GetImage();
    if(pImage) 
    {
      dmRegion _rgn = rgn;
      _rgn.ClipToRect(pImage->Rect());
      if(!_rgn.IsEmptyRoi()) 
      {
         dmLink<dmImage>  result = pImage->CreateCopy(_rgn.Rectangle());
         mLastDocError = dmSaveDocument(_path,result);

         if(mLastDocError==ERR_DOC_NOERROR)
           return true;
      }     
    }        
  }
  return false;
}
//------------------------------------------------------
void daim_info::ClearDocument()
{
  mPath.Clear();
  mDocAttrs = 0;
  mLastDocError = ERR_DOC_NOERROR;
}
//------------------------------------------------------
