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
     float mini,maxi,delta,fH,fL,fS;

     void getHSL255( const rgb_triple& tr, float& H, float& S, float& L ) 
     {
        maxi = maximum(tr.r, tr.g, tr.b);
        mini = minimum(tr.r, tr.g, tr.b);

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

          if (tr.r==maxi)
            H = (tr.g-tr.b)/(delta*6.0f);
          else if (tr.g==maxi)
            H = 0.333333f + (tr.b-tr.r)/(delta*6.0f);
          else
            H = 0.666667f + (tr.r-tr.g)/(delta*6.0f);
          if (H<0)
            H = H + 1.0f;

          L /= 2.0;
        }
     }

     float hue( const rgb_triple& tr )
      { 
        maxi = maximum(tr.r, tr.g, tr.b);
        mini = minimum(tr.r, tr.g, tr.b);
        delta = maxi-mini;

        if(delta == 0)  fH  = 0;
        else {
          if (tr.r==maxi)
            fH = (tr.g-tr.b)/(delta*6.0f);
          else if (tr.g==maxi)
            fH = 0.333333f + (tr.b-tr.r)/(delta*6.0f);
          else
            fH = 0.666667f + (tr.r-tr.g)/(delta*6.0f);
          if (fH<0)
            fH = fH + 1.0f;
        }
        return fH;
      }

     float luminance( const rgb_triple& tr ) 
      {  
        maxi = maximum(tr.r, tr.g, tr.b);
        mini = minimum(tr.r, tr.g, tr.b);
        return (maxi + mini)/2.0f;
      }

     float saturation( const rgb_triple& tr ) 
      { 
        maxi  = maximum(tr.r, tr.g, tr.b);
        mini  = minimum(tr.r, tr.g, tr.b);
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

     void operator()( const rgb_triple& tr, float& H, float& L, float& S ) 
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
     float f,fH,fS,fL;

     

     void setHSL255_low( rgb_triple& tr, 
                         const float& H, const float& S, const float& L ) 
     {
       i = static_cast<int>(H*6.0f);
       f = (H*6.0f)-i;

       #define  vv (L*(1.0f+S))
       #define  rr (L*(1.0f-S))
       #define  qq (L*(1.0f+S*(1.0f-2.0f*f)))
       #define  tt (2*L-qq)
          
       switch (i)
       {
         case 0:
         case 6:
              tr.r = to_rgb_channel(vv);
              tr.g = to_rgb_channel(tt); 	
              tr.b = to_rgb_channel(rr); 	
              break;

         case 1:
              tr.r = to_rgb_channel(qq);
              tr.g = to_rgb_channel(vv); 	
              tr.b = to_rgb_channel(rr); 	
              break;

         case 2:
              tr.r = to_rgb_channel(rr);
              tr.g = to_rgb_channel(vv); 	
              tr.b = to_rgb_channel(tt); 	
              break;

         case 3:
              tr.r = to_rgb_channel(rr);
              tr.g = to_rgb_channel(qq); 	
              tr.b = to_rgb_channel(vv); 	
              break;

         case 4:
              tr.r = to_rgb_channel(tt);
              tr.g = to_rgb_channel(rr);
              tr.b = to_rgb_channel(vv); 	
              break;

         case 5:
              tr.r = to_rgb_channel(vv);
              tr.g = to_rgb_channel(rr); 	
              tr.b = to_rgb_channel(qq); 	
              break;
       } // switch

       #undef rr
       #undef qq
       #undef tt
       #undef vv

     }


     void setHSL255_high( rgb_triple& tr, 
                          const float& H, const float& S, const float& L ) 
     {
       i = static_cast<int>(H*6.0f);
       f = (H*6.0f)-i;

       #define vv (L*(1.0f-S) + S * 255.0f)
       #define rr (L*(1.0f+S) - S * 255.0f)
       #define qq (S*(255.0f-L)*(1.0f-2.0f*f)+L)
       #define tt (2.0f*L-qq)
          
       switch (i)
       {
         case 0:
         case 6:
              tr.r = to_rgb_channel(vv); 	
              tr.g = to_rgb_channel(tt); 	
              tr.b = to_rgb_channel(rr); 	
              break;

         case 1:
              tr.r = to_rgb_channel(qq); 	
              tr.g = to_rgb_channel(vv); 	
              tr.b = to_rgb_channel(rr); 	
              break;

         case 2:
              tr.r = to_rgb_channel(rr); 	
              tr.g = to_rgb_channel(vv); 	
              tr.b = to_rgb_channel(tt); 	
              break;

         case 3:
              tr.r = to_rgb_channel(rr); 	
              tr.g = to_rgb_channel(qq); 	
              tr.b = to_rgb_channel(vv); 	
              break;

         case 4:
              tr.r = to_rgb_channel(tt); 	
              tr.g = to_rgb_channel(rr); 	
              tr.b = to_rgb_channel(vv); 	
              break;

         case 5:
              tr.r = to_rgb_channel(vv); 	
              tr.g = to_rgb_channel(rr); 	
              tr.b = to_rgb_channel(qq); 	
              break;
       } // switch

       #undef rr
       #undef qq
       #undef tt
       #undef vv

     }


     // Assume that values are between 0 and 1.0;
     void operator()( rgb_triple& tr, 
                      const float& H, const float& L, const float& S ) 
     {
		if (S==0) {
          tr.r  = tr.b = tr.g = to_rgb_channel(L);
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
          tr.r  = tr.b = tr.g = L;
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
     float operator()( const rgb_triple& tr ) { return  hue(tr); }
   };
   template<> struct getChannel<2,integer_false> : public splitter {
     float operator()( const rgb_triple& tr ) { 
        return luminance(tr)/255.0f; }
   };
   template<> struct getChannel<3,integer_false> : public splitter {
     float operator()( const rgb_triple& tr ) { return saturation(tr); }
   };

 } // namespace HLS

#endif // _dmHLS_h
