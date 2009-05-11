#ifndef dmKernelsLoader_h
#define dmKernelsLoader_h

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
// File         : dmKernelsLoader.h
// Date         : 7/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//
//--------------------------------------------------------

#include "dmKernelsSet.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_XMLParser.h"
//--------------------------------------------------------
class dmKernelsLoader : public dmIXMLHelper
{
  protected:
    long kX,kY,kWidth,kHeight; // valeurs par défaults
    dmStructuringElement rElement;
    dmKernelsSet* pSet;
    dmKernelsSet::Element* pCurrent; 
    int nState;

    virtual void OnBeginDocument();
    virtual void OnEndDocument();
    virtual void OnStartItem();
    virtual void OnEndItem();

  public:
    dmKernelsLoader(dmIXMLParser&);
   ~dmKernelsLoader() {}

    bool Create( dmKernelsSet& _set );
};
//--------------------------------------------------------
#endif // dmKernelsLoader_h
