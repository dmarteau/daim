#ifndef _dmRGB_h
#define _dmRGB_h
//--------------------------------------------------------
// Copyright (c) 2004 David Marteau
// 
// File         : _dmRGB.h
// Date         : 12/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//
//--------------------------------------------------------

 //----------------------------
 // RGB Splitter/Merger
 //----------------------------
 namespace RGB
 {   
   //-------------------------------------------------------
   // Extractors
   //-------------------------------------------------------
   template<int _Channel,class integer_type> struct getChannel {};

   struct channels {
     template<int _Channel,class integer_type>
     struct get : public getChannel<_Channel,integer_type> {};
   };

   template<> struct getChannel<1,integer_true>  {
     dm_uint8 operator()( const rgb_triple& tr ) { return tr.red; }
   };
   template<> struct getChannel<2,integer_true> {
     dm_uint8 operator()( const rgb_triple& tr ) { return tr.green; }
   };
   template<> struct getChannel<3,integer_true> {
     dm_uint8 operator()( const rgb_triple& tr ) { return tr.blue; }
   };

   template<> struct getChannel<1,integer_false> {
     dm_float operator()( const rgb_triple& tr ) { return tr.red/255.0f; }
   };
   template<> struct getChannel<2,integer_false> {
     dm_float operator()( const rgb_triple& tr ) { return tr.green/255.0f; }
   };
   template<> struct getChannel<3,integer_false> {
     dm_float operator()( const rgb_triple& tr ) { return tr.blue/255.0f; }
   };

   //-------------------------------------------------------
   // Splitter
   //-------------------------------------------------------
   struct splitter {
     void operator()( const rgb_triple& triple, dm_uint8& R, dm_uint8& G, dm_uint8& B ) 
     {
       R = triple.red  ;
       G = triple.green;
       B = triple.blue ;
     }

     void operator()( const rgb_triple& triple, dm_float& R, dm_float& G, dm_float& B ) 
     {
       R = triple.red  / 255.0f;
       G = triple.green/ 255.0f;
       B = triple.blue / 255.0f;
     }
   };

   //-------------------------------------------------------
   // Merger
   //-------------------------------------------------------
   struct merger {

     void operator()( rgb_triple& triple, 
                      const dm_float& R, const dm_float& G, const dm_float& B ) 
     {
       triple.red   = to_rgb_channel(R);
       triple.green = to_rgb_channel(G);
       triple.blue  = to_rgb_channel(B);
     }

     void operator()( rgb_triple& triple, 
                      const dm_uint8& R, const dm_uint8& G, const dm_uint8& B ) 
     {
       triple.red   = R;
       triple.green = G;
       triple.blue  = B;
     }
   
   };

  
  } // namespace RGB

#endif // _dmRGB_h
