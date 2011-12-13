#ifndef dmObjectDetection_h
#define dmObjectDetection_h

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
// File         : dmObjectDetection.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

#include <vector>

namespace daim {

//-----------------------------------------------------------
// Search for the boundary of a region
// You will need two operators in order to use this template
//
// check_bound( int x, int y ) :
// this operator should return true if the coordinates x and y
// are in bounds of the objects
//
// process( int i, int j ) : Will process the point at location i,j
//-----------------------------------------------------------
struct check_point_true : public std::binary_function<int,int,bool>
{
  bool operator()( int,int ) { return true; }
};
//-----------------------------------------------------------
#define __check_octant( s,sx,sy,x,y ) \
    switch(s) {                       \
      case 0: sy=y   ;sx=x+1 ; break; \
      case 1: sy=y-1 ;sx=x+1 ; break; \
      case 2: sy=y-1 ;sx=x   ; break; \
      case 3: sy=y-1 ;sx=x-1 ; break; \
      case 4: sy=y   ;sx=x-1 ; break; \
      case 5: sy=y+1 ;sx=x-1 ; break; \
      case 6: sy=y+1 ;sx=x   ; break; \
      case 7: sy=y+1 ;sx=x+1 ; break; \
    }
//-----------------------------------------------------------
template<class _Pr,class _Cb>
int boundary_search( const dmPoint&  p,
                     const dmRegion& _rgn,
                     _Pr _process,
                     _Cb _check_bounds
                   )
{
  #define __inbounds( ii,jj ) (_rgn.PointInRoi(ii,jj) && _check_bounds(ii,jj))
  #define __pix( jj,ii )      ( jj*bottom  + ii )

  int bottom = _rgn.BoxWidth();
  int count=0, i,j,s,si,sj,dir;
  i = p.x; j = p.y;

  // FIXME: Starting search direction
  // must be parametrable, the following leads to problems
  // if objects have holes at 1 pixel from border.

  si = i+1; sj = j+1;
  bool bprev, found = __inbounds(si,sj);
  for(s=0;s<8;++s) {
    bprev = !found;
    __check_octant( s,si,sj,i,j );
    found = __inbounds(si,sj);
    if(bprev && found ) break;
  }

  if(!(bprev && found)) {
    return (!_process(i,j)?1:0);
  }

  i = si; j = sj;
  int first    = __pix(j,i);
  int current  = 0;
  int second   = current;
  int previous = current;
  do {
    if(__inbounds( si,sj ))
    {
      ++count; j=sj; i=si;
      previous = current; current = __pix(j,i);
      if(!_process( si,sj ) )
        return count;
      if(count==2)
        second = current;
      dir = s;
      s = ( (dir & 0x1)    // is odd ?
              ? ((dir+6) % 8)
              : ((dir+7) % 8)
          );
    } else {
      ++s; s = (s % 8);
    }
    __check_octant( s,si,sj,i,j );
  } while( count <=2 || (current != second && previous != first));

  return count;

  #undef __inbounds
  #undef __pix

}

#undef __check_octant

//-----------------------------------------------------------
// Get the boundary of a region and store it into a vector
// Note : the vector should have been set to a proper size for
// avoiding multiple reallocation
//-----------------------------------------------------------
int extract_rgn_boundary( const dmRegion& _rgn, dmPoly& _p );
/*
{
  if(!_rgn.IsEmptyRoi())
  {
    if(_rgn.IsRectRoi()) {
      _p.Push_Back(_rgn.Rectangle().TopLeft());
      _p.Push_Back(_rgn.Rectangle().TopRight());
      _p.Push_Back(_rgn.Rectangle().BottomRight());
      _p.Push_Back(_rgn.Rectangle().BottomLeft());
      _p.Push_Back(_rgn.Rectangle().TopLeft());    // close polygone
    } else  {
      _append_to_vector _operator(_p);
      dmPoint ps = _rgn.RegionStart();  // Get the first point in region
      boundary_search(ps,_rgn,bind_ref(_operator),check_point_true());
      if(!_p.closed())
        _p.Push_Back(_p[0]);
    }
  }
  return _p.size();
}
*/

//-----------------------------------------------------------
// Use the _bind_ref operator to bind a r�ference to a predicat
// for the algorithm
//-----------------------------------------------------------

template<class _Pr>
struct _ref_binder : public std::binary_function<int,int,bool>
{
  _Pr& _pred;
  _ref_binder( _Pr& op ) : _pred(op) {}
  bool operator()( int x, int y ) { return _pred(x,y); }
};

// functional binder
template<class _Pr>
_ref_binder<_Pr> bind_ref( _Pr& op ) { return _ref_binder<_Pr>(op); }


//-----------------------------------------------------------
// boundary extractor : append point to a vector
//-----------------------------------------------------------
struct boundary_extractor : public std::binary_function<int,int,bool>
{
  dmPoly& _p;
  boundary_extractor( dmPoly& aVector ) : _p(aVector) {}

  bool operator()( int x, int y ) {
      _p.Push_Back(dmPoint(x,y));
      return true;
  }
};

//-----------------------------------------------------------
// Bind an an image to a predicat that act on pixel values
// This operator must be passed as a bound_checker
//-----------------------------------------------------------
template<class T,class _Pr>
struct _image_predicat : public std::binary_function<int,int,bool>
{
  _Pr _pr;
  image<T>& _im;
  _image_predicat( _Pr aPred , image<T>& anImg )
  : _pr(aPred),_im(anImg) {}

  bool operator()( int x, int y ) {
    return _pr(_im[y][x]);
  }
};

template<class T,class _Pr>
struct _const_image_predicat : public std::binary_function<int,int,bool>
{
  _Pr _pr;
  const image<T>& _im;
  _const_image_predicat( _Pr aPred , const image<T>& anImg )
  : _pr(aPred),_im(anImg) {}

  bool operator()( int x, int y ) {
    return _pr(_im[y][x]);
  }
};


// functional binder
template<class T, class _Pr>
_image_predicat<T,_Pr> bind_to_image( image<T>& _im, _Pr _op )
{
  return _image_predicat<T,_Pr>(_op,_im);
}

// functional binder
template<class T, class _Pr>
_const_image_predicat<T,_Pr> bind_to_const_image( const image<T>& _im, _Pr _op )
{
  return _const_image_predicat<T,_Pr>(_op,_im);
}


// Usage example :
//
// This will fill a vector with points of the
// boundary of object that have a pixel value of X,
// starting at position pstart
//
// boundary_search(pstart,image.rect(),
//   boundary_extractor(aVector)),  // _process
//   bind_to_image(                 // _check_bounds
//     image
//     bind2nd(equal_to<int>(),X),
//   ))
// );

// Thus, you can write a generalized extractor
// based on a predicat acting on the value of the pixel
template<class _Pr,class T>
int extract_boundary( _Pr _op,
                      const dmPoint&  _p0,
                      const dmRegion& _rgn,
                      const image<T>& _image,
                      dmPoly&         _vector
                    )
{
  return boundary_search(_p0,_rgn,
           boundary_extractor(_vector), // _process
           bind_to_const_image(_image,_op));  // _check_bounds
}

//-----------------------------------------------------------
// Generalized sweeping algorithm for objects detection
// the _process operator should return true if the coordinates
// correspond to an object location
// The algorithm ensure that a processed point is not hit twice.
//-----------------------------------------------------------
struct sweep_algorithm
{
  template<class _Pr>
  static int sweep(
               _Pr&   _process,
               const dmPoint& _p,
               const dmRect&  _r, connectivity _connect,
               std::vector<dmPoint>& _buffer )
  {
    dmDEBUG_ASSERT( _buffer.size() > 0 );

    if(!dmPointInRectangle(_p,_r))
      return 0;

    int x,y,way,cx,cy,cnt = 0;
    int xmin = _r.Left()+1  , ymin = _r.Top()+1;
    int xmax = _r.Width()-2 , ymax = _r.Height()-2;

    std::vector<dmPoint>::iterator pfirst = _buffer.begin();
    std::vector<dmPoint>::iterator pfree  = pfirst + 1;

    if(!_process( _p.x, _p.y ))
    return 0;

    ++cnt;

    *pfirst = _p;
     while( pfirst != pfree )
     {
       x = (*pfirst).x;
       y = (*pfirst).y;

       if( ++pfirst == _buffer.end() )
          pfirst = _buffer.begin();

       for(way=1; way<=_connect; ++way ) {
         cx = x; cy = y;
         switch( way ) {
           case 1 : if( cx <= xmax ) ++cx; break;
           case 2 : if( cy <= ymax ) ++cy; break;
           case 3 : if( cx >= xmin ) --cx; break;
           case 4 : if( cy >= ymin ) --cy; break;
           case 5 : if( cx <= xmax && cy <= ymax) { ++cx; ++cy; } break;
           case 6 : if( cx <= xmax && cy >= ymin) { ++cx; --cy; } break;
           case 7 : if( cx >= xmin && cy <= ymax) { --cx; ++cy; } break;
           case 8 : if( cx >= xmin && cy >= ymin) { --cx; --cy; } break;
         }

         if(_process(cx,cy)) {
           ++cnt;
           (*pfree).x = cx;
           (*pfree).y = cy;
           if( ++pfree == _buffer.end() )
             pfree=_buffer.begin()+1;
         }
      }
    }
    return cnt;
  }


  template<class T>
  struct swap_pixel
  {
     image<T>& _im;
     T _new_val;
     T _old_val;
     swap_pixel( image<T>& anImg, T _new, T _old )
     : _im(anImg), _new_val(_new),_old_val(_old) {}

     bool operator()( const int& x, const int& y ) {
       if(_im[y][x] == _old_val) {
         _im[y][x] = _new_val;
         return true;
       }
       return false;
     }
  };

  //---------------------------------------------------------------------
  // Operator used for setting a new pixel value on a connexe region value
  // according to the actual value of that pixel
  //---------------------------------------------------------------------
  template<class T>
  struct _select_object
  {
    image<T>&             _im;
    std::vector<dmPoint>& _buf;
    connectivity          _c;

    typedef typename pixel_traits<T>::value_type value_type;

    value_type _pixobj;
    value_type _done;

    _select_object( image<T>&  image,
                    value_type pixobj,
                    value_type done,
                    std::vector<dmPoint>& pts,
                    connectivity c
                  )
    : _im(image),_buf(pts),_c(c),_pixobj(pixobj),_done(done) {}


    void operator()( int y, int x1, int x2 )
    {
      typename image<T>::pointer_type line = _im[y];

      sweep_algorithm::swap_pixel<T> op(_im,_done,_pixobj);
      for(int x=x1;x<=x2;++x) {
        if(line[x]==_pixobj) {
          sweep_algorithm::sweep(op,dmPoint(x,y),_im.rect(),_c,_buf);
        }
      }
    }
  };


  //---------------------------------------------------------------------
  // Select objects  by sweeping from a whole region
  //---------------------------------------------------------------------
  template<class T>
  static void select_objects(  connectivity c,
                const dmRegion&  rgn,
                image<T>&        img,
                typename image<T>::value_type pixobj,
                typename image<T>::value_type done,
                std::vector<dmPoint>& buf )
  {
  	 sweep_algorithm::_select_object<T> op(img,pixobj,done,buf,c);
     dmDigitalRegion(rgn,op);
  }

  //---------------------------------------------------------------------
  // Select one object from a starting point
  //---------------------------------------------------------------------
  template<class T>
  static void select_object(  connectivity c,
                const dmPoint&  p,
                image<T>&       img,
                typename image<T>::value_type pixobj,
                typename image<T>::value_type done,
                std::vector<dmPoint>& buf )
  {
  	sweep_algorithm::swap_pixel<T> op(img,done,pixobj);
    sweep_algorithm::sweep(op,p,img.rect(),c,buf);
  }

}; // sweep_algorithm

//---------------------------------------------------------------------

} // namespace daim

#endif // dmObjectDetection_h

