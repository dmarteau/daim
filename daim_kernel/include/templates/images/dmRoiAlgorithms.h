#ifndef dmRoiAlgorithms_h
#define dmRoiAlgorithms_h

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
// File         : dmRoiAlgorithms.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

#include <vector>

namespace daim {

//------------------------------------------------------------
/// unary sequences operations
//------------------------------------------------------------
template<class line, class Op>
Op _RoiOperation(  const dmRgnHandle& rgn,line In, Op op )
{
  if(!rgn.Empty())
  {
	dmRgnHandle::iterator it  = rgn.Begin();
	dmRgnHandle::iterator end = rgn.End();
	for(dmRgnPair *p,*pend;it!=end; ++it,++In) {
      for(p=it.begin(),pend=it.end();p!=pend;++p) {
          op( In,(*p).x1,(*p).x2 );
      }
	}
  }
  return op;
}
//-------------------------------------------------------------
template<class line, class Op>
Op _RoiOperation( const dmRect& r,line In, Op op)
{
  long xs = r.Left(), xe = r.Right();
  for(long h=r.BoxHeight();--h>=0;++In) {
    op( In,xs,xe );
  }
  return op;
}
//-------------------------------------------------------------
template<class line,class Op>
inline Op _RoiOperation( const dmRegion& roi,line In, Op op)
{
  return (roi.IsRectRoi()
           ? _RoiOperation( roi.Rectangle(), In, op)
           : _RoiOperation( roi.Region()   , In, op)
         );
}
//-------------------------------------------------------------
template<class line, class Op>
Op _ReverseRoiOperation( const dmRgnHandle& rgn,line In, Op op )
{
  if(!rgn.Empty()) 
  {
    int k,n;
    dmRgnHandle::iterator it  = rgn.Begin();
    dmRgnHandle::iterator end = rgn.End();
    std::vector<dmRgnLinePtr> lines(rgn.Box().Height());
    for(k=0;it!=end;++it,++k)  { lines[k] = *it; }

    std::vector<dmRgnLinePtr>::reverse_iterator l  = lines.rbegin();
    for(dmRgnPair *p;l!=lines.rend(); ++l,++In) {
      p = (*l)->rl_xpair;
      n = (*l)->rl_npair;
      for(k=0;k<n;++k,++p) 
        op( In,(*p).x1,(*p).x2 );
    }
  }
  return op;
}
//-------------------------------------------------------------
template<class line, class Op>
Op _ReverseRoiOperation( const dmRegion& roi,line In, Op op )
{
  return (roi.IsRectRoi()
			 ? _RoiOperation( roi.Rectangle(), In, op)
			 : _ReverseRoiOperation( roi.Region(), In, op)
		 );
}
///
//-------------------------------------------------------------
///  Binary operation Binding
//-------------------------------------------------------------
template<class Line1,class Line2> struct _Bind2 {
   Line1 L1;
   Line2 L2;
   _Bind2( Line1 l1,Line2 l2 ) : L1(l1), L2(l2) {}
   _Bind2<Line1,Line2>& operator++() { ++L1;++L2;return *this; }
   _Bind2<Line1,Line2>& operator--() { --L1;--L2;return *this; }
};
//
template<class Line1,class Line2,class BinOp>
class _BinaryLineOp
{
   private: BinOp op;
   public:
   _BinaryLineOp(const BinOp& _op) : op(_op) {}
   BinOp Op() const { return op; }
   void operator()( _Bind2<Line1,Line2>& bind,long x1,long x2)
   { op( bind.L1, bind.L2, x1, x2 ); }
};
//
template<class ROI,class L1,class L2, class BinOp>
BinOp _RoiOperation(const ROI& roi, L1 In1, L2 In2, const BinOp& op)
{ return _RoiOperation( roi, _Bind2<L1,L2>(In1,In2),_BinaryLineOp<L1,L2,BinOp>(op) ).Op(); }
//
template<class ROI,class L1,class L2, class BinOp>
BinOp _ReverseRoiOperation(const ROI& roi, L1 In1, L2 In2, const BinOp& op)
{ return _ReverseRoiOperation( roi, _Bind2<L1,L2>(In1,In2),_BinaryLineOp<L1,L2,BinOp>(op) ).Op(); }
//-------------------------------------------------------------
///  Ternary operation Binding
//-------------------------------------------------------------
template<class Line1,class Line2,class Line3> 
struct _Bind3 {
   Line1 L1;
   Line2 L2;
   Line3 L3;
   _Bind3( Line1 l1,Line2 l2,Line3 l3 ) : L1(l1), L2(l2), L3(l3) {}
   _Bind3<Line1,Line2,Line3>& operator++() { ++L1;++L2;++L3; return *this; }
   _Bind3<Line1,Line2,Line3>& operator--() { --L1;--L2;--L3; return *this; }
};
//
template<class Line1,class Line2,class Line3,class TerOp>
class _TernaryLineOp
{
   private : TerOp op;
   public:
   _TernaryLineOp( const TerOp& _op) : op(_op) {}
   TerOp Op() const { return op; }
   void operator()( _Bind3<Line1,Line2,Line3>& bind,long x1,long x2)
   { op( bind.L1,bind.L2,bind.L3,x1, x2 ); }
};
//
template<class ROI,class L1,class L2,class L3, class TerOp>
TerOp _RoiOperation(const ROI& roi, L1 In1,L2 In2,L3 In3, const TerOp& op)
{ return _RoiOperation( roi,_Bind3<L1,L2,L3>(In1,In2,In3),_TernaryLineOp<L1,L2,L3,TerOp>(op) ).Op(); }
//
template<class ROI,class L1,class L2,class L3, class TerOp>
TerOp _ReverseRoiOperation(const ROI& roi, L1 In1,L2 In2,L3 In3, const TerOp& op)
{ return _ReverseRoiOperation( roi,_Bind3<L1,L2,L3>(In1,In2,In3),_TernaryLineOp<L1,L2,L3,TerOp>(op) ).Op(); }
///
//-------------------------------------------------------------
/// Bind a line with a offset
//-------------------------------------------------------------
template<class line,class IT>
struct _OffsetLine 
{
  line L;
  long X;

  _OffsetLine(line l,long x=0) : L(l), X(x) {} 
  
  IT operator*() { return (*L + X); } 
  IT operator[](int i) { return (L[i]+X); }
      
  _OffsetLine& operator++() { ++L; return *this; }
  _OffsetLine& operator--() { --L; return *this; } 
};
//-------------------------------------------------------------
template<class line,class IT>
_OffsetLine<line, IT> _BindLineOffset( line L, long X, type_of<IT> _type )
{
  return _OffsetLine<line,IT>( L, X );
}
//-------------------------------------------------------------
/// Functional binding of std operators
//-------------------------------------------------------------

// for_each
template<class ROI,class I,class UnOp>
inline UnOp _for_each(const ROI& roi,I line, UnOp op )
{ return _RoiOperation(roi,line,_For_Each<UnOp>(op) ).Op(); }

// count
template<class ROI,class I,class T,class Size>
inline Size _count(const ROI& roi,I line,T& value,Size& res )
{ return (res = _RoiOperation( roi,line,_Count<T,Size>(value) ).count()); }

// count_if 
template<class ROI,class I,class T,class Pred,class Size>
inline Size _count_if(const ROI& roi,I line,const Pred& p,Size& res )
{ return (res = _RoiOperation(roi,line,_Count_If<Pred,Size>(p) ).count()); }

// copy
template<class ROI,class In,class Out>
inline void _copy(const ROI& roi,In in,Out out)
{ _RoiOperation(roi,in,out,_Copy()); }

// copy_if 
template<class ROI,class In,class Out,class Pred>
inline Pred _copy_if(const ROI& roi,In in,Out out,const Pred& p)
{ return _RoiOperation(roi,in,out,_Copy_If<Pred>(p)).Op(); }

// transform (binary)
template<class ROI,class In,class In2,class Out,class BinOp>
inline BinOp _transform(const ROI& roi,In in,In2 in2,Out out,const BinOp& op)
{ return _RoiOperation(roi,in,in2,out,_BinaryTransform<BinOp>(op)).Op(); }

// transform (unary)
template<class ROI,class In,class Out,class UnOp>
inline UnOp _transform(const ROI& roi,In in,Out out,const UnOp& op)
{  return _RoiOperation(roi,in,out,_UnaryTransform<UnOp>(op)).Op(); }

// replace 
template<class ROI,class In,class T>
inline  void _replace(const ROI& roi, In in, const T& val, const T& new_val)
{ _RoiOperation(roi,in,_Replace<T>(val,new_val)); }

// replace_if
template<class ROI,class In,class Pred,class T>
inline Pred _replace_if( const ROI& roi,In in, const Pred& p, const T& val )
{ return _RoiOperation(roi,in,_Replace_If<Pred,T>(val,p)).Op(); }

// make_binary
template<class ROI,class In,class Out,class Pred,class T>
inline Pred _make_binary( const ROI& roi,In in,Out out, const Pred& p, const T& t_val, const T& f_val )
{ return _RoiOperation(roi,in,out,_Binarize<Pred,T>(t_val,f_val,p)).Op(); }

// replace_copy
template<class ROI,class In,class Out,class T>
inline  void _replace_copy( const ROI& roi,In in,Out out, const T& val,const T& new_val)
{ _RoiOperation(roi,in,out,_Replace_Copy<T>(val,new_val)); }

// replace_copy_if
template<class ROI,class In,class Out,class Pred,class T>
inline Pred _replace_copy_if( const ROI& roi, In in, Out out, const Pred& p, const T& val )
{ return _RoiOperation(roi,in,out,_Replace_If<Pred,T>(val,p)).Op(); }

// fill
template<class ROI,class In,class T>
inline void _fill( const ROI& roi, In in, const T& val )
{ _RoiOperation(roi,in,_Fill<T>(val)); }

// generate
template<class ROI,class In,class Gen>
inline Gen _generate( const ROI& roi, In in, const Gen& g )
{ return _RoiOperation(roi,in,_Generate<Gen>(g)).Op(); }

// accumulate
template<class ROI,class In,class T>
inline T _accumulate( const ROI& roi, In in, T v )
{ return _RoiOperation(roi,in,_Accumulate<T>(v)).value(); }

// accumulate_if
template<class ROI,class In,class T,class Pred>
inline T _accumulate_if( const ROI& roi, In in, T v, const Pred& p )
{ return RoiOperation(roi,in,_Accumulate_If<T,Pred>(p,v)).value(); }

// swap
template<class ROI,class In,class Out>
inline void _swap( const ROI& roi, In in, Out out )
{ _RoiOperation(roi,in,out,_Swap()); }

// combine
template<class ROI,class In,class Out,class BinaryFun>
inline BinaryFun _combine( const ROI& roi, In in, Out out, const BinaryFun& f )
{ return _RoiOperation(roi,in,out,_Combine<BinaryFun>(f) ).Op(); }

// combine3
template<class ROI,class In1,class In2,class Out,class TernaryFun>
inline TernaryFun _combine3( const ROI& roi, In1 in1, In2 in2, Out out, const TernaryFun& f )
{ return _RoiOperation(roi,in1,in2,out,_Combine3<TernaryFun>(f) ).Op(); }

// evaluate
template<class ROI,class In1,class In2,class BinaryFun>
inline BinaryFun _evaluate( const ROI& roi, In1 in1, In2 in2, const BinaryFun& f )
{ return _RoiOperation(roi,in1,in2,_Evaluate<BinaryFun>(f) ).Op(); }


}; // namespace daim

#endif // dmRoiAlgorithms_h

