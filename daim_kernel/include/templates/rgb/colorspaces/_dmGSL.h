#ifndef _dmGSL_h
#define _dmGSL_h
//--------------------------------------------------------
// Copyright (c) 2004 David Marteau
// 
// File         : _dmGSL.h
// Date         : 12/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//
//--------------------------------------------------------

 //----------------------------
 // GSL Splitter 
 //----------------------------

 // The formula for these decomposition have been
 // Taken out from LTI Digital Image/Signal Processing Library, LTI, RWTH Aachen.
 // (Authors: Pablo Alvarado, Stefan Syberichs, Thomas Rusert)
 //
 // Citation from the original implementation:
 // 
 // Split image in its l1, l2 and l3 components, as described in 
 // T. Gevers and A. Smeulders "Color-based object recognition"
 // Pattern Recognition, Vol. 32, 1999, pp. 453-464.
 //
 // This color space is appropriate when trying to detect only 
 // material changes, but ignoring shadow, shape or highlight edges.
 //

  namespace GSL {

   //-------------------------------------------------------
   // Extractors
   //-------------------------------------------------------
   struct _getter {
     dm_float fr,fg,fb,fden;
     void _calc( const rgb_triple& tr ) {
       fr   = absdiff(tr.red  ,tr.green);
       fg   = absdiff(tr.red  ,tr.blue );
       fb   = absdiff(tr.green,tr.blue );
       fden = fr+fg+fb;
     }
   };

   template<int _Channel,class integer_type> struct getChannel {};

   struct channels {
     template<int _Channel,class integer_type>
     struct get : public getChannel<_Channel,integer_type> {};
   };

   template<> struct getChannel<1,integer_true> : public _getter {
     dm_uint8 operator()( const rgb_triple& tr ) {
       _calc(tr);
       return (fden > 0 ? 
         static_cast<dm_uint8>( daim::round( 255.0f*fr/fden) ) : 0);
     } 
   };
   template<> struct getChannel<2,integer_true> : public _getter {
     dm_uint8 operator()( const rgb_triple& tr ) {
       _calc(tr);
       return (fden > 0 ? 
         static_cast<dm_uint8>( daim::round( 255.0f*fg/fden) ) : 0);
     } 
   };
   template<> struct getChannel<3,integer_true> : public _getter {
     dm_uint8 operator()( const rgb_triple& tr ) {
       _calc(tr);
       return (fden > 0 ? 
         static_cast<dm_uint8>( daim::round( 255.0f*fb/fden) ) : 0);
     } 
   };

   template<> struct getChannel<1,integer_false> : public _getter {
     dm_float operator()( const rgb_triple& tr ) {
       _calc(tr);
       return (fden > 0 ? fr/fden : 0.0f);
     } 
   };
   template<> struct getChannel<2,integer_false> : public _getter {
     dm_float operator()( const rgb_triple& tr ) {
       _calc(tr);
       return (fden > 0 ? fg/fden : 0.0f);
     } 
   };
   template<> struct getChannel<3,integer_false> : public _getter {
     dm_float operator()( const rgb_triple& tr ) {
       _calc(tr);
       return (fden > 0 ? fb/fden : 0.0f);
     } 
   };

   //-------------------------------------------------------
   // Splitter
   //-------------------------------------------------------
   struct splitter {

    dm_float fr,fg,fb,fden;
     
     // version for integer type, 
     // return normalized value from 0 -> 255 
     void operator()( const rgb_triple& tr, dm_uint8& L1, dm_uint8& L2, dm_uint8& L3 ) 
     { 
       fr   = absdiff(tr.red  ,tr.green);
       fg   = absdiff(tr.red  ,tr.blue );
       fb   = absdiff(tr.green,tr.blue );
       fden = fr+fg+fb;

       if(fden > 0) { 
         L1 = static_cast<dm_uint8>( daim::round( 255.0f*fr/fden) );
         L2 = static_cast<dm_uint8>( daim::round( 255.0f*fg/fden) );
         L3 = static_cast<dm_uint8>( daim::round( 255.0f*fb/fden) );
       } else { L1 = L2 = L3 = 0; }
     }

     // return value from [-1..1] 
     void operator()( const rgb_triple& tr,  dm_float& L1, dm_float& L2, dm_float& L3 )
     {
       fr   = absdiff(tr.red  ,tr.green);
       fg   = absdiff(tr.red  ,tr.blue );
       fb   = absdiff(tr.green,tr.blue );
       fden = fr+fg+fb;

       if(fden > 0) { 
         L1 = fr/fden;
         L2 = fg/fden;
         L3 = fb/fden;
       } else { L1 = L2 = L3 = 0; }
     }
   };

  } // namespace GSL



#endif // _dmGSL_h
