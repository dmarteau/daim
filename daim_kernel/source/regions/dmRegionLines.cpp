
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

#define dmUserIncludes
#include "daim_kernel.h"
#include "regions/dmRegionLineBuffer.h"
#include "regions/dmBresenham.h"

//-----------------------------------------------------

#define THROW_MEMORY_ERROR \
    throw
    //dmTHROW_MSG_EXCEPTION(ERegionError,_TXT("MEMORY ALLOCATION FAILURE !"))

//
static inline size_t __BEST_SIZE(const dmRgnHandle* r1,const dmRgnHandle* r2) {
  dm_size opsize = r1->Size()+r2->Size()+dmRgnLineArray::SecurityBuffer;
  return (opsize < dmRgnLineArray::MaxDfltArraySize ?
                   dmRgnLineArray::MaxDfltArraySize : opsize);
}
//
static inline size_t __MAX_SIZE(size_t sz) {
  dm_size opsize = sz+dmRgnLineArray::SecurityBuffer;
  return (opsize < dmRgnLineArray::MaxDfltArraySize ?
                   dmRgnLineArray::MaxDfltArraySize : opsize);
}
//
static inline bool __CHECK_SIZE( dm_size _size ) {
  return (_size < (dmRgnLineArray::MaxDfltArraySize-dmRgnLineArray::SecurityBuffer));
}

static inline dm_size __ALLOC_BUFFER( dm_size _size , dmRegionLineBuffer& _Buffer ) {
  return _Buffer.Allocate( dm_max(_size, (dm_size)dmRgnLineArray::MaxDfltArraySize) );
}
//--------------------------------------------------------------
// __CHECK_RGN_SIZE
// $COMMENT {
//    23/05/2001 :
//       CHANGED : Comparaison is now strict inequality
//--------------------------------------------------------------
static inline bool __CHECK_RGN_SIZE( dm_size _size , const dmRgnHandle* rgn ) {
  return (_size < rgn->Size());
}

//--------------------------------------------------------------
/// RegionLines Operations
//--------------------------------------------------------------

// Define Mathematical operations  between dmRegionLine Structures

template<class T> static inline void EXCHANGE(T& a, T& b) { T _x = a; a = b; b = _x; }

#define _DECLARE \
   dmRgnPair  *seg1,*seg2,*segd;  \
   long n1,n2,x1,x2;              \
   long n;                                 \
								           \
   seg1 = &l1->rl_xpair[0];                \
   n1   = l1->rl_npair;                    \
								           \
   seg2 = &l2->rl_xpair[0];                \
   n2   = l2->rl_npair;                    \
								           \
   segd = &dest->rl_xpair[0];              \
   n    = 0;

#define _END dest->rl_npair = n; return n;

#define STORE(xs,xe)  {\
				 segd->x1 = xs;  \
				 segd->x2 = xe;  \
				 ++segd;         \
				 ++n;            \
			   }

//--------------------------------------------------------------
// Compute the union of the x intervals between l1 and l2
//--------------------------------------------------------------
static inline long AddRegionLine( dmRegionLine* l1,dmRegionLine *l2,dmRegionLine *dest )
{
   _DECLARE

   if(n2>n1) {
     EXCHANGE(seg1,seg2);
     EXCHANGE(n1,n2);
   }

   while(n1) {
     if(n2) {
       if(seg1->x1 > seg2->x1) {
         EXCHANGE(seg1,seg2); // Choose the leftmost segment of reference
         EXCHANGE(n1,n2);     // keep track of the end of each array
       }
     }

     x1 = seg1->x1 ;
     x2 = seg1->x2 ;

     if(n2)
     {
       while( seg2->x1 <= (x2+1) ) // process all segments (2) which have a part in seg1
       {                           // or are joined
         if( seg2->x2 > x2) {      // end of seg2 is the upper bound
           x2 = seg2->x2;
           EXCHANGE(seg1,seg2);// exchange seg1 and seg2
           EXCHANGE(n1,n2);    // Keep track of the respective end of each array
         }
         if( --n2 == 0 ) break;
         ++seg2;               // go to next segment
       }
     }
     // All overlapping segments have been processed
     STORE(x1,x2);
     if( --n1 == 0 ) {
       EXCHANGE(seg1,seg2);
       EXCHANGE(n1,n2);
     } else ++seg1;   // go to next segment
   }
   _END
}
//--------------------------------------------------------------
// Compute the intersection of the x intervals between l1 and l2   => dest = A & B
//--------------------------------------------------------------
static inline long AndRegionLine( dmRegionLine* l1,dmRegionLine *l2,dmRegionLine *dest )
{
   _DECLARE

  while(n1 && n2) {
  	if(seg1->x1 > seg2->x1) {
      EXCHANGE(seg1,seg2); // Choose the leftmost (first) segment
      EXCHANGE(n1,n2);     // keep track of the end of each array
    }
    x1 = seg2->x1;         // begining of new segment set to the rightmost origine

    while( x1 <= seg1->x2 ) {       // Does segment 2 overlap  ?
      if( seg1->x2 <= seg2->x2) {   // end of (1) is the upper bound
        EXCHANGE(seg1,seg2);
        EXCHANGE(n1,n2);
      }
      x2 = seg2->x2;
      STORE(x1,x2);
      if( --n2 == 0 ) break;
      x1 = (++seg2)->x1;      // set beginning of new segment
    }
    ++seg1; n1--;   // go to next segment
  }

  _END
}
//--------------------------------------------------------------
// Compute the complementary of the intersection between l1 and l2 => dest = A^B
//--------------------------------------------------------------
static inline long XorRegionLine( dmRegionLine* l1,dmRegionLine *l2,dmRegionLine *dest )
{
   _DECLARE

   if(n2>n1) {
     EXCHANGE(seg1,seg2);
     EXCHANGE(n1,n2);
   }

   while(n1) {
     if(n2) {
       if(seg1->x1 > seg2->x1) {
         EXCHANGE(seg1,seg2); // Choose the leftmost segment of reference
         EXCHANGE(n1,n2);     // keep track of the end of each array
       }
     }
     x1 = seg1->x1;
     if(n2) {
       while( seg2->x1 <= seg1->x2 )  // process all segments 2 which have a part in seg1
       {                                // beginning of seg2 is the upper bound
         if( (x2 = seg2->x1 - 1) >= x1) STORE(x1,x2); // store new segment
         if( seg2->x2  > seg1->x2 ) {  // end of seg2 outside seg1
           EXCHANGE(seg1,seg2);
           EXCHANGE(n1,n2);
         }
         x1 = seg2->x2 + 1;       // set beginning of new segment at the end of seg2
         if( --n2 == 0 ) break;
         ++seg2;                  // go to next segment
       }
     }
     if( (x2 = seg1->x2) >= x1) STORE(x1,x2); // store new segment
     // All overlapping segments have been processed
     if( --n1 == 0 ) {
       EXCHANGE(seg1,seg2);
       EXCHANGE(n1,n2);
     } else ++seg1;   // go to next segment
   }

   _END
}
//--------------------------------------------------------------
// Remove common elements from l2 => dest = l1-l2
// Equivalent A - B = A Xor (A And B)
// Non-symmetric operation
// 1.Do a XOR operation between l1 and l2
// 2.Keep only parts from l1
//--------------------------------------------------------------
static inline long SubRegionLine( dmRegionLine* l1,dmRegionLine *l2,dmRegionLine *dest )
{
   bool  is1 = true;
   _DECLARE

   while(n1) {
     if(n2) {
       if(seg1->x1 > seg2->x1) {
         EXCHANGE(seg1,seg2); // Choose the leftmost segment of reference
         EXCHANGE(n1,n2);     // keep track of the end of each array
         is1 = !is1;
       }
     }
     x1 = seg1->x1;
     if(n2) {
       while( seg2->x1 <= seg1->x2 )    // process all segments 2 which have a part in seg1
       {                                // beginning of seg2 is the upper bound
         if( is1 && ((x2 = seg2->x1 - 1) >= x1) ) STORE(x1,x2);
         if( seg2->x2  > seg1->x2 ) {  // end of seg2 outside seg1
           EXCHANGE(seg1,seg2);
           EXCHANGE(n1,n2);
           is1 = !is1;
         }
         x1 = seg2->x2 + 1;       // set beginning of new segment at the end of seg2
         if( --n2 == 0 ) break;
         ++seg2;                  // go to next segment
       }
     }
     if( is1 && ((x2 = seg1->x2) >= x1)) STORE(x1,x2);
     // All overlapping segments have been processed
     --n1; ++seg1;   // go to next segment
     if( !is1 ){     // set (1) as our reference
       EXCHANGE(seg1,seg2);
       EXCHANGE(n1,n2);
       is1 = !is1;
     }
   }

   _END
}
//--------------------------------------------------------------
// Check the intersection of the x intervals between l1 and l2
//--------------------------------------------------------------
static inline bool IntersectRegionLine( dmRegionLine* l1,dmRegionLine *l2 )
{
  dmRgnPair  *seg1,*seg2;
  long n1,n2;
  
  seg1 = &l1->rl_xpair[0];
  n1   = l1->rl_npair;

  seg2 = &l2->rl_xpair[0];
  n2   = l2->rl_npair;

  while(n1 && n2) {
    if(seg1->x1 > seg2->x1) {
      EXCHANGE(seg1,seg2);  // Choose the leftmost (first) segment
      EXCHANGE(n1,n2);      // keep track of the end of each array
    }

    if( seg2->x1 <= seg1->x2 ) // Does segment 2 overlap  ?
       return true;
 
    ++seg1; n1--;   // go to next segment
  }

  return false;
}
//--------------------------------------------------------------
#undef _DECLARE
#undef _END
#undef STORE
//--------------------------------------------------------------
#define DECLARE_DoRgnLinesOperation( WhatOp ) \
dm_size DoRgnLinesOperation_ ## WhatOp         \
(rgnline_iterator l1,rgnline_iterator l2,rgnline_iterator dest, long height)  \
{                                                                             \
   dm_size s = 0;                                                              \
   for(long i=0;i<height;i++) {                                               \
     s   += dmRegionLineSize( WhatOp ## RegionLine(*l1,*l2,*dest) );          \
     ++l1; ++l2; ++dest;                                                      \
   }                                                                          \
   return s;                                                                  \
}

DECLARE_DoRgnLinesOperation( And )
DECLARE_DoRgnLinesOperation( Add )
DECLARE_DoRgnLinesOperation( Sub )
DECLARE_DoRgnLinesOperation( Xor )

bool DoRgnLinesOperation_Intersect(rgnline_iterator l1,rgnline_iterator l2,long height)
{
   for(long i=0;i<height;i++) {
     if(IntersectRegionLine(*l1,*l2))
        return true;
     ++l1; ++l2;
   }
   return false; 
}

#undef DECLARE_DoRgnLinesOperation


//--------------------------------------------------------------
// dmRgnLineArray allocation
//--------------------------------------------------------------
__dmKernel void  dmRgnLineArray::Dealloc()
{
  if(ptr) {
    dm_realloc_handler(ptr,0);
    ptr = NULL;
    buffsize = 0;
  }
}
//--------------------------------------------------------------
__dmKernel dm_size dmRgnLineArray::Realloc( dm_size size  )
{
  if( size > buffsize )
  {
    ptr      = static_cast<dmRgnLinePtr>(dm_realloc_handler(ptr,size));
    buffsize = size;
    if(ptr==NULL)
      dmTHROW_ERROR( "Memory Reallocation failure :" );
  }

  return size;
}
//--------------------------------------------------------------
// dmRgnHandle

//-----------------------------------------------------------
__dmKernel void dmRgnHandle::Destroy()
{
   rgn_data.Dealloc();
   dmRegionLineBuffer::Release(this);
}
//--------------------------------------------------------------
// Rectangle
//--------------------------------------------------------------
__dmKernel void dmRgnHandle::Construct(const dmRect& r)
{
  rgn_size  = 0;
  rgn_opbuf = NULL;

  if(!r.IsEmpty())
  {
    rgn_size = r.BoxHeight() * dmRegionLineSize(1);
    if(rgn_data.Realloc(rgn_size)) {
      rgn_box = r;
      long x1 = r.top_left.x;
      long x2 = r.bottom_right.x;
      iterator l = Begin(), lend = End();
      while(l!=lend) {
        (*l)->rl_npair = 1;
        (*l)->rl_xpair[0].x1 = x1;
        (*l)->rl_xpair[0].x2 = x2;
        ++l;
      }
  } else
    THROW_MEMORY_ERROR;
  }
}


// Line

#define __GetPair( y )  (*( _begin + (dm_uint)((y)*lsize) ))->rl_xpair
/////////////////////////////////////////////////////////

// Region Line Factory

struct __BrLineFactory__
{
   dmRgnPair*        pair;
   int               oldy;
   dm_size           lsize;

   rgnline_iterator _begin;

   __BrLineFactory__() : _begin(NULL) {}

   size_t SetCoordinates( const dmLine& line, dmRect& bbox,
                          dmRgnLineArray& rgn_data )
   {
      lsize    = dmRegionLineSize(1);

      dm_size rgn_size = line.BoxHeight() * lsize;
      if(rgn_data.Realloc(rgn_size))
      {
        long x1,x2,y1,y2;

        _begin = rgn_data();

        rgnline_iterator _l   = _begin;
        rgnline_iterator _end = _begin+rgn_size;

        // Initialize Data
        for(;_l!=_end; ++_l) {
           (*_l)->rl_npair = 1;
        }

        bbox = line.BoundingBox(); // get BoundingBox

        y1 = line.start.y - bbox.top_left.y;    // Make y coordinates relative to boundingbox
        y2 = line.end.y   - bbox.top_left.y;
        x1 = line.start.x;
        x2 = line.end.x;

        pair = __GetPair( (oldy=y2) );
        (*pair).x1 = (*pair).x2 = x2;

        daim::bresenham_line(*this, x1,y1,x2,y2 );

     } else
       THROW_MEMORY_ERROR;

     return rgn_size;

   }

   // For Bresenham operator
   void operator()( int x, int y )
   {
     if(y==oldy ) {
       if ( x > (*pair).x2 ) (*pair).x2 = x; else
       if ( x < (*pair).x1 ) (*pair).x1 = x;
     } else {
       pair = __GetPair(y);
       (*pair).x1 = (*pair).x2 = x;
       oldy = y;
     }
   }

};

// Region Circle Factory

struct __BrCircleFactory__
{
   dmRgnPair *pair1,*pair2;
   dm_size    lsize;
   long       cx,cy,r;

   rgnline_iterator _begin;

   __BrCircleFactory__() : _begin(NULL) {}

   size_t SetCoordinates( const dmCircle& circle, dmRect& bbox,
                          dmRgnLineArray& rgn_data )

   {
     lsize = dmRegionLineSize(1);

     dm_size rgn_size = circle.BoxHeight() * lsize;
     if(rgn_data.Realloc(rgn_size))
     {
       _begin = rgn_data();

       rgnline_iterator _l   = _begin;
       rgnline_iterator _end = _begin+rgn_size;

        // Initialize Data
        for(;_l!=_end; ++_l) {
           (*_l)->rl_npair = 1;
        }

        bbox = circle.BoundingBox(); // get BoundingBox

        cy = circle.center.y - bbox.top_left.y;
        cx = circle.center.x;
        r  = circle.r;

        if(r==0) {
          pair1 = (*_begin)->rl_xpair;
          (*pair1).x1 = (*pair1).x2 = cx;
        } else
          daim::bresenham_circle(*this,r);

    } else
      THROW_MEMORY_ERROR;

    return rgn_size;
  }

   // For Bresenham operator
   void operator()( int x, int y )
   {
  	 pair1 = __GetPair(cy+x);
	 pair2 = __GetPair(cy-x);
	 (*pair1).x1 = (*pair2).x1 = cx-y;
	 (*pair1).x2 = (*pair2).x2 = cx+y;
	 pair1 = __GetPair(cy+y);
	 pair2 = __GetPair(cy-y);
	 (*pair1).x1 = (*pair2).x1 = cx-x;
	 (*pair1).x2 = (*pair2).x2 = cx+x;
   }
};

//--------------------------------------------------------------
__dmKernel void dmRgnHandle::Construct(const dmLine& line)
{
  __BrLineFactory__ _Factory;

  rgn_opbuf = NULL;
  rgn_size  = _Factory.SetCoordinates(line,rgn_box,rgn_data);
}
//--------------------------------------------------------------
// Circle
//--------------------------------------------------------------
__dmKernel void dmRgnHandle::Construct(const dmCircle& c)
{
  __BrCircleFactory__ _Factory;

  rgn_opbuf = NULL;
  rgn_size  =_Factory.SetCoordinates(c,rgn_box,rgn_data);

}
//--------------------------------------------------------------
// Ellipse
//--------------------------------------------------------------
#define _GetLinePair(y)  (*( Begin() + (dm_uint)((y)*lsize) ))->rl_xpair
#define _InitRegion {\
    iterator _l = Begin(), _lend = End();           \
    while(_l!=_lend) { (*_l)->rl_npair = 1; ++_l; } \
  }


__dmKernel void dmRgnHandle::Construct(const dmEllipse& e)
{
   rgn_opbuf = NULL;

   dm_size lsize = dmRegionLineSize(1);
   rgn_size = e.BoxHeight() * lsize;
   if(rgn_data.Realloc(rgn_size))
   {
     dmRgnPair *pair1,*pair2;
     long cx,cy,rx,ry;

     rgn_box =  e.BoundingBox();

     cy = e.center.y - rgn_box.top_left.y;
     cx = e.center.x;
     rx = e.rx;
     ry = e.ry;

     // bug dans bresenham => on initialise � la largeur de la bounding box
     iterator _l = Begin(), _lend = End();
     while(_l!=_lend) {
       (*_l)->rl_npair = 1;
       pair1 = (*_l)->rl_xpair;
       pair1->x1 = cx - rx;
       pair1->x2 = cx + rx;
       ++_l;
     }

#define _SetPixel_qads(x,y) {           \
	pair1 = _GetLinePair(cy+y);         \
	pair2 = _GetLinePair(cy-y);         \
	(*pair1).x1 = (*pair2).x1 = cx-x;   \
	(*pair1).x2 = (*pair2).x2 = cx+x;   \
}
     _DigitalEllipse(rx,ry);

#undef _SetPixel_qads
   } else rgn_size = 0;
}

#undef _GetLinePair
#undef _InitRegion

//--------------------------------------------------------------
// Utility function :
// Obtain a Region line containing the intersection points with a polygone
// Defining the InLine shape of the CLOSED polygone (points contained in poly)
//--------------------------------------------------------------

struct XBUF
{
  long *xx;
  long *pos;
  XBUF( dm_size sz ) {
    xx  = new (dm_arena) long[sz];
    pos = new (dm_arena) long[sz];
  }

  ~XBUF() {
     ::operator delete [] (xx,dm_arena);
     ::operator delete [] (pos,dm_arena);
   }

};

static inline long _IntersectionLine(
           XBUF&          xbuf,
           const dmPoly&  p,
           dmRegionLine*  line,
           long y )
{
  dmRgnPair *pair;
  long i,ninters,np = 0;

  dm_int32 x1,x2;

  long *xx  = xbuf.xx;
  long *pos = xbuf.pos;

  pair = line->rl_xpair;

  ninters = p.IntersectHor(y,xx,pos) - 1;
  for(i=0;i<ninters;i+=2) {
    x1 = xx[i];   if( pos[i]   > 0 ) x1++;
    x2 = xx[i+1]; if( pos[i+1] < 0 ) x2--;
    if( (x2-x1) >= 0) {
      (*pair).x1 = x1;
      (*pair).x2 = x2;
      ++pair;++np;
    }
  }
  return (line->rl_npair = np);
}
//--------------------------------------------------------------
// Polygone
// $COMMENT {
//    23/05/2001 : dmRgnHandle::dmRgnHandle(const dmPoly& poly)
//       BUG FIXED : dmRegionLineSize(0) -> changed to dmRegionLineSize(1)
//                   so default is to take into account a least one pair
//                   the routine crashed for big bounding boxes
//       BUG FIXED : dmRgnHandle::rgn_size was not correctly handled
//       CHANGED   : reallocate space Instead of throwing exception;
//    14/06/2001
//       BUG FIXED : Plantage lors de la construction d'une region � partir
//                   d'un polygone vide : il n'y avait pas de test !!!!
// }
//--------------------------------------------------------------
__dmKernel void dmRgnHandle::Construct(const dmPoly& poly)
{
  rgn_size  = 0;
  rgn_opbuf = NULL;

  dmRegionLineBuffer buffer(this);
  dm_int32            i,height,n;

  dm_size  calc_size = 0;

  if(poly.Size()==0) return;

  XBUF xbuf(poly.Size());

  height   = (rgn_box = poly.BoundingBox()).BoxHeight();
  rgn_size = __MAX_SIZE(height * dmRegionLineSize(1));


  if(__ALLOC_BUFFER(rgn_size,buffer) && rgn_data.Realloc(rgn_size) )
  {
    iterator lin = Begin();       // initialise region
    for(i=0;i<height;i++) {
      (*lin)->rl_npair = 0;
      ++lin;
    }

    // compute the region corresponding to inner part of the polygon shape
    if(poly.Closed() && height>1)
    {
      calc_size = 0;
	  for(lin = Begin(),i=0;i<height;i++)
      {
        calc_size += dmRegionLineSize(
           _IntersectionLine(xbuf,poly,*lin,i+rgn_box.top_left.y)
        );

        if(!__CHECK_RGN_SIZE(calc_size,this)) {
           rgn_size = rgn_data.Inflate(calc_size);
        }
        ++lin;
      }
    }
    // Now we need to make the outline of the polygone
    if(!__CHECK_SIZE(rgn_size)) buffer.Inflate(rgn_size);

    // copy data into buffer
    dm_memcpy_handler( buffer(), rgn_data(), rgn_size);

    dmRgnHandle lineRgn;
    dm_size s1,s2,h;

    __BrLineFactory__ LineFactory;

    for(n=poly.Size()-1,i=0;i<n;++i)
    {
      lineRgn.rgn_size = LineFactory.SetCoordinates(dmLine(poly[i],poly[i+1]),lineRgn.rgn_box,lineRgn.rgn_data);
      dmASSERT( lineRgn.Size() != 0 );
      h = lineRgn.rgn_box.BoxHeight();
      iterator l = buffer();

      lin  = dmGetRegionLine( Begin(), (lineRgn.rgn_box.top_left.y - rgn_box.top_left.y) );
      l    = l + (s1 = DISTANCE( *lin, *Begin()) ); // start for buffer

      // ADD operation between the line and the region previously computed
      // -> store into buffer
      s1  += ( s2 = DoRgnLinesOperation_Add(lineRgn.Begin(),lin,l,h) );
      // Add remaining data at end of buffer
      if(lineRgn.rgn_box.bottom_right.y < rgn_box.bottom_right.y)
      {
        lin = dmGetRegionLine(lin,h); // get start line of remaining data
        l = l + s2;                   // get end of buffer
        (void)dm_memcpy_handler(*l,*lin,(s2 = DISTANCE( *End(),*lin )) );
        s1+=s2;
      }
      rgn_size = s1;
      if(!__CHECK_SIZE(rgn_size)) rgn_data.Inflate(rgn_size);
      (void)dm_memcpy_handler( rgn_data(), buffer(), rgn_size);  // copy buffer back to rgn_data
      lineRgn.FreeRegion();
    }

  } else
    THROW_MEMORY_ERROR;
}

//--------------------------------------------------------------
/// Others region constructors
//--------------------------------------------------------------
__dmKernel void dmRgnHandle::Construct(const dmRgnHandle& rgn)
{
  rgn_box    = dmRect(0,0,0,0);
  rgn_size  = 0;
  rgn_opbuf = NULL;

  if( rgn_data.Realloc(rgn.rgn_size) )
  {
    rgn_size = rgn.rgn_size;
    rgn_box  = rgn.rgn_box;
    (void)dm_memcpy_handler(rgn_data(),rgn.rgn_data(),rgn_size);

    dmRegionLineBuffer::Obtain(this,rgn.rgn_opbuf);
  }
}
//--------------------------------------------------------------
__dmKernel void dmRgnHandle::FreeRegion()
{
  rgn_box.top_left = rgn_box.bottom_right = dm_POINT(0,0);
  rgn_size = 0;
}
//--------------------------------------------------------------
// Operateur de copie
//--------------------------------------------------------------
__dmKernel dmRgnHandle& dmRgnHandle::operator=(const dmRgnHandle& rgn)
{
  if(&rgn!=this) {
    if( rgn_data.Realloc( rgn.rgn_size) )
    {
      (void)dm_memcpy_handler(rgn_data(),rgn.rgn_data(),(rgn_size=rgn.rgn_size));
      rgn_box = rgn.rgn_box;

      dmRegionLineBuffer::Obtain(this,rgn.rgn_opbuf);

    } else
      rgn_size = 0;
  }
  return *this;
}
//--------------------------------------------------------------
// Translate the region by dx & dy
//--------------------------------------------------------------
__dmKernel void dmRgnHandle::Translate( long dx, long dy )
{
  if(!Empty())
  {
    rgn_box.Translate(dx,dy);
    if (dx)
    {
      long i;
      iterator it = Begin(), last = End();
      while(it!=last) {
        dmRgnPair *pair = (*it)->rl_xpair;
        for(i=(*it)->rl_npair;--i>=0;pair++) {
          (*pair).x1 += dx;
          (*pair).x2 += dx;
        }
        ++it;
	  }
    }
  }
}
//--------------------------------------------------------------
__dmKernel dmRgnHandle::iterator dmRgnHandle::Reserve(const dmRect& rect,dm_size size)
{
   dm_size minsize  = sizeof(dmRegionLine)*rect.BoxHeight();
   rgn_size = (size < minsize ? minsize : size);
   if(rgn_data.Realloc(rgn_size)) dm_memset_handler( rgn_data(),0,rgn_size );
   rgn_box  = rect;
   return Begin();
}
//--------------------------------------------------------------
// Adjust the BBox by removing empty lines at the beginning and
// at the end of the region definition (RegionArray) and compute
// the new resulting BBox.
// The main purpose is to adjust the BBox after region operations,
//--------------------------------------------------------------
// Define a helper function
static dm_size __AdjustBoundingBox( dmRgnLineArray& rgn_data,
                                   dmRect& rgn_box,
                                   dmRgnLineArray& _buffer )
{
  long  i,x;
  dm_size rgn_size = 0;

  dmRgnHandle::iterator lstart  = _buffer();
  long h  = rgn_box.BoxHeight();

  i = 0;
  while( (i<h) && ((*lstart)->rl_npair==0) ) { ++lstart; i++; }// skip 0 length RegionLines
  if( i<h ) {  // region is not empty
    long n,xmin,xmax,ymin,ymax,y0;
    dmRgnHandle::iterator l    = lstart;
    dmRgnHandle::iterator lend = lstart;

    ymin = ymax = (y0 = rgn_box.top_left.y) + i;
    xmin = (*l)->rl_xpair[0].x1;
    xmax = (*l)->rl_xpair[(*l)->rl_npair-1].x2;
    for(++i; i<h ; i++) {
      l++;
      if( (n=(*l)->rl_npair) ) {
        lend = l; ymax = y0+i;
        if( (x = (*l)->rl_xpair[0].x1)   < xmin) xmin = x;
        if( (x = (*l)->rl_xpair[n-1].x2) > xmax) xmax = x;
      }
    }

    rgn_box.top_left.x  = xmin; rgn_box.bottom_right.x = xmax;
    rgn_box.top_left.y  = ymin; rgn_box.bottom_right.y = ymax;

    if( (&_buffer == &rgn_data) && (ymax-ymin+1)<h ) {
      rgn_size = DISTANCE( *++lend, *lstart);
      dm_memmove_handler( rgn_data(), *lstart, rgn_size);
    } else {
      rgn_size = DISTANCE( *++lend, *lstart);
      dm_memcpy_handler(rgn_data(),*lstart,rgn_size);
    }
  }
  return rgn_size;
}
//--------------------------------------------------------------
__dmKernel dmRgnHandle* dmRgnHandle::AdjustBoundingBox()
{
  if(!Empty()) {
    rgn_size = __AdjustBoundingBox(rgn_data,rgn_box,rgn_data);
    if(!rgn_size) FreeRegion();
  }
  return this;
}
//--------------------------------------------------------------
// Clip a region to a given Rectangle structure (Similar to a AND operation)
//--------------------------------------------------------------
typedef struct {
  long       rl_npair;
  dmRgnPair  rl_xpair[1];
} SingleRegionLine;
//--------------------------------------------------------------
__dmKernel dmRgnHandle* dmRgnHandle::ClipToRect( const dm_rect& r )
{
  long h,x1,y1,x2,y2;
  dm_size s=0,bufsize=0;
  dmRgnHandle *v = NULL;
  dmRegionLineBuffer buffer(this);
  SingleRegionLine sl;

  sl.rl_npair = 1;

  dmGetBoxIntersection(x1,y1,x2,y2,rgn_box,r);
  h = y2-y1 + 1;
  if( (h<=0) || (x1>x2) ) { FreeRegion(); return (this); } // NoIntersection

  // Check if rgn_box is contained in r
  else if((rgn_box.top_left.x==x1)  && (rgn_box.top_left.y==y1) &&
          (rgn_box.bottom_right.x==x2) && (rgn_box.bottom_right.y==y2)) return (this);

  else if( (bufsize=__ALLOC_BUFFER(rgn_size,buffer)) )
  {
    rgnline_iterator l2 = buffer();
    rgnline_iterator l1 = dmGetRegionLine( Begin(), y1 - rgn_box.top_left.y );
    //sl()->rl_npair = 1;
    //sl()->rl_xpair[0].x1 = x1;
    //sl()->rl_xpair[0].x2 = x2;
    sl.rl_xpair[0].x1 = x1;
    sl.rl_xpair[0].x2 = x2;
    for(long i=0;i<h;i++)
    {
      s += dmRegionLineSize( AndRegionLine(*l1,(dmRegionLine*)&sl,*l2) );
      if(s>bufsize) THROW_MEMORY_ERROR;
      ++l1; ++l2;
    }
    if(rgn_data.Realloc(s))
    {
      rgn_box  = dm_RECT(x1,y1,x2,y2);
      rgn_size = s;
      (void)dm_memcpy_handler( rgn_data(), buffer(), s);
      v = AdjustBoundingBox();
    }
  }
  return (v);
}
//--------------------------------------------------------------
// test si la region est rectangulaire
//--------------------------------------------------------------
__dmKernel bool dmRgnHandle::IsRectRgn() const
{
  if(Empty()) return false;
  iterator l = Begin(), last = End();
  dmRgnPair *pair = (*l)->rl_xpair;
  long x1 = (*pair).x1, x2 = (*pair).x2;
  do {
    if( (*l)->rl_npair == 1) {
      pair = (*l)->rl_xpair;
      if( (*pair).x1 == x1 && (*pair).x2 == x2 ) continue;
    }
    return false;
  } while(++l!=last);
  return true;
}
//--------------------------------------------------------------
// Return region area in pixel
//--------------------------------------------------------------
__dmKernel dm_uint dmRgnHandle::Area() const
{
  long i,count = 0;
  if(!Empty())
  {
	iterator it = Begin(), last = End();
	while(it!=last) {
      dmRgnPair *pair = (*it)->rl_xpair;
      for(i=(*it)->rl_npair;--i>=0;++pair) {
        count += (*pair).x2 - (*pair).x1 + 1;
      }
      ++it;
    }
  }
  return count;
}
//--------------------------------------------------------------
// Test if a rectangle intersect the region
//--------------------------------------------------------------
__dmKernel bool dmRgnHandle::Intersect( const dm_rect& r ) const
{
  int xs,xe,ys,ye;
  dmGetBoxIntersection(xs,ys,xe,ye,r,rgn_box);
  if(xs<=xe && ys <= ye ) {
    iterator it = dmGetRegionLine(Begin(),(ys-rgn_box.top_left.y)), last = End();
  	for(long i,y = ys; it!=last && y <= ye; ++it, ++y) {
      dmRgnPair *pair = (*it)->rl_xpair;
      for(i=(*it)->rl_npair;--i>=0;++pair) {
        if( xs <= (*pair).x2 && xe >= (*pair).x1) return true;
      }
    }
  }
  return false;
}
//--------------------------------------------------------------
// Test if a region intersect the region
//--------------------------------------------------------------
__dmKernel bool dmRgnHandle::Intersect( const dmRgnHandle& _rgn ) const
{
  long h,ys,ye,xs,xe;

  const dmRgnHandle& d1   = *this;
  const dmRgnHandle& d2   = _rgn;

  if(d1.Empty()||d2.Empty())
     return false;  

  dmGetBoxIntersection(xs,ys,xe,ye,d1.rgn_box,d2.rgn_box);
  h = ye-ys + 1;  // Handle regions with no common area
  if( (h<=0) || (xs>xe) )
     return false;
 
  rgnline_iterator l1 = dmGetRegionLine( d1.Begin(), ys - d1.rgn_box.top_left.y ); // start line for d1
  rgnline_iterator l2 = dmGetRegionLine( d2.Begin(), ys - d2.rgn_box.top_left.y ); // start line for d2

  return DoRgnLinesOperation_Intersect(l1,l2,h);
}
//--------------------------------------------------------------
__dmKernel bool dmRgnHandle::operator==(const dmRgnHandle& _rgn) const
{
  bool res = false;
  if(!Empty() && !_rgn.Empty()) {
    if(Box()==_rgn.Box()) {
      iterator it1 = Begin(), last = End();
      iterator it2 = _rgn.Begin();
      for(res=true;res && it1!=last;++it1,++it2) {
        if( (res = ((*it1)->rl_npair == (*it2)->rl_npair)) )
        {
          dmRgnPair *pair1 = (*it1)->rl_xpair;
          dmRgnPair *pair2 = (*it2)->rl_xpair;
          for(long i=(*it1)->rl_npair;--i>=0;++pair1,++pair2) {
            if(!PairEqual( *pair1,*pair2) ) return false;
          }
        }
      }
    }
  }
  return res;
}
//--------------------------------------------------------------
__dmKernel bool dmRgnHandle::PointInRegion( long x, long y ) const
{
  if( dmPointInRectangle(x,y,rgn_box) ) {
    iterator l = dmGetRegionLine(Begin(),(y-rgn_box.top_left.y));
    dmRgnPair *pair = (*l)->rl_xpair;
    for(long i=0;i<(*l)->rl_npair;++i,++pair) {
      if( (x >= (*pair).x1) && (x <= (*pair).x2) )
        return (true);
    }
  }
  return (false);
}
//--------------------------------------------------------------
// Get the Inner Boundary as a region (8 connectivity)
//--------------------------------------------------------------
__dmKernel dmRgnHandle* dmRgnHandle::InnerBoundaryRgn( dmRgnHandle* destRgn,
                                                       const dmRgnHandle* srcRgn )
{
   dmRgnLineArray buffer1,buffer2;
   dm_size i,size,np;
   long x1,x2,n;
   dmRgnPair *pair1,*pair2;

 //dmTRY

   if( destRgn==NULL || srcRgn==NULL) return NULL;
   if(!destRgn->Empty())
     destRgn->FreeRegion() ;

   const dmRgnHandle& rgn  = *srcRgn;

   if(!rgn.Empty())
   {
	 size = 2*rgn.Size();
     if( buffer1.Realloc(size) && buffer2.Realloc(size) ) {
	   dm_uint h = rgn.rgn_box.BoxHeight();
	   rgnline_iterator  l1 = buffer1();
	   rgnline_iterator  l2 = buffer2();
		 rgnline_iterator  ls = rgn.Begin();

	   (*l1)->rl_npair = 0; // first zero length dmRegionLine
	   (void)dm_memcpy_handler( (*l1) + 1, *ls, rgn.rgn_size);
	   size = DoRgnLinesOperation_And( ls,   l1, l2, h  ); // AND each line with the previous one
	   size = DoRgnLinesOperation_And( ++ls, l2, l1, h-1); // AND each line with the next one
	   (*(l1+size))->rl_npair = 0; // last zero length dmRegionLine

       // Shrink region by 1 in the x direction
       size = 0;
	   for(i=0;i<h;i++) {    // store result in buffer2
		   pair2 = (*l2)->rl_xpair;
		   pair1 = (*l1)->rl_xpair;
		   np    = 0;
		   for(n=0;n<(*l1)->rl_npair;n++) {
			   x1 = (*pair1).x1 + 1;
			   x2 = (*pair1).x2 - 1;
		     if( (x2-x1) >= 0) {
			     (*pair2).x1 = x1;
			     (*pair2).x2 = x2;
			     ++pair2; ++np;
				 }
			   ++pair1;
			 }
		   (*l2)->rl_npair = np;
			 size += dmSizeofRgnLine(**l2);
		   ++l1; ++l2;
	   }

	   // Do a SUB operation between rgn_data and buffer2
	   size = DoRgnLinesOperation_Sub(rgn.Begin(),(l2=buffer2()),(l1=buffer1()),h);

		 if(size) {
		   if( (*destRgn).rgn_data.Realloc(size) ) {
		     (void)dm_memcpy_handler( (*destRgn).rgn_data(),buffer1(),size);
		  	 (*destRgn).rgn_size = size;
		  	 (*destRgn).rgn_box  = rgn.rgn_box;
			 } else destRgn = NULL;
	   }
	 } else destRgn = NULL;
	 buffer1.Dealloc();
	 buffer2.Dealloc();
  }

  //dmRETHROW_ERROR
  return (destRgn);
}
//--------------------------------------------------------------
// Get the Outer Boundary as a region (8 connectivity)
//--------------------------------------------------------------
// BUG BUG BUG !!!!!!!!!!!!!!!!!!
__dmKernel dmRgnHandle* dmRgnHandle::OuterBoundaryRgn( dmRgnHandle* destRgn,
                                                       const dmRgnHandle* srcRgn )
{
   dmRgnLineArray buffer1,buffer2;
   dm_size size,np;
   long i,x1,x2,ox1,ox2,n;
   dmRgnPair *pair1,*pair2;

   if( destRgn==NULL || srcRgn==NULL) return NULL;
   if(!destRgn->Empty())
     destRgn->FreeRegion() ;

   const dmRgnHandle& rgn  = *srcRgn;

   if(!rgn.Empty())
   {
	 if( buffer1.Realloc() && buffer2.Realloc() ) {
	   dm_uint h = rgn.rgn_box.BoxHeight();
	   rgnline_iterator  l1 = buffer1();
	   rgnline_iterator  l2 = buffer2();

	   (*l1)->rl_npair = 0; // first zero length dmRegionLine
	   (void)dm_memcpy_handler( (*l1) + 1, *rgn.Begin(), rgn.rgn_size);    // Copy rgn in buffer;
	   size = DoRgnLinesOperation_Add(l1,rgn.Begin(),l2,h);     // ADD each line with the previous one
	   (*(l2+size))->rl_npair = 0;                              // Add empty line to l2
	   size = DoRgnLinesOperation_Add(++l2,rgn.Begin(),++l1,h); // ADD each line with the next one

	   l1 = rgn.Begin(); // start of data
	   (*(l2 = buffer2()))->rl_npair = 0; ++l2;
 	   // Expand source region by 1 in the x direction
	   for(i=h;--i>=0;) {    // store result in buffer2
		 np = 0;
		 if( (n=(*l1)->rl_npair) ) {
           pair2 = (*l2)->rl_xpair;
		   pair1 = (*l1)->rl_xpair;
		   ox1 = (*pair1).x1 - 1;
		   ox2 = (*pair1).x2 + 1;
		   for(++pair1;--n>0;) {
		     x1 = (*pair1).x1 - 1;
			 x2 = (*pair1).x2 + 1;
			 if( x1 - ox2 > 1 ) {
			    (*pair2).x1 = ox1; ox1 = x1;
			    (*pair2).x2 = ox2; ox2 = x2;
			    ++pair2; ++np;
			 } else ox2 = x2;
			 ++pair1;
		   }
		   ++np;
		   (*pair2).x1 = ox1;
		   (*pair2).x2 = ox2;
		 }
		 (*l2)->rl_npair = np;
		 ++l1; ++l2;
	   }
	   (*l2)->rl_npair = 0;           // set empty line at end of buffer
       (*destRgn).rgn_data.Realloc(); // allocate space for storage
	   size = DoRgnLinesOperation_Add((l2=buffer2()),(l1=buffer1()),*destRgn->Begin(),h+2);
	   size = DoRgnLinesOperation_Sub(*destRgn->Begin(),*rgn.Begin()  ,(l1=buffer1()),h+2);
	   if(size) {
		 if( (*destRgn).rgn_data.Realloc(size) ) {
			(void)dm_memcpy_handler( (*destRgn).rgn_data(),buffer1(),size);
			(*destRgn).rgn_size = size;
			(*destRgn).rgn_box.top_left.y  = rgn.rgn_box.top_left.y-1; // Set new bounding Box
			(*destRgn).rgn_box.top_left.x  = rgn.rgn_box.top_left.x-1;
			(*destRgn).rgn_box.bottom_right.y = rgn.rgn_box.bottom_right.y+1;
			(*destRgn).rgn_box.bottom_right.x = rgn.rgn_box.bottom_right.x+1;
		 } else destRgn = NULL;
	   }
	 } else destRgn = NULL;
	 buffer1.Dealloc();
	 buffer2.Dealloc();
  }

  return (destRgn);
}
//--------------------------------------------------------------
// Invert region vertically / horizontally
//--------------------------------------------------------------
__dmKernel dmRgnHandle* dmRgnHandle::FlipVerRgn( dmRgnHandle* destRgn,
                                                 const dmRgnHandle* srcRgn)
{
  if( srcRgn->Empty() )
    destRgn->FreeRegion();
  else {
    long i,h = srcRgn->Box().Height();
    dmRgnLinePtr* lines = new (dm_arena) dmRgnLinePtr[h];
    dmRgnHandle::iterator lfrom = srcRgn->Begin();
    dmRgnHandle::iterator lto   = destRgn->Reserve(srcRgn->Box(),srcRgn->Size());
    for(i=0;i<h;++i,++lfrom ) {
      lines[i] = *lfrom;
    }

    dmRgnLinePtr* lptr = &lines[h-1];

    for(i=h;--i>=0;++lto,--lptr) {
     (void)dm_memcpy_handler(*lto,*lptr,dmSizeofRgnLine(**lptr)); // Copy the buffer
    }
    ::operator delete [] (lines,dm_arena);
  }

  return destRgn;
}
//--------------------------------------------------------------
__dmKernel dmRgnHandle* dmRgnHandle::FlipHorRgn( dmRgnHandle* destRgn,
                                                 const dmRgnHandle* srcRgn)
{
  if( srcRgn->Empty() )
    destRgn->FreeRegion();
  else {
    long xa,xb,i,np;
    xa = srcRgn->Box().Left();
    xb = srcRgn->Box().Right();

    dmRgnHandle::iterator lfrom = srcRgn->Begin();
    dmRgnHandle::iterator last  = srcRgn->End();
    dmRgnHandle::iterator lto   = destRgn->Reserve(srcRgn->Box(),srcRgn->Size());

    dmRgnPair *pair1;
    dmRgnPair *pair2;
	  for(;lfrom!=last;++lto,++lfrom) {
      np = (*lto)->rl_npair = (*lfrom)->rl_npair;
      pair1  = (*lfrom)->rl_xpair;
      pair2  = (*lto)->rl_xpair + np - 1;
      for(i=np;--i>=0;++pair1,--pair2) {
        pair2->x1 = xa - pair1->x2 + xb;
        pair2->x2 = xa - pair1->x1 + xb;
      }
    }
  }

  return destRgn;
}
//--------------------------------------------------------------
// Handle mathematical operations between regions
//--------------------------------------------------------------
// AND Region together
//--------------------------------------------------------------
__dmKernel dmRgnHandle* dmRgnHandle::AndRegionData( const dmRgnHandle* p1,
                                                    const dmRgnHandle* p2,
                                                    dmRgnHandle* pdest)
{
  long h,ys,ye,xs,xe;
  dm_size size;
  //dmRgnLineArray buffer;
  dmRegionLineBuffer buffer(p1,p2,pdest);

  const dmRgnHandle& d1   = *p1;
  const dmRgnHandle& d2   = *p2;
  dmRgnHandle& dest = *pdest;

  if(d1.Empty()||d2.Empty())
     dest.FreeRegion();
  else {
    dmGetBoxIntersection(xs,ys,xe,ye,d1.rgn_box,d2.rgn_box);
    h = ye-ys + 1;  // Handle regions with no common area
    if( (h<=0) || (xs>xe) ) dest.FreeRegion();
    else
    {
      if( buffer.Allocate(__BEST_SIZE(p1,p2)) )      // new buffer for operation
      {
        rgnline_iterator l  = buffer();
        rgnline_iterator l1 = dmGetRegionLine( d1.Begin(), ys - d1.rgn_box.top_left.y ); // start line for d1
        rgnline_iterator l2 = dmGetRegionLine( d2.Begin(), ys - d2.rgn_box.top_left.y ); // start line for d2
        size = DoRgnLinesOperation_And(l1,l2,l,h);

        if(size > (h*dmRegionLineSize(0)) )  // take care that region is not empty
        {
          if( dest.rgn_data.Realloc(size) )
          {
            dest.rgn_size = size;
            dest.rgn_box.top_left.y  = ys;   // Set new bouding Box
            dest.rgn_box.top_left.x  = xs;
            dest.rgn_box.bottom_right.y = ye;
            dest.rgn_box.bottom_right.x = xe;

            dest.rgn_size = __AdjustBoundingBox(dest.rgn_data,dest.rgn_box,buffer);
            pdest = &dest;
          }
          else
          {
            pdest = NULL;
            dest.FreeRegion();
          }
        } else  dest.FreeRegion();
      } else pdest = NULL;
    }
  }

  return (pdest);
}
//--------------------------------------------------------------
// SUB Region (d1 - d2)
//--------------------------------------------------------------
__dmKernel dmRgnHandle* dmRgnHandle::SubRegionData( const dmRgnHandle* p1,
                                                    const dmRgnHandle* p2,
                                                    dmRgnHandle* pdest)
{
  long h,ys,ye,xs,xe;
  dm_size size,s1,s2;
  dmRect box;
  dmRegionLineBuffer buffer(p1,p2,pdest);

  const dmRgnHandle& d1   = *p1;
  const dmRgnHandle& d2   = *p2;
  dmRgnHandle& dest = *pdest;

  if (d1.Empty()||d2.Empty()) {
    dest = d1;
    return (&dest);
  }

  dmGetBoxIntersection(xs,ys,xe,ye,d1.rgn_box,d2.rgn_box);
  h = ye-ys + 1;  // Handle regions with no common area
  if( (h<=0) || (xs>xe) ) { dest = d1; }
  else
  {
    if( buffer.Allocate(__BEST_SIZE(p1,p2)) )  // new buffer for operation
    {
       (void)dm_memcpy_handler(buffer(),d1.rgn_data(),d1.Size()); // copy d1 into buffer
       rgnline_iterator l1 = dmGetRegionLine( d1.Begin(), ys - d1.rgn_box.top_left.y ); // start line for d1
       rgnline_iterator l2 = dmGetRegionLine( d2.Begin(), ys - d2.rgn_box.top_left.y ); // start line for d2
       rgnline_iterator l = buffer();
       l  = l + (s1 = DISTANCE( *l1, *d1.Begin()) );    // start line for buffer
       size = DoRgnLinesOperation_Sub(l1,l2,l,h);
       l  = l + size; // end of buffer

       if(d1.rgn_box.bottom_right.y > d2.rgn_box.bottom_right.y)  // still remaining data to be copied
       {
         l1 = dmGetRegionLine(l1,h);                    // get start line of remaining data
         s2 = d1.Size() - DISTANCE( *l1, *d1.Begin());  // size of remaining data
         (void)dm_memcpy_handler(*l,*l1,s2);                       // copy remaining lines
       } else
         s2 = 0;

       size += (s1+s2);
       box = d1.rgn_box;

       if( dest.rgn_data.Realloc(size) )
       {
         //dest.rgn_size = size;
         dest.rgn_box  = box;
         dest.rgn_size = __AdjustBoundingBox(dest.rgn_data,dest.rgn_box,buffer);
         pdest = &dest;
       }
       else
       {
         dest.FreeRegion();
         pdest = NULL;
       }
    } else
      pdest = NULL;
  }

  return (pdest);
}
//--------------------------------------------------------------
// ADD region
//--------------------------------------------------------------
__dmKernel dmRgnHandle* dmRgnHandle::AddRegionData( const dmRgnHandle* p1,
                                                    const dmRgnHandle* p2,
                                                    dmRgnHandle* pdest)
{
  long h,ys,ye,xs,xe,i;
  dm_size size,s1,s2;
  dmRect box;
  dmRegionLineBuffer buffer(p1,p2,pdest);

  const dmRgnHandle& d1   = *p1;
  const dmRgnHandle& d2   = *p2;
  dmRgnHandle& dest = *pdest;

  // Handle empty regions
  if(d1.Empty()) { dest=d2; return (pdest); } else
  if(d2.Empty()) { dest=d1; return (pdest); }

  dmGetBoxIntersection(xs,ys,xe,ye,d1.rgn_box,d2.rgn_box);
  h = ye-ys + 1;
  if( buffer.Allocate(__BEST_SIZE(p1,p2)) )  // new buffer for operation
  {
    const dmRgnHandle *r1 = &d1, *r2 = &d2;
    // Get the upper region in r1
    if( r1->rgn_box.top_left.y> r2->rgn_box.top_left.y) EXCHANGE(r1,r2);
    (void)dm_memcpy_handler(buffer(),r1->rgn_data(),r1->Size());  // copy region r1 into buffer
    rgnline_iterator l2 = buffer();
    if(h>0)   // Handle intersection
    {
       rgnline_iterator l1 = dmGetRegionLine( r1->Begin(), ys - r1->rgn_box.top_left.y );  // start line for r1
       l2 = l2 + (s1 = DISTANCE( *l1, *r1->Begin() ) );  // start line for buffer
       size = DoRgnLinesOperation_Add(l1,r2->Begin(),l2,h);
       l2 = l2 + size;  // End of buffer
       if( r1->rgn_box.bottom_right.y <= r2->rgn_box.bottom_right.y) { // copy remaining data from r2
          l1 = dmGetRegionLine( r2->Begin(), h );            // get start line of remaining data
          s2 = r2->Size() - DISTANCE( *l1, *r2->Begin() );   // size of remaining data
       } else {                                              // copy remaining data from r1
          l1 = dmGetRegionLine(l1,h);                        // get start line of remaining data
          s2 = r1->Size() - DISTANCE( *l1, *r1->Begin() );   // size of remaining data
       }
       (void)dm_memcpy_handler(*l2,*l1,s2);  // copy remaining data
    } else {                      // No common area : Insert 0 length region line
       h = -h;
       l2 = l2 + (s1 = r1->Size() );
       for(size=0,i=0;i<h;i++) { (*l2)->rl_npair = 0; size+= sizeof(dmRegionLine); ++l2; }
       (void)dm_memcpy_handler(*l2,*r2->Begin(),(s2 = r2->Size()) ); // copy remaining data
    }
    size += (s1+s2);

    // Set new bounding Box
    box.top_left.y  = dm_min(d1.rgn_box.top_left.y , d2.rgn_box.top_left.y);
    box.top_left.x  = dm_min(d1.rgn_box.top_left.x , d2.rgn_box.top_left.x);
    box.bottom_right.y = dm_max(d1.rgn_box.bottom_right.y, d2.rgn_box.bottom_right.y);
    box.bottom_right.x = dm_max(d1.rgn_box.bottom_right.x, d2.rgn_box.bottom_right.x);

    if( dest.rgn_data.Realloc(size) ) {
      dest.rgn_box  = box;
      dest.rgn_size = __AdjustBoundingBox(dest.rgn_data,dest.rgn_box,buffer);
      pdest = &dest;
    } else {
      pdest = NULL;
      dest.FreeRegion();
    }
  } else
    pdest = NULL;

  return (pdest);
}
//--------------------------------------------------------------
// XOR region
//--------------------------------------------------------------
__dmKernel dmRgnHandle* dmRgnHandle::XorRegionData( const dmRgnHandle* p1,
                                                    const dmRgnHandle* p2,
                                                    dmRgnHandle* pdest)
{
  long h,ys,ye,xs,xe,i;
  dm_size size,s1,s2;
  dmRect box;
  dmRegionLineBuffer buffer(p1,p2,pdest);

  const dmRgnHandle& d1   = *p1;
  const dmRgnHandle& d2   = *p2;
  dmRgnHandle& dest = *pdest;

  // Handle empty regions
  if(d1.Empty()) { dest=d2; return (pdest); } else
  if(d2.Empty()) { dest=d1; return (pdest); }

  dmGetBoxIntersection(xs,ys,xe,ye,d1.rgn_box,d2.rgn_box);
  h = ye-ys + 1;
  if( buffer.Allocate(__BEST_SIZE(p1,p2)) )  // new buffer for operation
  {
    const dmRgnHandle *r1 = &d1, *r2 = &d2;
    rgnline_iterator l2 = buffer();
    // Get the upper region in r1
    if(r1->rgn_box.top_left.y > r2->rgn_box.top_left.y) EXCHANGE(r1,r2);
    (void)dm_memcpy_handler(buffer(),r1->rgn_data(),r1->rgn_size ); // copy region r1 into buffer
    if(h>0)    // Handle intersection
    {
      rgnline_iterator l1 = dmGetRegionLine( r1->Begin(), (ys - r1->rgn_box.top_left.y) );  // start line for r1
      l2 = l2 + (s1 = DISTANCE( *l1, *r1->Begin()));       // start line for buffer
      size = DoRgnLinesOperation_Xor(l1,r2->Begin(),l2,h);
      l2 = l2 + size;  // End of buffer
      if(r1->rgn_box.bottom_right.y <= r2->rgn_box.bottom_right.y) { // copy remaining data from r2
        l1 = dmGetRegionLine( r2->Begin(), h );            // get start line of remaining data
        s2 = r2->Size() - DISTANCE( *l1, *r2->Begin());    // size of remaining data
      } else {                                             // copy remaining data from r1
        l1 = dmGetRegionLine( l1, h );                     // get start line of remaining data
        s2 = r1->Size() - DISTANCE( *l1, *r1->Begin());    // size of remaining data
      }
      (void)dm_memcpy_handler(*l2,*l1,s2); // copy remaining data
    } else {                    // No common area : Insert 0 length region line
      h = -h;
      s1 = r1->Size();
      l2 = l2 + s1;
      for(size=0,i=0;i<h;i++) { (*l2)->rl_npair = 0; size+= sizeof(dmRegionLine); ++l2; }
      dm_memcpy_handler(*l2,*r2->Begin(),(s2=r2->Size())); // copy remaining data
    }
    size += (s1+s2);

    // Set new bounding Box
    box.top_left.y  = dm_min(d1.rgn_box.top_left.y , d2.rgn_box.top_left.y);
    box.top_left.x  = dm_min(d1.rgn_box.top_left.x , d2.rgn_box.top_left.x);
    box.bottom_right.y = dm_max(d1.rgn_box.bottom_right.y, d2.rgn_box.bottom_right.y);
    box.bottom_right.x = dm_max(d1.rgn_box.bottom_right.x, d2.rgn_box.bottom_right.x);

    if( dest.rgn_data.Realloc(size) ) {
      dest.rgn_box  = box;
      dest.rgn_size = __AdjustBoundingBox(dest.rgn_data,dest.rgn_box,buffer);
      pdest = &dest;
    } else {
      pdest = NULL;
      dest.FreeRegion();
    }
  } else
    pdest = NULL;

  return (pdest);
}
//--------------------------------------------------------------
// Regions factory
//--------------------------------------------------------------
template<class coordinate>
static inline dmRgnHandle* CREATE_REGION_(const coordinate* coord)
{
  if(coord) {
    dmRgnHandle *rgn = new dmRgnHandle(*coord);
    if( rgn->Size() ) return rgn;
    else delete rgn;
  }
  return NULL;
}
//--------------------------------------------------------------
#define CREATE_REGION( c ) { return CREATE_REGION_(c); }
//--------------------------------------------------------------
__dmKernel dmRgnHandle* dmRgnHandle::CreateRegion() { return (new dmRgnHandle()); }
//--------------------------------------------------------------
__dmKernel dmRgnHandle* dmRgnHandle::CreateRegion(const dmRect* r)
{
  return ( (r==NULL || r->IsEmpty()) ?
    NULL :
    CREATE_REGION_(r));
}
//--------------------------------------------------------------
__dmKernel dmRgnHandle* dmRgnHandle::CreateRegion(const dmCircle*  c) CREATE_REGION(c)
__dmKernel dmRgnHandle* dmRgnHandle::CreateRegion(const dmLine*    l) CREATE_REGION(l)
__dmKernel dmRgnHandle* dmRgnHandle::CreateRegion(const dmEllipse* e) CREATE_REGION(e)
__dmKernel dmRgnHandle* dmRgnHandle::CreateRegion(const dmPoly*    p) CREATE_REGION(p)

#undef CREATE_REGION

//-----------------------------------------------------------
__dmKernel void dmRgnHandle::DisposeRegion( dmRgnHandle* rgn )
{
  if(rgn)
    delete rgn;
}
//-----------------------------------------------------------
// dmRgnEditor (Region Builder)
//-----------------------------------------------------------
__dmKernel void dmRgnEditor::Initialize(const dmRect& r,dm_size size )
{
   s_iter = s_rgn.Reserve(r,size);
   s_pair = s_iter.begin();
   s_size = 0;
}
//-----------------------------------------------------------
__dmKernel void dmRgnEditor::EnlargeBuffer(dm_size _size)
{
   s_size = DISTANCE( *s_iter, *s_rgn.Begin());
   if(s_rgn.rgn_data.Realloc(s_size+_size))
   {
     s_rgn.rgn_size = s_size+_size;
     s_iter = rgnline_iterator((dmRgnLinePtr)( ((char*)s_rgn.rgn_data())+s_size ) );
     (*s_iter)->rl_npair = 0;         // set number of pair to zero;
	 s_pair = s_iter.begin();
   }
}
//-----------------------------------------------------------
__dmKernel const dmRgnHandle& dmRgnEditor::Validate()
{
  s_rgn.rgn_data.Realloc(s_size);
  s_rgn.rgn_size = s_size;
  s_rgn.AdjustBoundingBox();
  return s_rgn;
}
//-----------------------------------------------------------
__dmKernel void dmRgnEditor::PushLine()
{
  (*++s_iter)->rl_npair = 0; // set number of pair to zero;
  s_pair = s_iter.begin();   // increase line
  s_size = DISTANCE( *s_iter, *s_rgn.Begin());
  if(s_size > (s_rgn.Size()-dmRgnLineArray::SecurityBuffer) ) {
     EnlargeBuffer(dmRgnLineArray::MaxDfltArraySize);
  }
}
//-----------------------------------------------------------
