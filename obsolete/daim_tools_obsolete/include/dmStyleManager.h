#ifndef dmStyleManager_h
#define dmStyleManager_h

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
// File         : dmStyleManager.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

class __dmTools dmStyleManager : public dmServices
{
  typedef dmLink<dmDrawStyle> _StyleLink;

  public:
   dmTList<_StyleLink> mStyles;

  public:
   dmStyleManager() {};
  ~dmStyleManager();

   virtual void SetDefaultConfiguration();

   dmLink<dmDrawStyle> GetStyle( const dmString::E* _id, bool _bCreateIfNotExists );
   dmLink<dmDrawStyle> SetStyle( const dmDrawStyle& , bool _bCreateIfNotExists = false );

   dmDeclareClassInfo( dmStyleManager );

};

inline dmLink<dmDrawStyle> dmGetDrawStyleClass ( const dmString::E* _id, bool _bCreateIfNotExists = false ) {
   return dmGetServices<dmStyleManager>()->GetStyle(_id,_bCreateIfNotExists);
}

//--------------------------------------------------------
// Coercitive way for getting drawing attributs
//--------------------------------------------------------
inline dmPenAttributs&  dmGetDrawStylePen ( const dmString::E* _id ) {
   return dmGetServices<dmStyleManager>()->GetStyle(_id,true)->mPenAttributs;
}

inline dmTextAttributs& dmGetDrawStyleText(  const dmString::E* _id ) {
  return dmGetServices<dmStyleManager>()->GetStyle(_id,true)->mTextAttributs;
}

#endif // dmStyleManager_h
