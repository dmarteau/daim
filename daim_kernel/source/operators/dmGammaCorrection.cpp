#define dmUseKernelImageTemplates
#define dmUseKernelBuffer
#include "daim_kernel.h"
#include "common/dmUserLib.h"

#include <math.h>
#include "templates/processing/dmArithmetics.h"
#include "templates/processing/dmDensityMap.h"


using namespace daim;

//----------------------------------------------------------------------
// Gamma Correction Mapping
//------------------------------------------------------------------------
struct __dm_impl_gamma
{
  dmBufferParameters& Params;
  dm_real             MinRange;
  dm_real             MaxRange;
  dm_real             Gamma;

  __dm_impl_gamma(dmBufferParameters& _Params,
                  dm_real _MinRange,
                  dm_real _MaxRange,
                  dm_real _Gamma )
  :Params(_Params)
  ,MinRange(_MinRange)
  ,MaxRange(_MaxRange)
  ,Gamma(_Gamma)
  {}

  //-------------------------------------------------------
  // Generic operation on scalar
  //-------------------------------------------------------
  template<EPixelFormat _PixelFormat>
  void operator()( dmIImage<_PixelFormat>& _img )
  {
    typedef typename dmIImage<_PixelFormat>::traits_type traits_type;
    typedef typename dmIImage<_PixelFormat>::image_type  image_type;

    typedef typename traits_type::value_type   value_type;
    typedef typename traits_type::integer_type integer_type;

     gap<value_type> _range(
        _get_range_value(MinRange,traits_type(),integer_type()),
        _get_range_value(MaxRange,traits_type(),integer_type())
     );

    image_type& _image = _img.Gen();

    dmRegion reg;
    if(_range.upper <= _range.lower)
    {
      reg    = Params.thisRegion; // Get the min and the max for the region
      _range = minmax(_image.rect(),_image);
    }
    else if(_range.upper < traits_type::max() &&   // Don't scan if we have
            _range.lower > traits_type::min())     // the full pixel range
    {
      // Create pixel range region
      create_rgnroi(_image,between<value_type>(_range.min(),_range.max()),
                    reg,Params.thisRegion);
    }

    gamma_transform(reg,_image,_range.min(),_range.max(),Gamma);
  }


  void operator()( dmIImage<dmPixelFormat24bppRGB>&  _img )
  {
    dmColorIndexTable ctable;
    gamma_correction(ctable,Gamma);

    // Compute a gamma correction

    dmRGBColorTable rgb_table;
    unsigned char   value;

    for(int i=0;i<dmLUT8_MAX_COLORS;++i)
    {
      value = static_cast<unsigned char>(ctable[i]);
      rgb_table[i].r = value;
      rgb_table[i].g = value;
      rgb_table[i].b = value;
    }

    apply_map(_img.Gen(),Params.thisRegion,rgb_table);
  }


  void operator()( dmIImage<dmPixelFormat8bppIndexed>&  _img )
  {
    // compute a gamma map correction
    dmColorIndexTable ctable;
    gamma_correction(ctable,Gamma);
    apply_map(_img.Gen(),Params.thisRegion,ctable);
  }
};
//----------------------------------------------------------------------
bool dmGammaCorrection::Apply( dmBufferParameters& _Params )
{
  __dm_impl_gamma _filter(_Params,
                          this->_MinRange,this->_MaxRange,
                          this->_Gamma);

  return dmImplementOperation(_filter,_Params.thisImage);
}
//------------------------------------------------------------------------
