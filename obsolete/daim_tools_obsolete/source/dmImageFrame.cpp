
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
// Image tool
//----------------------------------------------------------------
#define dmImageFrame_SCHEMA_VERSION 1

dmImplementClassInfo( dmImageFrame, dmRectEdit, dmImageFrame_SCHEMA_VERSION );
//----------------------------------------------------------------
dmImageFrame::dmImageFrame( dmTool* _parent  )
: dmRectEdit( _parent )
 ,mbCanDisplay(false)
 ,mbRealSize(false)
 ,mbInitRealSize(false)
 ,mbKeepAspectRatio(true)
 ,mbCenterImage(false)
 ,mMargins(1,1)
{}
//----------------------------------------------------------------
dmImageFrame::dmImageFrame( const dmRect& r, dmTool* _parent  )
: dmRectEdit(r,_parent )
 ,mbCanDisplay(false)
 ,mbRealSize(false)
 ,mbInitRealSize(false)
 ,mbKeepAspectRatio(true)
 ,mbCenterImage(false)
 ,mMargins(1,1)
{}
//----------------------------------------------------------------
dmImageFrame::dmImageFrame( const dmImageFrame& _toCopy, dmTool* _parent )
: dmRectEdit(_toCopy,_parent)
 ,mbCanDisplay(_toCopy.mbCanDisplay)
 ,mbRealSize(false)
 ,mbInitRealSize(false)
 ,mbKeepAspectRatio(_toCopy.mbKeepAspectRatio)
 ,mbCenterImage(_toCopy.mbCenterImage)
 ,mMargins(_toCopy.mMargins)
 ,mDisplayRect(_toCopy.mDisplayRect)
{}
//----------------------------------------------------------------
dmImageFrame::~dmImageFrame() {}
//----------------------------------------------------------------
dmTool* dmImageFrame::Clone( dmTool* _parent ) const
{
  return new dmImageFrame( *this,_parent );
}
//----------------------------------------------------------------
void dmImageFrame::OnMove( int dx, int dy,bool _update )
{
  if(CarretIndex()==-1 && _update) { 
    mDisplayRect.Translate(GetMove().x,GetMove().y); 
  }
  dmRectEdit::OnMove(dx,dy,_update);
}
//----------------------------------------------------------------
bool dmImageFrame::OnImageRect( dmRect& )
{
  return false;
}
//----------------------------------------------------------------
void dmImageFrame::Recalculate()
{
  mbCanDisplay = false;
  
  dmRect imrect;
  if(OnImageRect(imrect)) { // Recupère la taille de l'image
    dmRect orect  = mRect;

    if(mbRealSize) 
    { // Affichage de l'image à la taille (pixels) reelle.
      int wr,hr,hi,wi;
      wi = imrect.Width();
      hi = imrect.Height();
      hr = hi + 2*mMargins.y;
      wr = wi + 2*mMargins.x;
      mRect =  dmRect(orect.Left(), orect.Top(), wr, hr);
      mDisplayRect = dmRect(  orect.Left()+mMargins.x, orect.Top()+mMargins.y,wi,hi );
      mbCanDisplay = true;
    } 
    else if(mbKeepAspectRatio) // Redimensionne en preservant le repport d'aspect.
    {
      float aspect_ratio = (float)imrect.Height()/(float)imrect.Width();
      if(mbCenterImage) {
        int hi,wi;
        wi = orect.Width()  - 2*mMargins.x;
        hi = orect.Height() - 2*mMargins.y;
        if(wi > 0 && hi > 0) { 
          float zw = (float)wi/(float)imrect.Width();
          float zh = (float)hi/(float)imrect.Height();
          if( zh < zw ) wi = (int)(hi /  aspect_ratio + 0.5);
          else          hi = (int)(wi * aspect_ratio  + 0.5);
          mDisplayRect = dmRect(  orect.Left()+mMargins.x, orect.Top()+mMargins.y,wi,hi );
          mDisplayRect.Translate( orect.Center() - mDisplayRect.Center() );
          mbCanDisplay = true;
        }
      } else { 
        int hr,hi,wi = orect.Width() - 2*mMargins.x;
        if(wi > 0) {  
          // Calcule la hauteur de l'image (hi) et du cadre (hr)
          hi = (int)(wi * aspect_ratio + 0.5);
          hr = hi + 2*mMargins.y; 
          SetCoordinates( dmRect(orect.Left(), orect.Top(), orect.Width(), hr) );
          mDisplayRect = dmRect(  orect.Left()+mMargins.x, orect.Top()+mMargins.y,wi,hi );
          mbCanDisplay = true;
        }
      }
    } else {
      int hi,wi;
      wi = orect.Width()  - 2*mMargins.x;
      hi = orect.Height() - 2*mMargins.y;
      mDisplayRect = dmRect( orect.Left()+mMargins.x, orect.Top()+mMargins.y,wi,hi );
      mbCanDisplay = true;      
    }
  }
  mbRealSize = false;
  dmRectEdit::Recalculate();
}
//----------------------------------------------------------------
void dmImageFrame::SetRealSize()
{
  mbRealSize = true;
  UpdateObject();
}

//----------------------------------------------------------------
void dmImageFrame::KeepAspectRatio( bool bKeep, bool bCenter )
{
  mbKeepAspectRatio = bKeep;
  mbCenterImage     = bCenter;
  UpdateObject();
}
//----------------------------------------------------------------
void dmImageFrame::OnDisplay( dmGraphics& gr ) const
{
  dmRectEdit::OnDisplay(gr);
  if(!mbCanDisplay) {
    dmRect r = mRect;
    gr.SetPenColor(DM_RGB(0,0,0));
    gr.SetBrush(DM_RGB(220,220,220));
    r.Resize( -mMargins.x, -mMargins.y );
    gr.DrawRectangle(r);
    gr.DrawLine(r.TopLeft()   ,r.BottomRight());
    gr.DrawLine(r.BottomLeft(),r.TopRight());
  } 
}
//----------------------------------------------------------------
void dmImageFrame::SetHorMargin( int _x )
{
  mMargins.x = _x;
  UpdateObject();
}
//----------------------------------------------------------------
void dmImageFrame::SetVerMargin( int _y )
{
  mMargins.y = _y;
  UpdateObject();
}
//----------------------------------------------------------------
void dmImageFrame::SetMargins(  int _x, int _y  )
{
  mMargins.x = _x;
  mMargins.y = _y;
  UpdateObject();
}
//----------------------------------------------------------------
// Fonctions d'Edition 
//----------------------------------------------------------------
bool dmImageFrame::OnMouseDown(const dmPoint& pt, dm_uint _button, dm_uint _keyQual)
{
  return ( mbInitRealSize 
            ? true 
            : dmRectEdit::OnMouseDown(pt,_button,_keyQual)
         );
} 
//----------------------------------------------------------------
bool dmImageFrame::OnMouseUp(const dmPoint& pt, dm_uint _button, dm_uint _keyQual)
{
  mbInitRealSize = false;
  return dmRectEdit::OnMouseUp(pt,_button,_keyQual);
} 
//----------------------------------------------------------------
bool dmImageFrame::OnMouseMove(const dmPoint& pt, dm_uint _button, dm_uint _keyQual) 
{
  if(!IsInitialized() && mbRealSize) {
    Show(true);
    if(Initialize()) {
      SetEditMode(true);
      MoveBy( ObjectRect().TopLeft() - ObjectRect().Center(),false);
      mbInitRealSize = true;
    }
    return mbInitRealSize;
  } else  
      return (mbInitRealSize 
         ? dmRectEdit::OnMouseMove(pt,_button & buttonLeft,_keyQual)
         : dmRectEdit::OnMouseMove(pt,_button,_keyQual)
      );
}        
//----------------------------------------------------------------
