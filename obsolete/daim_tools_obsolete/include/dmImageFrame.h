#ifndef dmImageFrame_h
#define dmImageFrame_h

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
// File         : dmImageFrame.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

class __dmTools dmImageFrame : public dmRectEdit
{
  protected:    
    bool    mbCanDisplay;
    bool    mbRealSize;       // Affiche en taille réelle
    bool    mbInitRealSize;    
    bool    mbKeepAspectRatio;// Preserve le rapport d'aspect
    bool    mbCenterImage;    // avec mbKeepAspectRatio : ne deforme pas le cadre

    dmPoint mMargins;         // Définie la taille des marges
    dmRect  mDisplayRect;     // Définie le rectangle d'affichage de l'image

    virtual void OnMove( int dx, int dy,bool _update );
    virtual void Recalculate();
    virtual bool OnImageRect( dmRect& ); // On demande le rectangle initiale de l'image !
     
  public:
    dmImageFrame( dmTool* _parent = NULL);	
    dmImageFrame( const dmRect& , dmTool* _parent = NULL );
    dmImageFrame( const dmImageFrame& _toCopy, dmTool* _parent = NULL );
    virtual ~dmImageFrame();

    void SetHorMargin( int );
    void SetVerMargin( int );
    void SetMargins( int _dx, int _dy  );
    void SetMargins( const dmPoint& p ) { SetMargins(p.x,p.y); }

    int GetHorMargin() const { return mMargins.x; }
    int GetVerMargin() const { return mMargins.y; }

    const dmPoint& GetMargins()     const { return mMargins; }
    const dmRect&  GetDisplayRect() const { return mDisplayRect; }
    
    bool KeepRatio()  const  { return mbKeepAspectRatio; }
    bool CenterImage() const { return mbCenterImage;     }

    void SetRealSize();
    void KeepAspectRatio( bool b, bool _center = false);

  public:
    virtual dmTool* Clone( dmTool* _parent = NULL ) const;
    virtual void OnDisplay( dmGraphics& ) const;

    virtual bool OnMouseDown  (const dmPoint&,dm_uint _button,dm_uint _keyQual); 
    virtual bool OnMouseUp    (const dmPoint&,dm_uint _button,dm_uint _keyQual); 
    virtual bool OnMouseMove  (const dmPoint&,dm_uint _button,dm_uint _keyQual); 

    dmDeclareClassInfo( dmImageFrame );
};
//--------------------------------------------------------
#endif // dmImageFrame_h
