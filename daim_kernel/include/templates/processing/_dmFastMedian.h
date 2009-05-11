#ifndef _dmFastMedian_h
#define _dmFastMedian_h

/* ::: BEGIN LICENSE BLOCK:::
 *
 * Copyright (c) 2006 David Marteau
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

//----------------------------------------------------------------------------------------
/*! \file _dmFastMedian.h
 *  \date 7/2006 
 *  \author David Marteau 
 *
 * Fast median implementation for 8 bits and 16 bits greyscale
 * images, this is a variation of the algorithm given in 
 * "Image Processing, Analysis, and Machine Vision, sd. edition" 
 * M.Sonka,V.Hlavac,R.Boyle, PWS publishing, pp75 
 */
//----------------------------------------------------------------------------------------
 
namespace daim {

  namespace filter 
  {
    template<class K,class argument_type,class result_type,int N> 
    struct medianFunctor
    {
     typedef K  kernel_type;
     
     const K& _k;

     int _stride ;
     int w,h,th;
     
     int* H;
   
     medianFunctor( const K& _kernel, int* storage  ) 
     :_k(_kernel)
     {
       w = _k.width();
       h = _k.height();
       
       th = ((w*h)+1) / 2; 
       
       H = storage;
      
     }
      
     template<class In,class Out>
     void operator()( In _in, Out _out, long x1, long x2 )
     {
       _stride  = daim::line_stride(_in);

       int i,j,pg,lt_med,med;

       _in = _in - _k.y();
  
       const argument_type *_corner;
  
       // COMPILER: Seems that optimization of this piece of code
       // has to be done on a per machine/compiler basis.

       _corner = *_in + (x1 - _k.x()); 

       // Clear data
       for(pg=0;pg<N;++pg)
           H[pg] = 0;

       for(j=h;--j>=0;) {
          for(i=0;i<w;++i) {
             ++H[_corner[i]];
          }
          _corner = daim::const_offset_ptr(_corner,_stride);
       }
       
       med = lt_med = 0;
       
       // Find median from histogram
       for(;;) {
         lt_med += H[med];
         if(lt_med >= th)
            break;
         ++med;
       }
              
       for(long x=x1;x<x2;++x) 
       {
         (*_out)[x] = static_cast<result_type>(med);

         _corner = *_in + (x-_k.x());

         for(j=h;--j>=0;) 
         {
           if((pg = _corner[0])<=med)
               --lt_med;     
           --H[pg];

           if((pg = _corner[w])<=med)
               ++lt_med;      
           ++H[pg]; 
           
           _corner = daim::const_offset_ptr(_corner,_stride);
         }

         if(lt_med<th) {
           do {
             lt_med += H[++med];
           } while( lt_med < th );
         } 
         
         else 
   
         for(;;) {
           pg = lt_med - H[med];
           if(pg < th )
              break;                   
           lt_med = pg;
           --med;           
         }
       } 

       // Add last value 
       (*_out)[x2] = static_cast<result_type>(med);
     }  
  };

  template<class K, class R> 
  medianFunctor<K,dm_uint8,R,0xFF>
  getMedianFunctor8bits( const K& _k, const pixel_traits<R>&, int* storage ) {
    return medianFunctor<K,dm_uint8,R,0xFF>(_k,storage);
  }
    
  template<class K,class R> 
  medianFunctor<K,dm_uint16,R,0xFFFF>
  getMedianFunctor16bits( const K& _k, const pixel_traits<R>&, int* storage  ) {
    return medianFunctor<K,dm_uint16,R,0xFFFF>(_k,storage);
  }


  } // namespace filter
} // namespace daim



#endif // _dmFastMedian_h
