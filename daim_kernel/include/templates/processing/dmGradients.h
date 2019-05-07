#ifndef dmGradients_h
#define dmGradients_h

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
// File         : dmGradients.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

//--------------------------------------------------
// Compute gradients for input image
//--------------------------------------------------

#include <math.h>

namespace daim {


// Note : Could also be applied using convolution

struct _LinearGradientX
{
   template<class In,class Out>  
   void operator()( In _in, Out _out, long x1, long x2 ) {
     for(int i=x1; i<=x2; ++i )
      (*_out)[i] = (*_in)[i+1] - (*_in)[i-1];
   }
};

struct _LinearGradientY
{
   template<class In,class Out>  
   void operator()( In _in, Out _out, long x1, long x2 ) {
     for(long i=x1; i<=x2; ++i )
      (*_out)[i] = _in[1][i] - _in[-1][i];
   }
};

// Apply an unary operator to gradient along x and y coordinate

template<class BinOp> 
struct _LinearGradient
{
   BinOp op;
   _LinearGradient( const BinOp& _op ) : op(_op) {}

   template<class In,class Out>  
   void operator()( In _in, Out _out, long x1, long x2 ) {
     for(long i=x1; i<=x2; ++i ) 
      (*_out)[i] = op(  _in[0][i+1]  - _in[ 0][i-1],
                        _in[1][i]    - _in[-1][i]
                     );
   }

   BinOp Op() const { 
     return op; 
   }
};


template<class ROI,class In,class Out>
inline void _gradient_x( const ROI& roi, In in, Out out ) {
  core::RoiOperation(roi,in,out,_LinearGradientX());
} 

template<class ROI,class In,class Out>
inline void _gradient_y( const ROI& roi, In in, Out out ) {
  core::RoiOperation(roi,in,out,_LinearGradientY());
}

template<class ROI,class In,class Out,class BinOp>
inline BinOp _gradient( const ROI& roi, In in, Out out, BinOp& op ) {
  core::RoiOperation(roi,in,out,_LinearGradient<BinOp>(op)).Op();
}


//----------------------------------------------------------
// half norm operator
//----------------------------------------------------------
inline double half_norm_( double x, double y, integer_true ) {
  return sqrt(x*x+y*y)/2.0 + 0.5;
}

inline double half_norm_( double x, double y, integer_false ) {
  return  sqrt(x*x+y*y)/2.0;
}

template<class In,class Out>
struct half_norm : pixel_binary_function<In,In,Out>
{
  typedef typename type_tag<Out>::integer_type integer_type;
  
  Out operator()( const In& x, const In& y ) {
    return static_cast<Out>(
      half_norm_(x,y,integer_type())
    );
  }
};
//----------------------------------------------------------
// Implementation of a canny operator using linear gradients
//----------------------------------------------------------

// first define an operator that will generate a binary map
// holding object values if the input pixel gradient is positive 
// in the direction of the associated gradients, zero value otherwise
template<class T> 
struct _GradientDetection
{
   // In1 : map to computed gradient x replaced by output
   // In2 : map to computed gradient y
   // In3 : map to input

   template<class In1,class In2, class In3>  
   void operator()( In1 in1, In2 in2, In3 in3, long x1, long x2 ) 
   {
     for(long i=x1; i<=x2; ++i) 
     {
       (*in1)[i] = ((*in1)[i] * ( in3[0][i+1] - in3[0][i-1] ) +
                    (*in2)[i] * ( in3[1][i]   - in3[-1][i]  )) >= 0
                   ? pixel_traits<T>::object()
                   : pixel_traits<T>::zero();

     }
   }
};

// non maximal suppression : the process will create 
// 1 pixel wide gradient value lines

template<class T> 
struct _InnerNonMaximalSuppression
{
   template<class In,class Out>  
   void operator()( In in, Out out, long x1, long x2 ) 
   {
     for(long i=x1; i<=x2; ++i) 
     {
       if( (*in)[i] > 0) 
       {
         if( ( in[-1][i]   <= 0) || // N
             ( in[-1][i+1] <= 0) || // NE
             ( in[0][i+1]  <= 0) || // E
             ( in[1][i+1]  <= 0) || // SE
             ( in[1][i]    <= 0) || // S
             ( in[1][i-1]  <= 0) || // SW
             ( in[0][i-1]  <= 0) || // W
             ( in[-1][i-1] <= 0) ) continue;// NW  
	    } 
        (*out)[i] = pixel_traits<T>::zero();
     }
   }
};

template<class T> 
struct _OuterNonMaximalSuppression
{
   template<class In,class Out>  
   void operator()( In in, Out out, long x1, long x2 ) 
   {
     for(long i=x1; i<=x2; ++i) 
     {
       if( (*in)[i] <= 0) 
       {
         if( ( in[-1][i]   > 0) || // N
             ( in[-1][i+1] > 0) || // NE
             ( in[0][i+1]  > 0) || // E
             ( in[1][i+1]  > 0) || // SE
             ( in[1][i]    > 0) || // S
             ( in[1][i-1]  > 0) || // SW
             ( in[0][i-1]  > 0) || // W
             ( in[-1][i-1] > 0) ) continue; // NW
       }
       (*out)[i] = pixel_traits<T>::zero();
     }
   }
};

// Canny filter applied to rectangle for the input image in
// The process will create a two pixel wide garbage area due
// to the application of the gradient operator of size 3x3
// WARNING : no check is performed on input rectangle

template<class T,class _I>
struct _CannyFilter
{
 void operator()( const dmRect& _rect, image<T>& in, bool inner ) 
 {
   cont_image<_I> _out1(_rect);
   cont_image<_I> _out2(_rect);
 
   dmRect r = _rect;
   r.Resize(-1);
 
   _gradient_x(r,in.begin(r),_out1.begin(r));
   _gradient_y(r,in.begin(r),_out2.begin(r));

   core::transform(r,_out1.begin(r),_out2.begin(r),in.begin(r),half_norm<_I,T>());
  
   r.Resize(-1);
   core::RoiOperation(r,_out1.begin(r),_out2.begin(r),in.begin(r),_GradientDetection<_I>());

   if(inner)
     core::RoiOperation(r,_out1.begin(r),in.begin(r),_InnerNonMaximalSuppression<T>());  
   else
     core::RoiOperation(r,_out1.begin(r),in.begin(r),_OuterNonMaximalSuppression<T>());
 }
};

template<class T>
inline void canny_filter(const dmRect& _rect, image<T>& in, bool inner, integer_true ) {
  _CannyFilter<T,integer_holder> _filter;
  _filter(_rect,in,inner);
}

template<class T>
inline void canny_filter(const dmRect& _rect, image<T>& in, bool inner, integer_false ) {
  _CannyFilter<T,T> _filter;
  _filter(_rect,in,inner);
}

template<class T1,class T2> 
void gradient_filter( const dmRegion& rgn,const dmPoint& p,const image<T1>& in,image<T2>& out )
{ 
  RoiOperation(_LinearGradient< half_norm<double,T2> >(half_norm<double,T2>()),
               in,out,rgn,p);
}

template<class T1,class T2> 
void non_maximal_supression( const dmRegion& rgn,const dmPoint& p,
                             const image<T1>& in,image<T2>& out, bool _inner )
{
  if(_inner)
    RoiOperation(_InnerNonMaximalSuppression<T2>(),in,out,rgn,p);
  else
    RoiOperation(_OuterNonMaximalSuppression<T2>(),in,out,rgn,p);
}


}; //namespace daim

#endif //dmGradients_h
