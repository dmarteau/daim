
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

/* $ChangeLog  v2.3, 25 sept. 2006 12:03:21 , David
 *
 * Now return failure when Apply() return false in 'Contrast'
 * method.
 */

/* $ChangeLog  v2.4, 27 oct. 2006 20:04:43 , David
 *
 * Hysteresis threshold is now a method by itself
 */ 


#define dmUseCCI
#define dmUseUtilitiesExtra
#define dmUseKernelBuffer
#define dmUseKernelImageTemplates
#include "daim_kernel.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_Parameters.h"
#include "daim_modules/CCI_ImageList.h"
#include "daim_modules/CCI_StorageMap.h"

#include "daim_modules/processing/CCI_Histograms.h"
#include "daim_modules/processing/CCI_Colorspaces.h"
#include "daim_modules/processing/CCI_RGBFunctor.h"

//------------------------------------------------------------------------
#define CCIC_NAME  CCIC_Histograms
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_Histograms_Version
#define CCI_REVISION  4
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))
//--------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"
#include "common/dmUserLib.h"

#define DFLT_HISTO_CHANNELS 3
#define _NOINDEX_ ~0L
//---------------------------------------------------------------------
CCI_DECL_DATA( Histograms ) 
{
  dm_uint          ScanFlags;
  dm_real          MaxRange;
  dm_real          MinRange;

  dmColorHistogram Histo;

  dm_uint          LastError;
};

typedef CCI_INSTANCE_PTR(Histograms) _Instance_Ptr;
//------------------------------------------------------------------------
static bool CHECK_ERROR( dm_param err, cci_Object* ob, dm_param me ) 
{
  if(err == dmFunctorErrors::E_NO_ERROR)
     return false;

  const char_t* txt = dmColorSpaceFunctorHelper::GetErrorText(err);
  if(txt) CCI_SET_ERROR(ob,me,txt);
  return true;  
} 

#define CHECK_LAST_ERROR(me) CHECK_ERROR(_This->LastError,_THIS_OBJECT,me)
#define SET_LAST_ERROR(err) _This->LastError = static_cast<dm_uint>(err)
#define RETURN_ERROR_CHECK()                                    \
        if(_This->LastError!=dmFunctorErrors::E_NO_ERROR) {     \
          CHECK_ERROR(_This->LastError,_THIS_OBJECT,CCI_P(Id)); \
          CCI_RETURN_FAIL()                                     \
        }                                                       \
        CCI_RETURN_OK()

//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( Histograms )
{
  CCI_GENERIC_CONSTRUCT(Histograms)

  _This->MinRange  = 0;
  _This->MaxRange  = 0;
  _This->ScanFlags = CCIV_Histograms_AutoRange;
  _This->LastError = dmFunctorErrors::E_NO_ERROR;
  _This->Histo.SetChannels(DFLT_HISTO_CHANNELS);

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( Histograms )
{
  CCI_GENERIC_DESTROY(Histograms)
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( Histograms )
  CCI_ATTR_GET( Histograms, Channels, CCIA_RETVAL = _This->Histo.GetChannels() )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( Histograms )
  CCI_ATTR_SET( Histograms, Channels, _This->Histo.SetChannels(CCIA_DATA) )
CCI_END_SETTER()
//---------------------------------------------------------------------
// Method: Scan
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Histograms, Scan )
{
  CCI_INSTANCE(Histograms)

  SET_LAST_ERROR(dmFunctorErrors::E_NO_ERROR);

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*       img    = _IParams.GetImage();
    dmImageBuffer* buffer = _IParams.GetImageBuffer();
    dmRegion*      rgn    = _IParams.GetRoi();

    if(img) 
    {      
      dmASSERT( buffer != NULL );
      _This->ScanFlags = CCI_P(flags);

      dm_uint  channel = _IParams.GetChannel();
      dmRegion roi = *rgn;

      dmHistogram* _Histogram = NULL;

      //------------------------------------------
      // Handle RGB image
      //------------------------------------------

      if(!dmIsPixelFormatScalar(img->PixelFormat()))
      {
        dmIColorSpace _ColorSpace;
        if(_ColorSpace.QueryInterface(_IParams.GetColorSpace()))
        {
          if(channel == dmIParameters::flAllChannels)
          {
            if(_ColorSpace.Split(_IParams)) 
            {
              dm_uint nChannels = _ColorSpace.NumChannels();
              for(dm_uint i=0;i<nChannels;++i)
              {
                _Histogram = _This->Histo[i+CCI_P(index)];
                if(_Histogram!=NULL)
                {
                   dmDensityMap _Filter(*_Histogram,
                                  _This->MinRange,_This->MaxRange,
                                 (_This->ScanFlags & CCIV_Histograms_AutoRange)!=0 ||
                                  _This->MinRange >= _This->MaxRange);

                  img = _ColorSpace.GetBuffer(i);

                  // Attention : the image has been cropped
                  // to the size of the roi => wee need to offset
                  // the roi 

                  roi.OffsetRoi();

                  dmBufferParameters _params(*buffer,*img,roi);
                  if(!_Filter.Apply(_params))
                    SET_LAST_ERROR(dmFunctorErrors::E_BAD_IMAGE);
                  else {
                    _This->MinRange = _Filter._MinRange;
                    _This->MaxRange = _Filter._MaxRange;
                  }
                } else {
                  CCI_SET_ERROR_MSG(_TXT("Invalid histogram index !"));
                  CCI_RETURN_INVALID_PARAM()
                }
              }
            } else
              SET_LAST_ERROR(dmFunctorErrors::E_SPLIT_ERROR);

            RETURN_ERROR_CHECK()
          }      
          else
          {
            cci_Object* _savedList = _IParams.GetImageList();
            // Extract the channel, setting index to -1 should store image in the 
            // buffer
            _IParams.SetImageList(_ColorSpace);
            if(_ColorSpace.Extract(_IParams,channel,_NOINDEX_)) {
               img = buffer->Buffer();
               roi = buffer->BufferRgn();
            } else
              SET_LAST_ERROR(dmFunctorErrors::E_BAD_CHANNEL);

            // Restore list to params
            _IParams.SetImageList(_savedList);
          }
        } 
        else
         SET_LAST_ERROR(dmFunctorErrors::E_BAD_COLORSPACE);
      }

      //------------------------------------------
      // Compute histogram on scalar
      //------------------------------------------
      {
        // Get histogram at specified Index
        _Histogram = _This->Histo[CCI_P(index)];
        if(_Histogram==NULL) {
           CCI_SET_ERROR_MSG(_TXT("Invalid histogram index !"));
           CCI_RETURN_INVALID_PARAM()
        }

        dmDensityMap _Filter(*_Histogram,_This->MinRange,_This->MaxRange,
                            (_This->ScanFlags & CCIV_Histograms_AutoRange)!=0 ||
                             _This->MinRange >= _This->MaxRange);

        // Compute histogram     
        dmBufferParameters _params(*buffer,*img,roi);
        if(!_Filter.Apply(_params))
          SET_LAST_ERROR(dmFunctorErrors::E_BAD_IMAGE);
        else {
          _This->MinRange = _Filter._MinRange;
          _This->MaxRange = _Filter._MaxRange;
        }
      }
      //------------------------------------------

    } else 
      SET_LAST_ERROR(dmFunctorErrors::E_NO_IMAGE);
  } else 
    SET_LAST_ERROR(dmFunctorErrors::E_BAD_PARAM);

  RETURN_ERROR_CHECK()
}
//---------------------------------------------------------------------
// Method: Extend
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Histograms, Extend )
{
  CCI_INSTANCE(Histograms)

  SET_LAST_ERROR(dmFunctorErrors::E_NO_ERROR);

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*       img    = _IParams.GetImage();
    dmImageBuffer* buffer = _IParams.GetImageBuffer();
    dmRegion*      rgn    = _IParams.GetRoi();

    if(img) 
    {  
      dmASSERT( buffer != NULL );

      dmExtendMap _Filter(static_cast<dm_real>(CCI_P(rmin)),
                          static_cast<dm_real>(CCI_P(rmax)),
                          _This->MinRange,_This->MaxRange);

      dmColorSpaceFunctor<dmExtendMap> _Functor(_Filter);
      if(!_Functor.Apply(_IParams,buffer,img,rgn,true))
        SET_LAST_ERROR(_Functor.ReturnedError);
 
    } else 
      SET_LAST_ERROR(dmFunctorErrors::E_NO_IMAGE);
  } else 
    SET_LAST_ERROR(dmFunctorErrors::E_BAD_PARAM);

  RETURN_ERROR_CHECK()
}  
//---------------------------------------------------------------------
// Method: Transform    
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Histograms, Transform )
{
  CCI_INSTANCE(Histograms)

  if(CCI_P(data)==NULL) 
    CCI_RETURN_INVALID_PARAM()

  if(CCI_P(count)<dmLUT8_MAX_COLORS) 
    CCI_RETURN_INVALID_PARAM()

  SET_LAST_ERROR(dmFunctorErrors::E_NO_ERROR);

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*       img    = _IParams.GetImage();
    dmImageBuffer* buffer = _IParams.GetImageBuffer();
    dmRegion*      rgn    = _IParams.GetRoi();

    if(img) 
    {      
      dmColorIndexTable ctable;
      unsigned short imap;
      dm_uint*        values = CCI_P(data);
      for(int i=0;i<dmLUT8_MAX_COLORS;++i) {
        imap = static_cast<unsigned short>(values[i]);
        if(imap>dmLUT8_MAX_COLOR_INDEX) imap = dmLUT8_MAX_COLOR_INDEX;
        ctable[i]  = imap; 
      }

      dmASSERT( buffer != NULL ); 

      dmTransformMap _Filter(ctable,_This->MinRange,_This->MaxRange);

      dmColorSpaceFunctor<dmTransformMap> _Functor(_Filter);
      if(!_Functor.Apply(_IParams,buffer,img,rgn,true))
        SET_LAST_ERROR(_Functor.ReturnedError);

    } else 
      SET_LAST_ERROR(dmFunctorErrors::E_NO_IMAGE);
  } else 
    SET_LAST_ERROR(dmFunctorErrors::E_BAD_PARAM);

  RETURN_ERROR_CHECK()
}  
//---------------------------------------------------------------------
// Method: Histogram   
// Retourne l'histogramme spécifié par MapIndex
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Histograms, Histogram )
{
  CCI_INSTANCE(Histograms)

  dmHistogram* _Histogram = _This->Histo[CCI_P(index)];
  if(_Histogram==NULL) 
    SET_LAST_ERROR(dmFunctorErrors::E_BAD_CHANNEL);
  else {
    SET_LAST_ERROR(dmFunctorErrors::E_NO_ERROR);
    CCI_RETVAL_P(data)  = _Histogram->Data();
    CCI_RETVAL_P(count) = dmHistogram::hsize;
  }

  if(!CHECK_LAST_ERROR(CCI_P(Id)))
    CCI_RETURN_OK()

  CCI_RETURN_FAIL()
}  
//---------------------------------------------------------------------
// Method: Equalize    
//---------------------------------------------------------------------

//---------------------------------------------
// Define a specific functor for equalization
//---------------------------------------------
class dmEqualize : public dmDensityMap,
                   public dmTransformMap
{
  public:
   dmEqualize( dmHistogram&    anHistogram,
               dmColorMapArray aMap,
               dm_real&        minRange, 
               dm_real&        maxRange,
               bool            autoScan )
   :dmDensityMap(anHistogram,minRange,maxRange,autoScan)
   ,dmTransformMap(aMap,minRange,maxRange) {}

   bool Apply( dmBufferParameters& _Params ) 
   {
     if(dmDensityMap::Apply(_Params)) {
       this->_Histogram.Equalize( this->_Map );
       return dmTransformMap::Apply(_Params);
     }
     return false;
   }
};
//---------------------------------------------
static CCI_IMPL_METHOD( Histograms, Equalize )
{
  CCI_INSTANCE(Histograms)

  SET_LAST_ERROR(dmFunctorErrors::E_NO_ERROR);

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*       img    = _IParams.GetImage();
    dmImageBuffer* buffer = _IParams.GetImageBuffer();
    dmRegion*      rgn    = _IParams.GetRoi();

    if(img) 
    {  
      dmASSERT(buffer!=NULL);
       
      dmHistogram   Histogram;
      dmColorIndex  Cmap[dmHistogram::hsize];

      dmEqualize _Filter(Histogram,Cmap,_This->MinRange,_This->MaxRange,
                        (_This->ScanFlags & CCIV_Histograms_AutoRange)!=0);

      dmColorSpaceFunctor<dmEqualize> _Functor(_Filter);
      if(!_Functor.Apply(_IParams,buffer,img,rgn,true))
        SET_LAST_ERROR(_Functor.ReturnedError);

    } else 
      SET_LAST_ERROR(dmFunctorErrors::E_NO_IMAGE);
  } else 
    SET_LAST_ERROR(dmFunctorErrors::E_BAD_PARAM);

  RETURN_ERROR_CHECK()
}  
//---------------------------------------------------------------------
// Method: SetRange    
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Histograms, SetRange )
{
  CCI_INSTANCE(Histograms)
  _This->MaxRange = static_cast<dm_real>(CCI_P(rmax));
  _This->MinRange = static_cast<dm_real>(CCI_P(rmin));
  CCI_RETURN_OK()
}  
//---------------------------------------------------------------------
// Method: GetRange    
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Histograms, GetRange )
{
  CCI_INSTANCE(Histograms)
  CCI_RETVAL_P(rmax) = _This->MaxRange;
  CCI_RETVAL_P(rmin) = _This->MinRange;
  CCI_RETURN_OK()
}  
//---------------------------------------------------------------------
// Method: Threshold    
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Histograms, Threshold )
{
  CCI_INSTANCE(Histograms)

  SET_LAST_ERROR(dmFunctorErrors::E_NO_ERROR);

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*       img    = _IParams.GetImage();
    dmImageBuffer* buffer = _IParams.GetImageBuffer();
    dmRegion*      rgn    = _IParams.GetRoi();

    if(img) 
    {  
      dm_uint channel = _IParams.GetChannel();

      // Apply threshold on multichannel data ?

      if(!dmIsPixelFormatScalar(img->PixelFormat()) && 
          channel==dmIParameters::flAllChannels)
        SET_LAST_ERROR(dmFunctorErrors::E_BAD_CHANNEL);
      else 
      {
        dmRegion _Result;
 
        dmThreshold _Filter(CCI_P(rmin),CCI_P(rmax),_Result);

        dmColorSpaceFunctor<dmThreshold> _Functor(_Filter);
        if(!_Functor.Apply(_IParams,buffer,img,rgn,false))
           SET_LAST_ERROR(_Functor.ReturnedError);
        else
          _IParams.SetMask(&_Result);
      }
    } else 
      SET_LAST_ERROR(dmFunctorErrors::E_NO_IMAGE);
  } else 
    SET_LAST_ERROR(dmFunctorErrors::E_BAD_PARAM);

  RETURN_ERROR_CHECK()
}  
//---------------------------------------------------------------------
// Method: HysteresisThreshold    
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Histograms, HysteresisThreshold )
{
  CCI_INSTANCE(Histograms)

  SET_LAST_ERROR(dmFunctorErrors::E_NO_ERROR);

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*       img    = _IParams.GetImage();
    dmImageBuffer* buffer = _IParams.GetImageBuffer();
    dmRegion*      rgn    = _IParams.GetRoi();

    if(img) 
    {  
      dm_uint channel = _IParams.GetChannel();

      // Apply threshold on multichannel data ?

      if(!dmIsPixelFormatScalar(img->PixelFormat()) && 
          channel==dmIParameters::flAllChannels)
        SET_LAST_ERROR(dmFunctorErrors::E_BAD_CHANNEL);
      else 
      {
        dmRegion _Result;
 
        dmHysteresisThreshold _Filter(CCI_P(rmin),CCI_P(rmax),
                                      CCI_P(connect),
                                      _Result);

        dmColorSpaceFunctor<dmHysteresisThreshold> _Functor(_Filter);
        if(!_Functor.Apply(_IParams,buffer,img,rgn,false))
           SET_LAST_ERROR(_Functor.ReturnedError);
        else
          _IParams.SetMask(&_Result);
      }
    } else 
      SET_LAST_ERROR(dmFunctorErrors::E_NO_IMAGE);
  } else 
    SET_LAST_ERROR(dmFunctorErrors::E_BAD_PARAM);

  RETURN_ERROR_CHECK()
}  
//---------------------------------------------------------------------
// Method: FillChannel    
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Histograms, FillChannel )
{
  CCI_INSTANCE(Histograms)

  SET_LAST_ERROR(dmFunctorErrors::E_NO_ERROR);

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    if(_IParams.FillChannel(CCI_P(value)))
       CCI_RETURN_OK()
    else
       CCI_RETURN_FAIL()
/*
    dmImage*       img    = _IParams.GetImage();
    dmImageBuffer* buffer = _IParams.GetImageBuffer();
    dmRegion*      rgn    = _IParams.GetRoi();

    if(img) {

      dmASSERT(buffer!=NULL);

      dmFillScalar _Filter(CCI_P(value));

      dmColorSpaceFunctor<dmFillScalar> _Functor(_Filter);
      if(!_Functor.Apply(_IParams,buffer,img,rgn,true))
          SET_LAST_ERROR(_Functor.ReturnedError);

    } else 
      SET_LAST_ERROR(dmFunctorErrors::E_NO_IMAGE);
*/
  } else 
    SET_LAST_ERROR(dmFunctorErrors::E_BAD_PARAM);

  RETURN_ERROR_CHECK()
}
//---------------------------------------------------------------------
// Method: InvertMap   
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Histograms, InvertMap )
{
  CCI_INSTANCE(Histograms)

  SET_LAST_ERROR(dmFunctorErrors::E_NO_ERROR);

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    if(_IParams.InvertPixels(_This->MinRange,_This->MaxRange))
       CCI_RETURN_OK()
    else
       CCI_RETURN_FAIL()
/*
    dmImage*       img    = _IParams.GetImage();
    dmImageBuffer* buffer = _IParams.GetImageBuffer();
    dmRegion*      rgn    = _IParams.GetRoi();

    if(img) 
    {  
      dmASSERT(buffer!=NULL);

      dmInvertMap _Filter(_This->MinRange,_This->MaxRange);

      dmColorSpaceFunctor<dmInvertMap> _Functor(_Filter);
      if(!_Functor.Apply(_IParams,buffer,img,rgn,true))
        SET_LAST_ERROR(_Functor.ReturnedError);

    } else 
      SET_LAST_ERROR(dmFunctorErrors::E_NO_IMAGE);
*/
  } else 
    SET_LAST_ERROR(dmFunctorErrors::E_BAD_PARAM);

  RETURN_ERROR_CHECK()
} 
//---------------------------------------------------------------------
// Method: Specify   
//---------------------------------------------------------------------

//---------------------------------------------
// Define a specific functor for histogram 
// specification
//---------------------------------------------
class dmSpecifyMap : public dmDensityMap,
                     public dmTransformMap
{
  public:
   dmColorMapArray _Input;
   dmSpecifyMap( dmHistogram&    anHistogram,
                 dmColorMapArray aMap,
                 dmColorMapArray anInput,
                 dm_real&        minRange, 
                 dm_real&        maxRange,
                 bool            autoScan )
   :dmDensityMap(anHistogram,minRange,maxRange,autoScan)
   ,dmTransformMap(aMap,minRange,maxRange)
   ,_Input(anInput)
   {}

   bool Apply( dmBufferParameters& _Params ) 
   {
     // Get histogram from image
     if(dmDensityMap::Apply(_Params)) 
     {
       this->_Histogram.Specify( this->_Map, _Input );
       return dmTransformMap::Apply(_Params);
     }
     return false;
   }
};
//---------------------------------------------
static CCI_IMPL_METHOD( Histograms, Specify )
{
  CCI_INSTANCE(Histograms)

  SET_LAST_ERROR(dmFunctorErrors::E_NO_ERROR);

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*       img    = _IParams.GetImage();
    dmImageBuffer* buffer = _IParams.GetImageBuffer();
    dmRegion*      rgn    = _IParams.GetRoi();

    if(img) 
    {  
      dmASSERT(buffer!=NULL);
 
      // Get histogram at specified Index
      dmHistogram* _Histogram = _This->Histo[CCI_P(index)];
      if(_Histogram==NULL) {
         CCI_SET_ERROR_MSG(_TXT("Invalid histogram index !"));
         CCI_RETURN_INVALID_PARAM()
      }

      dmHistogram   Histogram;
      dmColorIndex  Cmap[dmHistogram::hsize];
      dmColorIndex  Inpt[dmHistogram::hsize];

      // Compute equalized input transformation
      _Histogram->Equalize(Inpt);

      dmSpecifyMap _Filter(Histogram,Cmap,Inpt,_This->MinRange,_This->MaxRange,
                          (_This->ScanFlags & CCIV_Histograms_AutoRange)!=0);

      dmColorSpaceFunctor<dmSpecifyMap> _Functor(_Filter);
      if(!_Functor.Apply(_IParams,buffer,img,rgn,true))
        SET_LAST_ERROR(_Functor.ReturnedError);

    } else 
      SET_LAST_ERROR(dmFunctorErrors::E_NO_IMAGE);
  } else 
    SET_LAST_ERROR(dmFunctorErrors::E_BAD_PARAM);

  RETURN_ERROR_CHECK()
} 
//---------------------------------------------------------------------
// Method: Gamma    
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Histograms, Gamma )
{
  CCI_INSTANCE(Histograms)

  SET_LAST_ERROR(dmFunctorErrors::E_NO_ERROR);

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*       img    = _IParams.GetImage();
    dmImageBuffer* buffer = _IParams.GetImageBuffer();
    dmRegion*      rgn    = _IParams.GetRoi();

    if(img) {

      dmASSERT(buffer!=NULL);

      dmGammaCorrection _Filter(_This->MinRange,_This->MaxRange,CCI_P(gamma));

      dmColorSpaceFunctor<dmGammaCorrection> _Functor(_Filter);
      if(!_Functor.Apply(_IParams,buffer,img,rgn,true))
          SET_LAST_ERROR(_Functor.ReturnedError);

    } else 
      SET_LAST_ERROR(dmFunctorErrors::E_NO_IMAGE);
  } else 
    SET_LAST_ERROR(dmFunctorErrors::E_BAD_PARAM);

  RETURN_ERROR_CHECK()
}
//---------------------------------------------------------------------
// Method: Contrast    
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Histograms, Contrast )
{
  CCI_INSTANCE(Histograms)

  SET_LAST_ERROR(dmFunctorErrors::E_NO_ERROR);

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*       img    = _IParams.GetImage();
    dmImageBuffer* buffer = _IParams.GetImageBuffer();
    dmRegion*      rgn    = _IParams.GetRoi();

    if(img) {

      dmASSERT(buffer!=NULL);

      dmEnhanceContrast _Filter(_This->MinRange,_This->MaxRange,
                                CCI_P(brightness),
                                CCI_P(contrast));

      // Apply to all channels
      if(_IParams.GetChannel() == dmIParameters::flAllChannels)
      {
         dmBufferParameters _params(*buffer,*img,*rgn);
         if(!_Filter.Apply(_params)) 
             CCI_RETURN_FAIL()
      }
      else // Apply to specific channel using Colorspace decomposition
      {
        dmColorSpaceFunctor<dmEnhanceContrast> _Functor(_Filter);
        if(!_Functor.Apply(_IParams,buffer,img,rgn,true))
            SET_LAST_ERROR(_Functor.ReturnedError);
      }

    } else 
      SET_LAST_ERROR(dmFunctorErrors::E_NO_IMAGE);
  } else 
    SET_LAST_ERROR(dmFunctorErrors::E_BAD_PARAM);

  RETURN_ERROR_CHECK()
}
//---------------------------------------------------------------------
// Method: OptimalThreshold    
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Histograms, OptimalThreshold )
{
  CCI_INSTANCE(Histograms)

  SET_LAST_ERROR(dmFunctorErrors::E_NO_ERROR);

  dmHistogram* _Histogram = NULL;

  // Get histogram at specified Index
  _Histogram = _This->Histo[CCI_P(index)];
  if(_Histogram==NULL) {
    SET_LAST_ERROR(dmFunctorErrors::E_BAD_CHANNEL);
  } 
  else 
  {
    dm_real  background = CCI_P(background);
    dm_real  hmax = static_cast<dm_real>(dmHistogram::hmax);   
    dm_uint8 thr;
 
    if(background >= _This->MaxRange)
      thr = dmHistogram::hmax;
    else if( background <= _This->MinRange )
      thr = 0;
    else
      thr  = static_cast<dm_uint8>( hmax*(background-_This->MinRange)/(_This->MaxRange - _This->MinRange) + 0.5 );

    daim::histogram_value val = daim::bimodal_optimal_thrindex(_Histogram->Data(),thr);
  
    if(_This->MaxRange > _This->MinRange)
      CCI_RETVAL_P(value) = _This->MinRange + ((_This->MaxRange-_This->MinRange)/hmax) * val; 
    else
      CCI_RETVAL_P(value) = val;
  }

  RETURN_ERROR_CHECK()

}
//---------------------------------------------------------------------
// Method: RGBThreshold    
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Histograms, RGBThreshold )
{
  CCI_INSTANCE(Histograms)

  SET_LAST_ERROR(dmFunctorErrors::E_NO_ERROR);

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*  img = _IParams.GetImage();
    dmRegion* rgn = _IParams.GetRoi();

    if(img) 
    {  
      dmRGBColor _Color;
      _Color.red   = static_cast<dm_uint8>(CCI_P(red));
      _Color.green = static_cast<dm_uint8>(CCI_P(green));
      _Color.blue  = static_cast<dm_uint8>(CCI_P(blue));

      dm_float rr = static_cast<dm_float>(daim::max(CCI_P(rradius),0.5));
      dm_float rg = static_cast<dm_float>(daim::max(CCI_P(gradius),0.5));
      dm_float rb = static_cast<dm_float>(daim::max(CCI_P(bradius),0.5));

      dmRegion _Result;

      if(!dmRGBThreshold(*img,*rgn,_Color,rr,rg,rb,_Result))
         SET_LAST_ERROR(dmFunctorErrors::E_BAD_IMAGE);
      else
        _IParams.SetMask(&_Result);

    } else 
      SET_LAST_ERROR(dmFunctorErrors::E_NO_IMAGE);
  } else 
    SET_LAST_ERROR(dmFunctorErrors::E_BAD_PARAM);

  RETURN_ERROR_CHECK()
}    
//---------------------------------------------------------------------
// Method: GetValues    
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Histograms, GetValues )
{
  CCI_INSTANCE(Histograms)

  SET_LAST_ERROR(dmFunctorErrors::E_NO_ERROR);

  dmIStorageMap _IStorage;
  if(_IStorage.QueryInterface(CCI_P(storage)))
  {
      dm_real* store = _IStorage.GetCol(CCI_P(col),dmHistogram::hsize);

      if(store == NULL)
         CCI_RETURN_FAIL()

      store++; // Start at index 0

      if(CCI_P(index) < 0) // Return density values
      { 
        if(_This->MinRange < _This->MaxRange) 
        {
          dm_real fminrange = _This->MinRange; 
          dm_real fDelta    = ((_This->MaxRange-_This->MinRange)/(dmHistogram::hsize-1));
          for(int index=0;index<dmHistogram::hsize;++index)
             store[index] = fminrange + fDelta * index;
        } else {
          for(int index=0;index<dmHistogram::hsize;++index)
              store[index] = index;   
        }
      }
      else // Return distributions values
      { 
        dmHistogram* _Histogram = NULL;
       
        _Histogram = _This->Histo[CCI_P(index)];
        if(_Histogram!=NULL) 
        {
          for(int index=0;index<dmHistogram::hsize;++index)
             store[index] = (*_Histogram)[index];  
        } else
          SET_LAST_ERROR(dmFunctorErrors::E_BAD_CHANNEL);
      }
  } else 
    SET_LAST_ERROR(dmFunctorErrors::E_BAD_PARAM); 

  RETURN_ERROR_CHECK()   
}
//---------------------------------------------------------------------
// Method: Scale    
//---------------------------------------------------------------------

// Define the following functor so that we can use the 
// colorspace separation functor
//---------------------------------------------
class dmScaleFunctor 
{
  dm_real Min;
  dm_real Max;
  
  public:
  
  dmScaleFunctor( dm_real _Min, dm_real _Max )
  : Min(_Min)
  , Max(_Max) {}
  
  bool Apply( dmBufferParameters& _Params ) {
     return dmScaleImage( _Params, this->Min, this->Max );
  }      
};
//---------------------------------------------
static CCI_IMPL_METHOD( Histograms, Scale )
{
  CCI_INSTANCE(Histograms)

  SET_LAST_ERROR(dmFunctorErrors::E_NO_ERROR);

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*       img    = _IParams.GetImage();
    dmImageBuffer* buffer = _IParams.GetImageBuffer();
    dmRegion*      rgn    = _IParams.GetRoi();

    if(img) {

      dmASSERT(buffer!=NULL);

      dmScaleFunctor _Filter(CCI_P(minval),CCI_P(maxval));

      dmColorSpaceFunctor<dmScaleFunctor> _Functor(_Filter);
      if(!_Functor.Apply(_IParams,buffer,img,rgn,true))
          SET_LAST_ERROR(_Functor.ReturnedError);

    } else 
      SET_LAST_ERROR(dmFunctorErrors::E_NO_IMAGE);
  } else 
    SET_LAST_ERROR(dmFunctorErrors::E_BAD_PARAM);

  RETURN_ERROR_CHECK()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( Histograms )  	
  CCI_REGISTER_METHOD_PP( Histograms, Scan        )      
  CCI_REGISTER_METHOD_PP( Histograms, Extend      )
  CCI_REGISTER_METHOD_PP( Histograms, Transform   )
  CCI_REGISTER_METHOD_PP( Histograms, Histogram   )
  CCI_REGISTER_METHOD_PP( Histograms, Equalize    )
  CCI_REGISTER_METHOD_PP( Histograms, GetRange    )
  CCI_REGISTER_METHOD_PP( Histograms, SetRange    )
  CCI_REGISTER_METHOD_PP( Histograms, Threshold   )
  CCI_REGISTER_METHOD_PP( Histograms, FillChannel )
  CCI_REGISTER_METHOD_PP( Histograms, InvertMap   )
  CCI_REGISTER_METHOD_PP( Histograms, Specify     )
  CCI_REGISTER_METHOD_PP( Histograms, Gamma       )

  CCI_REGISTER_METHOD_PP( Histograms, Contrast         )
  CCI_REGISTER_METHOD_PP( Histograms, OptimalThreshold )
  CCI_REGISTER_METHOD_PP( Histograms, RGBThreshold     )
  CCI_REGISTER_METHOD_PP( Histograms, GetValues        )
  CCI_REGISTER_METHOD_PP( Histograms, Scale            )
  
  CCI_REGISTER_METHOD_PP( Histograms, HysteresisThreshold )
CCI_NEXT_DISPATCH_MAP()

//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( Histograms )
{
  CCI_INVOKE_FACTORY( Histograms )
}
//---------------------------------------------------------------------
