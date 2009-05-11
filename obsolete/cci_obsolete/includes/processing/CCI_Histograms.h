#ifndef CCI_Histograms_h
#define CCI_Histograms_h

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

//--------------------------------------------------------
// File         : CCI_Histograms.h
// Date         : 12/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//
//--------------------------------------------------------

#define CCIC_Histograms _TXT("processing/histograms.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( Histograms, Notify )

  CCI_MODULE_UUID( Histograms, 4034c0f8-b28b-4cea-9a24-dcb7cd1fc64e)

  CCI_MODULE_VERSION( Histograms, 2 ) 

  /* Methods  */
  CCI_DECL_METHOD_PP(Histograms,1,  Scan , 
     _in dm_cci(Parameters) params
     _in dm_uint            index
     _in dm_uint            flags 
  )

  CCI_DECL_METHOD_PP(Histograms,2,  Extend ,
     _in dm_cci(Parameters) params
     _in dm_real            rmin 
     _in dm_real            rmax 
  )

  CCI_DECL_METHOD_PP(Histograms,3,  Transform , 
     _in dm_cci(Parameters)         params
     _in dm_array(dm_uint,count)    data 
     _in dm_uint                    count 
  )

  CCI_DECL_METHOD_PP(Histograms,4,  Histogram , 
     _in  dm_uint                 index 
     _out dm_array(dm_uint,count) data
     _out dm_uint                 count 
  )

  CCI_DECL_METHOD_PP(Histograms,5,  Equalize  , _in  dm_cci(Parameters) params )
  CCI_DECL_METHOD_PP(Histograms,6,  GetRange  , _out dm_real rmin _out dm_real rmax )
  CCI_DECL_METHOD_PP(Histograms,7,  SetRange  , _in  dm_real rmin _in  dm_real rmax )

  CCI_DECL_METHOD_PP(Histograms,8,  Threshold , 
      _in dm_cci(Parameters) params
      _in dm_real            rmin 
      _in dm_real            rmax
      _in dm_uint            flags 
  )

  CCI_DECL_METHOD_PP(Histograms,9,  FillChannel, 
      _in dm_cci(Parameters) params
      _in dm_real            value 
  )

  CCI_DECL_METHOD_PP(Histograms,10,  InvertMap , 
      _in dm_cci(Parameters) params 
  )

  CCI_DECL_METHOD_PP(Histograms,11,  Specify , 
      _in dm_cci(Parameters) params 
      _in dm_uint            index
  )

  CCI_DECL_METHOD_PP(Histograms,12,  Gamma , 
      _in dm_cci(Parameters) params 
      _in dm_real            gamma
  )

  CCI_DECL_METHOD_PP(Histograms,13,  Contrast , 
      _in dm_cci(Parameters) params
      _in dm_real            brightness  /* Between 0 and 1 */
      _in dm_real            contrast    /* Between 0 and 1 */
  )

  CCI_DECL_METHOD_PP(Histograms,14,  OptimalThreshold , 
     _in    dm_uint  index 
     _in    dm_real  background
     _out   dm_real  value
  )

  CCI_DECL_METHOD_PP(Histograms,15,  RGBThreshold , 
     _in dm_cci(Parameters) params
     _in dm_uint16 red 
     _in dm_uint16 green 
     _in dm_uint16 blue 
     _in dm_real   rradius
     _in dm_real   gradius
     _in dm_real   bradius
  )

  CCI_DECL_METHOD_PP(Histograms,16, GetValues , 
     _in  dm_int                  index 
     _in  dm_cci(StorageMap)      storage
     _in  CONST char_t*           col
  )

  CCI_DECL_METHOD_PP(Histograms,17, Scale , 
     _in dm_cci(Parameters) params
     _in dm_real minval
     _in dm_real maxval
  )

  CCI_DECL_METHOD_PP(Histograms,19,  HysteresisThreshold , 
      _in dm_cci(Parameters) params
      _in dm_real            rmin 
      _in dm_real            rmax
      _in dm_uint            connect
      _in dm_uint            flags 
  )

  
  /* Attributs */
  CCI_DECL_ATTR(Histograms,1,  Channels  , dm_uint, [.SGN] )

  /* Special values */
  #define CCIV_Histograms_AutoRange   1L

CCI_END_MODULE_DECL( Histograms )

#endif // __doxygen
//----------------------------------------------------------------
#ifdef __cplusplus

#include "daim_modules/CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Manipulate density histograms and pixel mapping.
 *
 *  dmIHistograms method will use the dmIColorSpace from the the dmIParameters
 *  objet to retrieve which channel to use on RGB images. Set the appropriate
 *  colorspace object to retrieve histogram informations about a particular 
 *  channel or to apply transformations.
 */
//------------------------------------------------------------------------------------
class dmIHistograms : public dmINotify
{ 
  public:
   dmDECLARE_GENERIC_IINTERFACE(Histograms)

   /*! Flags Constants */
   enum Constants {
     flAutoRange = CCIV_Histograms_AutoRange
   };

   //-------------------------------------------------------
   /*! \brief Scan the image parameter and build the corresponding
    *  histogram.
    *
    *  The histogram is build from 8 bits quantized pixel values of the 
    *  original image. For scalar valued pixel image, the resulting histogram
    *  will be associated to the \a index channel. For multichannels images,
    *  the channel from which the histogram is to be extracted is obtained 
    *  by the dmIParameter::GetChannel() method.
    *
    *  If flags is set to \a dmIHistograms::flAutoRange, the range
    *  is computed from the min and the max of the image pixel values.
    *  \sa SetChannels()
    */
   //-------------------------------------------------------
   bool Scan( dm_cci(Parameters) params, dm_uint index, dm_uint  flags ) {
     CCI_RETURN_IMETHOD_PP_OK(Histograms,Scan, _in params _in index _in flags )
   }

   //-------------------------------------------------------
   /*! \brief Apply an an histogram extension to the pixel values
    *
    *  The extension is performed from the range set by the SetRange()
    *  method to the range specified by the \a min and the \a max parameters.
    */
   //-------------------------------------------------------
   bool Extend( dm_cci(Parameters) params, dm_real rmin, dm_real rmax ) {
     CCI_RETURN_IMETHOD_PP_OK(Histograms,Extend, _in params _in rmin _in rmax)
   }

   //-------------------------------------------------------
   /*! \brief Apply an 8 bits histogram transformation to the pixels 
    *  values of the image parameter.
    */
   //-------------------------------------------------------
   bool Transform( dm_cci(Parameters) params, dm_uint* data, dm_uint count ) {
     CCI_RETURN_IMETHOD_PP_OK(Histograms,Transform , _in params _in data _in count )
   }

   //-------------------------------------------------------
   /*! \brief Get the 256 histogram values for the channel specified
    *  by the \a index parameter.
    *
    *  \sa Scan()
    */
   //-------------------------------------------------------
   bool Histogram( dm_uint index, dm_uint*& data, dm_uint& count ) {
     IF_CCI_IMETHOD_RP(Histograms,Histogram, _in index _out(data) 0 ) THEN 
       CCI_RETURN_P(data)
       CCI_RETURN_P(count)
     ENDIF_RETURN_BOOL()
   }

   //-------------------------------------------------------
   /*! \brief Apply an equalization to the pixels values of the image
    *  parameter.
    */
   //-------------------------------------------------------
   bool Equalize( dm_cci(Parameters) params ) {
     CCI_RETURN_IMETHOD_PP_OK(Histograms,Equalize, _in params )
   }

   //-------------------------------------------------------
   /*! \brief Apply an histogram specification using a stored histogram
    *  \param index the histogram index
    *  \param params the dmIParameter object containing the target image.
    */
   //-------------------------------------------------------
   bool Specify( dm_cci(Parameters) params, dm_uint index ) {
     CCI_RETURN_IMETHOD_PP_OK(Histograms,Specify, _in params _in index )
   }

   //-------------------------------------------------------
   /*! \brief Create a binary mask corresponding to the pixels values
    *  between \a rmin and \a rmax.
    */
   //-------------------------------------------------------
   bool Threshold( dm_cci(Parameters) params, dm_real rmin, dm_real rmax, dm_uint flags ) {
     CCI_RETURN_IMETHOD_PP_OK(Histograms,Threshold,_in params  
                              _in rmin  _in rmax _in flags
     )
   }

   //-------------------------------------------------------
   /*! \brief Set the image pixels values to the value \a value.
    *
    *  For rgb image, the modified channel is set according to
    *  the \a params values (see dmIParameters)
    */
   //-------------------------------------------------------
   bool FillChannel( dm_cci(Parameters) params, dm_real  value ) { 
     CCI_RETURN_IMETHOD_PP_OK(Histograms,FillChannel, _in params _in value )
   }

   //-------------------------------------------------------
   /*! \brief Return the stored range */
   //-------------------------------------------------------
   bool GetRange( dm_real& rmin, dm_real& rmax ) {
     IF_CCI_IMETHOD_RP(Histograms,GetRange, _out(rmin) 0 _out(rmax) 0 ) THEN
       CCI_RETURN_P(rmin)
       CCI_RETURN_P(rmax)
     ENDIF_RETURN_BOOL()
   }

   //-------------------------------------------------------
   /*! \brief Set the range of pixels values */
   //-------------------------------------------------------
   bool SetRange( dm_real rmin, dm_real rmax ) {
     CCI_RETURN_IMETHOD_PP_OK(Histograms,SetRange,_in rmin _in rmax )
   }

   //-------------------------------------------------------
   /*! \brief Set the number of channels (i.e histograms) */
   //-------------------------------------------------------
   bool SetChannels( dm_uint channels ) {
     return CCI_ISET(Histograms,Channels,channels);
   }

   dm_uint GetChannels() {
      return CCI_IGET(Histograms,Channels,(dm_uint)0);
   }

   //-------------------------------------------------------
   /*! \brief Apply gamma correction to image */
   //-------------------------------------------------------
   dm_uint Gamma( dm_cci(Parameters) params, dm_real  gamma ) {
     CCI_RETURN_IMETHOD_PP_OK(Histograms,Gamma,_in params _in gamma);
   }

   //-------------------------------------------------------
   /*! \brief Apply brightness-contrast correction to image
    *
    *  IMPORTANT: If the Image buffer of the \a params object is valid, 
    *  it will be used as the input image. Otherwise, it will use 
    *  the current image stored in \a params. 
    *
    *  \param params     a instance of a dmIParameter object
    *  \param brightness the brightness parameter, between 0 et 1
    *  \param contrast   the contrast parameter, between 0 et 1    
    */
   //-------------------------------------------------------
   dm_uint Contrast( dm_cci(Parameters) params, dm_real brightness, dm_real contrast ) {
     CCI_RETURN_IMETHOD_PP_OK(Histograms,Contrast, _in params _in brightness _in contrast )  
   }

   //-------------------------------------------------------
   /*! \brief Compute optimal threshold for a bimodal histogram distribution 
    *
    *  \param index      index of a precomputed histogram
    *  \param background an initial guess for the background value
    *  \param value      return value    
    */
   //-------------------------------------------------------
   bool OptimalThreshold( dm_uint index, dm_real background, dm_real& value )  {
     IF_CCI_IMETHOD_RP(Histograms,OptimalThreshold, _in index _in background _out(value) )
     THEN CCI_RETURN_P(value)
     ENDIF_RETURN_BOOL()
   }


   //-------------------------------------------------------
   /*! \brief Create a binary mask corresponding to the pixels values corresponding 
    *  to the given RGB value neighborhood.
    *
    *  The neighborhood is given by the 'ellipsoid' determined by the three 
    *  parameters \a rradius \a gradius \a bradius. 
    *  Distance are computed using absolute norm.  
    *  \Note This function applies only to RGB images
    */
   //-------------------------------------------------------
   bool RGBThreshold( dm_cci(Parameters) params, 
                      dm_uint16 red, dm_uint16 green, dm_uint16 blue,
                      dm_real rradius, dm_real gradius, dm_real bradius )  {
     CCI_RETURN_IMETHOD_PP_OK(Histograms,RGBThreshold, 
          _in params
          _in red 
          _in green 
          _in blue 
          _in rradius
          _in gradius
          _in bradius
     )
   }
   
   //-------------------------------------------------------
   /*! \brief Scale pixels values between minval and maxval */
   //-------------------------------------------------------
   dm_uint Scale( dm_cci(Parameters) params, dm_real minval, dm_real maxval ) {
     CCI_RETURN_IMETHOD_PP_OK(Histograms,Scale,_in params _in minval _in maxval);
   }

   //-------------------------------------------------------
   /*! \brief Create a binary mask corresponding to hysteresis 
    * thresholding operation
    *
    *  The mask is computed from an hysteresis threshold using \a rmin as the lower 
    *  threshold and \a rmax as the upper threshold.
    */
   //-------------------------------------------------------
   bool HysteresisThreshold( dm_cci(Parameters) params, dm_real rmin, dm_real rmax, 
                             dm_uint connect, dm_uint flags ) {
     CCI_RETURN_IMETHOD_PP_OK(Histograms,HysteresisThreshold,_in params  
                              _in rmin  _in rmax _in connect 
                              _in flags
     )
   }
   
};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_Histograms_h */
