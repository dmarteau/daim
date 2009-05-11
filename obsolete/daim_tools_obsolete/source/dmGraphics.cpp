
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

#include "daim_tools.h"
//-----------------------------------------------------
// dmPenAttributs
//-----------------------------------------------------
dmPenAttributs::dmPenAttributs()
: mPenColor  (eClr_BLACK)
 ,mForeColor (eClr_NONE)
 ,mBackColor (eClr_WHITE)
 ,mLineType  (ePenStyle_solid)
 ,mBrushStyle(eBrushStyle_null) 
 ,mPenWidth  (1)
{}
//-----------------------------------------------------
dmPenAttributs::dmPenAttributs( EColorType   _PenColor,
                    EColorType   _BackColor, int _Width,
                    EPenStyle    _PenStyle, 
                    EBrushStyle  _BrushStyle
                  )

: mPenColor  (_PenColor)
 ,mForeColor (_PenColor)
 ,mBackColor (_BackColor)
 ,mLineType  (_PenStyle)
 ,mBrushStyle(_BrushStyle)
 ,mPenWidth  (_Width)
{}
//-----------------------------------------------------
dmPenAttributs::dmPenAttributs( const dmPenAttributs& toCopy )
{
  operator=(toCopy);
}
//-----------------------------------------------------
dmPenAttributs& dmPenAttributs::operator=(const dmPenAttributs& toCopy)
{
   mPenColor    = toCopy.mPenColor;
   mForeColor   = toCopy.mForeColor;
   mBackColor   = toCopy.mBackColor;
   mPenWidth    = toCopy.mPenWidth;
   mLineType    = toCopy.mLineType;
   mBrushStyle  = toCopy.mBrushStyle;
   return *this;
}
//-----------------------------------------------------
// dmTextAttributs
//-----------------------------------------------------
dmTextAttributs::dmTextAttributs() 
: mTextColor (eClr_WHITE)
 ,mFontWeight(eFontWeight_normal)
 ,mFontStyle (eFontStyle_normal )
 ,mFontSizePt(12)

{ 
  mFontFamily[0]='\0'; 
}
//--------------------------------------------------
dmTextAttributs::dmTextAttributs( const dmTextAttributs& _attrs)
{
  operator=(_attrs);
}
//--------------------------------------------------
dmTextAttributs& dmTextAttributs::operator=( const dmTextAttributs& toCopy )
{
  if(&toCopy != this ) 
  {
    mTextColor  = toCopy.mTextColor ;
    mFontWeight = toCopy.mFontWeight;
    mFontStyle  = toCopy.mFontStyle ;
    mFontSizePt = toCopy.mFontSizePt;
    _tcsncpy(mFontFamily,toCopy.mFontFamily,sizeof(mFontFamily));
  }
  return *this;
}


//------------------------------------------------------
// dmGraphics	
//------------------------------------------------------
__dmTools void dmGraphics::Construct()
{
  mInvalidCount = 0;
  mFastRefresh  = false;
  mCursor       = eCursor_undefined;
  mSavedStyle   = NULL;
}
//------------------------------------------------------
__dmTools void dmGraphics::Destroy()
{
  if(IsReferenced()) 
    dmTRACE(_TXT("WARNING : Destroying <dmGraphics> while still referenced"));
}
//-------------------------------------------------------
__dmTools void dmGraphics::OptimizeRefresh( bool _st ) 
{ 
  mFastRefresh = _st;
  if(mFastRefresh) {
    dmRect r;
    if(mInvalidRect.Size()>1) {
      dmTList<dmRect>::iterator it   = mInvalidRect.Begin();
      dmTList<dmRect>::iterator last = mInvalidRect.End();
      for(;it!=last;++it) r.Add( *it );
      mInvalidRect.Clear();
    }
    mInvalidRect.Push_Back(r);
  }
}
//------------------------------------------------------
__dmTools void dmGraphics::AddInvalidRect( const dmRect& r)
{
 if(mFastRefresh) {
   if(!r.IsEmpty()) {
     if(mInvalidRect.Empty()) { mInvalidRect.Push_Back(r); }
     else (*mInvalidRect.Begin()).Add(r);
   } 
 } else {
   if(!mInvalidRect.Empty() && !r.IsEmpty())  
   {
      bool add = true;
      dmTList<dmRect>::iterator it   = mInvalidRect.Begin();
      dmTList<dmRect>::iterator last = mInvalidRect.End();
      do {
        if(r>=*it) { it = mInvalidRect.Erase(it); continue; } // ri inside/equal r
        else if(*it>=r) { add = false; break;  }              // ri inside r (strict)
        ++it;
      } while(it!=last && add);
    
      if(add) { mInvalidRect.Push_Back(r); }
   } else mInvalidRect.Push_Back(r);
 }
}
//------------------------------------------------------
__dmTools int dmGraphics::BeginRefresh()
{
  return ++mInvalidCount;
}
//------------------------------------------------------
__dmTools int dmGraphics::BeginRefresh( const dmRect& r )
{
  AddInvalidRect(r);
  return ++mInvalidCount;
}
//------------------------------------------------------
__dmTools int dmGraphics::EndRefresh( const dmRect& r )
{
  AddInvalidRect(r);
  return EndRefresh();
}
//------------------------------------------------
__dmTools int dmGraphics::EndRefresh( bool _bRedraw )
{
  if(--mInvalidCount<=0) {
    if(_bRedraw) {
      dmTList<dmRect>::iterator it   = mInvalidRect.Begin();
      dmTList<dmRect>::iterator last = mInvalidRect.End();
      for(;it!=last;++it) {
        if(!(*it).IsEmpty()) { Refresh(*it); }
      }
    }
    mInvalidCount = 0;
    mInvalidRect.Clear();
  }
  return mInvalidCount;
}
//------------------------------------------------------
__dmTools const dmDrawStyle*  dmGraphics::SetDrawStyle( const dmDrawStyle* _Style, int _Flags )
{
  const dmDrawStyle* _OldStyle = NULL;

  if(_Flags==dmDrawStyleSave) {
     _OldStyle   = mSavedStyle;
     mSavedStyle = _Style;
  } 

  if(_Flags==dmDrawStyleRestore)
     mSavedStyle = _Style;

  if(_Style) {
    SetPenAttrs (_Style->mPenAttributs );
    SetTextAttrs(_Style->mTextAttributs);
  }

  return _OldStyle;
}
//------------------------------------------------------
