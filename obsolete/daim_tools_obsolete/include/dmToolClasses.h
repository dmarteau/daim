#ifndef dmToolsClasses_h
#define dmToolsClasses_h

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
// File         : dmToolsClasses.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

#define dmTOOL_LINE           _TXT("line")
#define dmTOOL_ARROW          _TXT("arrow")
#define dmTOOL_RECT           _TXT("rectangle")
#define dmTOOL_ELLIPSE        _TXT("ellipse")
#define dmTOOL_CIRCLE         _TXT("circle")
#define dmTOOL_POLYLINES      _TXT("polylines")
#define dmTOOL_CLOSEDPOLY     _TXT("closedpoly")
#define dmTOOL_OPENFREEHAND   _TXT("openfreehand")
#define dmTOOL_CLOSEDFREEHAND _TXT("closedfreehand")
#define dmTOOL_LABEL          _TXT("label")
#define dmTOOL_EDITLABEL      _TXT("editlabel")
#define dmTOOL_IMAGE          _TXT("image")

#define dmTOOL_POLY_EX        _TXT("poly_ex")
#define dmTOOL_RECT_EX        _TXT("rectangle_ex")
#define dmTOOL_CIRCLE_EX      _TXT("circle_ex")

//---------------------------------------------------------------------
#define __dmFN_DECLARE(fn) dmTool* fn( const dmString::E* _class, dmClassInfo* infos ,const dmPoly* pts  )
//---------------------------------------------------------------------
__dmTools  bool dmRegisterTools();
__dmTools  bool dmRegisterClass( const dmString::E* _class, const dmString::E* _base, dmClassInfo*, dmTool* (*pfnCreate)( const dmString::E*, dmClassInfo*, const dmPoly* ) );
__dmTools  bool dmRegisterTool ( const dmString::E* _class, dmClassInfo*, dmTool* (*pfnCreate)( const dmString::E*, dmClassInfo*, const dmPoly* ), dmClassInfo* pPrefs = NULL );

__dmTools  dmTool*      dmCreateTool  ( const dmString::E* _class, const dmPoly* pts = NULL, dm_uint flags = 0);
__dmTools  dmClassInfo* dmGetToolPrefs( const dmString::E* _class );
//---------------------------------------------------------------------
#endif // dmToolsClasses_h

