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

namespace core {

//------------------------------------------------------------
/// unary sequences operations
//------------------------------------------------------------
template<class line, class Op>
Op RoiOperation(  const dmRgnHandle& rgn,line In, Op op )
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
Op RoiOperation( const dmRect& r,line In, Op op)
{
  long xs = r.Left(), xe = r.Right();
  for(long h=r.BoxHeight();--h>=0;++In) {
    op( In,xs,xe );
  }
  return op;
}
//-------------------------------------------------------------
template<class line,class Op>
inline Op RoiOperation( const dmRegion& roi,line In, Op op)
{
  return (roi.IsRectRoi()
           ? RoiOperation( roi.Rectangle(), In, op)
           : RoiOperation( roi.Region()   , In, op)
         );
}
//-------------------------------------------------------------
template<class line, class Op>
Op ReverseRoiOperation( const dmRgnHandle& rgn,line In, Op op )
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
Op ReverseRoiOperation( const dmRegion& roi,line In, Op op )
{
  return (roi.IsRectRoi()
			 ? RoiOperation( roi.Rectangle(), In, op)
			 : ReverseRoiOperation( roi.Region(), In, op)
		 );
}
///
//-------------------------------------------------------------
///  Binary operation Binding
//-------------------------------------------------------------
template<class Line1,class Line2> struct Bind2 {
   Line1 L1;
   Line2 L2;
   Bind2( Line1 l1,Line2 l2 ) : L1(l1), L2(l2) {}
   Bind2<Line1,Line2>& operator++() { ++L1;++L2;return *this; }
   Bind2<Line1,Line2>& operator--() { --L1;--L2;return *this; }
};
//
template<class Line1,class Line2,class BinOp>
class BinaryLineOp
{
   private: BinOp op;
   public:
   BinaryLineOp(const BinOp& _op) : op(_op) {}
   BinOp Op() const { return op; }
   void operator()( Bind2<Line1,Line2>& bind,long x1,long x2)
   { op( bind.L1, bind.L2, x1, x2 ); }
};
//
template<class ROI,class L1,class L2, class BinOp>
BinOp RoiOperation(const ROI& roi, L1 In1, L2 In2, const BinOp& op)
{ return RoiOperation( roi,Bind2<L1,L2>(In1,In2),BinaryLineOp<L1,L2,BinOp>(op) ).Op(); }
//
template<class ROI,class L1,class L2, class BinOp>
BinOp ReverseRoiOperation(const ROI& roi, L1 In1, L2 In2, const BinOp& op)
{ return ReverseRoiOperation( roi,Bind2<L1,L2>(In1,In2),BinaryLineOp<L1,L2,BinOp>(op) ).Op(); }
//-------------------------------------------------------------
///  Ternary operation Binding
//-------------------------------------------------------------
template<class Line1,class Line2,class Line3> 
struct Bind3 {
   Line1 L1;
   Line2 L2;
   Line3 L3;
   Bind3( Line1 l1,Line2 l2,Line3 l3 ) : L1(l1), L2(l2), L3(l3) {}
   Bind3<Line1,Line2,Line3>& operator++() { ++L1;++L2;++L3; return *this; }
   Bind3<Line1,Line2,Line3>& operator--() { --L1;--L2;--L3; return *this; }
};
//
template<class Line1,class Line2,class Line3,class TerOp>
class TernaryLineOp
{
   private : TerOp op;
   public:
   TernaryLineOp( const TerOp& _op) : op(_op) {}
   TerOp Op() const { return op; }
   void operator()( Bind3<Line1,Line2,Line3>& bind,long x1,long x2)
   { op( bind.L1,bind.L2,bind.L3,x1, x2 ); }
};
//
template<class ROI,class L1,class L2,class L3, class TerOp>
TerOp RoiOperation(const ROI& roi, L1 In1,L2 In2,L3 In3, const TerOp& op)
{ return RoiOperation( roi,Bind3<L1,L2,L3>(In1,In2,In3),TernaryLineOp<L1,L2,L3,TerOp>(op) ).Op(); }
//
template<class ROI,class L1,class L2,class L3, class TerOp>
TerOp ReverseRoiOperation(const ROI& roi, L1 In1,L2 In2,L3 In3, const TerOp& op)
{ return ReverseRoiOperation( roi,Bind3<L1,L2,L3>(In1,In2,In3),TernaryLineOp<L1,L2,L3,TerOp>(op) ).Op(); }
///
//-------------------------------------------------------------
/// Bind a line with a offset
//-------------------------------------------------------------
template<class line,class IT>
struct OffsetLine 
{
  line L;
  long X;

  OffsetLine(line l,long x=0) : L(l), X(x) {} 
  
  IT operator*() { return (*L + X); } 
  IT operator[](int i) { return (L[i]+X); }
      
  OffsetLine& operator++() { ++L; return *this; }
  OffsetLine& operator--() { --L; return *this; } 
};
//-------------------------------------------------------------
template<class line,class IT>
OffsetLine<line, IT> BindLineOffset( line L, long X, type_of<IT> _type )
{
  return OffsetLine<line,IT>( L, X );
}
//-------------------------------------------------------------
/// Functional binding of std operators
//-------------------------------------------------------------

// for_each
template<class ROI,class I,class UnOp>
inline UnOp for_each(const ROI& roi,I line, UnOp op )
{ return RoiOperation(roi,line,functional::For_Each<UnOp>(op) ).Op(); }

// count
template<class ROI,class I,class T,class Size>
inline Size count(const ROI& roi,I line,T& value,Size& res )
{ return (res = RoiOperation( roi,line,functional::Count<T,Size>(value) ).count()); }

// count_if 
template<class ROI,class I,class T,class Pred,class Size>
inline Size count_if(const ROI& roi,I line,const Pred& p,Size& res )
{ return (res = RoiOperation(roi,line,functional::Count_If<Pred,Size>(p) ).count()); }

// copy
template<class ROI,class In,class Out>
inline void copy(const ROI& roi,In in,Out out)
{ RoiOperation(roi,in,out,functional::Copy()); }

// copy_if 
template<class ROI,class In,class Out,class Pred>
inline Pred copy_if(const ROI& roi,In in,Out out,const Pred& p)
{ return RoiOperation(roi,in,out,functional::Copy_If<Pred>(p)).Op(); }

// transform (binary)
template<class ROI,class In,class In2,class Out,class BinOp>
inline BinOp transform(const ROI& roi,In in,In2 in2,Out out,const BinOp& op)
{ return RoiOperation(roi,in,in2,out,functional::BinaryTransform<BinOp>(op)).Op(); }

// transform (unary)
template<class ROI,class In,class Out,class UnOp>
inline UnOp transform(const ROI& roi,In in,Out out,const UnOp& op)
{  return RoiOperation(roi,in,out,functional::UnaryTransform<UnOp>(op)).Op(); }

// replace 
template<class ROI,class In,class T>
inline  void replace(const ROI& roi, In in, const T& val, const T& new_val)
{ RoiOperation(roi,in,functional::Replace<T>(val,new_val)); }

// replace_if
template<class ROI,class In,class Pred,class T>
inline Pred replace_if( const ROI& roi,In in, const Pred& p, const T& val )
{ return RoiOperation(roi,in,functional::Replace_If<Pred,T>(val,p)).Op(); }

// make_binary
template<class ROI,class In,class Out,class Pred,class T>
inline Pred make_binary( const ROI& roi,In in,Out out, const Pred& p, const T& t_val, const T& f_val )
{ return RoiOperation(roi,in,out,functional::Binarize<Pred,T>(t_val,f_val,p)).Op(); }

// replace_copy
template<class ROI,class In,class Out,class T>
inline  void replace_copy( const ROI& roi,In in,Out out, const T& val,const T& new_val)
{ RoiOperation(roi,in,out,functional::Replace_Copy<T>(val,new_val)); }

// replace_copy_if
template<class ROI,class In,class Out,class Pred,class T>
inline Pred replace_copy_if( const ROI& roi, In in, Out out, const Pred& p, const T& val )
{ return RoiOperation(roi,in,out,functional::Replace_Copy_If<Pred,T>(val,p)).Op(); }

// fill
template<class ROI,class In,class T>
inline void fill( const ROI& roi, In in, const T& val )
{ RoiOperation(roi,in,functional::Fill<T>(val)); }

// generate
template<class ROI,class In,class Gen>
inline Gen generate( const ROI& roi, In in, const Gen& g )
{ return RoiOperation(roi,in,functional::Generate<Gen>(g)).Op(); }

// accumulate
template<class ROI,class In,class T>
inline T accumulate( const ROI& roi, In in, T v )
{ return RoiOperation(roi,in,functional::Accumulate<T>(v)).value(); }

// accumulate_if
template<class ROI,class In,class T,class Pred>
inline T accumulate_if( const ROI& roi, In in, T v, const Pred& p )
{ return RoiOperation(roi,in,functional::Accumulate_If<T,Pred>(p,v)).value(); }

// swap
template<class ROI,class In,class Out>
inline void swap( const ROI& roi, In in, Out out )
{ RoiOperation(roi,in,out,functional::Swap()); }

// combine
template<class ROI,class In,class Out,class BinaryFun>
inline BinaryFun combine( const ROI& roi, In in, Out out, const BinaryFun& f )
{ return RoiOperation(roi,in,out,functional::Combine<BinaryFun>(f) ).Op(); }

// combine3
template<class ROI,class In1,class In2,class Out,class TernaryFun>
inline TernaryFun combine3( const ROI& roi, In1 in1, In2 in2, Out out, const TernaryFun& f )
{ return RoiOperation(roi,in1,in2,out,functional::Combine3<TernaryFun>(f) ).Op(); }

// evaluate
template<class ROI,class In1,class In2,class BinaryFun>
inline BinaryFun evaluate( const ROI& roi, In1 in1, In2 in2, const BinaryFun& f )
{ return RoiOperation(roi,in1,in2,functional::Evaluate<BinaryFun>(f) ).Op(); }

}; // namespace core

}; // namespace daim



#endif // dmRoiAlgorithms_h

