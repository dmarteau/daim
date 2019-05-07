
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
#include "dmArrows.h"
//----------------------------------------------------------------------------
dmImplementObject( dmLineTerminator, dmObject );
//----------------------------------------------------------------------------
#define dmArrowTerminator_SCHEMA_VERSION 1
dmImplementClassInfo( dmArrowTerminator, dmLineTerminator, dmArrowTerminator_SCHEMA_VERSION );
//----------------------------------------------------------------------------
dmArrowTerminator::dmArrowTerminator( const dmArrowTerminator& toCopy )
: nA(toCopy.nA)
 ,nB(toCopy.nB)
 ,nC(toCopy.nC) 
{}
//----------------------------------------------------------------------------
dmArrowTerminator::dmArrowTerminator( int a, int b, int c )
: nA(a)
 ,nB(b)
 ,nC(c) 
{}
//----------------------------------------------------------------------------
dmArrowTerminator::~dmArrowTerminator() 
{}
//----------------------------------------------------------------------------
dmArrowTerminator& dmArrowTerminator::operator=( const dmArrowTerminator& _toCopy )
{
  nA=_toCopy.nA; 
  nB=_toCopy.nB; 
  nC=_toCopy.nC;
  return *this;
}
//----------------------------------------------------------------------------
dmLineTerminator* dmArrowTerminator::Clone() const
{
  return new dmArrowTerminator(*this);
}
//----------------------------------------------------------------------------
void dmArrowTerminator::Display(  dmGraphics& gr , 
                                  const dmPoint& ptStart, const dmPoint& ptEnd ) const
{ 
  PtArray p;
  GetPoints( p, ptStart, ptEnd );
  gr.DrawPoly(p,5,true);
}
//----------------------------------------------------------------------------
void dmArrowTerminator::GetPoints( PtArray& p, const dmPoint& ptStart, const dmPoint& ptEnd ) const
{
  dm_double ux,uy;
  if( ptStart.x!=ptEnd.x || ptStart.y!=ptEnd.y) {
    dm_int dx   = ptEnd.x - ptStart.x;
    dm_int dy   = ptEnd.y - ptStart.y;
    dm_double L = sqrt( (dm_double)(dx)*(dx) + (dm_double)(dy)*(dy) );
    ux = (dx)/L;
    uy = (dy)/L;
  } else { ux = 1; uy = 0; };

  p[4] = p[0] = ptEnd;
  p[1].x = ptEnd.x - (dm_int)daim::round(nC * ux - nB * uy);
  p[1].y = ptEnd.y - (dm_int)daim::round(nC * uy + nB * ux);
  p[3].x = ptEnd.x - (dm_int)daim::round(nC * ux + nB * uy);
  p[3].y = ptEnd.y - (dm_int)daim::round(nC * uy - nB * ux);
  p[2].x = ptEnd.x - (dm_int)daim::round(nA * ux);
  p[2].y = ptEnd.y - (dm_int)daim::round(nA * uy); 
} 
//----------------------------------------------------------------------------
void dmArrowTerminator::SetParameters( int a, int b, int c ) { nA=a; nB=b; nC=c; }
//----------------------------------------------------------------------------
void dmArrowTerminator::GetParameters( int& a, int& b, int& c ) const 
{ a=nA; b=nB; c=nC; }
//----------------------------------------------------------------------------
