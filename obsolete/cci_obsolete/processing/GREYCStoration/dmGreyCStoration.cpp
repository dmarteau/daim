
/* ::: BEGIN LICENSE BLOCK:::
 *
 * Copyright (c) 2004-2005 David Marteau
 *
 * This file is part of the DAIM Image Processing Library (DAIM library)
 *
 * The DAIM library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License (LGPL)
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * The DAIM library is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the DAIM library; see the file LICENSE.  If
 * not, write to the Free Software Foundation, Inc., 59 Temple Place -
 * Suite 330, Boston, MA 02111-1307, USA.
 *
 *  ::: END LICENSE BLOCK::: */


using namespace daim;

#define cimg_display_type  0     // Disable display
#define cimg_debug         0     // Disable modal window in CImg exceptions.
#define cimg_plugin           "daimCImg.h"
#define greycstoration_plugin "daimCImgPlugins.h"

#include "CImg.h"
#include "greycstoration.h"

using namespace cimg_library;

#include "common/dmUserLib.h"

struct __dm_impl_greycstoration
{
  dmRegion&       _Rgn;
  const dmRegion* _Mask;
  dmIUnknown&     _Progress;
  _Instance_Ptr   _This; 
  dm_uint         _Operation;

  GREYCStoration greycs;

  __dm_impl_greycstoration( dmRegion& Rgn, const dmRegion* Mask, 
                            dmIUnknown& Progress,_Instance_Ptr Instance,
                            const dm_uint Operation)
  : _Rgn(Rgn)
  , _Mask(Mask)
  , _This(Instance)
  , _Progress(Progress)
  , _Operation(Operation)
   {
     greycs._Instance = _This;
     greycs._Progress = &_Progress;

     // InitMask
     if(_Operation == GREYCStoration::inpaint && _Mask!=NULL) 
     {    
       const dmRect& _Rect = _Rgn.Rectangle();

       greycs.mask.assign(_Rect.Width(),_Rect.Height(),1,1,0);
       greycs.set_inpaint_parameters(_This->inpaint);
       
       dmImageData _MaskData;
       _MaskData.PixelFormat = dmPixelFormat8bppIndexed;
      
       greycs.mask.getImageData(_MaskData);
       dmIImage<dmPixelFormat8bppIndexed> _mask2(_MaskData);      
  
       _mask2.Fill(*_Mask,255);
     }
   } 

  void _Apply(  CImg<float>& img , const unsigned int operation ) 
  {
    greycs.nb_iter = _This->nb_iter;
    greycs.setprogress(0,0);
    
    const float dt             = static_cast<float>(_This->dt);
    const float p              = static_cast<float>(_This->p);
    const float anisotropy     = static_cast<float>(_This->anisotropy);
    const float alpha          = static_cast<float>(_This->alpha);
    const float sigma          = static_cast<float>(_This->sigma);
    const float gauss_prec     = static_cast<float>(_This->gauss_prec);
    const float dl             = static_cast<float>(_This->dl);
    const float da             = static_cast<float>(_This->da);   
    const unsigned int interp  = _This->interp;
    const bool fast_approx     = _This->fast_approx;
    const float sdt            = static_cast<float>(_This->sdt);
    const float sp             = static_cast<float>(_This->sp);
 
    greycs.greycstoration(img,operation,dt,p,anisotropy,alpha,sigma,
                          fast_approx,gauss_prec,dl,da,interp,sdt,sp);
  }
  
  template<EPixelFormat _PixelFormat>
  void operator()( dmIImage<_PixelFormat>& _Img ) 
  { 
    typedef typename dmIImage<_PixelFormat>::value_type  value_type;
  
    const unsigned int operation = GREYCStoration::restore;
    const dmRect& _Rect = _Rgn.Rectangle();

    CImg<float> img(_Rect.Width(),_Rect.Height());

    dmImageData _CImgData;
    _CImgData.PixelFormat = dmPixelFormat32bppFloat;
 
    img.getImageData(_CImgData);
    
    // Create a shared Daim image
    dmIImage<dmPixelFormat32bppFloat> _Src(_CImgData);    
    _Src.GetCopy(_Img,_Rect,dmPoint(0,0));
 
    _Apply(img,_Operation);
    
    // Copy back data to image
    const dmPoint _SrcPt = _Rect.TopLeft(); _Rgn.OffsetRoi();
    daim::truncate(_Rgn,_SrcPt,_Src.Gen(),_Img.Gen());   
  }
  
  // Specialize for RGB
  void operator()( dmIImage<dmPixelFormat24bppRGB>& _Img ) 
  {
    const dmRect& _Rect = _Rgn.Rectangle();

    CImg<float> img(_Rect.Width(),_Rect.Height(),1,3);

    dmImageData _CImgData;
    _CImgData.PixelFormat = dmPixelFormat32bppFloat;

    // Create a shared Daim image
    img.getImageData(_CImgData,0,0); dmIImage<dmPixelFormat32bppFloat> _RImg(_CImgData);
    img.getImageData(_CImgData,0,1); dmIImage<dmPixelFormat32bppFloat> _GImg(_CImgData);
    img.getImageData(_CImgData,0,2); dmIImage<dmPixelFormat32bppFloat> _BImg(_CImgData);

   dmIImage<dmPixelFormat8bppIndexed> _Buffer(_Rect.Width(),_Rect.Height());
    
    // Copy data from source image
    dmPoint p(0,0);    
    dmCopyRGBData( _Img, _Buffer , 1, _Rect, p); _RImg.GetCopy(_Buffer);
    dmCopyRGBData( _Img, _Buffer , 2, _Rect, p); _GImg.GetCopy(_Buffer);
    dmCopyRGBData( _Img, _Buffer , 3, _Rect, p); _BImg.GetCopy(_Buffer);

    _Apply(img,_Operation);

    p = _Rect.TopLeft(); _Rgn.OffsetRoi();
    
    // Copy back data to image
    daim::truncate(_RImg.Gen(),_Buffer.Gen()); dmCopyRGBData(_Buffer,_Img,1,_Rgn,p);
    daim::truncate(_GImg.Gen(),_Buffer.Gen()); dmCopyRGBData(_Buffer,_Img,2,_Rgn,p); 
    daim::truncate(_BImg.Gen(),_Buffer.Gen()); dmCopyRGBData(_Buffer,_Img,3,_Rgn,p);
  }
};
//----------------------------------------------------------
bool dmGREYCStoration( const dmImage&  _Src,
                       const dmRegion& _Rgn,
                       const dmRegion* _Mask, 
                       dmIUnknown& _Progress,
                       _Instance_Ptr _This,
                       const unsigned int _Operation )
{
  dmRegion Rgn(_Rgn);
    
  Rgn.ClipToRect(_Src.Rect());
  if(!Rgn.IsEmptyRoi())
  {
    __dm_impl_greycstoration _filter(Rgn,_Mask,_Progress,_This,_Operation);
    return dmImplementOperation(_filter,_Src);
  }
  return false;
}
//----------------------------------------------------------
