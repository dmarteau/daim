
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
#define dmUseServicesMngr
#include "daim_tools.h"
#include "dmToolFactory.h"
//------------------------------------------
#define dmToolFactory_SCHEMA_VERSION 1
dmImplementClassInfo( dmToolFactory, dmServices, dmToolFactory_SCHEMA_VERSION )
//------------------------------------------
dmToolFactory::dmToolFactory() 
{
  // Initialise factory;
}
//------------------------------------------
dmToolFactory::~dmToolFactory() 
{}
//----------------------------------------------------------
void dmToolFactory::SetDefaultConfiguration()
{}
//------------------------------------------
dmToolFactory& dmToolFactory::Instance()
{
  dmToolFactory* _instance = dmGetServices<dmToolFactory>();  
  return *_instance;
}
//------------------------------------------
// Retourne la 'factory' associé à la classe
// si bDeep est spécifié, alors _class doit faire réferences
// au nom de la classe telle qu'elle est spécifié par
// 'dmClassInfo': dans ce cas, la 'factory' retournée 
// correpond à celle qui correspond à l'ancêtre 
// le plus directe de '_class'
//------------------------------------------
dmLPTOOLFACTORY dmToolFactory::GetFactory( const dmString::E* _class, bool bDeep )
{
  if(_class && _class[0]!='\0') {
    dmString className = _class;
    hash_type::iterator it = hList.find(className.Hash());
    if(it!=hList.end()) 
      return &(*it).second;
    else if(bDeep) 
    {
      dmClassInfo* pInfo = dmClassInfo::GetClass(className.CStr());
      if(pInfo!=NULL) {
        hash_type::iterator __I = hList.begin();
        hash_type::iterator __L = hList.end();
        dmLPTOOLFACTORY lpBase  = NULL;
        for(;__I!=__L;++__I) 
        {
          dmTOOLFACTORY& _f = (*__I).second;
          if(_f.pClassInfo && pInfo->IsUnderClass(_f.pClassInfo)) 
          {
             if(lpBase) {
               if(_f.pClassInfo->ClassLevel() < lpBase->pClassInfo->ClassLevel())
                 lpBase = &_f; 
             } else
               lpBase = &_f;
          }
        }
        return lpBase;
      }
    }
  }
  return NULL;
}
//------------------------------------------
dmTool* dmToolFactory::CreateTool( const dmString::E* _class, const dmPoly* pts, dm_uint flags )
{
  if(_class) 
  {
    dmString className = _class;
    
    hash_type::iterator it = hList.find(className.Hash());
    if(it!=hList.end()) 
    {
      dmTOOLFACTORY& _factory = (*it).second;
      if(_factory.pfnCreate)
        return _factory.pfnCreate( _class, _factory.pClassInfo, pts );
      else if(_factory.pClassInfo) 
        return dmTool::Cast(_factory.pClassInfo->Create());
    	
    /*
      dmTOOLFACTORY& _factory = (*it).second;
      if(pts && _factory.pfnCreate && pts->Size()>0)
        return _factory.pfnCreate( _class, _factory.pClassInfo, pts );
      else if(_factory.pClassInfo) 
        return dmTool::Cast(_factory.pClassInfo->Create());
     */
    }
  }
  return NULL;
}
//------------------------------------------
bool dmToolFactory::RegisterClass( dmLPTOOLFACTORY lpFactory )
{
  if(lpFactory && !lpFactory->ClassName.Empty()) 
  {
    const dmString& className = lpFactory->ClassName;
    hash_type::iterator it = hList.find(className.Hash());
    if(it!=hList.end()) {
      dmTOOLFACTORY& _factory = (*it).second;
      if(_factory.ClassName==className)
        dmLOG(_TXT("ERROR : class <%s> already registered !\n"),className.CStr());
      else 
        dmLOG(_TXT("ERROR : class name <%s> collide with <%s> !\n"),
          className.CStr(),_factory.ClassName.CStr());
    } else {
      hList[className.Hash()] = *lpFactory;
      return true;
    }  
  }
  return false;
}
//-----------------------------------------------------------------------------------
