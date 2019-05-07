#ifndef _dmYUV_h
#define _dmYUV_h
//--------------------------------------------------------
// Copyright (c) 2004 David Marteau
// 
// File         : _dmYUV.h
// Date         : 12/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//
//--------------------------------------------------------

 //----------------------------
 // YUV  Splitter/Merger
 //----------------------------
 namespace YUV {

   //-------------------------------------------------------
   // Extractors
   //-------------------------------------------------------
   template<int _Channel,class integer_type> struct getChannel {};

   struct channels {
     template<int _Channel,class integer_type>
     struct get : public getChannel<_Channel,integer_type> {};
   };

   // Y
   template<> struct getChannel<1,integer_true> {
     dm_uint8 operator()( const rgb_triple& tr ) {
       return to_rgb_channel( 0.256635f*tr.r + 0.503971f*tr.g + 0.098218f*tr.b +  16.0f);
     } 
   };
   // U
   template<> struct getChannel<2,integer_true> {
     dm_uint8 operator()( const rgb_triple& tr ) {
       return to_rgb_channel(-0.148135f*tr.r - 0.290902f*tr.g + 0.439036f*tr.b + 128.0f);
     } 
   };
   // V
   template<> struct getChannel<3,integer_true> {
     dm_uint8 operator()( const rgb_triple& tr ) {
       return to_rgb_channel( 0.439327f*tr.r - 0.367673f*tr.g - 0.071655f*tr.b + 128.0f);
     } 
   };

   // Y
   template<> struct getChannel<1,integer_false> {
     float operator()( const rgb_triple& tr ) {
       return 0.256635f * tr.r + 0.503971f * tr.g + 0.098218f * tr.b +  16.0f;
     } 
   };
   // U
   template<> struct getChannel<2,integer_false> {
     float operator()( const rgb_triple& tr ) {
       return -0.148135f * tr.r - 0.290902f * tr.g + 0.439036f * tr.b + 128.0f;
     } 
   };
   // V
   template<> struct getChannel<3,integer_false> {
     float operator()( const rgb_triple& tr ) {
       return 0.439327f * tr.r - 0.367673f * tr.g - 0.071655f * tr.b + 128.0f;
     } 
   };

   
   //-------------------------------------------------------
   // Splitter
   //-------------------------------------------------------
   struct splitter {

     // ranges: Y [16..235], U/V [16..240]
     void operator()( const rgb_triple& tr, dm_uint8& Y, dm_uint8& U, dm_uint8& V ) 
     { 
       Y = to_rgb_channel( 0.256635f*tr.r + 0.503971f*tr.g + 0.098218f*tr.b +  16.0f);
       U = to_rgb_channel(-0.148135f*tr.r - 0.290902f*tr.g + 0.439036f*tr.b + 128.0f); 
       V = to_rgb_channel( 0.439327f*tr.r - 0.367673f*tr.g - 0.071655f*tr.b + 128.0f); 
     }

     // specialize for float
     void operator()( const rgb_triple& tr, 
                      float& Y, float& U, float& V ) 
     {
       Y =  0.256635f * tr.r + 0.503971f * tr.g + 0.098218f * tr.b +  16.0f;
       U = -0.148135f * tr.r - 0.290902f * tr.g + 0.439036f * tr.b + 128.0f; 
       V =  0.439327f * tr.r - 0.367673f * tr.g - 0.071655f * tr.b + 128.0f;
     }


   };

   // TODO: checking these values

   //-------------------------------------------------------
   // Merger
   //-------------------------------------------------------
   struct merger {
     template<class T>
     void operator()( rgb_triple& tr, 
                      const T& Y, const T& U, const T& V )
     {
       tr.r = to_rgb_channel(Y + 1.402f   * (V-128.0f));
       tr.g = to_rgb_channel(Y - 0.34414f * (U-128.0f) - 0.71414f * (V-128.0f));
       tr.b = to_rgb_channel(Y + 1.772f   * (U-128.0f));
     }
   };

  } // namespace YUV



#endif // _dmYUV_h
