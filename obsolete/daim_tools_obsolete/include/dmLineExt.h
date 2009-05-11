#ifndef dmLineExt_h
#define dmLineExt_h

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
// File         : dmLineExt.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

class __dmTools dmLineExtEdit : public dmLineEdit
{
  private:
    dmLineTerminator* pDrawStart;
    dmLineTerminator* pDrawEnd;

  protected:
    virtual void Recalculate();

  public:
    dmLineExtEdit( dmTool* _parent = NULL );	 
    dmLineExtEdit( const dmLine&, dmTool* _parent = NULL );
    dmLineExtEdit( const dmLineExtEdit& _toCopy, dmTool* _parent = NULL );
    virtual ~dmLineExtEdit();

    virtual void OnDisplay( dmGraphics& ) const;

    void SetArrowStart( const dmLineTerminator* );
    void SetArrowEnd  ( const dmLineTerminator* );

    const dmLineTerminator* GetArrowStart() const { return pDrawStart; }
    const dmLineTerminator* GetArrowEnd  () const { return pDrawEnd; }

    dmLineTerminator* GetArrowStart() { return pDrawStart; }
    dmLineTerminator* GetArrowEnd  () { return pDrawEnd; }

    virtual dmTool* Clone(dmTool* _parent=NULL) const;

    dmDeclareClassInfo( dmLineExtEdit );
};
//---------------------------------------------------------------------------
#endif // dmLineExt.h
