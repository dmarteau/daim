#ifndef _dmHSV_h
#define _dmHSV_h
//--------------------------------------------------------
// Copyright (c) 2004 David Marteau
// 
// File         : _dmHSV.h
// Date         : 12/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//
//--------------------------------------------------------

 //----------------------------
 // HSV  Splitter/Merger
 //
 // Value is defined to be maximum(R,G,B)
 //----------------------------
 namespace HSV {

   //-------------------------------------------------------
   // Splitter
   //-------------------------------------------------------
   struct splitter 
   {
     dm_float mini,delta,fH,fS,fV;

     void getHSV255( const rgb_triple& tr, dm_float& H, dm_float& S, dm_float& V ) 
     {
        V    = maximum(tr.r,tr.g,tr.b);
        mini = minimum(tr.r,tr.g,tr.b);
        if (V!=0) {
          S = 1.0f - (mini/V);
          if (S==0) {
            H=0;
          } else {
            delta = (V-mini)*6.0f;
            if (tr.r==V) {
              H = (tr.g-tr.b)/delta;
            } else if (tr.g==V) {
              H = 0.333333f + (tr.b-tr.r)/delta;
            } else {
              H = 0.666667f + (tr.r-tr.g)/delta;
            }
            if (H<0) {
              H = H + 1.0f;
            }
          }
        } else {
          S = 1.0f;
          H = 0;
        }
     }

      dm_float hue( const rgb_triple& tr )
      { 
        fV   = maximum(tr.r,tr.g,tr.b);
        mini = minimum(tr.r,tr.g,tr.b);
        if (fV!=0) {
          fS = 1.0f - (mini/fV);
          if (fS==0) 
            fH = 0;
          else {
            delta = (fV-mini)*6.0f;
            if (tr.r==fV) {
              fH = (tr.g-tr.b)/delta;
            } else if (tr.g==fV) {
              fH = 0.333333f + (tr.b-tr.r)/delta;
            } else {
              fH = 0.666667f + (tr.r-tr.g)/delta;
            }
            if (fH<0) {
              fH = fH + 1.0f;
            }
          }
        } else 
          fH = 0;

        return fH;
      }

      dm_float saturation( const rgb_triple& tr ) 
      { 
        fV   = maximum(tr.r,tr.g,tr.b);
        mini = minimum(tr.r,tr.g,tr.b);
        if (fV!=0)  return 1.0f - (mini/fV);
        else        return 1.0f;
      }

      dm_uint8 value( const rgb_triple& tr ) {  
         return maximum(tr.r,tr.g,tr.b);
      }

     void operator()( const rgb_triple& tr, dm_float& H, dm_float& S, dm_float& V ) 
     {
        getHSV255(tr,H,S,V);
        V /= 255.0f;
     }

     void operator()( const rgb_triple& tr, 
                      dm_uint8& H, dm_uint8& S, dm_uint8& V ) 
     {  
        getHSV255(tr,fH,fS,fV);
        H = to_rgb_channel(fH*255.0f);
        S = to_rgb_channel(fS*255.0f);
        V = to_rgb_channel(fV);
     }

   };

   //-------------------------------------------------------
   // Merger
   //-------------------------------------------------------
   struct merger 
   {
     int i; 
     dm_float f,fH,fS,fV;


     void setHSV255( rgb_triple& tr, 
                     const dm_float& H, const dm_float& S, const dm_float& V ) 
     {
       i  = static_cast<int>(H*6);
       f  = H*6-i;

       #define rr (V*(1.0f-S))
       #define qq (V*(1.0f-(S*f)))
       #define tt (V*(1.0f-(S*(1.0f-f))))
       #define vv (V)

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
                      const dm_float& H, const dm_float& S, const dm_float& V ) 
     {
		if (S==0) {
          tr.r = tr.b = tr.g = to_rgb_channel(V);
		}
		else { 
          fV = V * 255.0f;
          fH = H;
          fS = S;
          setHSV255(tr,fH,fS,fV);
		}
     }

     // Assume that values are between 0 and 255;
     void operator()( rgb_triple& tr, 
                      const dm_uint8& H, const dm_uint8& S, const dm_uint8& V ) 
     {
		if (S==0) {
          tr.r = tr.b = tr.g = V;
		}
		else { 
          fV = V; // let V unormalized
          fH = H/255.0f;
          fS = S/255.0f;
          setHSV255(tr,fH,fS,fV);
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
        return to_rgb_channel(saturation(tr)*255.0f); 
     }
   };
   template<> struct getChannel<3,integer_true> : public splitter {
     dm_uint8 operator()( const rgb_triple& tr ) {
        return value(tr); 
     }
   };

   template<> struct getChannel<1,integer_false> : public splitter {
     dm_float operator()( const rgb_triple& tr ) { return  hue(tr); }
   };
   template<> struct getChannel<2,integer_false> : public splitter {
     dm_float operator()( const rgb_triple& tr ) { return saturation(tr); }
   };

   template<> struct getChannel<3,integer_false> : public splitter {
     dm_float operator()( const rgb_triple& tr ) { 
        return static_cast<dm_float>(value(tr))/255.0f; }
   };

 } // namespace HSV

#endif // _dmHSV_h
