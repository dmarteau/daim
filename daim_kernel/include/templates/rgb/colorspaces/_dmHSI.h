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
     dm_float mini,delta,fH,fS,fI;

     void getHSI255( const rgb_triple& tr, dm_float& H, dm_float& S, dm_float& I )
     {
        I    = 0.333333f * (static_cast<int>(tr.red)+tr.green+tr.blue);
        mini = minimum(tr.red,tr.green,tr.blue);
        if (I!=0) {
          S = 1.0f - (mini/I);
          if (S==0) {
            H=0;
          } else {
            delta = (I-mini)*6.0f;
            if (tr.red==I) {
              H = (tr.green-tr.blue)/delta;
            } else if (tr.green==I) {
              H = 0.333333f + (tr.blue-tr.red)/delta;
            } else {
              H = 0.666667f + (tr.red-tr.green)/delta;
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
        fI   = 0.333333f * (static_cast<int>(tr.red)+tr.green+tr.blue);
        mini = minimum(tr.red,tr.green,tr.blue);
        if (fI!=0) {
          fS = 1.0f - (mini/fI);
          if (fS==0)
            fH = 0;
          else {
            delta = (fI-mini)*6.0f;
            if (tr.red==fI) {
              fH = (tr.green-tr.blue)/delta;
            } else if (tr.green==fI) {
              fH = 0.333333f + (tr.blue-tr.red)/delta;
            } else {
              fH = 0.666667f + (tr.red-tr.green)/delta;
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
        fI   = 0.333333f * (static_cast<int>(tr.red)+tr.green+tr.blue);
        mini = minimum(tr.red,tr.green,tr.blue);
        if (fI!=0)  return 1.0f - (mini/fI);
        else        return 1.0f; // undefined
      }

      dm_float intensity( const rgb_triple& tr ) {
         return 0.333333f * (static_cast<int>(tr.red)+tr.green+tr.blue);
      }

     void operator()( const rgb_triple& tr, dm_float& H, dm_float& S, dm_float& I )
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
     dm_float f,fH,fS,fI;


     void setHSI255( rgb_triple& tr,
                     const dm_float& H, const dm_float& S, const dm_float& I )
     {
       i  = static_cast<int>(H*6);
       f  = H*6-i;

       #define r (I*(1.0f-S))
       #define q (I*(1.0f-(S*f)))
       #define t (I*(1.0f-(S*(1.0f-f))))
       #define v (I)

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
                      const dm_float& H, const dm_float& S, const dm_float& I )
     {
		if (S==0) {
          tr.red  = tr.blue = tr.green = to_rgb_channel(I);
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
          tr.red  = tr.blue = tr.green = I;
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
     dm_float operator()( const rgb_triple& tr ) { return  hue(tr); }
   };
   template<> struct getChannel<2,integer_false> : public splitter {
     dm_float operator()( const rgb_triple& tr ) { return saturation(tr); }
   };

   template<> struct getChannel<3,integer_false> : public splitter {
     dm_float operator()( const rgb_triple& tr ) { return intensity(tr)/255.0f; }
   };

 } // namespace HSI

#endif // _dmHSI_h
