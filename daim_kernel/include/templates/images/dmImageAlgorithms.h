#ifndef dmImageAlgorithms_h
#define dmImageAlgorithms_h

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
// File         : dmImageAlgorithms.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

namespace daim {

//--------------------------------------------------------------
// define unary operators on a image (act on rectangular area)
//--------------------------------------------------------------
template<class Op,class T> Op apply( const image<T>& im, Op op )
{ return core::RoiOperation(im.rect(),im.begin(),op); }
//--------------------------------------------------------------
template<class Pr,class T> size_t count_if(const image<T>& im,Pr op,size_t& res) 
{ return core::count_if(im.rect(),im.begin(),op,res); }
//--------------------------------------------------------------
template<class Op,class T> Op for_each(image<T>& im,Op op) 
{ return core::for_each (im.rect(),im.begin(),op); }
//--------------------------------------------------------------
template<class Op,class T> Op for_each(const image<T>& im,Op op) 
{ return core::for_each (im.rect(),im.begin(),op); }
//--------------------------------------------------------------
template<class Op,class T> Op transform(image<T>& im,Op op) 
{ return core::transform(im.rect(),im.begin(),im.begin(),op); }
//--------------------------------------------------------------
template<class Op,class T> Op generate (image<T>& im,Op op) 
{ return core::generate(im.rect(),im.begin(),op);}
//--------------------------------------------------------------
template<class X,class T> void replace(image<T>& im,X val,X new_val)
{ core::replace(im.rect(),im.begin(),val,new_val); }
//--------------------------------------------------------------
template<class Pr,class X,class T> Pr replace_if(image<T>& im,Pr op,X val) 
{ return core::replace_if(im.rect(),im.begin(),op,val); }
//--------------------------------------------------------------
template<class X,class T> X accumulate(const image<T>& im,X v )
{ return core::accumulate(im.rect(),im.begin(),v); }
//--------------------------------------------------------------
template<class X,class T,class Op> X accumulate_if(const image<T>& im,X v,Op op)
{ return core::accumulate_if(im.rect(),im.begin(),v,op ); }
//--------------------------------------------------------------
template<class X,class T> void fill( image<T>& im,X v )
{ core::fill(im.rect(),im.begin(),v ); }
//--------------------------------------------------------------
// define unary operators on a image (act on region)
// Warning : No check is done on the region 
//--------------------------------------------------------------
template<class ROI,class Op,class T> Op apply( const ROI& rgn, const image<T>& im, Op op )
{ return core::RoiOperation(rgn,im.begin(rgn.Rectangle()),op); }
//--------------------------------------------------------------
template<class ROI,class Pr,class T> size_t count_if(const ROI& rgn, const image<T>& im,Pr op,size_t& res) 
{ return core::count_if(rgn,im.begin(rgn.Rectangle()),op,res); }
//--------------------------------------------------------------
template<class ROI,class Op,class T> Op for_each(const ROI& rgn, image<T>& im,Op op) 
{ return core::for_each (rgn,im.begin(rgn.Rectangle()),op); }
//--------------------------------------------------------------
template<class ROI,class Op,class T> Op for_each(const ROI& rgn, const image<T>& im,Op op) 
{ return core::for_each (rgn,im.begin(rgn.Rectangle()),op); }
//--------------------------------------------------------------
template<class ROI,class Op,class T> Op transform(const ROI& rgn, image<T>& im,Op op) 
{ return core::transform(rgn,im.begin(rgn.Rectangle()),im.begin(rgn.Rectangle()),op); }
//--------------------------------------------------------------
template<class ROI,class Op,class T> Op generate (const ROI& rgn, image<T>& im,Op op) 
{ return core::generate(rgn,im.begin(rgn.Rectangle()),op);}
//--------------------------------------------------------------
template<class ROI,class X,class T> void replace(const ROI& rgn, image<T>& im,X val,X new_val)
{ core::replace(rgn,im.begin(),val,new_val); }
//--------------------------------------------------------------
template<class ROI,class Pr,class X,class T> Pr replace_if(const ROI& rgn, image<T>& im,Pr op,X val) 
{ return core::replace_if(rgn,im.begin(rgn.Rectangle()),op,val); }
//--------------------------------------------------------------
template<class ROI,class X,class T> X accumulate(const ROI& rgn, const image<T>& im,X v )
{ return core::accumulate(rgn,im.begin(rgn.Rectangle()),v); }
//--------------------------------------------------------------
template<class ROI,class X,class T,class Op> X accumulate_if(const ROI& rgn, const image<T>& im,X v,Op op)
{ return core::accumulate_if(rgn,im.begin(rgn.Rectangle()),v,op ); }
//--------------------------------------------------------------
template<class ROI,class X,class T> void fill( const ROI& rgn, image<T>& im,X v )
{ core::fill(rgn,im.begin(rgn.Rectangle()),v ); }
//-------------------------------------------------------------
// Functional binding for binary operators
// Check are performed on the region 
//-------------------------------------------------------------
#define ROI dmRegion

template<class In,class Out>
inline void copy( const ROI& roi, const dmPoint& p,const image<In>& in, image<Out>& out ) 
{ RoiOperation(core::functional::Copy(),in,out,roi,p); }

template<class In,class Out,class Pred>
inline Pred copy_if( const ROI& roi, const dmPoint& p, const image<In>& in, image<Out>& out, const Pred& pr)
{ return RoiOperation(core::functional::Copy_If<Pred>(pr),in,out,roi,p).Op(); }

template<class In,class Out,class UnOp>
inline UnOp transform( const ROI& roi, const dmPoint& p,const image<In>& in, image<Out>& out ,const UnOp& op)
{ return RoiOperation(core::functional::UnaryTransform<UnOp>(op),in,out,roi,p).Op(); }

template<class In,class Out,class Pred,class T>
inline Pred make_binary( const ROI& roi, const dmPoint& p,const image<In>& in, image<Out>& out, const Pred& pr, const T& t_val, const T& f_val )
{ return RoiOperation(core::functional::Binarize<Pred,T>(t_val,f_val,pr),in,out,roi,p).Op(); }

template<class In,class Out,class T>
inline  void replace_copy( const ROI& roi, const dmPoint& p,const image<In>& in, image<Out>& out, const T& val,const T& new_val)
{ RoiOperation(core::functional::Replace_Copy<T>(val,new_val),in,out,roi,p); }

template<class In,class Out,class Pred,class T>
inline Pred replace_copy_if( const ROI& roi, const dmPoint& p,const image<In>& in, image<Out>& out, const Pred& pr, const T& val )
{ return RoiOperation(core::functional::Replace_If<Pred,T>(val,pr),in,out,roi,p).Op(); }

template<class In,class Out>
inline void swap( const ROI& roi, const dmPoint& p,image<In>& in, image<Out>& out )
{ RoiOperation(core::functional::Swap(),in,out,roi,p); }

template<class In,class Out,class BinaryFun>
inline BinaryFun combine( const ROI& roi, const dmPoint& p,const image<In>& in, image<Out>& out, const BinaryFun& f )
{ return RoiOperation(core::functional::Combine<BinaryFun>(f),in,out,roi,p).Op(); }

template<class In,class Out,class BinaryFun>
inline BinaryFun evaluate( const ROI& roi, const dmPoint& p,const image<In>& in, image<Out>& out, const BinaryFun& f )
{ return RoiOperation(core::functional::Evaluate<BinaryFun>(f),in,out,roi,p).Op(); }

//-------------------------------------------------------------
// Ternary operators
//-------------------------------------------------------------

template<class In1,class In2,class Out,class TernaryFun>
inline TernaryFun combine( const ROI& roi, const dmPoint& p,
                const image<In1>& in1, 
                const image<In2>& in2, 
                image<Out>& out, const TernaryFun& f )
{
  return RoiOperation(core::functional::Combine3<TernaryFun>(f),
                      const_cast< image<In1>& >(in1), 
                      const_cast< image<In2>& >(in2),
                      out,roi,p).Op(); 
}

template<class In1,class In2,class Out,class BinaryFun>
inline BinaryFun transform( const ROI& roi, const dmPoint& p,
          const image<In1>& in1,
          const image<In2>& in2, 
          image<Out>& out, const BinaryFun& f )
{
 return RoiOperation(core::functional::BinaryTransform<BinaryFun>(f),
                 const_cast< image<In1>& >(in1),
                 const_cast< image<In2>& >(in2),
                 out,roi,p).Op();
}
//-------------------------------------------------------------
  
#undef ROI

}; // namespace daim

#endif // dmImageAlgorithms_h
