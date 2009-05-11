#ifndef _dmHLS_h
#define _dmHLS_h
//--------------------------------------------------------
// Copyright (c) 2004 David Marteau
// 
// File         : _dmHLS.h
// Date         : 12/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//
//--------------------------------------------------------

 //----------------------------
 // HLS  Splitter/Merger
 //
 // Luminance is defined to be (min(R,G,B)+max(R,G,B))/2
 //
 //----------------------------
 namespace HLS {

   //-------------------------------------------------------
   // Splitter
   //-------------------------------------------------------

   // L will be returned as a value between 0 and 255
   // H and S will be returned as a value between 0 and 1
   struct splitter 
   {
     dm_float mini,maxi,delta,fH,fL,fS;

     void getHSL255( const rgb_triple& tr, dm_float& H, dm_float& S, dm_float& L ) 
     {
        maxi = maximum(tr.red, tr.green, tr.blue);
        mini = minimum(tr.red, tr.green, tr.blue);

        // Luminance
        delta = maxi-mini;
        L     = maxi+mini;

        if(delta == 0) {
          H  = 0;
          L /= 2;
          S  = 0;
        }
        else {

          // Saturation

          if(L<256.0f)
            S = delta / L;
          else
            S = delta/(510.0f-L);

          // Hue

          if (tr.red==maxi)
            H = (tr.green-tr.blue)/(delta*6.0f);
          else if (tr.green==maxi)
            H = 0.333333f + (tr.blue-tr.red)/(delta*6.0f);
          else
            H = 0.666667f + (tr.red-tr.green)/(delta*6.0f);
          if (H<0)
            H = H + 1.0f;

          L /= 2.0;
        }
     }

      dm_float hue( const rgb_triple& tr )
      { 
        maxi = maximum(tr.red, tr.green, tr.blue);
        mini = minimum(tr.red, tr.green, tr.blue);
        delta = maxi-mini;

        if(delta == 0)  fH  = 0;
        else {
          if (tr.red==maxi)
            fH = (tr.green-tr.blue)/(delta*6.0f);
          else if (tr.green==maxi)
            fH = 0.333333f + (tr.blue-tr.red)/(delta*6.0f);
          else
            fH = 0.666667f + (tr.red-tr.green)/(delta*6.0f);
          if (fH<0)
            fH = fH + 1.0f;
        }
        return fH;
      }

      dm_float luminance( const rgb_triple& tr ) 
      {  
        maxi = maximum(tr.red, tr.green, tr.blue);
        mini = minimum(tr.red, tr.green, tr.blue);
        return (maxi + mini)/2.0f;
      }

      dm_float saturation( const rgb_triple& tr ) 
      { 
        maxi  = maximum(tr.red, tr.green, tr.blue);
        mini  = minimum(tr.red, tr.green, tr.blue);
        delta = maxi-mini;
        fL    = maxi+mini;
        if(delta == 0)  fS  = 0;
        else {
          if(fL<256.0f)
            fS = delta / fL;
          else
            fS = delta/(510.0f-fL);
        }
        return fS;
      }

     void operator()( const rgb_triple& tr, dm_float& H, dm_float& L, dm_float& S ) 
     {
        getHSL255(tr,H,S,L);
        L /= 255.0f;
     }

     void operator()( const rgb_triple& tr,dm_uint8& H, dm_uint8& L, dm_uint8& S ) 
     {  
        getHSL255(tr,fH,fS,fL);
        H = to_rgb_channel(fH*255.0f);
        S = to_rgb_channel(fS*255.0f);
        L = to_rgb_channel(fL);
     }

   };

   //-------------------------------------------------------
   // Merger
   //-------------------------------------------------------
   struct merger 
   {
     int i; 
     dm_float f,fH,fS,fL;

     

     void setHSL255_low( rgb_triple& tr, 
                         const dm_float& H, const dm_float& S, const dm_float& L ) 
     {
       i = static_cast<int>(H*6.0f);
       f = (H*6.0f)-i;

       #define  v (L*(1.0f+S))
       #define  r (L*(1.0f-S))
       #define  q (L*(1.0f+S*(1.0f-2.0f*f)))
       #define  t (2*L-q)
          
       switch (i)
       {
         case 0:
         case 6:
              tr.red   = to_rgb_channel(v); 	
              tr.green = to_rgb_channel(t); 	
              tr.blue  = to_rgb_channel(r); 	
              break;

         case 1:
              tr.red   = to_rgb_channel(q); 	
              tr.green = to_rgb_channel(v); 	
              tr.blue  = to_rgb_channel(r); 	
              break;

         case 2:
              tr.red   = to_rgb_channel(r); 	
              tr.green = to_rgb_channel(v); 	
              tr.blue  = to_rgb_channel(t); 	
              break;

         case 3:
              tr.red   = to_rgb_channel(r); 	
              tr.green = to_rgb_channel(q); 	
              tr.blue  = to_rgb_channel(v); 	
              break;

         case 4:
              tr.red   = to_rgb_channel(t); 	
              tr.green = to_rgb_channel(r); 	
              tr.blue  = to_rgb_channel(v); 	
              break;

         case 5:
              tr.red   = to_rgb_channel(v); 	
              tr.green = to_rgb_channel(r); 	
              tr.blue  = to_rgb_channel(q); 	
              break;
       } // switch

       #undef r
       #undef q
       #undef t
       #undef v

     }


     void setHSL255_high( rgb_triple& tr, 
                          const dm_float& H, const dm_float& S, const dm_float& L ) 
     {
       i = static_cast<int>(H*6.0f);
       f = (H*6.0f)-i;

       #define v (L*(1.0f-S) + S * 255.0f)
       #define r (L*(1.0f+S) - S * 255.0f)
       #define q (S*(255.0f-L)*(1.0f-2.0f*f)+L)
       #define t (2.0f*L-q)
          
       switch (i)
       {
         case 0:
         case 6:
              tr.red   = to_rgb_channel(v); 	
              tr.green = to_rgb_channel(t); 	
              tr.blue  = to_rgb_channel(r); 	
              break;

         case 1:
              tr.red   = to_rgb_channel(q); 	
              tr.green = to_rgb_channel(v); 	
              tr.blue  = to_rgb_channel(r); 	
              break;

         case 2:
              tr.red   = to_rgb_channel(r); 	
              tr.green = to_rgb_channel(v); 	
              tr.blue  = to_rgb_channel(t); 	
              break;

         case 3:
              tr.red   = to_rgb_channel(r); 	
              tr.green = to_rgb_channel(q); 	
              tr.blue  = to_rgb_channel(v); 	
              break;

         case 4:
              tr.red   = to_rgb_channel(t); 	
              tr.green = to_rgb_channel(r); 	
              tr.blue  = to_rgb_channel(v); 	
              break;

         case 5:
              tr.red   = to_rgb_channel(v); 	
              tr.green = to_rgb_channel(r); 	
              tr.blue  = to_rgb_channel(q); 	
              break;
       } // switch

       #undef r
       #undef q
       #undef t
       #undef v

     }


     // Assume that values are between 0 and 1.0;
     void operator()( rgb_triple& tr, 
                      const dm_float& H, const dm_float& L, const dm_float& S ) 
     {
		if (S==0) {
          tr.red  = tr.blue = tr.green = to_rgb_channel(L);
		}
		else { 
          fL = L * 255.0f;
          fH = H;
          fS = S;
          if (L<=127.5f)
             setHSL255_low(tr,fH,fS,fL);
          else
             setHSL255_high(tr,fH,fS,fL);
		}
     }

     // Assume that values are between 0 and 255;
     void operator()( rgb_triple& tr, 
                      const dm_uint8& H, const dm_uint8& L, const dm_uint8& S ) 
     {
		if (S==0) {
          tr.red  = tr.blue = tr.green = L;
		}
		else { 
          fL = L; // let L unormalized
          fH = H/255.0f;
          fS = S/255.0f;
          if (L<=127.5f)
             setHSL255_low(tr,fH,fS,fL);
          else
             setHSL255_high(tr,fH,fS,fL);
		}
      }

   };

   //-------------------------------------------------------
   // Extractors
   //-------------------------------------------------------
   template<int _Channel,class integer_type> struct getChannel {};

   struct channels {
     template<int _Channel,class integer_type>
     struct get : public getChannel<_Channel,integer_type> {};
   };

   template<> struct getChannel<1,integer_true> : public splitter {
     dm_uint8 operator()( const rgb_triple& tr ) { 
        return to_rgb_channel(hue(tr)*255.0f); }
   };
   template<> struct getChannel<2,integer_true> : public splitter {
     dm_uint8 operator()( const rgb_triple& tr ) {
        return to_rgb_channel(luminance(tr)); 
     }
   };
   template<> struct getChannel<3,integer_true> : public splitter {
     dm_uint8 operator()( const rgb_triple& tr ) { 
        return to_rgb_channel(saturation(tr)*255.0f); 
     }
   };

   template<> struct getChannel<1,integer_false> : public splitter {
     dm_float operator()( const rgb_triple& tr ) { return  hue(tr); }
   };
   template<> struct getChannel<2,integer_false> : public splitter {
     dm_float operator()( const rgb_triple& tr ) { 
        return luminance(tr)/255.0f; }
   };
   template<> struct getChannel<3,integer_false> : public splitter {
     dm_float operator()( const rgb_triple& tr ) { return saturation(tr); }
   };

 } // namespace HLS

#endif // _dmHLS_h
