#ifndef dmUserLib_h
#define dmUserLib_h

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

//---------------------------------------------------------------------------------------
// File         : dmUserLib.h
// Date         : 7/2004
// Author       : David Marteau
//---------------------------------------------------------------------------------------

//
// $FUNCTIONS:
// dmScaleTo8Bits          : Convert a range of pixel values from scalar image to 8 bits values
// dmFillScalar            : Set the pixels values on scalar images
// dmFillRGB               : Set the rgb pixels values on rgb images
// dmConvertToRGB          : Convert scalar source image to rgb image
// dmConvertTo8bits        : Convert source image to 8 bits image
// dmCopyRGBData           : Copy pixel values from (or to) the specified rgb channel
// dmSplitRGB              : Split rgb image channels in 8 bits destination images
// dmMergeRGB              : Merge 8 bits values in rgb destination image
// dmMakePeriodic          : Create a periodic pattern image from rectangular area
// dmCreateMotif           : Create a regular motif from rectangular area
// dmFlipCopy              : Flip a rectangular area of the operand image
// dmRotateCopy            : Apply a 90 degrees rotation to a rectangular area of the operand image
// dmArithmetics           : Apply binary arithmetic operation between images
// dmAddMulImage           : Apply transformation x => a*x+b on input image pixels
// dmLocalMax              : Local max detector
// dmConvolution           : Apply a convolution operator
// dmMFilter               : Apply an optimized filter to operand image
// dmRFilter               : Apply a rank filter of to the operand image
// dmLFilter               : Apply a linear filter to the operand image
// dmMorphology            : Apply mathematical morphological operator
// dmThreshold             : Threshold
// dmHysteresisThreshold   : Threshold with hysteresis
// dmRGBThreshold          : Build a region correspondig to pixel around a given rgb value
// dmCannyFilter           : Apply a Canny operator
// dmNagaoFilter           : Apply a Nagao operator
// dmGradientFilter        : Compute the euclidian norm of the linear first order gradient
// dmHistogram             : Build a 255 valued histogram
// dmExtendMap             : Apply histogram extension for a range of pixel values
// dmInvertMap             : Invert a range of pixels values
// dmTransformMap          : Apply histogram mapping on pixels values
// dmCompareAndCopy        : Copy pixels conditionaly
// dmCompareAndCopyMask    : Copy pixels conditionaly and build a mask of pixels copied.
// dmCopyIfMaskCondition   : Copy pixels conditionaly to mask values
// dmCreateRegion          : Create a region from a predicat
// dmCreateRegionRoi       : Create a region from a predicat using a roi
// dmDownSampling          : Create a filtered down sampled image
// dmUpSampling            : Create a filtered up sampled image
// dmEnhanceContrast       : Apply Constrast/luminosity transformation to image
// dmGammaCorrection       : Apply Gamma correction to image
// dmBlendImage            : Blend two images
// dmLinearStretch         : Apply linear deformation to image
// dmRotateImage           : Apply rotation to image
// dmImageStat             : Compute statistical moment
// dmImageStats            : Compute multiple statistical moments
// dmScaleImage            : Scale a image
// dmTruncateImage         : Truncate image to a different format
//---------------------------------------------------------------------------------------

class dmColorHistogram;
class dmHistogram;

/*---------------------------------------------------------------------------------------
 function   : dmScaleTo8Bits
 description: Convert a range of pixel values from scalar image to 8 bits values
              in destination image. If rmin == rmax then scaling is performed according
              to minimum and maximum values computed from source image pixels.
              Pixels out of range in source image are converted to zero pixel value
              in destination image.
 Note       : The destination image must be a 8 bits image type.
              Range is ignored on 8 bits images
---------------------------------------------------------------------------------------*/
bool dmScaleTo8Bits( const dmImage& _src, dmImage& _dest, dm_real rmin, dm_real rmax);

/*---------------------------------------------------------------------------------------
 functor    : dmFillScalar
 description: Set the pixels values from the region parameter in the source image
              to the value '_value'. For rgb images, value is mapped to the
              corresponding gray scale rgb triplet.
---------------------------------------------------------------------------------------*/
class dmFillScalar
{
  public:
    dm_real _Value;
    dm_real _Alpha;

    dmFillScalar( dm_real aValue, dm_real Alpha )
    : _Value(aValue)
    , _Alpha(Alpha)
    {}

    bool Apply( dmBufferParameters& );

    static bool SetValue( dmImage& _Image, const dmRegion& aRgn, dm_real aValue,
                          dm_real aAlpha );
};
/*---------------------------------------------------------------------------------------
 functor    : dmFillRGB
 description: Fill red, green and blue channel width the values specified by parameters.
              Only pixels located in the region parameter are affected.
 Note       : The source image must be a rgb image type
---------------------------------------------------------------------------------------*/
class dmFillRGB
{
  public:
   dm_uint16 _Red;
   dm_uint16 _Green;
   dm_uint16 _Blue;
   dm_real   _Alpha;

   dmFillRGB( dm_uint16 Red,dm_uint16 Green, dm_uint16 Blue, dm_real Alpha )
   :_Red(Red)
   ,_Green(Green)
   ,_Blue(Blue)
   ,_Alpha(Alpha)
   {}

  bool Apply( dmBufferParameters& );

  static bool SetValues( dmImage&, const dmRegion& ,
                         dm_uint16 Red,dm_uint16 Green, dm_uint16 Blue, dm_real Alpha );

};

/*---------------------------------------------------------------------------------------
 function   : dmConvertToRGB
 description: Convert source image to rgb image. I the source image is not a 8 bits image
              then pixels sources are rescaled using the dmScaleTo8Bits fonctions
 Note       : The destination image must be a rgb image type
---------------------------------------------------------------------------------------*/
bool dmConvertToRGB( const dmImage& _src, dmImage& _dest, dm_real rmin, dm_real rmax );

/*---------------------------------------------------------------------------------------
 function   : dmConvertTo8bits
 description: Convert source image to 8 bits image. If the source image is not a 8 bits
              image then pixels sources are rescaled using 'dmScaleTo8Bits'
 Note       : The destination image must be a rgb image type
---------------------------------------------------------------------------------------*/
bool dmConvertTo8bits( const dmImage& _src, dmImage& _dest, dm_real rmin, dm_real rmax );

/*---------------------------------------------------------------------------------------
 function   : dmCopyRGBData
 description: Copy pixel values from (or to) the specified channel from source image to
              destination image.
              The two first version of the function use 'channel' to specify which
              channel to copy.
              - if source image is rgb and dest is 8bits : pixels are copied
                from the specified source channel to destination image.
              - if source image is rgb and dest is rgb : pixels are copied
                from the specified source channel to corresponding channel
                in destination image.
              - if source image is 8bits and dest is rgb : pixels are copied
                from source image to the specified channel of the destination image.

---------------------------------------------------------------------------------------*/
bool dmCopyRGBData( const dmImage& _src, dmImage& _dest , dm_uint channel );
bool dmCopyRGBData( const dmImage& _src, dmImage& _dest , dm_uint channel,
                    const dmRegion&, const dmPoint& );

bool dmCopyRGBData( dmImage& _rgb, dmImage& _red, dmImage& _green, dmImage& _blue , dm_bool _toRGB );

/*---------------------------------------------------------------------------------------
 function   : dmMakePeriodic
 description: This function create a periodic representation of
              the original rectangular area of _rsrc of the operand
              image over the area specified by  _rdest.
 Note       : The position of the original area is left
              unchanged in the image.
---------------------------------------------------------------------------------------*/
bool dmMakePeriodic( dmImage& _img, const dmRect& _rsrc, const dmRect _rdest );

/*---------------------------------------------------------------------------------------
 function   : dmCreateMotif
 description: This function create a regular representation of
              the original rectangular area of _rsrc of the operand
              image over the area specified by  _rdest.
 Note       : The position of the original area is left
              unchanged in the image.
---------------------------------------------------------------------------------------*/
bool dmCreateMotif( dmImage& _img, const dmRect& _rsrc, const dmRect _rdest );


/*---------------------------------------------------------------------------------------
 function   : dmFlipCopy
 description: Flip the rectangle _r of the operand image '_src' according
              to the '_flip' parameter and store the result at location _p into
              the destination image '_dst'.
              Note that source and destination images must be of the same type
---------------------------------------------------------------------------------------*/
bool dmFlipCopy( const dmImage& _src, dmImage& _dst,
                 const dmRect& _r, const dmPoint& _p, int _flip );
/*---------------------------------------------------------------------------------------
 function   : dmRotateCopy
 description: Apply a 90 degrees rotation the rectangle _r of the operand image
              '_src' according to the '_way' parameter and store the result at
              location _p into the destination image '_dst'.
 Note       : that source and destination images must be of the same type
---------------------------------------------------------------------------------------*/
bool dmRotateCopy( const dmImage& _src, dmImage& _dst,
                   const dmRect& _r, const dmPoint& _p, int _way  );
//---------------------------------------------------------------------------------------

#include "dmOperators.h"

/*---------------------------------------------------------------------------------------
 function   : dmArithmetics
 description: Apply binary operation specified by the '_op' aparemeter
              between operand image and buffer
              (see dmOperators.h for a list of operations).
              Store the result into operand image
---------------------------------------------------------------------------------------*/
bool dmArithmetics( dmBufferParameters&, dm_int _Operation );
bool dmArithmetics( dm_int _Operation, const dmImage& _Lhs, dmImage& _Rhs );
bool dmArithmetics( dm_int _Operation, const dmImage& _Lhs, dmImage& _Rhs,
                                       const dmRegion&, const dmPoint& );

/*---------------------------------------------------------------------------------------
 function   : dmAddMulImage
 description: Apply the transformation x => a*x+b on each pixels of the ROI
              of the input image.
---------------------------------------------------------------------------------------*/
bool dmAddMulImage( dmImage&, const dmRegion& ,dm_real a, dm_real b );
bool dmAddMulImage( dmImage&, dm_real a, dm_real b );

/*---------------------------------------------------------------------------------------
 function   : dmDivideImage
---------------------------------------------------------------------------------------*/
bool dmDivideImage( dmImage& _Lhs, const dmImage& _Rhs,
                    const dmRegion& _Rgn, const dmPoint& _Pt );
/*---------------------------------------------------------------------------------------
 function   : dmMultiplyImage
---------------------------------------------------------------------------------------*/
bool dmMultiplyImage( dmImage& _Lhs, const dmImage& _Rhs,
                      const dmRegion& _Rgn, const dmPoint& _Pt );

/*---------------------------------------------------------------------------------------
 function   : dmLocalMax
 description: Local max detector : build a region corresponding
              to local max  of the operand image
---------------------------------------------------------------------------------------*/
bool dmLocalMax( dmBufferParameters&, dmRegion& _Result );

/*---------------------------------------------------------------------------------------
 functor    : dmConvolution
 description: Convolution operator
              Apply 'iter' times the kernel family to operand image
---------------------------------------------------------------------------------------*/
class dmConvolution
{
  public:
    dmKernelFamily& _Family;
    int             _Iter;

    dmConvolution( dmKernelFamily& aFamily, int Iter = 1 )
    : _Family(aFamily),_Iter(Iter) {}

    bool Apply( dmBufferParameters& );
};
/*---------------------------------------------------------------------------------------
 functor    : dmMFilter
 description: Apply a 'block' filter type to operand image
              in a optimized way.
---------------------------------------------------------------------------------------*/
typedef enum  {
   dmFilterMax,
   dmFilterMin,
   dmFilterMiddle,
   dmFilterMean,
   dmFilterMedian,
} dmFilterType;
//---------------------------------------------------------------------------------------
class dmMFilter
{
  public:
    const dmMaskDescription& _Mask;
    dmFilterType             _Type;

  dmMFilter( const dmMaskDescription& aMask,  dmFilterType aType )
  : _Mask(aMask),_Type(aType) {}

  bool Apply( dmBufferParameters& );
};
/*---------------------------------------------------------------------------------------
 functor    : dmRFilter
 description: Apply a rank filter of order 'rank' to the operand image
              using the given mask
---------------------------------------------------------------------------------------*/
class dmRFilter
{
  public:
    const dmMaskDescription& _Mask;
    unsigned int             _Rank;

  dmRFilter( const dmMaskDescription& aMask, unsigned int aRank )
  :_Mask(aMask)
  ,_Rank(aRank)
  {}

  bool Apply( dmBufferParameters& );
};
/*---------------------------------------------------------------------------------------
 functor    : dmLFilter
 description: Apply a linear 'L' filter on the operand image using the
              given mask  and the coefficients given in '_coeffs',
              the number of elements in '_coeffs' must be the same as
              the number of elements in the specified mask.
---------------------------------------------------------------------------------------*/
class dmLFilter
{
  public:
    const dmMaskDescription& _Mask;
    dm_real*                 _Coeffs;

  dmLFilter( const dmMaskDescription& aMask, dm_real* Coeffs )
  :_Mask(aMask)
  ,_Coeffs(Coeffs)
  {}

  bool Apply( dmBufferParameters& );
};
/*---------------------------------------------------------------------------------------
 functor    : dmMorphology
 description: Apply mathematical morphological operator using the given
              kernel family. The 'operation' parameter specifies
              the type of morphological operation to perform.
---------------------------------------------------------------------------------------*/
class dmMorphology
{
  public:
    dmKernelFamily& _Family;
    int             _Operation;
    int             _Iter;

  dmMorphology(dmKernelFamily& aFamily, int anOperation = dmTk::None, int Iter = 1 )
  : _Family(aFamily),_Operation(anOperation),_Iter(Iter) {}

  bool Apply( dmBufferParameters& );

};
/*---------------------------------------------------------------------------------------
 functor    : dmThreshold
 description: Build a binary mask from threshold
---------------------------------------------------------------------------------------*/
class dmThreshold
{
  public:
   dm_real   _LowThreshold;
   dm_real   _HighThreshold;
   dmRegion& _Result;

   dmThreshold( dm_real LowThreshold, dm_real HighThreshold, dmRegion& Result)
   :_LowThreshold(LowThreshold)
   ,_HighThreshold(HighThreshold)
   ,_Result(Result)
   {}

   bool Apply( dmBufferParameters& );
};
/*---------------------------------------------------------------------------------------
 functor    : dmHysteresisThreshold
 description: Threshold with hysteresis
---------------------------------------------------------------------------------------*/
class dmHysteresisThreshold
{
  public:
   dm_real   _LowThreshold;
   dm_real   _HighThreshold;
   dm_uint   _Connect;
   dmRegion& _Result;

   dmHysteresisThreshold( dm_real LowThreshold, dm_real HighThreshold,  dm_uint Connect,
                          dmRegion& Result)
   :_LowThreshold(LowThreshold)
   ,_HighThreshold(HighThreshold)
   ,_Connect(Connect)
   ,_Result(Result)
   {}

   bool Apply( dmBufferParameters& );
};
/*---------------------------------------------------------------------------------------
 function   : dmRGBThreshold
 description: Build a region correspondig to pixel around a given rgb value
---------------------------------------------------------------------------------------*/
bool dmRGBThreshold( const dmImage&    _Src,
                     const dmRegion&   _Rgn,
                     const dmRGBColor& _Color,
                     dm_float rv,dm_float rg, dm_float rb,
                     dmRegion& _Result );

/*---------------------------------------------------------------------------------------
 functor    : dmCannyFilter
 description: Apply a Canny operator.
              If inner is set to 'true', then contours corresponds to
              the inner boundary of objets.
---------------------------------------------------------------------------------------*/
class dmCannyFilter
{
   public:
     bool _Inner;

   dmCannyFilter( bool Inner = true ) : _Inner(Inner) {}

   bool Apply( dmBufferParameters& );
};
/*---------------------------------------------------------------------------------------
 functor    : dmNagaoFilter
 description: Apply a Nagao operator to operand image
---------------------------------------------------------------------------------------*/
class dmNagaoFilter
{
  public:
    dmNagaoFilter() {}

    bool Apply( dmBufferParameters& );
};
/*---------------------------------------------------------------------------------------
 functor    : dmGradientFilter
 description: Compute the euclidian norm of the linear first order gradient
---------------------------------------------------------------------------------------*/
class dmGradientFilter
{
  public:
    dmGradientFilter() {}

    bool Apply( dmBufferParameters& );
};
/*---------------------------------------------------------------------------------------
 functor    : dmNonMaxSuppress
 description: Compute non maximal suppression (zero-crossing)
---------------------------------------------------------------------------------------*/
class dmNonMaxSuppress
{
  public:
    bool _Inner;

    dmNonMaxSuppress( bool Inner = true ) : _Inner(Inner) {}

    bool Apply( dmBufferParameters& );
};
//---------------------------------------------------------------------------------------
#ifdef dmUseKernelImageTemplates
#include "templates/processing/dmHistogram.h"
#include "templates/processing/dmColorHistogram.h"
#endif

/*---------------------------------------------------------------------------------------
 functor    : dmDensityMap
 description: Build a 255 values histogram
              for 8 bits and RGB images => compute the standard histogram.
              For gray levels images > 8 bits => compute the histogram for
              values between [MinRange and MaxRange].
              If AutoScan is set to 'true' then returned _MinRange and _MaxRange
              will be set respectively to the minimum and maximum image pixel
              values.
---------------------------------------------------------------------------------------*/
class dmDensityMap
{
  public:
   dmHistogram& _Histogram;
   dm_uint      _Index;
   dm_real&     _MinRange;
   dm_real&     _MaxRange;
   bool         _AutoScan;

   dmDensityMap( dmHistogram& anHistogram,dm_real& MinRange,dm_real& MaxRange,
                bool AutoScan )
   :_Histogram(anHistogram)
   ,_MinRange(MinRange)
   ,_MaxRange(MaxRange)
   ,_AutoScan(AutoScan)
   {}

   bool Apply( dmBufferParameters& );
};
/*---------------------------------------------------------------------------------------
 functor    : dmExtendMap
 description: Apply histogram extension from [MinExt,MaxExt]
              to [MinRange,MaxRange]
---------------------------------------------------------------------------------------*/
class dmExtendMap
{
   public:
     dm_real _MinExt;
     dm_real _MaxExt;
     dm_real _MinRange;
     dm_real _MaxRange;

   dmExtendMap(dm_real MinExt  , dm_real MaxExt  ,
               dm_real MinRange, dm_real MaxRange )
   :_MinExt(MinExt)
   ,_MaxExt(MaxExt)
   ,_MinRange(MinRange)
   ,_MaxRange(MaxRange)
   {}

   bool Apply( dmBufferParameters& );
};
/*---------------------------------------------------------------------------------------
 functor    : dmInvertMap
 description: Invert pixels values between MinRange and MaxRange.
              Range is ignored for 8 bits or rgb images.
---------------------------------------------------------------------------------------*/
class dmInvertMap
{
  public:
   dm_real _MinRange;
   dm_real _MaxRange;

   dmInvertMap(dm_real MinRange, dm_real MaxRange )
   :_MinRange(MinRange)
   ,_MaxRange(MaxRange)
   {}

   bool Apply( dmBufferParameters& );
};
/*---------------------------------------------------------------------------------------
 functor    : dmTransformMap
 description: Apply histogram mapping from 'dmColorMapArray' on pixels values
              between MinRange and MaxRange. (Note that 8bits interpolation
              is performed on non-8bits images ion order to apply the mapping.
              Range is ignored for 8 bits or rgb images.
              Interpolation method can be specified by the 'flags' parameter
              (not implemented for now).
---------------------------------------------------------------------------------------*/
class dmTransformMap
{
 public:
  dmColorMapArray _Map;
  dm_real         _MinRange;
  dm_real         _MaxRange;
  int             _Flags;

  dmTransformMap( dmColorMapArray aMap,dm_real MinRange, dm_real MaxRange,
                  int Flags = 0 )
  :_Map(aMap)
  ,_MinRange(MinRange)
  ,_MaxRange(MaxRange)
  ,_Flags(Flags)
  {}

  bool Apply( dmBufferParameters& );
};
/*---------------------------------------------------------------------------------------
 function   : dmCompareAndCopy
 description: Copy src pixels from region in dest at location p
              if the binary predicat between the src and the dest pixel
              is verified.
---------------------------------------------------------------------------------------*/
bool dmCompareAndCopy( const dmImage& src, dmImage& dst, dm_uint  predicat,
                       const dmRegion& rgn, const dmPoint& p );
/*---------------------------------------------------------------------------------------
 function   : dmCompareAndCopyMask
 description: Copy src pixels in dest if the binary predicat between
              the src and the dest pixel is verified.
              value \a value is copied in mask at the same location
              as the pixel beeing copied.
---------------------------------------------------------------------------------------*/
bool dmCompareAndCopyMask( const dmImage& src, dmImage& dst, dm_uint  predicat,
                           dmImage& mask, dm_real  value );
/*---------------------------------------------------------------------------------------
 function   : dmCopyIfMaskCondition
 description: Copy src pixels in dest if the unary predicat on the same mask pixel
              is verified.
---------------------------------------------------------------------------------------*/
bool dmCopyIfMaskCondition( const dmImage& src, dmImage& dst,
                            dmImage& mask, dm_uint  predicat, dm_real  value );
/*---------------------------------------------------------------------------------------
 function   : dmCreateRegion
 description: Create a region correponding to the pixels that verify the
              predicat for the value \a value.
---------------------------------------------------------------------------------------*/
bool dmCreateRegion( const dmImage& src, dmRegion& result,
                     dm_uint predicat,
                     dm_real value );
/*---------------------------------------------------------------------------------------
 function   : dmCreateRegionRoi
 description: Create a region correponding to the pixels located in the region
              \a roi that verify the predicat for the value \a value.
---------------------------------------------------------------------------------------*/
bool dmCreateRegionRoi( const dmImage& src, dmRegion& result,
                        dm_uint predicat,
                        dm_real value,
                        const dmRegion& roi );
/*---------------------------------------------------------------------------------------
 functor    : dmDownSampling
 description: Create a down sampled image
---------------------------------------------------------------------------------------*/
class dmDownSampling
{
  public:
    dmKernelFamily& _Family;
    dmImage&        _Result;
    const dmPoint&  _Step;

    dmDownSampling(dmKernelFamily& aFamily, dmImage& Result, const dmPoint& Step )
    :_Family(aFamily)
    ,_Result(Result)
    ,_Step(Step) {}

    bool Apply( dmBufferParameters& );

};
/*---------------------------------------------------------------------------------------
 functor    : dmUpSampling
 description: Perform an up sampling of the image \a image into the buffer
              then apply convolution from buffer to the image held in \a params.
---------------------------------------------------------------------------------------*/
class dmUpSampling
{
   public:
    dmKernelFamily& _Family;
    dmImage&        _Image;
    const dmRect&   _Rect;
    const dmPoint&  _Step;

    dmUpSampling(  dmKernelFamily& aFamily,dmImage& anImage,
                   const dmRect& Rect, const dmPoint& Step )
    :_Family(aFamily)
    ,_Image(anImage)
    ,_Rect(Rect)
    ,_Step(Step) {}

    bool Apply( dmBufferParameters& );
};
/*---------------------------------------------------------------------------------------
 functor    : dmEnhanceContrast
 description: Apply Constrast/Luminosity transformation
              to image for the given range of pixel values
---------------------------------------------------------------------------------------*/
class dmEnhanceContrast
{
   public:
    dm_real   _MinRange;
    dm_real   _MaxRange;
    dm_real   _Brightness;
    dm_real   _Contrast;
    dm_bool   _UseBuffer;

    dmEnhanceContrast( dm_real MinRange,dm_real MaxRange,
                       dm_real Brightness,dm_real Contrast, dm_bool UseBuffer)
    :_MinRange(MinRange)
    ,_MaxRange(MaxRange)
    ,_Brightness(Brightness)
    ,_Contrast(Contrast)
    ,_UseBuffer(UseBuffer)
    {}

    bool Apply( dmBufferParameters& );
};
/*---------------------------------------------------------------------------------------
 functor    : dmGammaCorrection
 description: Apply Gamma correction to image for the given range of pixel values
---------------------------------------------------------------------------------------*/
class dmGammaCorrection
{
   public:
    dm_real   _MinRange;
    dm_real   _MaxRange;
    dm_real   _Gamma;

    dmGammaCorrection( dm_real MinRange,dm_real MaxRange,dm_real Gamma)
    :_MinRange(MinRange)
    ,_MaxRange(MaxRange)
    ,_Gamma(Gamma)
    {}

    bool Apply( dmBufferParameters& );
};
/*---------------------------------------------------------------------------------------
 function   : dmBlendImage
 description: Blend two images, Store the result into operand image
---------------------------------------------------------------------------------------*/
bool dmBlendImage( dmBufferParameters& _Params, dm_real _Percent );
bool dmBlendImage( dm_real _Percent, const dmImage& _Src, dmImage& _Dst );
bool dmBlendImage( dm_real _Percent, const dmImage& _Src, dmImage& _Dst,
                                     const dmRegion& _Rgn, const dmPoint& _Pt );
//---------------------------------------------------------------------------------------
typedef enum  {
   dmDecimation = 1,
   dmBilinear   = 2,
   dmBicubic    = 3,
} dmInterpolationType;
/*---------------------------------------------------------------------------------------
 function   : dmLinearStretch
 description: Apply Linear stretch to image
              Store the result into operand image
---------------------------------------------------------------------------------------*/
bool dmLinearStretch( dmBufferParameters& _Params, dmInterpolationType mode );
bool dmLinearStretch( dmInterpolationType mode, const dmImage& _Src, dmImage& _Dest );
bool dmLinearStretch( dmInterpolationType mode, const  dmImage& _Src, const  dmRect& _Rect,
                      dmImage& _Dest );
/*---------------------------------------------------------------------------------------
 function   : dmRotateImage
 description: Apply rotation to  image
              Store the result into operand image
---------------------------------------------------------------------------------------*/
bool dmRotateImage( dmBufferParameters& _Params, dm_real _Angle );
bool dmRotateImage( dm_real _Angle, const dmImage& _Rhs, dmImage& _Lhs );
bool dmRotateImage( dm_real _Angle, const  dmImage& _Rhs, const  dmRect& _Rect, dmImage& _Lhs );
/*---------------------------------------------------------------------------------------
 function   : dmImageStat
 description: Compute the statistical moment of order \a _Order, using the central
              value \a _Central. VALUES ARE NOT NORMALIZED
---------------------------------------------------------------------------------------*/
bool dmImageStat( const dmImage& _Src, const dmRegion& _Rgn,
                   dm_uint  _Order,
                   dm_real  _Central,
                   dm_real& _Value );
/*---------------------------------------------------------------------------------------
 function   : dmImageStats
 description: Compute several statistical moments up to order \a _Order,
              using the central value \a _Central.
              VALUES ARE NOT NORMALIZED
---------------------------------------------------------------------------------------*/
bool dmImageStats( const dmImage& _Src, const dmRegion& _Rgn,
                   dm_uint  _Order,
                   dm_real  _Central,
                   dm_real* _Values );
/*---------------------------------------------------------------------------------------
 function    : dmScaleImage
 description: Rescale pixel values between _Min and _Max;
---------------------------------------------------------------------------------------*/
bool dmScaleImage( dmBufferParameters& _Params, dm_real _Min, dm_real _Max );
bool dmScaleImage( const dmImage& _Src, const dmRegion& _Rgn, dm_real _Min, dm_real _Max );
bool dmScaleImage( const dmImage& _Src, dm_real _Min, dm_real _Max );
/*---------------------------------------------------------------------------------------
 function    : dmScaleImage
 description: Rescale pixel values between _Min and _Max;
---------------------------------------------------------------------------------------*/
bool dmTruncateImage( const dmImage& _Src, dmImage& _Dst, const dmRegion& _Rgn,
                      const dmPoint& _Point );
//---------------------------------------------------------------------------------------

/*---------------------------------------------------------------------------------------
 function    : dmApplyFilter
---------------------------------------------------------------------------------------*/

#ifdef dmUseKernelImageTemplates
template<class F>
bool dmApplyFilter(F& _Filter, dmImageBuffer& buffer, dmImage&  img, const dmRegion& rgn,
                  bool merge)
{
  if(img.PixelFormat()==dmPixelFormat24bppRGB)
  {
    // Create dest channel buffer
    dmLink<dmImage> chan =
       dmCreateImage<dmPixelFormat8bppIndexed>(img.Width(),img.Height());

    for(dm_uint32 ch=1;ch<=3;++ch)
    {
      // Extract src channel
      if(!dmCopyRGBData(img,*chan,ch))
          return false;

      dmBufferParameters _Params(buffer,*chan,rgn);
      if(!_Filter.Apply( _Params ))
        return false;

      if(merge)
      {
        // Merge channel into dest
        if(!dmCopyRGBData(*chan,img,ch,rgn,rgn.Loc()))
            return false;
      }
    }
    return true;
  }
  else
  {
    dmBufferParameters _Params(buffer,img,rgn);
    return _Filter.Apply( _Params );
  }
}
#endif //dmUseKernelImageTemplates
//---------------------------------------------------------------------------------------

#endif // dmUserLib_h
