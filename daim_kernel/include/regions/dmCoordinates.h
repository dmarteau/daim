#ifndef dmCoordinates_h
#define dmCoordinates_h

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
// File         : dmCoordinates.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

inline bool operator==( const dm_point& p1, const dm_point& p2 ) { return ((p1.x==p2.x) && (p1.y==p2.y)); }
inline bool operator!=( const dm_point& p1, const dm_point& p2 ) { return !(p1==p2); }

typedef enum {
  eUnknowType    = dmNoShape_t,
  eRectangleType = dmRectangle_t,
  eEllipseType   = dmEllipse_t,
  eCircleType    = dmCircle_t,
  ePolygoneType  = dmPoly_t,
  eLineType      = dmLine_t,
  ePointType,
  eRegionType,
} ECoordinatesType;

class dmCircle;
class dmRect;
class dmLine;
class dmEllipse;
class dmPoly;

inline dm_point dm_POINT( long x ,long y ) { dm_point p = { x,y }; return p; }
inline dm_rect  dm_RECT ( long x1,long y1,long x2, long y2 ) { dm_rect  r = { {x1, y1}, {x2,y2} }; return r; }

//
inline void dmTranslatePoint(dm_point& p,long dx,long dy) {
   p.x+=dx;
   p.y+=dy;
}
//------------------------------------------------------------
// Point
//------------------------------------------------------------
//
class dmPoint : public dm_point {
public:

   ECoordinatesType TypeOfCoords() const { return ePointType; }

   dmPoint(const dm_point& p)  :  dm_point(p) {}

   unsigned long BoxWidth()  const { return 1; }
   unsigned long BoxHeight() const { return 1; }

   dm_point Loc()         const { return *this; }
   dm_rect  BoundingBox() const { return dm_RECT( x,y,x,y ); }

   void Translate(long dx,long dy  )   { dmTranslatePoint(*this,dx,dy);   }
   void Translate( const dm_point& p ) { dmTranslatePoint(*this,p.x,p.y); }

   bool operator==(const dm_point& p) const {
     return (p.x == x) && (p.y == y);
   }

   dmPoint(long x0=0, long y0=0) { x=x0; y=y0; }
   dmPoint(const dmPoint& p)      : dm_point(p)  {}
   dmPoint& operator=(const dmPoint& p) { x=p.x; y=p.y; return *this; }

   dmPoint& operator-=(const dm_point& p) { x-=p.x; y-=p.y; return *this; }
   dmPoint& operator+=(const dm_point& p) { x+=p.x; y+=p.y; return *this; }

   dm_point  operator-() { return dm_POINT(-x,-y); }

   ~dmPoint() {}
};
//------------------------------------------------------------
inline dmPoint operator-(const dm_point& p1,const dm_point& p2) { return dmPoint( p1.x-p2.x, p1.y-p2.y ); }
inline dmPoint operator+(const dm_point& p1,const dm_point& p2) { return dmPoint( p1.x+p2.x, p1.y+p2.y ); }
//------------------------------------------------------------
inline dmPoint operator*(const dm_point& p,dm_real f)  { return dmPoint( (dm_int)daim::round(p.x*f),(dm_int)daim::round(p.y*f) ); }
inline dmPoint operator/(const dm_point& p,dm_real f)  { return dmPoint( (dm_int)daim::round(p.x/f),(dm_int)daim::round(p.y/f) ); }
inline dmPoint operator*(dm_real f, const dm_point& p) { return p*f; }
//------------------------------------------------------------
// Line
//------------------------------------------------------------
class dmLine : public dm_line {
public:

   ECoordinatesType TypeOfCoords() const { return eLineType; }

   explicit dmLine(const dm_line& l) : dm_line(l) {}

   unsigned long BoxWidth()  const { long w = end.x - start.x; return dm_abs(w)+1; }
   unsigned long BoxHeight() const { long h = end.y - start.y; return dm_abs(h)+1; }

   dm_point Loc() const { return dm_POINT(dm_max(start.x,end.x),dm_min(start.y,end.y)); }
   dm_rect  BoundingBox() const {
      return dm_RECT(
         dm_min(start.x,end.x),dm_min(start.y,end.y),
         dm_max(start.x,end.x),dm_max(start.y,end.y)
      );
   }

   void Translate(long dx,long dy) {
     dmTranslatePoint(start,dx,dy);
     dmTranslatePoint(end  ,dx,dy);
   }

   void Translate( const dm_point& p ) { Translate(p.x,p.y); }

   const dm_point& Start() const { return start; }
   const dm_point& End()   const { return end;   }

   dmPoint middle() const { return (start + end)/2.0; }

   bool operator==(const dmLine& l) const { return (l.start == start) && (l.end == end); }

   dmLine(const dmLine& l) : dm_line(l) {}
   dmLine(const dmPoint& p1, const dmPoint& p2) { start = p1, end = p2; }
   dmLine(long x1=0, long y1=0,long x2=0, long y2=0) { start = dm_POINT(x1,y1); end = dm_POINT(x2,y2); }

   dmLine& operator=(const dmLine& l) { start=l.start; end=l.end; return *this;}

   ~dmLine() {}
};
//------------------------------------------------------------
// Rectangle
//------------------------------------------------------------
class dmRect : public dm_rect {
public:
   ECoordinatesType TypeOfCoords() const { return eRectangleType; }

   bool IsEmpty() const { return (bottom_right.x < top_left.x || bottom_right.y < top_left.y); }

   long Top()    const { return top_left.y; }
   long Left()   const { return top_left.x; }
   long Right()  const { return bottom_right.x;}
   long Bottom() const { return bottom_right.y;}

   const dm_point& TopLeft()     const { return top_left;  }
   const dm_point& BottomRight() const { return bottom_right; }

   dm_point TopRight()   const { return dm_POINT(bottom_right.x,top_left.y); }
   dm_point BottomLeft() const { return dm_POINT(top_left.x,bottom_right.y); }

   // transtypage dm_rect -> dmRect
   dmRect(const dm_rect& r) : dm_rect(r) {}

   const dmRect&  Rectangle() const { return *this; }

   unsigned long Width()  const { return (bottom_right.x-top_left.x+1); }
   unsigned long Height() const { return (bottom_right.y-top_left.y+1); }

   unsigned long BoxWidth()  const { return Width(); }
   unsigned long BoxHeight() const { return Height();  }

   dm_point Loc()         const { return (top_left); }
   dm_rect  BoundingBox() const { return *this; }

   void Translate(long dx,long dy) {
     dmTranslatePoint(top_left ,dx,dy);
     dmTranslatePoint(bottom_right,dx,dy);
   }

   void Translate( const dm_point& p ) { Translate(p.x,p.y); }

   bool operator==(const dmRect& r) const { return (top_left==r.top_left && bottom_right==r.bottom_right); }

   dmRect(const dm_point& p1 ,const dm_point& p2) {
      top_left = dm_POINT(dm_min(p1.x,p2.x),dm_min(p2.y,p1.y));
     bottom_right = dm_POINT(dm_max(p1.x,p2.x),dm_max(p1.y,p2.y));
   }

   dmRect(const dmRect& r) : dm_rect(r) {}
   dmRect(long x1=0,long y1=0,unsigned long w=0,unsigned long h=0)
      { top_left = dm_POINT(x1,y1); bottom_right = dm_POINT(x1+w-1,y1+h-1); }

   dmRect(const dm_point& p,unsigned long w,unsigned long h)
      { top_left = p; bottom_right = dm_POINT(p.x+w-1,p.y+h-1); }

   dmRect(const dm_point& p,unsigned long w)
      { top_left = p; bottom_right = dm_POINT(p.x+w-1,p.y+w-1); }

   dmRect& operator=(const dm_rect& r) { top_left=r.top_left; bottom_right=r.bottom_right; return *this; }

   void Resize( int l,int t,int r, int b ) {
     top_left.x -= l; bottom_right.x += r;
     top_left.y -= t; bottom_right.y += b;
   }

   void Resize( int dx,int dy ) { Resize( dx,dy,dx,dy ); }
   void Resize( int d )         { Resize( d,d,d,d );     }

   dmPoint Center() const { return dmPoint( (Left()+Right())/2, (Top()+Bottom())/2 ); }

   dmPoint Dimensions() const { return dmPoint(Width(),Height()); }

   void Add( const dm_rect& r ) {
     if(IsEmpty()) operator=(r);
     else {
       top_left.x  = dm_min( top_left.x , r.top_left.x);
       top_left.y  = dm_min( top_left.y , r.top_left.y);
       bottom_right.x = dm_max( bottom_right.x, r.bottom_right.x);
       bottom_right.y = dm_max( bottom_right.y, r.bottom_right.y);
     }
   }

   bool Clip( long x1, long y1, long x2, long y2) {
     return dmClipRectangle(*this,x1,y1,x2, y2);
   }

   bool Clip( const dm_rect& clip) {
      return dmClipRectangle(*this,clip);
   }

   void Clear() { top_left = dm_POINT(0,0); bottom_right = dm_POINT(-1,-1); }

   ~dmRect() {}
};
//
inline dmRect operator*( const dmRect& r, dm_real f )  { return dmRect(r.TopLeft(),(long)(r.Width()*f + 0.5),(long)(r.Height()*f+0.5));}
inline dmRect operator/( const dmRect& r, dm_real f )  { return dmRect(r.TopLeft(),(long)(r.Width()/f + 0.5),(long)(r.Height()/f+0.5));}
inline dmRect operator*( dm_real f , const dmRect& r ) { return r*f; }
inline dmRect operator/( dm_real f , const dmRect& r ) { return r/f; }
//------------------------------------------------------------
// Circle
//------------------------------------------------------------
class dmCircle : public dm_circle {
public:

   ECoordinatesType TypeOfCoords() const { return eCircleType; }

   // transtypage dm_circle <-> dmCircle
   explicit dmCircle(const dm_circle&  c) : dm_circle(c) {}

   unsigned long  BoxWidth()  const { return (2*r+1); }
   unsigned long  BoxHeight() const { return (2*r+1); }

   dm_point Loc()         const { return dm_POINT(center.x-r,center.y-r); }
   dm_rect  BoundingBox() const { return dm_RECT(center.x-r,center.y-r,center.x+r,center.y+r);}

   void Translate(long dx,long dy)     { dmTranslatePoint(center,dx,dy); }
   void Translate( const dm_point& p ) { Translate(p.x,p.y); }

   bool operator==(const dmCircle& c) const { return (center==c.center && r==c.r); }

   dmCircle(const dmPoint& c, const long rad=0) { center = c; r = rad;}
   dmCircle(const dmCircle& c)                  : dm_circle(c) {}
   dmCircle( long x0=0, long y0=0, long rad=0) { r=rad; center = dm_POINT(x0,y0); }

   dmCircle& operator=(const dmCircle& c)  {  center=c.center; r=c.r; return *this;}

   ~dmCircle() {}
};
//------------------------------------------------------------
// Ellipse
//------------------------------------------------------------
class dmEllipse : public dm_ellipse {
public:

   ECoordinatesType TypeOfCoords() const { return eEllipseType; }

   // transtypage dm_ellipse <-> dmEllipse
   explicit dmEllipse(const dm_ellipse& e)  : dm_ellipse(e) {}

   unsigned long BoxWidth()  const { return (2*rx+1); }
   unsigned long BoxHeight() const { return (2*ry+1); }

   dm_point Loc()         const { return dm_POINT(center.x-rx,center.y-ry); }
   dm_rect  BoundingBox() const { return dm_RECT(center.x-rx,center.y-ry,center.x+rx,center.y+ry); }

   void Translate(long dx,long dy) { dmTranslatePoint(center,dx,dy)  ; }
   void Translate(const dm_point& p) { dmTranslatePoint(center,p.x,p.y); }

   bool operator==(const dmEllipse& e) const { return (center==e.center && rx==e.rx && ry==e.ry); }

   dmEllipse(const dmEllipse& e)  : dm_ellipse(e) {}
   dmEllipse(const dmPoint& c, unsigned long rh=0,unsigned long rv=0)  { center=c; rx=rh; ry=rv; }
   dmEllipse( long x0=0,long y0=0, unsigned long rh=0, unsigned long rv=0)
    { rx = rh; ry = rv; center.x = x0; center.y = y0; }

   dmEllipse& operator=(const dmEllipse& e) { center=e.center; rx=e.rx; ry=e.ry; return *this;}

   ~dmEllipse() {}
};
//------------------------------------------------------------
// Polygone
//------------------------------------------------------------
class dmPoly
{
 protected :
  dmVector<dm_point> _store;

 private:
  __dmKernel void Construct( unsigned long nn );
  __dmKernel void Construct( const dm_point* poly, unsigned long nn);
  __dmKernel void Construct( const dmPoly& poly );

 public :
  typedef dmVector<dm_point>::iterator       iterator;
  typedef dmVector<dm_point>::const_iterator const_iterator;

  iterator Begin() { return _store.Begin(); }
  iterator End()   { return _store.End();   }
  dm_point&  Back()  { return _store.Back();  }

  const_iterator Begin() const { return _store.Begin(); }
  const_iterator End()   const { return _store.End();   }
  const dm_point&  Back()  const { return _store.Back();  }

  unsigned long Capacity() const { return _store.Capacity(); }
  void SetCapacity( unsigned long _c ) { _store.Reserve(_c); }

  void Push_Back (const dm_point& p) { _store.Push_Back(p); }
  void Pop_Back  ()                { _store.Pop_Back(); }

  void Insert( iterator it, const dm_point& p) { _store.Insert(it,p); }
  void Insert( int index,   const dm_point& p) { _store.Insert(Begin()+index,p); }

  void Remove( iterator it ) { _store.Erase(it); }
  void Remove( int index   ) { _store.Erase(Begin()+index); }

  void Resize( size_t _newsize ) { _store.Resize(_newsize); }
  void Clear() { _store.Clear(); }

  ECoordinatesType TypeOfCoords() const { return ePolygoneType; }

  dm_rect BoundingBox() const;

  unsigned long BoxWidth()  const { dmRect box = BoundingBox(); return (box.BoxWidth()); }
  unsigned long BoxHeight() const { dmRect box = BoundingBox(); return (box.BoxHeight()); }

  dm_point Loc() const { dmRect box = BoundingBox(); return (box.Loc()); }

  void Translate(long dx,long dy) {
     for(iterator it=Begin();it!=End();++it)
      dmTranslatePoint(*it,dx,dy);
  }

  void Translate( const dm_point& p ) {
     Translate(p.x,p.y);
  }

  const dm_point& operator[](unsigned long i) const { return _store[i]; }
  dm_point&       operator[](unsigned long i)       { return _store[i]; }

  size_t Size() const { return _store.Size(); };
  bool Closed() const { return ( (Size()>2) && (_store[0] == _store[Size()-1])); }

  dmPoly( unsigned long nn )                       { Construct(nn);      }
  dmPoly( const dm_point* poly, unsigned long nn)  { Construct(poly,nn); }
  dmPoly( const dmPoly& poly                    )  { Construct(poly);    }
  dmPoly() {}

  __dmKernel long IntersectHor(long,long*,long*) const;
  __dmKernel dmPoly&  operator=(const dmPoly& poly);

  __dmKernel int Bresenham( const dm_line& );
  __dmKernel int Bresenham( const dm_ellipse& );
  __dmKernel int Bresenham( const dmPoly& );

  ~dmPoly() {}

};

// BoudingBox of a polygone
inline dm_rect dmPoly::BoundingBox() const
{
   // unsigned long u
   long u,n=Size();
   dm_rect r;

   r.top_left.x = r.bottom_right.x = _store[0].x;
   r.top_left.y = r.bottom_right.y = _store[0].y;
   for(long i=1;i<n;i++) {
	 if      ( (u= _store[i].x) > r.bottom_right.x ) r.bottom_right.x = u;
	 else if ( u < r.top_left.x  ) r.top_left.x  = u;
	 if      ( (u= _store[i].y) > r.bottom_right.y ) r.bottom_right.y = u;
	 else if ( u < r.top_left.y  ) r.top_left.y  = u;
   }
   return (r);
}
//------------------------------------------------------------
#endif // dmCoordinates_h
