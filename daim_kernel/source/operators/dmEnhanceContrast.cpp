#define dmUseKernelImageTemplates
#define dmUseKernelBuffer
#include "daim_kernel.h"
#include "common/dmUserLib.h"

#include <math.h>
#include "templates/processing/dmArithmetics.h"
#include "templates/processing/dmDensityMap.h"


//=====================================================
namespace daim
{
   template<class T>
   struct _Contrast
   {
     typedef pixel_traits<T>           traits_type;
     typedef typename traits_type::value_type   value_type;
     typedef typename traits_type::integer_type integer_type;

     dm_real rmin;
     dm_real RMAX;
     dm_real C,B,X;

     _Contrast( dm_real _rmin,  dm_real _RMAX, dm_real aC , dm_real aB, dm_real aX )
     :rmin(_rmin)
     ,RMAX(_RMAX)
     ,C(aC)
     ,B(aB)
     ,X(aX)
     {}

     template<class In,class Out>
     void operator()( In _in, Out _out, long x1, long x2 )
     {
       dm_real u ;

       if( C >= 1.0 )
       {
         // B = RMAX - B;
         // X = RMAX/(2.0*C);

         if( B <= X )                          // __F( C*(u-2.0*B)+RMAX )
         {
            for(long x=x1; x<=x2; ++x ) {
              u = static_cast<dm_real>( (*_in)[x] ) -  rmin;
              u = C*(u-2.0*B)+RMAX;
              if(u<=0)    u = rmin;  else
              if(u>=RMAX) u = RMAX;
              (*_out)[x] = _round_value(u+rmin,traits_type(),integer_type());
            }
         }
         else if( B >= RMAX - X )              // __F( C*(u+RMAX-2.0*B) )
         {
            for(long x=x1; x<=x2; ++x ) {
              u = static_cast<dm_real>( (*_in)[x] ) -  rmin;
              u = C*(u+RMAX-2.0*B);
              if(u<=0)    u = rmin;  else
              if(u>=RMAX) u = RMAX;
              (*_out)[x] = _round_value(u+rmin,traits_type(),integer_type());
            }
         }
         else                                    //  __F( C*(u-B)+ RMAX/2.0f )
         {
            for(long x=x1; x<=x2; ++x ) {
              u = static_cast<dm_real>( (*_in)[x] ) -  rmin;
              u = C*(u-B)+ RMAX/2.0;
              if(u<=0)    u = rmin;  else
              if(u>=RMAX) u = RMAX;
              (*_out)[x] = _round_value(u+rmin,traits_type(),integer_type());
            }
         }
       }
       else
       {
         // X = RMAX*C/2.0;

         if( B >= RMAX - X )                    // __F( C*u-RMAX+2.0*B )
         {
            for(long x=x1; x<=x2; ++x ) {
              u = static_cast<dm_real>( (*_in)[x] ) -  rmin;
              u = C*u-RMAX+2.0*B;
              if(u<=0)    u = rmin;  else
              if(u>=RMAX) u = RMAX;
              (*_out)[x] = _round_value(u+rmin,traits_type(),integer_type());
            }
         }
         else if( B <= X )                      //  __F( C*(u-RMAX)+2.0*B )
         {
            for(long x=x1; x<=x2; ++x ) {
              u = static_cast<dm_real>( (*_in)[x] ) -  rmin;
              u = C*(u-RMAX)+2.0*B;
              if(u<=0)    u = rmin;  else
              if(u>=RMAX) u = RMAX;
              (*_out)[x] = _round_value(u+rmin,traits_type(),integer_type());
            }
         }
         else {                                 //   __F( C*(u-RMAX/2.0)+B )
            for(long x=x1; x<=x2; ++x ) {
              u = static_cast<dm_real>( (*_in)[x] ) -  rmin;
              u = C*(u-RMAX/2.0)+B;
              if(u<=0)    u = rmin;  else
              if(u>=RMAX) u = RMAX;
              (*_out)[x] = _round_value(u+rmin,traits_type(),integer_type());
            }
         }
       }
     }
  };


  //create a density mapping for
  //linear contrast/luminosity transformation


  template<class T>
  void enhance_contrast( const dmRegion& rgn, const dmPoint&  pt,
                         const daim::image<T>& img, daim::image<T>& dst,
                         dm_real rmin  , dm_real rmax,
                         dm_real bright, dm_real contr )
  {
    typedef typename daim::image<T>::value_type value_type;

    dm_real RMAX = rmax - rmin;
    dm_real X,B  = bright , C = contr;

    if( C >= 100.0 ) {  // special case
      B = RMAX - B;     // Thresholding with value B

      // if(x<=B) x = rmin; } else
      // if(x>B)  x = rmax; }
      if(bright <= 0 || bright >= RMAX ) {
         dmRegion dstRgn = rgn;
         dstRgn.OffsetRoi(pt);
         dstRgn.ClipToRect(dst.rect());
         daim::fill(dstRgn,dst,static_cast<value_type>(bright+rmin));
      } else
        daim::make_binary(rgn,pt,img,dst,
            std::bind2nd(std::less_equal<value_type>(),static_cast<value_type>(B+rmin)),
            static_cast<value_type>(rmin),
            static_cast<value_type>(rmax));

      return;
    }

    C /= (100.0-C);

    if( C >= 1.0 ) {
      B = RMAX - B;
      X = RMAX/(2.0*C);
    }
    else {
      X = RMAX*C/2.0;
    }

    RoiOperation(_Contrast<T>(rmin,RMAX,C,B,X),img,dst,rgn,pt);
  }


} // namespace daim


using namespace daim;

//----------------------------------------------------------------------
// Constrast enhancement
//------------------------------------------------------------------------
struct __dm_impl_constrast
{
  dmBufferParameters& Params;
  dm_real             MinRange;
  dm_real             MaxRange;
  dm_real             Brightness;
  dm_real             Contrast;
  dm_bool             UseBuffer;

  __dm_impl_constrast(dmBufferParameters& _Params,
                  dm_real _MinRange,
                  dm_real _MaxRange,
                  dm_real _Brightness,
                  dm_real _Contrast,
                  dm_bool _UseBuffer)
  :Params(_Params)
  ,MinRange(_MinRange)
  ,MaxRange(_MaxRange)
  ,Brightness(_Brightness)
  ,Contrast(_Contrast)
  ,UseBuffer(_UseBuffer)
  {}

  //-------------------------------------------------------
  // Generic operation on scalar
  //-------------------------------------------------------
  template<EPixelFormat _PixelFormat>
  void operator()( dmIImage<_PixelFormat>& _Image )
  {
    typedef typename dmIImage<_PixelFormat>::traits_type traits_type;
    typedef typename dmIImage<_PixelFormat>::image_type  image_type;

    typedef typename traits_type::value_type   value_type;
    typedef typename traits_type::integer_type integer_type;

    gap<value_type> _range(
       _get_range_value(MinRange,traits_type(),integer_type()),
       _get_range_value(MaxRange,traits_type(),integer_type())
    );

    image_type* in;
    image_type* out = &_Image.Gen();

    const dmRegion* rgn;
    dmPoint         src;

    if(UseBuffer)
    {
      dmImageBuffer& _buffer = Params.thisBuffer;

      in  = &dmIImage<_PixelFormat>::Cast(_buffer.Buffer())->Gen();

      rgn = &_buffer.BufferRgn();
      src =  _buffer.BufferSrc();
    }
    else
    {
      in  = out;
      rgn = &Params.thisRegion;
      src = rgn->Rectangle().TopLeft();
    }

    if(_range.upper <= _range.lower)
       _range = minmax(*rgn,*in); // Get the min and the max for the region

    dm_real Br = _range.upper * Brightness - _range.lower;
    dm_real Co = 100.0 * Contrast ;

    enhance_contrast(*rgn,src,*in,*out,_range.min(),_range.max(),Br,Co);

  }

  void operator()( dmIImage<dmPixelFormat24bppRGB>&  _Image )
  {
    typedef dmIImage<dmPixelFormat24bppRGB>::image_type  image_type;
    typedef dmIImage<dmPixelFormat24bppRGB>::traits_type traits_type;

    dm_uint16 Br = static_cast<dm_uint16>(255.0 * Brightness + 0.5);
    dm_uint16 Co = static_cast<dm_uint16>(100.0 * Contrast   + 0.5);

    dmColorIndexTable ctable;
    enhance_contrast(ctable,Br,Co);

    if(UseBuffer)
    {
      dmImageBuffer& _buffer = Params.thisBuffer;

      image_type& in  = dmIImage<dmPixelFormat24bppRGB>::Cast(_buffer.Buffer())->Gen();
      daim::copy(_buffer.BufferRgn(),_buffer.BufferSrc(),in,_Image.Gen());
    }

    // Compute a brigthness/contrast map correction

    dmRGBColorTable rgb_table;
    unsigned char   value;

    for(int i=0;i<dmLUT8_MAX_COLORS;++i)
    {
      value = static_cast<unsigned char>(ctable[i]);
      rgb_table[i].r = value;
      rgb_table[i].g = value;
      rgb_table[i].b = value;
    }

    apply_map(_Image.Gen(),Params.thisRegion,rgb_table);

  }

  void operator()( dmIImage<dmPixelFormat8bppIndexed>&  _Image )
  {
    typedef dmIImage<dmPixelFormat8bppIndexed>::image_type  image_type;
    typedef dmIImage<dmPixelFormat8bppIndexed>::traits_type traits_type;

    dm_uint16 Br = static_cast<dm_uint16>(255.0 * Brightness + 0.5);
    dm_uint16 Co = static_cast<dm_uint16>(100.0 * Contrast   + 0.5);

    // Compute a brigthness/contrast map correction

    dmColorIndexTable ctable;
    enhance_contrast(ctable,Br,Co);

    if(UseBuffer)
    {
      dmImageBuffer& _buffer = Params.thisBuffer;

      image_type& in  = dmIImage<dmPixelFormat8bppIndexed>::Cast(_buffer.Buffer())->Gen();
      daim::copy(_buffer.BufferRgn(),_buffer.BufferSrc(),in,_Image.Gen());
    }

    apply_map(_Image.Gen(),Params.thisRegion,ctable);
  }
};
//----------------------------------------------------------------------
bool dmEnhanceContrast::Apply( dmBufferParameters& _Params )
{
  __dm_impl_constrast _filter(_Params,
                          this->_MinRange,this->_MaxRange,
                          daim::range(this->_Brightness,0.0,1.0),
                          daim::range(this->_Contrast  ,0.0,1.0),
                          _UseBuffer);
  if(_UseBuffer)
  {
    // Ensure that buffer has the same type has the input image
    if(_Params.thisBuffer.IsEmpty() ||
       _Params.thisBuffer.Buffer()->PixelFormat()!=_Params.thisImage.PixelFormat())
      return false;
  }

  return dmImplementOperation(_filter,_Params.thisImage);
}
//------------------------------------------------------------------------
