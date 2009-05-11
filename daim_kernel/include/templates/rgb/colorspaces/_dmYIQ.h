#ifndef _dmYIQ_h
#define _dmYIQ_h
//--------------------------------------------------------
// Copyright (c) 2004 David Marteau
// 
// File         : _dmYIQ.h
// Date         : 12/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//
//--------------------------------------------------------

 //----------------------------
 // YIQ  Splitter/Merger
 //----------------------------
 namespace YIQ {

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
       return to_rgb_channel(0.299f * tr.red + 0.587f * tr.green + 0.114f * tr.blue);
     } 
   };
   // I
   template<> struct getChannel<2,integer_true> {
     dm_uint8 operator()( const rgb_triple& tr ) {
       return to_rgb_channel(0.500f * tr.red - 0.231f * tr.green - 0.269f * tr.blue + 127.5f);
     } 
   };
   // Q
   template<> struct getChannel<3,integer_true> {
     dm_uint8 operator()( const rgb_triple& tr ) {
      return to_rgb_channel(0.203f * tr.red - 0.500f * tr.green + 0.297f * tr.blue + 127.5f); 
     } 
   };

   // Y
   template<> struct getChannel<1,integer_false> {
     dm_float operator()( const rgb_triple& tr ) {
       return 0.299f * tr.red + 0.587f * tr.green + 0.114f * tr.blue;
     } 
   };
   // I
   template<> struct getChannel<2,integer_false> {
     dm_float operator()( const rgb_triple& tr ) {
       return 0.500f * tr.red - 0.231f * tr.green - 0.269f * tr.blue;
     } 
   };

   // Q
   template<> struct getChannel<3,integer_false> {
     dm_float operator()( const rgb_triple& tr ) {
       return 0.203f * tr.red - 0.500f * tr.green + 0.297f * tr.blue;
     } 
   };

   //-------------------------------------------------------
   // Splitter
   //-------------------------------------------------------
   struct splitter {
     // ranges : Y [0..1], I/Q [-0.5..+0.5]

     // version for integer type, 
     // return normalized value from 0 -> 255 
     void operator()( const rgb_triple& tr, 
                      dm_uint8& Y, dm_uint8& I, dm_uint8& Q ) 
     { 
       Y = to_rgb_channel(0.299f * tr.red + 0.587f * tr.green + 0.114f * tr.blue);
       I = to_rgb_channel(0.500f * tr.red - 0.231f * tr.green - 0.269f * tr.blue + 127.5f); 
       Q = to_rgb_channel(0.203f * tr.red - 0.500f * tr.green + 0.297f * tr.blue + 127.5f); 
     }

     // unormalized values
     void operator()( const rgb_triple& tr, 
                      dm_float& Y, dm_float& I, dm_float& Q )
     {
       Y = (0.299f * tr.red + 0.587f * tr.green + 0.114f * tr.blue)/255.0f;
       I = (0.500f * tr.red - 0.231f * tr.green - 0.269f * tr.blue)/255.0f;
       Q = (0.203f * tr.red - 0.500f * tr.green + 0.297f * tr.blue)/255.0f;
     }
   };

   //-------------------------------------------------------
   // Merger
   //-------------------------------------------------------
   struct merger {

     dm_float fY,fI,fQ;
     void operator()( rgb_triple& tr, 
                      const dm_uint8& Y, const dm_uint8& I, const dm_uint8& Q ) 
     {
       fY = static_cast<dm_float>(Y)/255.0f;
       fI = static_cast<dm_float>(I)/255.0f - 0.5f;  
       fQ = static_cast<dm_float>(Q)/255.0f - 0.5f;  

       tr.red   = to_rgb_channel(fY + 1.139f * fI + 0.648f * fQ);
       tr.green = to_rgb_channel(fY - 0.323f * fI - 0.677f * fQ);
       tr.blue  = to_rgb_channel(fY - 1.323f * fI + 1.785f * fQ);
     }

     void operator()( rgb_triple& tr, 
                      const dm_float& Y, const dm_float& I, const dm_float& Q ) 
     {
       tr.red   =  to_rgb_channel((Y + 1.139f * I + 0.648f * Q) * 255.0f);
       tr.green =  to_rgb_channel((Y - 0.323f * I - 0.677f * Q) * 255.0f);
       tr.blue  =  to_rgb_channel((Y - 1.323f * I + 1.785f * Q) * 255.0f);
     }
   };

  } // namespace YIQ


#endif // _dmYIQ_h
