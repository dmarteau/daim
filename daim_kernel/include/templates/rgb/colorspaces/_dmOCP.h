#ifndef _dmOCP_h
#define _dmOCP_h
//--------------------------------------------------------
// Copyright (c) 2004 David Marteau
// 
// File         : _dmOCP.h
// Date         : 12/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//
//--------------------------------------------------------

 //----------------------------
 // OCP  Splitter/Merger
 //----------------------------

 namespace OCP {

   // Definition:
   // RG =  (R - G)/2
   // BY =  B/2 - (R + G)/4
   // WB =  (R + G + B)/3

   //-------------------------------------------------------
   // Extractors
   //-------------------------------------------------------
   template<int _Channel,class integer_type> struct getChannel {};

   struct channels {
     template<int _Channel,class integer_type>
     struct get : public getChannel<_Channel,integer_type> {};
   };

   template<> struct getChannel<1,integer_true> {
     dm_uint8 operator()( const rgb_triple& tr ) {
       return to_rgb_channel(  0.5f  * (static_cast<float>(tr.red) - static_cast<float>(tr.green))  + 128.0f );
     } 
   };
   template<> struct getChannel<2,integer_true> {
     dm_uint8 operator()( const rgb_triple& tr ) {
       return to_rgb_channel( -0.25f * (static_cast<float>(tr.red) +  static_cast<float>(tr.green)) + 0.5f * tr.blue + 128.0f); 
     } 
   };
   template<> struct getChannel<3,integer_true> {
     dm_uint8 operator()( const rgb_triple& tr ) {
       return to_rgb_channel( (static_cast<float>(tr.red)+static_cast<float>(tr.green)+static_cast<float>(tr.blue)) * 0.3333333333333f); 
     } 
   };

   template<> struct getChannel<1,integer_false> {
     dm_float operator()( const rgb_triple& tr ) {
       return 0.00196078f  * (static_cast<dm_float>(tr.red) - static_cast<dm_float>(tr.green));
     } 
   };

   template<> struct getChannel<2,integer_false> {
     dm_float operator()( const rgb_triple& tr ) {
       return -0.000980392f * (static_cast<dm_float>(tr.red) + static_cast<dm_float>(tr.green)) + 0.00196078f * tr.blue;
     } 
   };

   template<> struct getChannel<3,integer_false> {
     dm_float operator()( const rgb_triple& tr ) {
       return to_rgb_channel( (static_cast<float>(tr.red)+static_cast<float>(tr.green)+static_cast<float>(tr.blue)) * 0.3333333333333f);
     } 
   };

   
   //-------------------------------------------------------
   // Splitter
   //-------------------------------------------------------
   struct splitter {

     // version for integer type, 
     // return normalized value from 0 -> 255 
     void operator()( const rgb_triple& tr, 
                      dm_uint8& RG, dm_uint8& BY, dm_uint8& WB ) 
     { 
       RG = to_rgb_channel(  0.5f  * (static_cast<float>(tr.red) - static_cast<float>(tr.green))  + 128.0f );
       BY = to_rgb_channel( -0.25f * (static_cast<float>(tr.red) +  static_cast<float>(tr.green)) + 0.5f * tr.blue + 128.0f); 
       WB = to_rgb_channel( (static_cast<float>(tr.red)+static_cast<float>(tr.green)+static_cast<float>(tr.blue)) * 0.3333333333333f); 
     }

     // normalized values in range [-1..1]
     void operator()( const rgb_triple& tr, 
                      dm_float& RG, dm_float& BY, dm_float& WB )
     {
       RG =  0.00196078f  * (static_cast<dm_float>(tr.red) - static_cast<dm_float>(tr.green));
       BY = -0.000980392f * (static_cast<dm_float>(tr.red) + static_cast<dm_float>(tr.green)) + 0.00196078f * tr.blue;
       WB =  0.00130719f  * (static_cast<dm_float>(tr.red) + static_cast<dm_float>(tr.green) + static_cast<dm_float>(tr.blue));
     }
   };

   //-------------------------------------------------------
   // Merger
   //-------------------------------------------------------
   struct merger {
     dm_float b,fRG,fBY,fWB;

     // expecting values in range [0..255]
     void operator()( rgb_triple& tr, 
                      const dm_uint8& RG, const dm_uint8& BY, const dm_uint8& WB ) 
     {
       fRG = static_cast<float>(RG) - 127.5f;
       fBY = static_cast<float>(BY) - 127.5f;
       fWB = static_cast<float>(WB);
        
       b = fBY * 0.666666666667f;
       tr.red   = to_rgb_channel(fWB + fRG - b);
       tr.green = to_rgb_channel(fWB - fRG - b);
       tr.blue  = to_rgb_channel(fWB + fBY*1.3333333333333f);
     }

     // expecting values in range [-1..1]
     void operator()( rgb_triple& tr, const dm_float& RG, const dm_float& BY, const dm_float& WB ) 
     {
       b = BY*0.666666666667f;
       tr.red   = to_rgb_channel(255.0f*(WB + RG - b));
       tr.green = to_rgb_channel(255.0f*(WB - RG - b));
       tr.blue  = to_rgb_channel(255.0f*(WB + BY*1.3333333333333f));
     }
   };

 } // namespace OCP

#endif // _dmOCP_h
