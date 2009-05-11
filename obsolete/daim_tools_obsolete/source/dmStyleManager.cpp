
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


#define dmUseServicesMngr
#include "daim_tools.h"
#include "dmStyleManager.h"

//----------------------------------------------------------
#define dmStyleManager_SCHEMA_VERSION 1
dmImplementClassInfo( dmStyleManager, dmServices, dmStyleManager_SCHEMA_VERSION )
//----------------------------------------------------------
dmStyleManager::~dmStyleManager()
{}
//----------------------------------------------------------
void dmStyleManager::SetDefaultConfiguration()
{
  dmTextAttributs _DfltText;
  dmPenAttributs  _DfltCarretPenAttrs(eClr_BLACK,eClr_LIGHTYELLOW ,0,ePenStyle_solid,eBrushStyle_solid);
  dmPenAttributs  _DfltRoiPenAttrs   (eClr_BLACK,eClr_WHITE  ,0,ePenStyle_dotted);
  dmPenAttributs  _DfltPenAttrs      (eClr_BLACK,eClr_WHITE  ,0,ePenStyle_solid );

  SetStyle(dmDrawStyle(_TXT("*default") ,_DfltPenAttrs      ,_DfltText),true);
  SetStyle(dmDrawStyle(_TXT("*carrets") ,_DfltCarretPenAttrs,_DfltText),true);
  SetStyle(dmDrawStyle(_TXT("*roi")     ,_DfltRoiPenAttrs   ,_DfltText),true);
}
//----------------------------------------------------------
dmLink<dmDrawStyle> dmStyleManager::GetStyle( const dmString::E* _id,
                                            bool _bCreateIfNotExists )
{
  if(_id && _id[0]!='\0') 
  {
    dmTList<_StyleLink>::iterator __F = mStyles.Begin();
    dmTList<_StyleLink>::iterator __L = mStyles.End();
    for(;__F!=__L;++__F) {
      if( (*__F)->mClass == _id)
        return *__F;
    }

    if(_bCreateIfNotExists) {
      dmLink<dmDrawStyle> _NewStyle = dmAutoLink<dmDrawStyle>(new dmDrawStyle);
      mStyles.Push_Back(_NewStyle);
      return _NewStyle;
    }
  }
  return dmReturnLink( (dmDrawStyle*)NULL );
}
//----------------------------------------------------------
dmLink<dmDrawStyle> dmStyleManager::SetStyle( const dmDrawStyle& _aStyle, 
                                            bool _bCreateIfNotExists )
{
  dmLink<dmDrawStyle> _Style = GetStyle(_aStyle.mClass.CStr(),_bCreateIfNotExists);
  if(!_Style.IsNull()) 
    *_Style = _aStyle;

  return _Style;
}
//----------------------------------------------------------
