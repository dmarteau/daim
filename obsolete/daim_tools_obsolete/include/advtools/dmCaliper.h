#ifndef dmCaliper_h
#define dmCaliper_h

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

// File         : dmCaliper.h
// Date         : 8/2005
// Author       : David Marteau
//

class dmCaliper : public dmLineEdit
{
  private:
    dm_int ticklength;
    bool   showHorz;
    bool   csHoldGraphics;

  protected:
    void Init();

    virtual void OnMove( int dx, int dy,bool _update );

  public:
    dmCaliper( dmTool* _parent = NULL );
    dmCaliper( const dmCaliper& _toCopy, dmTool* _parent = NULL );
   ~dmCaliper();
 
    void   SetTickLength( dm_int _length );
    dm_int GetTickLength() const { return ticklength; }

    void ShowHorizon( bool );

  public:
    virtual void    OnDisplay( dmGraphics& ) const;
    virtual dmTool* Clone( dmTool* _parent = NULL ) const;

    virtual bool OnKeyDown(dm_uint _keyCode,dm_uint _keyQual); 

  public:
    dmDeclareClassInfo( dmCaliper );
};

#endif // dmCaliper_h
