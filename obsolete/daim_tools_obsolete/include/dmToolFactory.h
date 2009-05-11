#ifndef dmToolsFactory_h
#define dmToolsFactory_h

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

//--------------------------------------------------------
// File         : dmToolsFactory.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

typedef struct {
  dmString     ClassName;
  dmClassInfo* pClassInfo;
  dmClassInfo* pPrefsInfo;
  dmTool*    (*pfnCreate)  ( const dmString::E*,  dmClassInfo* , const dmPoly* pts );
} dmTOOLFACTORY, *dmLPTOOLFACTORY;

//---------------------------------------------------------------------
// The tool factory is registered as a service
//---------------------------------------------------------------------
class dmToolFactory : public dmServices
{
  private:
   typedef daim::hashmap<size_t,dmTOOLFACTORY> hash_type;
   hash_type hList;   

  public:
   dmToolFactory();
  ~dmToolFactory();

   static dmToolFactory& Instance();

   virtual void SetDefaultConfiguration();

   dmDeclareClassInfo( dmToolFactory );

   bool            RegisterClass( dmLPTOOLFACTORY );
   dmTool*         CreateTool( const dmString::E* _class, const dmPoly* pts, dm_uint flags = 0);
   dmLPTOOLFACTORY GetFactory( const dmString::E* _class, bool bDeep = false );
};
//---------------------------------------------------------------------
#endif // dmToolsFactory_h
