
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
//----------------------------------------------------------------------------
#define dmLineExtEdit_SCHEMA_VERSION 1
dmImplementClassInfo( dmLineExtEdit, dmLineEdit, dmLineExtEdit_SCHEMA_VERSION );
//----------------------------------------------------------------------------
dmLineExtEdit::dmLineExtEdit( dmTool* _parent )
: dmLineEdit(_parent)
 ,pDrawStart(NULL)
 ,pDrawEnd(NULL) 
{}
//----------------------------------------------------------------------------
dmLineExtEdit::dmLineExtEdit( const dmLine& _line, dmTool* _parent )
: dmLineEdit(_line,_parent)
 ,pDrawStart(NULL)
 ,pDrawEnd(NULL)
{}
//----------------------------------------------------------------------------
dmLineExtEdit::dmLineExtEdit( const dmLineExtEdit& _toCopy, dmTool* _parent )
: dmLineEdit(_toCopy,_parent)
 ,pDrawStart(NULL)
 ,pDrawEnd(NULL)
{
  if(_toCopy.pDrawEnd)   pDrawEnd   = _toCopy.pDrawEnd->Clone();
  if(_toCopy.pDrawStart) pDrawStart = _toCopy.pDrawStart->Clone();
}
//----------------------------------------------------------------------------
dmLineExtEdit::~dmLineExtEdit()
{
  if(pDrawEnd)   delete pDrawEnd;
  if(pDrawStart) delete pDrawStart;
}
//----------------------------------------------------------------------------
void dmLineExtEdit::OnDisplay( dmGraphics& gr ) const
{
  gr.DrawLine(Start(),End());
  if(pDrawEnd  ) pDrawEnd->Display  (gr,Start(),End());
  if(pDrawStart) pDrawStart->Display(gr,End(),Start());
}
//----------------------------------------------------------------------------
void dmLineExtEdit::Recalculate()
{
  //size_t n1 = (pDrawStart ? pDrawStart->Size() : 0);
  //size_t n2 = (pDrawEnd   ? pDrawEnd->Size()   : 0);
  //SetArrowSize( __max__(n1,n2) );
  dmLineEdit::Recalculate();
}
//----------------------------------------------------------------------------
void dmLineExtEdit::SetArrowStart( const dmLineTerminator* pNew)
{
  if(pDrawStart) delete pDrawStart;
  pDrawStart = (pNew ? pNew->Clone() : NULL);
  UpdateObject();
}
//----------------------------------------------------------------------------
void dmLineExtEdit::SetArrowEnd( const dmLineTerminator* pNew)
{
  if(pDrawEnd) delete pDrawEnd;
  pDrawEnd = (pNew ? pNew->Clone() : NULL);
  UpdateObject();
}
//----------------------------------------------------------------------------
dmTool* dmLineExtEdit::Clone(dmTool* _parent ) const
{
  return new dmLineExtEdit(*this,_parent);
}
//----------------------------------------------------------------------------
