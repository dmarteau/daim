/* ::: BEGIN LICENSE BLOCK:::
 * Copyright (c) 2005 David Marteau
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

//--------------------------------------------------------------------
// File         : dmSnapshot.cpp
// Date         : 29/06/2005
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//--------------------------------------------------------------------

#define dmUseKernelImageTemplates
#include "daim_kernel.h"
#include "daim/daim_base.h"
#include "daim/daim_interfaces.h"

#include "images/dmGenericLoader.h"
#include "documents/dmDocFactory.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_LoaderFactory.h"

#include "dmSnapshot.h"

#include "templates/processing/dmLinearStretch.h"

#define IDEAL_IMAGE_WIDTH   768
#define IDEAL_IMAGE_HEIGHT  576

//----------------------------------------------------------------
dmSnapshot::dmSnapshot()
: m_Magnification(0.12f)
, m_BaseFormat(dmPixelFormatUndefined)
, mStatus(0)
, mDecoder(NULL)
, mEncoder(NULL)
{}
//----------------------------------------------------------------
dmSnapshot::~dmSnapshot() 
{}
//-----------------------------------------------------------------
void dmSnapshot::SetImagePath( const char_t* _path )
{
  mStatus = 0;
  m_CacheImagePath.Clear();

  const char_t* _sep = dmPortability::PathSeparator();

  m_BaseImagePath = _path;
  m_BaseImagePath.ToNativePath();

  // We compute a base name for the cache file
  unsigned long _hash = m_BaseImagePath.Hash();
  dmString _basename  = m_BaseImagePath.FileName();


  //const char_t* dbgstr;

  //dbgstr = m_BaseImagePath.CStr();
  //dbgstr = _basename.CStr();

  _basename.ReplaceString(_TXT(" ") ,1,_TXT("%20"),3);
  _basename.ReplaceString(_TXT(".") ,1,_TXT("%2e"),3);
  _basename.ReplaceString(_TXT(":") ,1,_TXT("%3a"),3);
  _basename.ReplaceString(_TXT("\""),1,_TXT("%22"),3);
  _basename.ReplaceString(_TXT("`") ,1,_TXT("%60"),3);
  _basename.ReplaceString(_TXT("'") ,1,_TXT("%27"),3);

  //dbgstr = _basename.CStr();

  m_CacheName.Format(dmString::npos,
       "%c%s%08lX%s%lu",'A'+_hash % 26,_sep,
        _hash,_basename.CStr(),
        m_BaseImagePath.Size());

  //dbgstr = m_CacheName.CStr();

  mStatus = 1;
}
//-----------------------------------------------------------------
dmGenericLoader* dmSnapshot::SetEncoder( dmGenericLoader* _Encoder )
{
  dmGenericLoader* _OldLoader = mEncoder;
  mEncoder = _Encoder;

  if(mEncoder != NULL) 
  {
    // Look for the mime extension associated to this mime type
    dmIGetService<dmILoaderFactory> _Factory("daim://");

    const char_t* _ext = _Factory.GetMimeExt(mEncoder->MimeType().CStr());

    mFileExt.Assign(".");
    mFileExt.Append(_ext);
  }
  else
    mFileExt.Clear();

  return _OldLoader;
}
//-----------------------------------------------------------------
dmGenericLoader* dmSnapshot::SetDecoder( dmGenericLoader* _Decoder )
{
  dmGenericLoader* _OldLoader = mDecoder;
  mDecoder = _Decoder;

  return _OldLoader;
}
//-----------------------------------------------------------------
bool dmSnapshot::Save()
{
  if(mEncoder == NULL)
     return false;

  bool res = false;

  dmTRY
    dmLink<dmImage> _src = GetSourceImage();
    if(!_src.IsNull())
    {
      m_BaseSize.x = _src->Width();
      m_BaseSize.y = _src->Height();

      int _width  = static_cast<dm_int>((IDEAL_IMAGE_WIDTH  * m_Magnification + 0.5));
      int _height = static_cast<dm_int>((IDEAL_IMAGE_HEIGHT * m_Magnification + 0.5));

      //---------------------------------------------
      // Convert image to 8 bits
      //---------------------------------------------
      EPixelFormat _Format = _src->PixelFormat();

      if(dmIsPixelFormatScalar(_Format) && 
         _Format != dmPixelFormat8bppIndexed )
      {
        dmImageDescriptor* _descriptor = dmGetDescriptor(dmPixelFormat8bppIndexed);
        dmASSERT( _descriptor != NULL );
        _src = _src->CreateCopy(*_descriptor);
      }
 
      if(_src->Width() > _src->Height()) {
         _height = static_cast<int>(_width *
                   static_cast<dm_real>(_src->Height())/_src->Width() + 0.5);
      } else {
         _width  = static_cast<int>(_height *
                   static_cast<dm_real>(_src->Width())/_src->Height() + 0.5);
      }

      dmLink<dmImage> snapshot = _src->TypeDescriptor()->CreateImage(_width,_height);
 
      //===================
      // Stretch Copy
      //===================
      switch( _src->PixelFormat() ) 
      {
         case dmPixelFormat8bppIndexed: {

           dmIImage<dmPixelFormat8bppIndexed>* src = dmIImage<dmPixelFormat8bppIndexed>::Cast(_src);
           dmIImage<dmPixelFormat8bppIndexed>* snp = dmIImage<dmPixelFormat8bppIndexed>::Cast(snapshot);

           daim::nn_stretch(src->Gen(),src->Rect(),snp->Gen());
         } break;

         case dmPixelFormat24bppRGB: {
           dmIImage<dmPixelFormat24bppRGB>* src = dmIImage<dmPixelFormat24bppRGB>::Cast(_src);
           dmIImage<dmPixelFormat24bppRGB>* snp = dmIImage<dmPixelFormat24bppRGB>::Cast(snapshot);

           daim::nn_stretch(src->Gen(),src->Rect(),snp->Gen());            
         } break;

         default :
           dmLOG("SNAPSHOT:ERROR: Invalid image format !\n");  
           return false;
      }
      //===================
      // Save image
      //===================

      dmString  _Fmt;

      dmFILE_BUFFER_INFO bfi;
     
      bfi.fbi_name  = m_CacheImagePath;
      bfi.fbi_index = 0;     // this is the first/default image in file
      bfi.fbi_count = 1;     // just one record

      bfi.fbi_flags = dmFBI_NOCOMPRESSION|dmFBI_TEXT;
      bfi.fbi_text  = NULL;

      mEncoder->SetText(NULL,0); // Reset text
      mEncoder->SetText("image:width" ,(_Fmt.Format(dmString::npos,"%ld",_src->Width()) ,_Fmt.CStr()));
      mEncoder->SetText("image:height",(_Fmt.Format(dmString::npos,"%ld",_src->Height()),_Fmt.CStr()));
      mEncoder->SetText("image:format",(_Fmt.Format(dmString::npos,"%ld",_Format)       ,_Fmt.CStr()));
      mEncoder->SetText("image:file"  , m_BaseImagePath.CStr());

      //const char_t* dbgstring = m_CacheImagePath.CStr();

      res = mEncoder->BufferToDisk(snapshot,bfi);
      
      if(!res)
          dmLOG(_TXT("ERROR: 'BufferToDisk' failed !\n"));

      mEncoder->Close();
      mStatus = 2;
    }
  dmRETHROW_ERROR
  return res;
}
//----------------------------------------------------------------
bool dmSnapshot::UpdateImageCache()
{
  dmTRY
    const dmString& _CacheBaseName = GetCacheName();

    m_CacheImagePath.Clear();

    if(!_CacheBaseName.Empty()) 
    { 
      const char_t* _sep = dmPortability::PathSeparator();

      dmString _CacheDir(dmEnvironment::LibCache());
      dmPortability::CreateDir(_CacheDir.Append(_CacheBaseName).FilePath().CStr());
      m_CacheImagePath = _CacheDir; 
      m_CacheImagePath.ReplaceString(_TXT("/") ,1,_sep,_tcslen(_sep));
      m_CacheImagePath.Append(mFileExt);

      //const char_t* dbgstr = mFileExt.CStr();
      //dbgstr = m_CacheImagePath.CStr();

      return Save();
    }
  dmRETHROW_ERROR
  return false;
}
//----------------------------------------------------------------
bool dmSnapshot::LoadCacheInfos()
{
  if(mDecoder == NULL || m_CacheImagePath.Empty())
     return false;

  //===================
  // Load image info from 
  // snapshot data
  //===================

  dmString  _Fmt;

  dmFILE_BUFFER_INFO bfi;
    
  bfi.fbi_name  = m_CacheImagePath.CStr();
  bfi.fbi_index = 0;     // this is the first/default image in file
  bfi.fbi_count = 1;     // just one record
  bfi.fbi_flags = dmFBI_TEXT;
  bfi.fbi_text  = NULL;

  bool rv = mDecoder->GetInfo(bfi);

  if(rv) {

    const char_t* _image_path = mDecoder->GetText("image:file");

    if(!(m_BaseImagePath == _image_path))
       dmLOG("WARNING: path does not match snapshot path:\n\texpecting %s\n\tfound %s\n",
              m_BaseImagePath.CStr(),_image_path);

    m_BaseSize.x = (_Fmt = mDecoder->GetText("image:width" )).AsInt();
    m_BaseSize.y = (_Fmt = mDecoder->GetText("image:height")).AsInt();
    m_BaseFormat = static_cast<EPixelFormat>(
                     (_Fmt = mDecoder->GetText("image:format")).AsUnsigned());
    rv = true;

    mDecoder->Close();
  }

  return rv;
}
//----------------------------------------------------------------
dmSnapshot::EAttemptResult dmSnapshot::AttemptLoadFromCache()
{
  EAttemptResult res = eAttemptFailed;

  const char_t* _sep = dmPortability::PathSeparator();

  dmTRY
    //-------------------------------------------------
    // Test if file exists in cache
    //-------------------------------------------------
    m_CacheImagePath = GetCacheName();
    m_CacheImagePath.Append(mFileExt);
    m_CacheImagePath = dmEnvironment::GetFileInCache(m_CacheImagePath.CStr());
    if(!m_CacheImagePath.Empty())
    {
      m_CacheImagePath.ReplaceString(_TXT("/") ,1,_sep,_tcslen(_sep));

      // Load informations from chache file
      if(LoadCacheInfos()) {
         mStatus = 3;
         res = eAttemptOK;
      }
    } else {  
      //-------------------------------------------------------
      // File is not in cache, try to reconstruct from original 
      //-------------------------------------------------------
      SetImagePath(m_BaseImagePath.CStr());
      res = eNeedUpdateCache;
    }

  dmRETHROW_ERROR
  return res; 
}
//----------------------------------------------------------------
void dmSnapshot::ClearImageCache()
{    
  const dmString& _cacheBaseName = GetCacheName();

  dmString _cachPath = dmEnvironment::GetFileInCache(_cacheBaseName.CStr());
  if(!_cachPath.Empty())
    dmPortability::RemoveFile(_cachPath.CStr()); 

  mStatus = 0;
}
//----------------------------------------------------------------
dmLink<dmImage> dmSnapshot::GetSourceImage() 
{ 
  dmLink<dmImage> _image;

  dmTRY

    m_BaseFormat = dmPixelFormatUndefined;

    if(!m_BaseImagePath.Empty()) 
    {
      const char_t* _url = m_BaseImagePath.CStr();

      int _DocAttributs = 0;
      int _LastDocError = dmOpenDocument(_url,_image,_DocAttributs);
      if(_LastDocError!=ERR_DOC_NOERROR) {
         dmLOG(_TXT("ERROR:INFO: Cannot open %s (error %d)\n"),_url,_LastDocError);
      } else {
        dmASSERT( !_image.IsNull() );
        m_BaseFormat = _image->PixelFormat();
      }
    }
  dmRETHROW_ERROR

  return _image;
}
//----------------------------------------------------------------

