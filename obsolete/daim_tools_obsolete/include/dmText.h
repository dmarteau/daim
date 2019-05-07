#ifndef dmText_h
#define dmText_h

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
// File         : dmText.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

class __dmTools dmText : public dmRectEdit
{
  private:
    int       mAlignement;
    int       mMarginSize;
    bool      mbFitBorder;
    dmString  mText;

  protected:
    virtual bool InitialUpdate();

  public:
    dmText( dmTool* _parent = NULL );
    dmText( const dmText&   _toCopy, dmTool* _parent = NULL );
    dmText( int _alignement, int _marginSize, dmTool* _parent = NULL );
    dmText( const dmString& _text, const dmRect&  , dmTool* _parent = NULL );
    dmText( const dmString& _text, const dmPoint& , dmTool* _parent = NULL );
    dmText( const dmString& _text, dmTool* _parent = NULL );
   ~dmText();

    void SetAlignement( int _align );
    void SetMarginSize( int _size  );
    void SetText( const dmString&  );

    bool FitBorderToText();

    dmText& operator=(const dmString& _s)  { SetText(_s); return *this; }

    int  MarginSize() const { return mMarginSize; }
    int  Alignement() const { return mAlignement; }

    void StaticText( bool _st );

    const dmString& Text()     const { return mText; }
    dmRect          TextRect() const;

  public:
    virtual void    OnDisplay( dmGraphics& ) const;
    virtual dmTool* Clone( dmTool* _parent = NULL ) const;

    dmDeclareClassInfo( dmText );
};
//--------------------------------------------------------
#endif // dmTextH
