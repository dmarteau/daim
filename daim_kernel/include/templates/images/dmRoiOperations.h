#ifndef dmRoiOperations_h
#define dmRoiOperations_h

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
// File         : dmRoiOperations.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

namespace daim {

//--------------------------------------------------------------
// Binary operation on two images
// rgn apply to first image && result is copied in <out> at P;
//--------------------------------------------------------------
bool __dmKernel _AdjustRegions( dmRect& _rs, dmRect& _rd, dmRegion&, const dmPoint& );
//--------------------------------------------------------------
template<class T1, class T2, class Op>
Op RoiOperation( Op _op, image<T1>& _src, image<T2>& _dest, 
                 const dmRegion& _rgn, 
                 const dmPoint& p = dmPoint(0,0) 
               )
{
  dmRegion rgn = _rgn;
  dmRect rs = _src.rect();
  dmRect rd = _dest.rect();
  if(_AdjustRegions( rs,rd,rgn,p ))
  { 
    typename image<T1>::line_type inp  = _src.begin(rs);
    typename image<T2>::line_type outp = _dest.begin(rd);
        
    if(rd.Left() != rs.Left() ) 
    {
      long xoffset = rd.Left() - rs.Left();
      return _RoiOperation(rgn,inp,_BindLineOffset(outp,xoffset,get_type_of(*outp)),_op);
    } 
    else 
     return _RoiOperation(rgn,inp,outp,_op);
  }
  return _op;
}
//--------------------------------------------------------------
template<class T1, class T2, class Op>
Op RoiOperation( Op _op, 
                 const image<T1>& _src, image<T2>& _dest, 
                 const dmRegion& _rgn, 
                 const dmPoint& p = dmPoint(0,0) )
{
  dmRegion rgn = _rgn;
  dmRect rs = _src.rect();
  dmRect rd = _dest.rect();

  if(_AdjustRegions( rs,rd,rgn,p ))
  { 
    typename image<T1>::const_line_type inp  = _src.begin(rs);
    typename image<T2>::line_type       outp = _dest.begin(rd);

    if(rd.Left() != rs.Left() ) 
    {
      long xoffset = rd.Left() - rs.Left();
      return _RoiOperation(rgn,inp,_BindLineOffset(outp,xoffset,get_type_of(*outp)),_op);
    } 
    else 
     return _RoiOperation(rgn,inp,outp,_op);
  }
  return _op;
}
//--------------------------------------------------------------
// Binding for ternary operator
// this operator assume that _src1 and _src2 have same dimensions
//--------------------------------------------------------------
template<class T1, class T2,class T3,class Op>
Op RoiOperation( Op _op, image<T1>& _src1, 
                         image<T2>& _src2, 
                         image<T3>& _dest, 
                         const dmRegion& _rgn, 
                         const dmPoint& p = dmPoint(0,0) )
{
  dmRegion rgn = _rgn;
  dmRect rs = _src1.rect();
  dmRect rd = _dest.rect();
  if(_AdjustRegions( rs,rd,rgn,p ))
  { 
    typename image<T1>::line_type inp1 = _src1.begin(rs);
    typename image<T2>::line_type inp2 = _src2.begin(rs);
    typename image<T3>::line_type outp = _dest.begin(rd);
        
    if(rd.Left() != rs.Left() ) 
    {
      long xoffset = rd.Left() - rs.Left();
      return _RoiOperation(rgn,inp1,inp2,_BindLineOffset(outp,xoffset,get_type_of(*outp)),_op);
    } 
    else 
     return _RoiOperation(rgn,inp1,inp2,outp,_op);
  }
  return _op;
}
//--------------------------------------------------------------
// Cette fonction construit une region basée sur le prédicat 
// Pred( line_type, x )
//--------------------------------------------------------------
template<class T,class Pred> Pred _CreateRoi_If( 
                               const image<T>& img,
                               Pred            predicat,
                               dmRegion&       region, 
                               const dmRect&   r )
{
   dmRgnEditor  rgn_init(r);
   int x1,x2,xs = r.Left(), xe = r.Right();
   typename image<T>::const_line_type line = img.begin(r);
   typename image<T>::const_line_type end  = img.end(r);
   for(int j=0;line!=end;++line,++j) {
     for(int x=xs;x<=xe;++x) {
       if(predicat( line , x )) {
	       for(x2=x1=x; (++x <= xe) && predicat( line, x );) { x2=x; }
	       rgn_init.AddSegment(x1,x2);
       }
     }
     rgn_init.PushLine();
   }
   region.SetRgnRoi(rgn_init.Validate());
   return predicat;
}
//--------------------------------------------------------------
// Simple predicat 
//--------------------------------------------------------------
template<class Pred> 
struct _PixelPredicat
{
  Pred p;
  _PixelPredicat( const Pred& _p ) : p(_p) {}
  template<class line_type> 
  bool operator()( line_type l, int x ) { return p( (*l)[x] ); }
};
//---------------------------------------------------------------
template<class T,class Pred> Pred create_roi( 
   const image<T>& img,Pred predicat,dmRegion& region )
{
   return _CreateRoi_If( img,_PixelPredicat<Pred>(predicat), region, img.rect() ).p;
};
//--------------------------------------------------------------
// Create a region from pixels included in rectangle r
// THE RECTANGLE IS NOT CHECKED
//---------------------------------------------------------------
template<class T,class Pred> Pred _CreateRectRoi_If( 
   const image<T>& img,Pred predicat,dmRegion& region, const dmRect& r )
{
   dmRgnEditor  rgn_init(r);
   int x1,x2,xs = r.Left(), xe = r.Right();
   typename image<T>::const_line_type line = img.begin(r);
   typename image<T>::const_line_type end  = img.end(r);
   for(;line!=end;++line) {
     for(int x=xs;x<=xe;++x) {
       if(predicat( line , x )) {
         for(x2=x1=x; (++x <= xe) && predicat( line, x );) { x2=x; }
         rgn_init.AddSegment(x1,x2);
       }
     }
     rgn_init.PushLine();
   }
   region.SetRgnRoi(rgn_init.Validate());
   return predicat;
}
//--------------------------------------------------------------
// Create a region from pixels included in region <rgn>
// THE REGION IS NOT CHECKED
//---------------------------------------------------------------
template<class T,class Pred> Pred _CreateRgnRoi_If( 
   const image<T>& img,Pred predicat,dmRegion& region, const dmRegion& rgn )
{
  if(!rgn.IsEmptyRoi()) 
  {
    if(rgn.IsRectRoi()) 
      return _CreateRectRoi_If(img,predicat,region,rgn.Rectangle());
    else
    {
      dmRgnEditor  rgn_init(rgn.Rectangle());

      typename image<T>::const_line_type line = img.begin(rgn.Rectangle());

      dmRgnHandle::iterator it  = rgn.Region().Begin();
      dmRgnHandle::iterator end = rgn.Region().End();
      int x1,x2,xe;
      for(REGISTER dmRgnPair *p,*pend;it!=end; ++it,++line) {
        for(p=it.begin(),pend=it.end();p!=pend;++p) {
          x1 = (*p).x1;
          xe = (*p).x2;
          for(int x=x1;x<=xe;++x) {
            if(predicat( line , x )) {
              for(x2=x1=x; (++x <= xe) && predicat( line, x );) x2=x;
              rgn_init.AddSegment(x1,x2);
            }
          } 
        }
        rgn_init.PushLine();
	  }
      region.SetRgnRoi(rgn_init.Validate());
    }
  }
  return predicat;
}
//---------------------------------------------------------------
template<class T,class Pred> Pred create_rgnroi( 
   const image<T>& img,Pred predicat,dmRegion& region,const dmRegion& rgn )
{
   return _CreateRgnRoi_If( img,_PixelPredicat<Pred>(predicat),region,rgn).p;
};
//--------------------------------------------------------------
// Do a ROI Operation by passing twice on the image 
// top to bottom the bottom to top
//--------------------------------------------------------------
template<class T1, class T2, class Op1,class Op2>
void _TwoPassesRoiOperation( const dmRegion& roi, const dmPoint& p, 
                             image<T1>& src, image<T2>& dest, 
                             const Op1& op1, const Op2& op2 )
{
  dmRegion rgn = roi;
  dmRect rs = src.rect();
  dmRect rd = dest.rect();

  if(_AdjustRegions( rs,rd,rgn,p ))
  {    
    typename image<T1>::line_type inp  = src.begin(rs);
    typename image<T2>::line_type outp = dest.begin(rd);
 
    typename image<T1>::reverse_line_type rinp  = src.rbegin(rs);
    typename image<T2>::reverse_line_type routp = dest.rbegin(rd);
     
    if(rd.Left() != rs.Left()) 
    {
      long xoffset = rd.Left() - rs.Left();
      _RoiOperation(rgn,inp,_BindLineOffset(outp,xoffset,get_type_of(*outp)), op1);
      _ReverseRoiOperation(rgn,rinp,_BindLineOffset(routp,xoffset,get_type_of(*routp)),op2);
    } else {
      _RoiOperation(rgn,inp,outp,op1);
      _ReverseRoiOperation(rgn,rinp,routp,op2);
    }
  }
}
//---------------------------------------------------------------

}; // namespace daim

#endif // dmRoiOperations_h
