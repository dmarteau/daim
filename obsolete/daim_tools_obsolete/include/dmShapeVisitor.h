#ifndef dmShapeVisitor_h
#define dmShapeVisitor_h

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
// File         : dmShapeVisitor.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

//---------------------------------------------------------------------------
class  dmShapeVisitor : public dmToolConstVisitor, public __dmShapeVisitor 
{
  public:
    virtual ~dmShapeVisitor() {}

    // virtual from dmToolConstVisitor
    virtual __dmTools void Visit( const dmTool& );

    // virtual from __dmShapeVisitor
    virtual  void Visit( const dmLine&    ) {}
    virtual  void Visit( const dmRect&    ) {}
    virtual  void Visit( const dmPoly&    ) {}
    virtual  void Visit( const dmEllipse& ) {}
    virtual  void Visit( const dmCircle&  ) {}
};
//---------------------------------------------------------------------------
#endif // dmShapeVisitor.h
