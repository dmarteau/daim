#ifndef dmToolGroup_h
#define dmToolGroup_h

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
// File         : dmToolGroup.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

class __dmTools dmToolGroup : public dmTool
{
  public:
    typedef dmTList<dmTool*>           ToolList_t;
    typedef ToolList_t::iterator       iterator;
    typedef ToolList_t::const_iterator const_iterator;

    typedef ToolList_t::reverse_iterator       reverse_iterator;
    typedef ToolList_t::const_reverse_iterator const_reverse_iterator;

  private:
    ToolList_t  mToolList;
 
  protected:
    dmToolGroup( const dmRect& , dmTool* _parent = NULL );

    virtual void OnMove(int dx,int dy, bool _update);
      
  public:
    dmToolGroup( const dmToolGroup& , dmTool* _parent = NULL );
    dmToolGroup( dmTool* _parent = NULL );
    virtual ~dmToolGroup();

    //------ Call DisplayObject for all objects ------------
    virtual void OnDisplay      ( dmGraphics& ) const;
    virtual void OnRedrawCarrets( dmGraphics& ) const;

    virtual void ClearAndDestroy();

    virtual void Recalculate();

  public:
    size_t Size()  const { return mToolList.Size(); }
    size_t Count() const { return Size(); }

    bool Empty() const { return mToolList.Empty(); }

    void Push_Front( dmTool* );
    void Push_Back ( dmTool* );

    void Add( dmTool* tool ) { Push_Back(tool); }
    void Insert( iterator it, dmTool* tool);

    dmTool* Remove( iterator );
    void    Remove( dmTool*  );

    void Clear();

    int Append( const dmToolGroup& _from  );
    int Merge ( const dmToolGroup& _from  );
    int Copy  ( const dmToolGroup& _from  );

    //-----------------------------------------------------

    iterator Begin() { return mToolList.Begin(); }
    iterator End()   { return mToolList.End();   }

    reverse_iterator rBegin() { return mToolList.rBegin(); }
    reverse_iterator rEnd()   { return mToolList.rEnd();   }

    const_iterator Begin() const { return mToolList.Begin(); }
    const_iterator End()   const { return mToolList.End();   }

    const_reverse_iterator rBegin() const { return mToolList.rBegin(); }
    const_reverse_iterator rEnd()   const { return mToolList.rEnd();   }

    iterator Find(  const dmPoint& );

    virtual dmTool*       GetObject  ( const dmPoint&, bool bHitTest );

    // Acyclic Visitor pattern
    virtual void Accept( dmVisitor& );
    virtual void Accept( dmConstVisitor& ) const;

  public:
    virtual dmTool* Clone( dmTool* _parent = NULL ) const;

  public:
    dmDeclareClassInfo( dmToolGroup );

};
//---------------------------------------------------------------------------
#endif // dmToolGroup_h

