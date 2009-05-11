
/* :::BEGIN LICENSE BLOCK:::
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
 *  :::END LICENSE BLOCK::: */

#define dmUserIncludes
#include "daim_kernel.h"
#include "dmKernelsSet.h"

#include "dmKernelsLoader.h"
#include "dmKernelsMngr.h"

#define DAIM_MEAN_AND_LEAN
#include "daim/daim_modules.h"

#define ITEM_SEP_STR _TXT("/")

//--------------------------------------------
bool dmKernelsMngr::LoadConfig()
{
  bool result = false;

  dmIInterface<dmIStringList> _IFileList;
  dmIInterface<dmIProperties> _IProperties;

  if(_IProperties.Open("daim://kernels.ini",NULL) &&
     _IProperties.GetSection(_TXT("KERNELS_FILES"),_IFileList)) 
  {
    dm_param pos = dm_null;
    while( (pos = _IFileList.GetNext(pos)) ) {
      const char_t* _file = _IFileList.GetString(pos);
      dmString _realpath = dmPortability::SearchEnv(__DAIM_LIBPATH__,_file);
      if(!_realpath.Empty())
        LoadFile( _realpath.CStr() );
      else
        dmLOG(_TXT("ERROR: Kernels file %s not found\n"),_file);
    } 
    result = true;
  } else
    dmLOG(_TXT("ERROR: Cannot get kernels configuration\n"));

  return result;
}
//--------------------------------------------
bool dmKernelsMngr::LoadFile( const dmString::E* _file )
{
  bool result = false;
  dmIInterface<dmIXMLParser> _IParser;
  dmString file = _file;
  if(!file.Empty() && _IParser.ParseFile(file.CStr(),"ISO-8859-1"))
  {
     dmKernelsLoader loader(_IParser);
     dmKernelsSet* pKernels = new dmKernelsSet;
     if(result = loader.Create(*pKernels)) {
       lKernels.push_back(pKernels);
     } else {
       dmLOG(_TXT("WARNING: No kernels defined in %s\n"),file.CStr()); 
       delete pKernels;
     }
  } else
    dmLOG(_TXT("ERROR: Cannot load %s\n"),file.CStr()); 

  return result;
}
//--------------------------------------------
void dmKernelsMngr::Clear()
{
  iterator it   = begin();
  iterator last = end();
  for(;it!=last;++it) { delete *it; }
  lKernels.clear();
}
//--------------------------------------------
dmKernelsMngr::iterator dmKernelsMngr::Erase( dmKernelsMngr::iterator it )
{
  delete *it;
  return lKernels.erase(it);
}
//--------------------------------------------
void dmKernelsMngr::Remove_Set( const dmString::E* _name )
{
  iterator it = Find_Set(_name);
  if(it!=end())   
    Erase(it);
}
//--------------------------------------------
dmKernelsMngr::iterator dmKernelsMngr::Find_Set( const dmString::E* _name )
{
  iterator it   = begin();
  iterator last = end();
  for(;it!=last;++it) {
    if( (*it)->GetName()==_name ) return it;
  }
  return last;
}
//--------------------------------------------
dmKernelsSet::Element* dmKernelsMngr::Find_Family( const dmString::E* _path )
{
  if(Empty()) 
    dmLOG(_TXT("WARNING : No kernels loaded !\n"));

  dmKernelsSet::Element* pElement = NULL;
  dmString path = _path;

  size_t _pos = path.FindFirstOf(ITEM_SEP_STR);
  if(_pos!=dmString::npos) {
    iterator it = Find_Set( path.SubStr(0,_pos).CStr() );
    if(it!=end()) pElement = (*it)->FindFamily( path.SubStr(_pos+1).CStr() );
  } else {
    for(size_t i=0;i<lKernels.size();++i) {
      pElement = lKernels[i]->FindFamily(_path);
    }
  }
  return pElement;
}
//--------------------------------------------

