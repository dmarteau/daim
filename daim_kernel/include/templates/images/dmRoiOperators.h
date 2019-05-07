#ifndef dmRoiOperators_h
#define dmRoiOperators_h

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
// File         : dmRoiOperators.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------
#include <algorithm>
#include <numeric>

//---------------------------------------------------------
// algorithm implementation as Function Objects
//---------------------------------------------------------

#define STD( f ) std::f 

namespace daim {

namespace core {

namespace functional {

template<class In,class F>
inline void __for_each(In __first, In __last, F& __f) {
  for ( ; __first != __last; ++__first)
    __f(*__first);
}

//---------------------------------------------------------
// For_Each ->  UnOp : x |-> UnOp(x) ; x E In;
//---------------------------------------------------------
template<class UnOp> struct For_Each {
  UnOp op;
  For_Each( const UnOp& _op ) : op(_op) {}
  
  template<class In> void operator()( In l, long x1, long x2 ) { 
     __for_each( *l+x1,*l+x2+1,op ); 
  }
  
  UnOp Op() const { return op; }
};
//---------------------------------------------------------
// Count ->  cnt = Card { x E In / x==value }
//---------------------------------------------------------
template<class T,class Size> struct Count {
  Size cnt;
  T    value;
  Count(const T& v) : value(v), cnt(0) {}
  
  template<class In> 
  Size operator()( In l, long x1, long x2 ) { return (cnt += STD(count)(*l+x1,*l+x2+1,value)); }
  
  Size count() const { return cnt; }
};
//---------------------------------------------------------
// Count_If -> cnt = Card { x E In / p(x)==true }
//---------------------------------------------------------
template<class Pred,class Size> struct Count_If {
  Pred p;
  Size cnt;
  Count_If( const Pred& _p ) : p(_p), cnt(0) {}

  template<class In> 
  Size operator()( In l, long x1, long x2 ) { return (cnt += STD(count_if)(*l+x1,*l+x2+1,p)); }
  
  Size count() const { return cnt; }
  Pred Op()    const { return p;   }
};
//---------------------------------------------------------
// Copy  -> {Out} = {In}
//---------------------------------------------------------
struct Copy {
  template<class In,class Out> 
  void operator()( In l, Out r, long x1, long x2 ) { 
    STD(copy)(*l+x1,*l+x2+1,*r+x1); 
  }
};
//---------------------------------------------------------
// Copy_If -> {Out} = { x E In / p(x,y)==true; }
//---------------------------------------------------------
template<class In,class Out,class Pred>
inline Out __copy_if( In first,In last,Out res, Pred& p) {
  while(first!=last) { if(p(*first,*res)) *res = *first; ++res;++first; }
  return res;
}
//
template<class Pred> struct Copy_If {
  Pred p;
  Copy_If( const Pred& _p ) : p(_p) {}
  
  template<class In,class Out> 
  void operator()( In l, Out r,long x1,long x2 ) { 
    __copy_if( *l+x1,*l+x2+1,*r+x1, p ); 
  }
  
  Pred Op() const { return p; }
};
//---------------------------------------------------------
// Transform   -> {Out} = { UnOp(x) ; x E In }
//---------------------------------------------------------
template<class UnOp> struct UnaryTransform {
  UnOp op;
  UnaryTransform( const UnOp& _op ) : op(_op) {}

  template<class In,class Out> 
  void operator()( In l,Out r, long x1, long x2 ) { 
     STD(transform)(*l+x1,*l+x2+1,*r+x1,op ); 
  }
  
  UnOp Op() const { return op; }
};
//---------------------------------------------------------
// Binary Transform ->  {Out} = { BinOp(x1,x2) ; x E In, x E In2 }
//---------------------------------------------------------
template<class BinOp> struct BinaryTransform {
  BinOp op;
  BinaryTransform( const BinOp& _op ) : op(_op) {}
  
  template<class In,class In2,class Out>
  void operator()( In l,In2 l2,Out r,long x1,long x2 )
  { STD(transform)( *l+x1,*l+x2+1,*l2+x1,*r+x1,op ); }
  
  BinOp Op() const { return op; }
};
//---------------------------------------------------------
// Replace -> x |-> new_val if x==val, x otherwise; x E In
//---------------------------------------------------------
template<class T> struct Replace {
  T val, new_val;
  Replace( const T& v, const T& new_v ) : val(v), new_val(new_v) {}
  
  template<class In>  void operator()( In l,long x1,long x2 ) { 
    STD(replace)( *l+x1,*l+x2+1,val,new_val ); 
  }
};
//---------------------------------------------------------
// Replace_if  x |-> new_val if Pred(x)==true, x otherwise; x E In
//---------------------------------------------------------
template<class Pred,class T> struct Replace_If {
  Pred p;
  T    val;
  Replace_If( const T& v, const Pred& _p) : p(_p), val(v) {}
  
  template<class In> void operator()( In l,long x1,long x2 ) {
     STD(replace_if)( *l+x1,*l+x2+1,p,val ); 
  }
  Pred Op() const { return p; }
};
//---------------------------------------------------------
// Replace_if2  x |-> true_val if Pred(x)==true, false_val otherwise; x E In
//---------------------------------------------------------
template<class In,class Out,class Pred,class T>
inline Pred __make_binary( In first,In last,Out res,const T& t_val,const T& f_val,Pred& p)
{
  while(first!=last) {  *res = (p(*first) ? t_val : f_val); ++res;++first; }
  return p;
}
//
template<class Pred,class T> struct Binarize {
  Pred p;
  T    t_val, f_val;
  Binarize( const T& _t,const T& _f, const Pred& _p) : p(_p), t_val(_t),f_val(_f) {}
  
  template<class In,class Out>
  void operator()( In l,Out r,long x1,long x2 ) { 
    p = __make_binary( *l+x1,*l+x2+1,*r+x1,t_val,f_val,p ); 
  }

  Pred Op() const { return p; }
};
//---------------------------------------------------------
// Replace_Copy  {Out} = { new_val if x==val, x otherwise; x E In }
//---------------------------------------------------------
template<class T> struct Replace_Copy {
  T  val, new_val;
  Replace_Copy( const T& v, const T& new_v) : val(v), new_val(new_v) {}
  
  template<class In,class Out>
  void operator()( In l,Out r,long x1,long x2 ) {
    STD(replace_copy)( *l+x1,*l+x2+1,*r+x1,val,new_val ); 
  }
};
//---------------------------------------------------------
// Replace_Copy_If {Out} = { val if Pred(x)==true, x otherwise; x E In }
//---------------------------------------------------------
template<class Pred,class T> struct Replace_Copy_If {
  Pred p;
  T    val;
  Replace_Copy_If( const T& v, const Pred& _p) : p(_p), val(v) {}
  
  template<class In,class Out>
  void operator()( In l,Out r,long x1,long x2 ) { 
    p =  STD(replace_copy_if)( *l+x1,*l+x2+1,*r+x1,p,val ); 
  }
  
  Pred Op() const { return p; }
};
//---------------------------------------------------------
// Fill  -> x |-> val; x E In
//---------------------------------------------------------
template<class T> struct Fill {
  T    val;
  Fill( const T& v ) : val(v) {}

  template<class In>
  void operator()( In l,long x1,long x2 ) { 
    STD(fill)( *l+x1,*l+x2+1,val ); 
  }
};
//---------------------------------------------------------
// Generate -> x |-> Gen(); x E In
//---------------------------------------------------------
template<class Gen> struct Generate {
  Gen g;
  Generate( const Gen& _g ) : g(_g) {}

  template<class In>  void operator()( In l,long x1,long x2 ) { 
    g = STD(generate)( *l+x1,*l+x2+1,g ); 
  }
  
  Gen Op() const { return g; }
};
//---------------------------------------------------------
template<class In,class Out,class BinaryFun>
void __combine( In first,In last, Out res, BinaryFun& f) {
  while(first!=last) { *res = f(*first,*res); res++; first++; }
}
//----------------------------------------------------------
template<class BinaryFun> struct Combine {
  BinaryFun f;
  Combine( const BinaryFun& _f) : f(_f) {}
  
  template<class In,class Out> void operator()( In l,Out r,long x1,long x2 ) { 
    __combine( *l+x1,*l+x2+1,*r+x1, f); 
  }
  BinaryFun Op() const { return f; }
};
//---------------------------------------------------------
template<class In,class In2,class Out,class TernaryFun>
void __combine3( In first,In last,In2 second,Out res, TernaryFun& f) {
  while(first!=last) { *res = f(*first,*second,*res); ++res; ++second; ++first; }
}
//----------------------------------------------------------
template<class TernaryFun> struct Combine3 {
  TernaryFun f;
  Combine3( const TernaryFun& _f) : f(_f) {}
  
  template<class In,class In2,class Out> 
  void operator()( In l,In2 l2,Out r,long x1,long x2 ) { 
    __combine3( *l+x1,*l+x2+1,*l2+x1,*r+x1, f); 
  }
  TernaryFun Op() const { return f; }
};
//---------------------------------------------------------
// Accumulate Sum( x ); x E In
//---------------------------------------------------------
template<class T> struct Accumulate {
  T val;
  Accumulate( T v) : val(v) {}
  
  template<class In> T operator()( In l,long x1,long x2 ) { 
    return (val = STD(accumulate)( *l+x1,*l+x2+1,val )); 
  }
  
  T value() const { return val; }
};
//---------------------------------------------------------
template<class T,class Pred> struct Accumulate_If {
  T    val;
  Pred p;
  Accumulate_If( const Pred& _p,const T& v) : val(v),p(_p) {}
  
  template<class In> T operator()( In l,long x1,long x2 ) { 
    return (val = STD(accumulate)( *l+x1,*l+x2+1,val,p )); 
  }
  
  Pred Op()  const { return p; }
  T value()  const { return val; }
};
//---------------------------------------------------------
// Swap : swap( {In}, {Out} )
//---------------------------------------------------------
struct Swap {
  template<class In,class Out> 
  void operator()( In l, Out r, long x1, long x2 ) { STD(swap_ranges)(*l+x1,*l+x2+1,*r+x1); }
};
//---------------------------------------------------------
// Evaluate : f( {In}, {Out} )
//---------------------------------------------------------
template<class In,class In2,class Out,class BinaryFun>
void __evaluate( In first,In2 last, Out second, BinaryFun& f) {
  while(first!=last) { f(*first,*second); ++second; ++first; }
}
//---------------------------------------------------------
template<class BinaryFun>
struct Evaluate {
  BinaryFun f;
  Evaluate( const BinaryFun& _f ) : f(_f) {}
  
  template<class In,class Out> void operator()( In l, Out r, long x1, long x2 ) { 
     __evaluate( *l+x1,*l+x2+1,*r+x1,f ); 
  }
  
  BinaryFun Op() const { return f; }
};

//---------------------------------------------------------

}; // namespace functional

}; // namespace core


}; // namespace daim

#undef STD

#endif // dmRoiOperators_h
