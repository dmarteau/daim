#define dmUserIncludes
#define dmUseKernelTemplates
#include "daim_kernel.h"
#include "templates/processing/_dmRGBHistogram.h"

namespace daim {
//-----------------------------------------------------------------------------------------
static void __cleardata( histogram_value* _hr,histogram_value* _sg, histogram_value* _vb )
{
  if(_hr)  std::fill(_hr,_hr+dmLUT8_MAX_COLORS,0);
  if(_sg)  std::fill(_sg,_sg+dmLUT8_MAX_COLORS,0);
  if(_vb)  std::fill(_vb,_vb+dmLUT8_MAX_COLORS,0);
}
//-----------------------------------------------------------------------------------------
void rgb_histogram_( const image<dm_rgb24>& img,const dmRegion& rgn,
                     histogram_value* _r,histogram_value* _g, histogram_value* _b )
{
  __cleardata(_r,_g,_b);
  image<dm_rgb24>::const_line_type line = img.begin(rgn.Rectangle());
  daim::_for_each(rgn,line,_GetRGBHistogram(_r,_g,_b)); 
}
//-----------------------------------------------------------------------------------------
void hsi_histogram_( const image<dm_rgb24>& img,const dmRegion& rgn,
                     histogram_value* _h,histogram_value* _s, histogram_value* _v )
{
  __cleardata(_h,_s,_v);
  image<dm_rgb24>::const_line_type line = img.begin(rgn.Rectangle());
  daim::_for_each(rgn,line,_GetHSIHistogram(_h,_s,_v)); 
}
//-----------------------------------------------------------------------------------------
void val_histogram_( const image<dm_uint8>& img,const dmRegion& rgn, histogram_value* _h )
{
  __cleardata(_h,NULL,NULL);
  image<dm_uint8>::const_line_type line = img.begin(rgn.Rectangle());
  daim::_for_each(rgn,line,_Get8bitsHistogram(_h));   
}
//-----------------------------------------------------------------------------------------
}; // namespace daim