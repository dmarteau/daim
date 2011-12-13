#ifndef _dmGSC_h
#define _dmGSC_h
//--------------------------------------------------------
// Copyright (c) 2004 David Marteau
// 
// File         : _dmGSC.h
// Date         : 12/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//
//--------------------------------------------------------

#include <math.h>

 //----------------------------
 // GSC Splitter 
 //----------------------------

 // The formula for these decomposition have been
 // taken out from LTI Digital Image/Signal Processing Library, LTI, RWTH Aachen.
 // (Authors: Pablo Alvarado, Stefan Syberichs, Thomas Rusert)
 //
 // Citation from the original implementation:
 // 
 //  Split image in its c1, c2 and c3 components, as described in 
 //  T. Gevers and A. Smeulders "Color-based object recognition"
 //  Pattern Recognition, Vol. 32, 1999, pp. 453-464.
 // 
 //  This color space is appropriate when trying to detect only highlight or
 //  material changes, but ignoring shadow or shape edges.
 // 
 // 
 //  They denote the angles of the body reflection vector and are invariant for
 //  matte, dull objects.

  namespace GSC {

   //-------------------------------------------------------
   // Extractors
   //-------------------------------------------------------
   template<int _Channel,class integer_type> struct getChannel {};

    struct channels {
     template<int _Channel,class integer_type>
     struct get : public getChannel<_Channel,integer_type> {};
   };

   static dm_real get_factor( const integer_true&   ) { return 2.0*255.0/acos(-1.0); }
   static dm_real get_factor( const integer_false&  ) { return 2.0/acos(-1.0); }

   template<> struct getChannel<1,integer_true> {
     dm_uint8 operator()( const rgb_triple& tr ) {
       static const dm_real factor =  get_factor(integer_true());
       return to_rgb_channel(factor*atan2(static_cast<dm_real>(tr.r),max(tr.g,tr.b)));
     } 
   };
   template<> struct getChannel<2,integer_true> {
     dm_uint8 operator()( const rgb_triple& tr ) {
       static const dm_real factor =  get_factor(integer_true());
       return to_rgb_channel(factor*atan2(static_cast<dm_real>(tr.g),max(tr.r,tr.b )));
     } 
   };
   template<> struct getChannel<3,integer_true> {
     dm_uint8 operator()( const rgb_triple& tr ) {
       static const dm_real factor =  get_factor(integer_true());
       return to_rgb_channel(factor*atan2(static_cast<dm_real>(tr.b),max(tr.r,tr.g)));
     } 
   };

   template<> struct getChannel<1,integer_false> {
     float operator()( const rgb_triple& tr ) {
       static const float factor =  static_cast<float>(get_factor(integer_false()));
       return factor*static_cast<float>(atan2(static_cast<dm_real>(tr.r),daim::max(tr.g,tr.b)));
     } 
   };
   template<> struct getChannel<2,integer_false> {
     float operator()( const rgb_triple& tr ) {
       static const float factor =  static_cast<float>(get_factor(integer_false()));
       return factor*static_cast<float>(atan2(static_cast<dm_real>(tr.g),daim::max(tr.r,tr.b)));
     } 
   };
   template<> struct getChannel<3,integer_false> {
     float operator()( const rgb_triple& tr ) {
       static const float factor =  static_cast<float>(get_factor(integer_false()));
       return factor*static_cast<float>(atan2(static_cast<dm_real>(tr.b),daim::max(tr.r,tr.g)));
     } 
   };

   //-------------------------------------------------------
   // Splitter
   //-------------------------------------------------------
   struct splitter {

     // version for integer type, 
     // return normalized value from 0 -> 255 
     void operator()( const rgb_triple& tr, dm_uint8& C1, dm_uint8& C2, dm_uint8& C3 ) 
     { 
       static const dm_real factor =  get_factor(integer_true());

       C1 = to_rgb_channel(factor*atan2(static_cast<dm_real>(tr.r),max(tr.g,tr.b)));
       C2 = to_rgb_channel(factor*atan2(static_cast<dm_real>(tr.g),max(tr.r,tr.b)));
       C3 = to_rgb_channel(factor*atan2(static_cast<dm_real>(tr.b),max(tr.r,tr.g)));
     }

     // return value from [-1..1] 
     void operator()( const rgb_triple& tr,  float& C1, float& C2, float& C3 )
     {
       static const float factor =  static_cast<float>(get_factor(integer_false()));

       C1 = factor*static_cast<float>(atan2(static_cast<dm_real>(tr.r),daim::max(tr.g,tr.b)));
       C2 = factor*static_cast<float>(atan2(static_cast<dm_real>(tr.g),daim::max(tr.r,tr.b)));
       C3 = factor*static_cast<float>(atan2(static_cast<dm_real>(tr.b),daim::max(tr.r,tr.g)));
     }
   };

  } // namespace GSC



#endif // _dmGSC_h
