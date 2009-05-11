#ifndef dmRegionEdit_h
#define dmRegionEdit_h

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
// File         : dmRegionEdit.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

// Exception to our rule, rarely needed stuff 
// except here
#include "dmRegionEditProto.h"
#include "tools/dmRegionTree.h"

//----------------------------------------------------------------------
class dmRegionEdit;

class dmRegionEditShape : public dmShape 
{
  friend class dmRegionEdit;
  private:
    dmRegionTree  mTree;

  protected:
   virtual void DoMove     ( int _index, int dx, int dy );
   virtual void DoMoveIndex( int _index, int dx, int dy );
   virtual void DoFrame    ( dmGraphics& , int _index, int drawFlags ) const;

  public:
   dmRegionEditShape() {}

   virtual ~dmRegionEditShape() {}
};
//----------------------------------------------------------------------

class dmRegionEdit : public dmRegionEditProto
{  
  private:
    dmRegionEditShape mShape;
 
  protected:
   virtual void Recalculate();

  public:
   dmRegionEdit( dmTool* _parent = NULL );
   dmRegionEdit( const dmRegion&, dmTool* _parent = NULL );
   dmRegionEdit( const dmRegionEdit& _toCopy, dmTool* _parent = NULL );

   virtual ~dmRegionEdit();

   virtual void OnDisplay   ( dmGraphics& ) const;

         dmRegionTree& GetTree()       { return mShape.mTree; }
   const dmRegionTree& GetTree() const { return mShape.mTree; }

   //-------------------------------------------------------------
   virtual dmTool* Clone( dmTool* _parent = NULL ) const;
   //--------------------------------------------------------------        
  public:
   dmDeclareClassInfo( dmRegionEdit );
};
//////////////////////////////////////////////////////////////////////////////
#endif // dmRegionEdit_h
