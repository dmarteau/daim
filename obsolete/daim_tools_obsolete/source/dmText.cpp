
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

#define dmUseExtendedTools
#include "daim_tools.h"
//----------------------------------------------------------------
// Text tool
//----------------------------------------------------------------
#define dmText_SCHEMA_VERSION 1
dmImplementClassInfo( dmText, dmRectEdit, dmText_SCHEMA_VERSION );
//----------------------------------------------------------------
#define __SIZE_EXT  2
//----------------------------------------------------------------
dmText::dmText( dmTool* _parent )
 :dmRectEdit(_parent)
 ,mAlignement(eAlignStyle_left)
 ,mMarginSize(2)
 ,mbFitBorder(false)
{}
//----------------------------------------------------------------
dmText::dmText( const dmText& _toCopy, dmTool* _parent )
 :dmRectEdit(_toCopy,_parent)
 ,mAlignement(_toCopy.mAlignement)
 ,mMarginSize(_toCopy.mMarginSize)
 ,mbFitBorder(false)
 ,mText(_toCopy.mText)
{}
//----------------------------------------------------------------
dmText::dmText( int _alignement, int _marginSize, dmTool* _parent )
: dmRectEdit(_parent)
 ,mAlignement(_alignement)
 ,mMarginSize(_marginSize)
 ,mbFitBorder(false)
{}
//----------------------------------------------------------------
dmText::dmText( const dmString& _text, const dmPoint& p, dmTool* _parent )
: dmRectEdit(_parent)
 ,mAlignement(eAlignStyle_left)
 ,mMarginSize(1)
 ,mbFitBorder(false)
 ,mText(_text)
{
  mRect = dmRect(p,p);
}
//----------------------------------------------------------------
dmText::dmText( const dmString& _text, const dmRect& r, dmTool* _parent )
: dmRectEdit(r,_parent)
 ,mAlignement(eAlignStyle_left)
 ,mMarginSize(1)
 ,mbFitBorder(false)
 ,mText(_text)
{}
//----------------------------------------------------------------
dmText::dmText( const dmString& _text,dmTool* _parent )
: dmRectEdit(_parent)
 ,mAlignement(eAlignStyle_left)
 ,mMarginSize(1)
 ,mbFitBorder(false)
 ,mText(_text)
{
  dmPoint p;
  mRect = dmRect(p,p);
}
//----------------------------------------------------------------   
dmText::~dmText() {} 
//----------------------------------------------------------------
// Initializer
//----------------------------------------------------------------
bool dmText::InitialUpdate()
{
  if(!IsEditing()||mbFitBorder) {
    if(IsNullObject() && !mText.Empty()) 
       FitBorderToText();
  }
  return dmRectEdit::InitialUpdate();
} 
//----------------------------------------------------------------
void dmText::SetAlignement( int _align )
{
  O_INVALIDATE(this)
  mAlignement = _align;
}
//---------------------------------------------------------------- 
void dmText::SetMarginSize( int _size ) 
{ 
  O_INVALIDATE(this)
  mMarginSize = _size;
}
//---------------------------------------------------------------- 
void dmText::SetText( const dmString& _s ) 
{ 
  O_INVALIDATE(this)
  mText = _s;
}
//----------------------------------------------------------------
bool dmText::FitBorderToText()
{
  dmSurface s = ObtainGraphics();
  if(s) {
    dmRect rect = mRect;
    dmPoint pos = rect.TopLeft();
    const dmDrawStyle* _DrawsStyle = GetDrawStyle();
    if(_DrawsStyle)
      s->SetTextAttrs(_DrawsStyle->mTextAttributs);  

    s->GetTextRect(mText.CStr(),rect,mAlignement);
    rect.Resize(mMarginSize+__SIZE_EXT,mMarginSize);
    rect.Translate( pos - rect.TopLeft() );
    mRect = rect;
    UpdateObject();
  }
  mbFitBorder = true;
  return s;
}
//----------------------------------------------------------------
void dmText::StaticText( bool _st )
{
  LockCarrets(_st);
}
//----------------------------------------------------------------
dmRect dmText::TextRect() const
{
  dmRect rect = mRect;
  rect.Resize(-mMarginSize,-mMarginSize);
  rect.top_left.x += __SIZE_EXT;
  return rect;
}
//----------------------------------------------------------------
void dmText::OnDisplay( dmGraphics& gr ) const
{
  dmRectEdit::OnDisplay(gr);
  gr.DrawText(mText.CStr(),mRect,mAlignement);
}
//----------------------------------------------------------------
dmTool* dmText::Clone( dmTool* _parent  ) const
{
  return new dmText(*this,_parent); 
}
//----------------------------------------------------------------
