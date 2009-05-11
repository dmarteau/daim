#ifndef dmKernelsMngr_h
#define dmKernelsMngr_h

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
// File         : dmKernelsMngr.h
// Date         : 7/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//--------------------------------------------------------

#include "dmKernelsSet.h"
//--------------------------------------------------------
class dmKernelsMngr
{
  typedef std::vector<dmKernelsSet*> KERNELS;
  protected:
    KERNELS lKernels;

  public:
    typedef KERNELS::iterator       iterator;
    typedef KERNELS::const_iterator const_iterator;

    dmKernelsMngr() {}
   ~dmKernelsMngr() { Clear(); }

    iterator begin() { return lKernels.begin(); }
    iterator end()   { return lKernels.end();   }

    const_iterator begin() const { return lKernels.begin(); }
    const_iterator end()   const { return lKernels.end();   }
    
    bool LoadConfig();
    bool LoadFile  ( const dmString::E* _file );

    dmKernelsSet* operator[](int i) { return lKernels[i]; }

    bool     Empty() const { return lKernels.empty(); }
    size_t   Size()  const { return lKernels.size();  }  
    void     Clear();
    iterator Erase( iterator ); 
    
    void Remove_Set( const dmString::E* );

    iterator               Find_Set   ( const dmString::E* );
    dmKernelsSet::Element* Find_Family( const dmString::E* );
};
//--------------------------------------------------------
#endif // dmKernelsMngr_h
