#ifndef dmBresenham_2_h
#define dmBresenham_2_h

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
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//--------------------------------------------------------

namespace daim {

  // Bresenham's algorithm for drawing digital line

  template<class pixel>
  inline void bresenham_line( pixel& _putpixel, long x1, long y1, long x2, long y2 ) 
  {
    long _d, _x, _y;
    long _ax, _ay, _sx, _sy, _dx, _dy;

    _dx = x2-x1;  _ax = dm_abs(_dx)<<1;  _sx = dm_sgn(_dx);
    _dy = y2-y1;  _ay = dm_abs(_dy)<<1;  _sy = dm_sgn(_dy);
												
    _x = x1; _y = y1;                           
    if (_ax>_ay) {                              
      _d = _ay-(_ax>>1);                        
      for (;;) {                                
        _putpixel(_x,_y);                       
        if (_x==x2) break;                      
        if (_d>=0) { _y += _sy; _d -= _ax; }    
        _x += _sx; _d += _ay;                   
	    }                                         
    } else {                                    
      _d = _ax-(_ay>>1);                       
      for (;;) {                               
        _putpixel(_x, _y);                    
        if (_y==y2) break;                    
        if (_d>=0) { _x += _sx; _d -= _ay; }  
        _y += _sy; _d += _ax;                 
      }                                        
    } 
  }

  // Bresenham's algorithm for drawing digital circle

  template<class pixel_oct>
  inline void bresenham_circle(pixel_oct& _putpixel_oct, long r)
  {                          
    long _x,_y;                                 
    long _s;                                    
    if(r>0) {                                               
      _x = 0; _y = r;                                    
      _s = 3-2*r;                                        
      while( (_x <= _y) ) {                              
        _putpixel_oct(_x,_y);                           
        _s+= (_s<=0) ? (4*_x++ + 6) : 4*(_x++ - _y--)+10;
	    }                                                  
    }                                                     
  }


  // Bresenham's algorithm for drawing digital ellipse

  template<class pixel_quad>
  inline void bresenham_ellipse(pixel_quad& _putpixel_quad, long a, long b )
  {                                                   
    long x,y;                              
    long a2,b2,s,t;                                 
                                                      
    a2 = a*a;                                       
    b2 = b*b;                                       
    x = 0;                                          
    y = b;                                          
    s = a2*(1-2*b) + 2*b2;                          
    t = b2 - 2*a2*(2*b-1);                          
    _putpixel_quad(x,y);                            
    do {                                            
      if (s<0) {                                  
          s += 2*b2*(2*x+3);                      
          t += 4*b2*(x+1);                        
          x++;                                    
      } else if (t<0) {                           
          s += 2*b2*(2*x+3) - 4*a2*(y-1);         
          t += 4*b2*(x+1) - 2*a2*(2*y-3);         
          ++x;                                    
          --y;                                    
      } else {                                    
          s -= 4*a2*(y-1);                        
          t -= 2*a2*(2*y-3);                      
          --y;                                    
      }                                           
      _putpixel_quad(x,y);                        
    } while (y>0);                                  
  }

  // Helpers for pixel_quads and pixel_octs

  template<class scanline>
  struct putpixel
  {
    scanline& _scanln;
    putpixel( scanline& scln ) : _scanln(scln) {}

    void operator()( long x, long y ) {
      _scanln( x, x, y );
    }
  };

  template<class scanline>
  struct putpixel_quad
  {
    long _x0,_y0;
    scanline& _scanln;
    putpixel_quad( long x0, long y0, scanline& scln ) 
    : _x0(x0),_y0(y0), _scanln(scln) {}

    void operator()( long x, long y ) {
      _scanln( _x0-x, _x0+x, _y0-y );
      _scanln( _x0-x, _x0+x, _y0+y );
    }
  };

  template<class scanline>
  struct putpixel_oct
  {
    long _x0,_y0;
    scanline& _scanln;
    putpixel_oct( long x0, long y0, scanline& scln ) 
    : _x0(x0),_y0(y0), _scanln(scln) {}

    void operator()( long x, long y ) {
      _scanln( _x0-y, _x0+y, _y0-x );
      _scanln( _x0-x, _x0+x, _y0-y );
      _scanln( _x0-y, _x0+y, _y0+x );
      _scanln( _x0-x, _x0+x, _y0+y );
    }
  };


  template<class scanline>
  struct bresenham
  {
    scanline& _scanln;
    bresenham( scanline& scln ) : _scanln(scln) {}

    void line( long x1, long y1, long x2, long y2 )  {
      bresenham_line( putpixel<scanline>(_scanln), x1,y1,x2,y2 );
    }
 
    void circle( long x0, long y0, long r )  {
      bresenham_circle( putpixel_oct<scanline>(x0,y0,_scanln), r );
    }

    void ellipse( long x0, long y0, long a, long b )  {
      bresenham_ellipse( putpixel_quad<scanline>(x0,y0,_scanln), a,b );
    }
 
  };



}; // namespace daim

#endif /* dmBresenham_2_h */
