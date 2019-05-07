#ifndef _dmHSI_h
#define _dmHSI_h
//--------------------------------------------------------
// Copyright (c) 2004 David Marteau
//
// File         : _dmHSI.h
// Date         : 12/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//
//--------------------------------------------------------

 //----------------------------
 // HSI  Splitter/Merger
 //
 // Value is defined to be (R+G+B)/3
 //----------------------------
 namespace HSI {

   //-------------------------------------------------------
   // Splitter
   //-------------------------------------------------------
   struct splitter
   {
     float mini,delta,fH,fS,fI;

     void getHSI255( const rgb_triple& tr, float& H, float& S, float& I )
     {
        I    = 0.333333f * (static_cast<int>(tr.r)+tr.g+tr.b);
        mini = minimum(tr.r,tr.g,tr.b);
        if (I!=0) {
          S = 1.0f - (mini/I);
          if (S==0) {
            H=0;
          } else {
            delta = (I-mini)*6.0f;
            if (tr.r==I) {
              H = (tr.g-tr.b)/delta;
            } else if (tr.g==I) {
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

     float hue( const rgb_triple& tr )
      {
        fI   = 0.333333f * (static_cast<int>(tr.r)+tr.g+tr.b);
        mini = minimum(tr.r,tr.g,tr.b);
        if (fI!=0) {
          fS = 1.0f - (mini/fI);
          if (fS==0)
            fH = 0;
          else {
            delta = (fI-mini)*6.0f;
            if (tr.r==fI) {
              fH = (tr.g-tr.b)/delta;
            } else if (tr.g==fI) {
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

     float saturation( const rgb_triple& tr )
      {
        fI   = 0.333333f * (static_cast<int>(tr.r)+tr.g+tr.b);
        mini = minimum(tr.r,tr.g,tr.b);
        if (fI!=0)  return 1.0f - (mini/fI);
        else        return 1.0f; // undefined
      }

     float intensity( const rgb_triple& tr ) {
         return 0.333333f * (static_cast<int>(tr.r)+tr.g+tr.b);
      }

     void operator()( const rgb_triple& tr, float& H, float& S, float& I )
     {
        getHSI255(tr,H,S,I);
        I /= 255.0f;
     }

     void operator()( const rgb_triple& tr,
                      dm_uint8& H, dm_uint8& S, dm_uint8& I )
     {
        getHSI255(tr,fH,fS,fI);
        H = to_rgb_channel(fH*255.0f);
        S = to_rgb_channel(fS*255.0f);
        I = to_rgb_channel(fI);
     }

   };

   //-------------------------------------------------------
   // Merger
   //-------------------------------------------------------
   struct merger
   {
     int i;
     float f,fH,fS,fI;


     void setHSI255( rgb_triple& tr,
                     const float& H, const float& S, const float& I )
     {
       i  = static_cast<int>(H*6);
       f  = H*6-i;

       #define rr (I*(1.0f-S))
       #define qq (I*(1.0f-(S*f)))
       #define tt (I*(1.0f-(S*(1.0f-f))))
       #define vv (I)

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
                      const float& H, const float& S, const float& I )
     {
		if (S==0) {
          tr.r = tr.b = tr.g = to_rgb_channel(I);
		}
		else {
          fI = I * 255.0f;
          fH = H;
          fS = S;
          setHSI255(tr,fH,fS,fI);
		}
     }

     // Assume that values are between 0 and 255;
     void operator()( rgb_triple& tr,
                      const dm_uint8& H, const dm_uint8& S, const dm_uint8& I )
     {
		if (S==0) {
          tr.r = tr.b = tr.g = I;
		}
		else {
          fI = I; // let I unormalized
          fH = H/255.0f;
          fS = S/255.0f;
          setHSI255(tr,fH,fS,fI);
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
        return to_rgb_channel(intensity(tr));
     }
   };

   template<> struct getChannel<1,integer_false> : public splitter {
     float operator()( const rgb_triple& tr ) { return  hue(tr); }
   };
   template<> struct getChannel<2,integer_false> : public splitter {
     float operator()( const rgb_triple& tr ) { return saturation(tr); }
   };

   template<> struct getChannel<3,integer_false> : public splitter {
     float operator()( const rgb_triple& tr ) { return intensity(tr)/255.0f; }
   };

 } // namespace HSI

#endif // _dmHSI_h
