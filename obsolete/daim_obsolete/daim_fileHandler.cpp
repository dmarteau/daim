
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

/* $ChangeLog  22 août 2006 01:09:56 , David
 *
 * Support file protocol url with leading "/" using
 * dmString::ToNativePath() 
 */ 

#define dmUseKernelImage
#define dmUseKernelBuffer
#include "daim_kernel.h"

#include "daim/daim_base.h"
#include "documents/dmDocFactory.h"
#include "documents/dmDocHandler.h"

#include "images/dmGenericLoader.h"

#define DAIM_MEAN_AND_LEAN
#include "daim/daim_modules.h"

//---------------------------------------
class daim_fileHandler : public dmDocHandler
{
  public:
    daim_fileHandler() {}

    virtual ~daim_fileHandler() {}
    virtual int GetAttributs( const dmString::E *_prot, const dmString::E* _args, const dmString::E* _path );
    virtual int OpenDocument( const dmString::E *_prot, const dmString::E* _args, const dmString::E* _path, dmLink<dmImage>& , int& _flags );
    virtual int SaveDocument( const dmString::E *_prot, const dmString::E* _args, const dmString::E* _path, dmImage* );
};
//---------------------------------------
int daim_fileHandler::GetAttributs( const dmString::E *_prot, const dmString::E* _args, const dmString::E* _path )
{
  if(_tcscmp(_prot,_TXT("buffer"))==0) return 0;
  if(_tcscmp(_prot,_TXT("info")  )==0) return FDOC_READONLY;
  if(_tcscmp(_prot,_TXT("file")  )==0) return FDOC_PERSISTENT;
  if(_tcscmp(_prot,_TXT("null")  )==0) return FDOC_READONLY;
  return FDOC_UNKNOWN;
}
//---------------------------------------
int daim_fileHandler::OpenDocument( const dmString::E* _prot, 
                                    const dmString::E* _args, 
                                    const dmString::E* _path, 
                                    dmLink<dmImage>& _img, int& _flags )
{
  dmTRY

    // null document handler : return empty document but
    // with a null source, this is useful to 
    // make the document valid when source are requested for seeking
    // to a particular document.

    if(_tcscasecmp(_prot,_TXT("null"))==0) {
      _img.Release();
      _flags  = FDOC_READONLY;
      return ERR_DOC_NOERROR;
    }

    if(_tcscasecmp(_prot,_TXT("buffer"))==0)
    {
      dmIGetService<dmIImageList> _IBuffers("daim://");
      dmImage* buff = _IBuffers.GetBuffer( _ttol(_args) );
      if(buff!=NULL) {
        _img    = buff->CreateCopy();
        return ERR_DOC_NOERROR;
      } else 
        return ERR_DOC_INVALIDOBJECT;
    } 
    
    //-------------------------------------------------------------
    // Handle File 
    //-------------------------------------------------------------
    _flags |= FDOC_PERSISTENT|FDOC_FILEURL;

    // Get the mime type for the file
    const dmString::E* mime = DM_GetMimeType(_path);
    if(mime) 
    {
      // Get the loader associated this mime type
      dmGenericLoader* pLoader = dmLoaderFactory::GetLoader(mime,dmDECODER);
      if(pLoader)
      {
        dmString nativePath = _path;
        nativePath.ToNativePath();
        
        int Err = ERR_DOC_NOERROR;
        dmFILE_BUFFER_INFO bfi;
        bfi.fbi_name  = nativePath;
        bfi.fbi_index = 0;     // we want the first/default image in file
        bfi.fbi_flags = 0;
        if(pLoader->GetInfo(bfi)) {
          dmImageDescriptor* descriptor = dmGetDescriptor(bfi.fbi_format);
          if(descriptor) 
          {
             _img = descriptor->CreateImage(bfi.fbi_width,bfi.fbi_height);

            if(pLoader->DiskToBuffer(_img,bfi)) {
              pLoader->Close();
              Err = ERR_DOC_NOERROR;
            }
          } else  Err = ERR_DOC_TYPE_UNKNOWN;
        } else Err = ERR_DOC_IOERROR;

        pLoader->Release();
        return Err;        
      }         
    } else
      dmLOG(_TXT("ERROR: Cannot get mime type for %s ...\n"),_path);

    return ERR_DOC_NOHANDLER;

  dmRETHROW_ERROR
  return ERR_DOC_IOERROR;
}
//---------------------------------------
int daim_fileHandler::SaveDocument( const dmString::E *_prot, const dmString::E* _args, const dmString::E* _path, dmImage* _img )
{
  dmTRY

  if(_tcsncasecmp(_prot,_TXT("buffer"),6)==0)        // Try to save into buffer
  {
    dmIGetService<dmIImageList> _IBuffers("daim://");

    int index = _ttoi(_args);
    if(index>=0)      
      return (_IBuffers.StoreBuffer(index,_img)?
        ERR_DOC_NOERROR:ERR_DOC_INVALIDOBJECT);
  }
  else if(_tcsncasecmp(_prot,_TXT("info"),4)==0) {    // Try to create new info
    return ERR_DOC_READONLY;
  } 
  else if(_tcsncasecmp(_prot,_TXT("null"),4)==0) {    // Try to save null document
    return ERR_DOC_READONLY;
  } 
  else 
  {
    //-------------------------------------------------------------
    // Handle File 
    //-------------------------------------------------------------

    // Get the mime type for the file
    const dmString::E* mime = DM_GetMimeType(_path);
    if(mime) 
    {
      // Get the loader associated this mime type
      dmGenericLoader* pLoader = dmLoaderFactory::GetLoader(mime,dmENCODER);
      if(pLoader)
      {
        int Err;
        if(pLoader->IsSupportedFormat(_img->PixelFormat()))
        {
          dmFILE_BUFFER_INFO bfi;
          dmNewLink<dmImage> img(_img);
        
          bfi.fbi_name  = _path;
          bfi.fbi_index = 0;     // this is the first/default image in file
          bfi.fbi_count = 1;     // just one record

          bfi.fbi_flags = 0;
          bfi.fbi_text  = NULL;

          if(pLoader->BufferToDisk(img,bfi)) {
            pLoader->Close();
            Err = ERR_DOC_NOERROR;
          } else
            Err = ERR_DOC_IOERROR;
         } else
           Err =  ERR_DOC_INVALIDFORMAT;
          
         pLoader->Release();
         return Err;
      } 
    } 
  }

  return ERR_DOC_NOHANDLER; 

  dmRETHROW_ERROR
  return ERR_DOC_IOERROR;
}
//---------------------------------------------
// Initialisation du getsionnaire de fichiers
//---------------------------------------------
void daim_initFileHandlers()
{
  daim_fileHandler* pHandler =  new daim_fileHandler;
  dmSetDocHandler(_TXT("file")  ,pHandler);
  dmSetDocHandler(_TXT("buffer"),pHandler);
  dmSetDocHandler(_TXT("info")  ,pHandler);
  dmSetDocHandler(_TXT("null")  ,pHandler);
  pHandler->Release();
}
//---------------------------------------
