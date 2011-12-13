
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

#include "regions/dmRegionsProcessing.h"
#include "common/dmMetrics.h"

#include <math.h>
#include <deque>
#include <vector>

//---------------------------------------------------------------
// function: dmRgnConvexHull
//---------------------------------------------------------------
static inline dm_int __d(const dm_point& pa, const dm_point& pb, const dm_point& pc ) 
{
  return (pb.x-pa.x)*(pc.y-pa.y) - (pb.y-pa.y)*(pc.x-pa.x); 
}
//------------------------------------------------------------------------
#define VERTEX( _v ) _input[_v]

typedef size_t __vertex;

#define STOP  (n>npm1)
#define INPUT( _v )  _v = n++
#define PUSH( _v )   x.push_back (_v)
#define INSERT( _v ) x.push_front(_v)
#define REMOVE() x.pop_front()
#define POP()    x.pop_back()
#define DBOT( _v ) __d(VERTEX(_v),VERTEX(*x.begin()),VERTEX(*(x.begin()+1)))
#define DTOP( _v ) __d(VERTEX(*(x.rbegin()+1)),VERTEX(*x.rbegin()),VERTEX(_v))

#define _UNDERFLOW_ (x.size()<3)
//------------------------------------------------------
size_t dmRgnConvexHull( const dmPoly& _input, dmPoly& _output )
{
  _output.Clear();

  size_t n,npm1,np = _input.Size();
  if(np>=3) // Protect               
  {
    npm1 = np-1;
    n = 0;
     
    std::deque<__vertex> x;

    __vertex v,v1,v2,v3;
    INPUT( v1 );
    INPUT( v2 );
    INPUT( v3 );

    if( __d(VERTEX(v1),VERTEX(v2),VERTEX(v3)) > 0 ) { 
      PUSH(v1);  
      PUSH(v2);
    } else { 
      PUSH(v2); 
      PUSH(v1); 
    }    
    
    PUSH(v3);
    INSERT(v3);  
    
    do {
      INPUT( v ); 
      if(!STOP) {
        while( DBOT(v)>=0 &&  DTOP(v)>=0 ) {
          INPUT( v ); if STOP break;
        }
      }

      while( DTOP(v)<=0 ) { POP(); if _UNDERFLOW_ return 0;  }
      PUSH(v);

      while( DBOT(v)<=0 ) REMOVE();
      INSERT(v);
    } while(!STOP);

    if(x.size()>0) {
      _output.Resize(x.size());
      for(size_t v=x.size();--v>=0;)
        _output[v] = _input[v];
    }
  }
  return _output.Size();
}
//---------------------------------------------------------------
// function: dmRgnComputeRotationPts
//---------------------------------------------------------------
size_t dmRgnComputeRotationPts( const dmPoly& _input, dmVector<size_t>& _output, 
                                size_t seglength )
{
  if(_input.Size() < seglength || !_input.Closed())
    return 0;

  _output.Clear();

  size_t v,n = _input.Size();
  std::vector<double> _ap(n+2);

  dm_point  p1,p2,p3;

  // Compute angle
  for(v=seglength;v<n+seglength;++v)
  {
    p1 = _input[v-seglength];
    p2 = _input[v % n];
    p3 = _input[(v+seglength) % n];
    _ap[v-seglength+1] = fabs(dmMetrics::Angle(p1,p2,p3));
  }
  _ap[0]   = _ap[n];
  _ap[n+2] = _ap[1];
 
  double a = 0;
  for(v=1;v<_ap.size()-1;++v) 
    a = _ap[v];
    if(a > _ap[v-1] && a > _ap[v+1] ) 
      _output.Push_Back(v-1);

  // Handle special case where no dominant points have been found
  if(_output.Size() <= 0) {
     _output.Push_Back(0);
     _output.Push_Back(n / 2);
  }

  return _output.Size();
}
//---------------------------------------------------------------
// function: dmRgnContourSketch
//---------------------------------------------------------------
size_t dmRgnContourSketch( const dmPoly& _input, dmPoly& _output, 
                           size_t seglength, size_t mindist )
{
  dmVector<size_t> _p;
  if(dmRgnComputeRotationPts(_input,_p,seglength)>0) 
  {
    double d = 0,dmax = 0;
    dm_line   l;
    bool done   = false;
    size_t k,imax,istart,iend;

    dmVector<size_t> _ptmp;

    // Append points located before the first control point
    // to the polygon
    dmPoly::iterator OI;

    _output.Resize(_input.Size() + _p[0]);
    OI = std::copy(_input.Begin(),_input.End(),_output.Begin());
    OI = std::copy(_input.Begin(),_input.Begin()+_p[0],OI);

    // Now the last point of the polygon is a control point
    _p.Push_Back(_output.Size()-1);

    while(!done) 
    {
      done = true;
      dmVector<size_t>::iterator _First = _p.Begin();
      dmVector<size_t>::iterator _Last  = _p.End() - 1;
      for(;_First!=_Last;++_First)
      {
        dmax    = mindist;
        l.start = _output[istart = *_First];
        l.end   = _output[iend   = *(_First+1)];

        _ptmp.Push_Back(istart);
        for(k=istart;k<=iend;++k) 
        {
          d = dmMetrics::Euclidian_Distance(_output[k],l);
          if(d>dmax) {
            imax = k;
            dmax = d;
          }
        }

        if(d>0) {
          _ptmp.Push_Back(imax);
          done = false;
        }
        _ptmp.Push_Back(iend);
      }

      _p.Resize(_ptmp.Size());
      std::copy(_ptmp.Begin(),_ptmp.End(),_p.Begin());
    }

    // Copy back the control point into output polygon
    for(k=0;k<_p.Size();++k) {
      _output[k] = _output[_p[k]];
    }
    _output.Resize(_p.Size());
    return _output.Size();
  }

  return 0;
} 
//---------------------------------------------------------------

