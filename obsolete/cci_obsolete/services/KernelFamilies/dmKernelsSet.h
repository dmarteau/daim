#ifndef dmKernelsSet_h
#define dmKernelsSet_h

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

//--------------------------------------------------------
// File         : dmKernelsSet.h
// Date         : 7/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//--------------------------------------------------------

#include "templates/processing/dmKernelFamily.h"

#include <vector>
//--------------------------------------------------------
class dmKernelsSet
{
  public:
   struct Element {
     int parent;
     int imode;
     dmString mode;
     dmString name;
     dmString title;
     dmString description;
     dmKernelFamily  family;
   };

  private:
   dmString              _name;
   dmString              _title;
   dmString              _description;
   std::vector<Element*> _list;

  public:
   typedef std::vector<Element*>::iterator iterator;

   void SetName       ( const dmString::E* );
   void SetTitle      ( const dmString::E* );
   void SetDescription( const dmString::E* );

   const dmString& GetName()        const { return _name;        }
   const dmString& GetTitle()       const { return _title;       }
   const dmString& GetDescription() const { return _description; }

   dmKernelsSet() {}
  ~dmKernelsSet() { Clear(); }

   iterator begin() { return _list.begin(); }
   iterator end()   { return _list.end();   }

   Element* operator[](int i) { return _list[i]; }
   Element* GetParent( Element* pElem ) {
     if(pElem && pElem->parent>=0) return _list[pElem->parent];
     return NULL;
   }

   int Add(  Element* pElem );

   Element* FindFamily( const dmString::E* name );

   void Clear();
   bool Empty() const { return _list.empty(); }
   int  Size()  const { return _list.size();  }

};
//--------------------------------------------------------
#endif // dmKernelsSet_h
