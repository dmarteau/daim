#ifndef dmBresenham_h
#define dmBresenham_h

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
// File         : dmBresenham.h
// Date         : 7/2004
// Author       : David Marteau
// THIS FILE IS SUBJECT TO CHANGE WITHOUT NOTICE
//--------------------------------------------------------

// y'a un bug dans le calcul de l'ellipse
// en particulier lorsque le nombre de ligne est impair
// la ligne du mileu n'est pas calcul�e
// En attendant de trouver l'erreur il faut initialiser toute
// les lignes � la largeur de la bounding-box de l'ellipse

#define BUG_BRESENHAM

// Bresenham's algorithm for drawing digital ellipse
#define _DigitalEllipse(rh,rv) {                        \
	  long _x,_y;                            \
		long _s,_rv2,_rh2;                              \
		_rv2 = rv*rv; _rh2 = rh*rh;                     \
		_x = 0; _y = rv;                                \
		_s = 2 * ( _rv2 - _rh2*rv ) + _rh2;             \
		if(rv>0) while( _x*_rv2 <= ((_y+1)*_rh2) ) {    \
		  _SetPixel_qads(_x,_y);                        \
		  if( _s>0  ) _s += 4*_rh2*(1 - _y-- );         \
		  _s += _rv2*(4 * _x++ + 6);                    \
		  if(_y<0 || _x>rh) break;                      \
		}                                               \
		if(_y<=0) {                                     \
		   while( _x <= rh ) {                          \
		     _SetPixel_qads(_x,0);                      \
		     _x++;                                      \
		   }                                            \
		} else {                                        \
		   _x = rh; _y  = 0;                            \
		   _s = 2 * ( _rh2 - _rv2*rh ) + _rv2;          \
		   if(rh>0) while( ((_x+1)*_rv2) >= _y*_rh2 ) { \
		      _SetPixel_qads(_x,_y);                    \
		      if( _s>0 ) _s += 4*_rv2*(1 - _x-- );      \
		      _s += _rh2*(4 * _y++ + 6);                \
		      if(_x<0 || _y>rv) break;                  \
		   }                                            \
		   if(_x<=0) while(_y <= rv) {                  \
		      _SetPixel_qads(0,_y);                     \
		      _y++;                                     \
		   }                                            \
		}                                               \
}

#include "dmBresenham_2.h"

#endif /* dmBresenham_h */
