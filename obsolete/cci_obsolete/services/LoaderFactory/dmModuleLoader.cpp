
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

#define dmUseKernelImageTemplates
#include "daim_kernel.h"
#include "images/dmGenericLoader.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_Loader.h"
#include "daim_modules/CCI_StringList.h"
#include "daim_modules/CCI_InputStream.h"
#include "daim_modules/CCI_OutputStream.h"
#include "daim_modules/CCI_FileInputStream.h"
#include "daim_modules/CCI_FileOutputStream.h"
#include "daim_modules/CCI_StringList.h"
#include "daim_modules/CCI_Interface.h"

#include "daim/daim_interfaces.h"
#include "daim/daim_io.h"

#include "dmModuleLoader.h"

//--------------------------------------------------------------------
dmFastMemory<dmModuleLoader> dmModuleLoader::_MemPool;
//--------------------------------------------------------------------
dmModuleLoader::dmModuleLoader( dmLOADER_DESCRIPTION* _Description ) 
: dmGenericLoader(_Description)
{}
//--------------------------------------------------------------------
dmModuleLoader::~dmModuleLoader() 
{
  _Self.Release();
  _InputStream.Release();
  _OutputStream.Release();
  _StringList.Release();
}
//--------------------------------------------------------------------
bool dmModuleLoader::GetInfo( dmFILE_BUFFER_INFO& bfi )
{
  dmASSERT( _Self.IsValid() );

  if(IsDecoder()) 
  {
     if(!_InputStream.IsValid())
         _InputStream.Create();

     if(_InputStream.IsValid()) 
     {
       dm_uint32 behaviors = _Self.HasStreamSupport() ? 0 : DM_IO_PRETEND_TO_OPEN;
      
       if(!_InputStream.Init(bfi.fbi_name.CStr(),0,0,behaviors)) {
         dmLOG(_TXT("ERROR: Cannot init input stream for %s\n"),bfi.fbi_name.CStr());
         return false;      
       }
     } else {
       dmLOG(_TXT("ERROR: Cannot create input stream for %s\n"),bfi.fbi_name.CStr());
       return false;
     }

     if(_StringList.IsValid() || (bfi.fbi_flags & dmFBI_TEXT))
     {
        if(!_StringList.IsValid())
            _StringList.Create();

        _Self.SetStringList(dmIINTERFACE_PTR(_StringList));
     }

     return _Self.GetImageInfo( dmIINTERFACE_PTR(_InputStream), 
           bfi.fbi_flags , 
           bfi.fbi_format,
           bfi.fbi_index ,
           bfi.fbi_count ,
           bfi.fbi_width ,
           bfi.fbi_height);
  }
  else 
   dmLOG(_TXT("ERROR: Cannot read as %s (Loader not available)\n"),
          bfi.fbi_name.CStr());

  return false;
}
//--------------------------------------------------------------------
void dmModuleLoader::Close()
{
  dmASSERT( _Self.IsValid() );
  
  _Self.Close();
  _StringList.Release();
}
//-------------------------------------------------------------- 
bool dmModuleLoader::BufferToDisk( dmLink<dmImage>& _buffer, dmFILE_BUFFER_INFO& bfi )
{
   dmASSERT( _Self.IsValid() );

   if(_buffer.IsNull()) 
     return false;

   if(IsEncoder()) 
   {
      if(!_OutputStream.IsValid())
          _OutputStream.Create();

      if(_OutputStream.IsValid()) 
      {
        dm_uint32 behaviors = _Self.HasStreamSupport() ? 0 : DM_IO_PRETEND_TO_OPEN;
        
        if(!_OutputStream.Init(bfi.fbi_name.CStr(),0,0,behaviors)) {
          dmLOG(_TXT("ERROR: Cannot init output stream for %s\n"),bfi.fbi_name.CStr());
          return false;      
        }    
      } else {
        dmLOG(_TXT("ERROR: Cannot create output stream for %s\n"),bfi.fbi_name.CStr());
        return false;
      }
      
      bfi.fbi_width  = _buffer->Width();
      bfi.fbi_height = _buffer->Height();
      bfi.fbi_format = _buffer->PixelFormat();

      if(_buffer->HasUnits()) {
        dmConstLink<dmSpatialUnits> _units = _buffer->Units();
        bfi.fbi_units = _units->mUnitName;
        bfi.fbi_uppx  = _units->mUnitsPerPixel;
        bfi.fbi_ar    = _units->mAspectRatio;
      }

      if(_StringList.IsValid())
         _Self.SetStringList(dmIINTERFACE_PTR(_StringList));
         
      if(_Self.SetImageInfo(dmIINTERFACE_PTR(_OutputStream),
           bfi.fbi_flags , 
           bfi.fbi_format,
           bfi.fbi_index ,
           bfi.fbi_count ,
           bfi.fbi_width ,
           bfi.fbi_height))         
      {
         // Send data to Saver
         dmImageData _data;
         if(_buffer->GetImageData(_data)) 
           return _Self.SetImageData(_data);
      } else
        dmLOG(_TXT("ERROR: 'SetImageInfo' failed\n"));
   } 
   else
    dmLOG(_TXT("ERROR: Cannot save as %s (Saver not available)\n"),
          bfi.fbi_name.CStr());

  return false;
}
//--------------------------------------------------------------
// dmFILE_BUFFER_INFO should be obtained through GetInfo
// It is the responsability of the Loader to map the data onto the specified 
// buffer: i.e 1 bit wide pixel will be mapped onto 8 bits buffer pixels
//--------------------------------------------------------------
bool dmModuleLoader::DiskToBuffer( dmLink<dmImage>& _buffer, dmFILE_BUFFER_INFO& bfi )
{
  if(!IsDecoder()) 
    dmLOG(_TXT("ERROR: Cannot read as %s (Loader not available)\n"),
          bfi.fbi_name.CStr());

  if( !_buffer.IsNull() &&
      _buffer->PixelFormat() == bfi.fbi_format &&
      _buffer->Width()       == bfi.fbi_width  &&
      _buffer->Height()      == bfi.fbi_height )
  {
      dmImageData _data;
      if(_buffer->GetImageData(_data))
         return _Self.GetImageData(_data);
  } else
    dmLOG(_TXT("ERROR: Invalid buffer format for %s \n"),bfi.fbi_name.CStr());

  return false;
}
//--------------------------------------------------------------------
bool dmModuleLoader::IsSupportedFormat( EPixelFormat _PixelFormat )
{
  dmLOADER_DESCRIPTION* ldrDesc = GetDescription();
  if(ldrDesc != NULL) {
    dmIModuleClass cl(ldrDesc->ld_name.CStr());
    if(cl.IsValid()) {
      return CCI_SUCCEEDED(CCI_InvokeService(cl,Loader,IsSupportedFormat, 
                                             _in _PixelFormat));
    }
  }
  return false;
}
//--------------------------------------------------------------------
dmGenericLoader* dmModuleLoader::CreateInstance( dmLOADER_DESCRIPTION* aDescription, int _type )
{
  dmModuleLoader* ldr = new dmModuleLoader(aDescription);

  // Create the loader object
  cci_Object* aLoader = CCI_NewObjectVA(aDescription->ld_name.CStr(),TGZ_DEFAULT,TGZ_DONE,NULL);

  ldr->_Self.Attach(aLoader);
  ldr->_Self.AddRef();
   
  if(!ldr->_Self.IsValid()) 
  {
    delete ldr;
    ldr = NULL;
  } 
  else
  {    
    // Initialize the loader
    ldr->_Self.Initialize(NULL,aDescription->ld_mime.CStr(),
                               static_cast<dm_param>(_type));
  }
  return ldr;
}
//--------------------------------------------------------------------
// Auto delete pattern
void dmModuleLoader::Release()
{
  delete this;
}
//--------------------------------------------------------------------
const char_t* dmModuleLoader::GetText( const char_t* attribute )
{
  const char_t* _Value = NULL;

  if(_StringList.IsValid() && attribute!=NULL) 
  {
    dm_param pos = 0;
    if( (pos = _StringList.FindString(attribute,pos)) != 0 ) {
         pos = _StringList.GetNext(pos);

      if(pos)
         _Value = _StringList.GetString(pos);
    }
  }

  return _Value;
}
//--------------------------------------------------------------------
size_t dmModuleLoader::SetText( const char_t* attribute, const char_t* value )
{
   dmASSERT( _Self.IsValid() );

  if(attribute == NULL) { // reset text infos 
     if(_StringList.IsValid())
        _StringList.Clear();
  }
  else if( value != NULL ) 
  {
    if(!_StringList.IsValid())
        _StringList.Create();

    _StringList.AddTail(attribute);
    _StringList.AddTail(value);

    return _StringList.Count();
  }
  return 0;
}
//--------------------------------------------------------------------
