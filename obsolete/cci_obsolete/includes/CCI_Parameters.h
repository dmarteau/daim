#ifndef CCI_Parameters_h
#define CCI_Parameters_h

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
// File         : CCI_Parameters.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------
#define CCIC_Parameters  _TXT("parameters.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( Parameters, Notify )
  
  CCI_MODULE_UUID( Parameters, 6b551c9d-7283-472c-8e56-aefc436e443a)

  CCI_MODULE_VERSION( Parameters, 1 ) 

  /* Methods */
  CCI_DECL_METHOD_PP(Parameters, 1,  UseNameSpace, _in CONST char_t* url      )
  CCI_DECL_METHOD_NP(Parameters, 5,  Refresh     ) 
  CCI_DECL_METHOD_NP(Parameters, 7,  MaskToRoi   )
  CCI_DECL_METHOD_NP(Parameters, 8,  KillRoi     )
  CCI_DECL_METHOD_NP(Parameters, 9,  HasMask     )

  CCI_DECL_METHOD_PP(Parameters,10,  CreateRoi, 
      _in dm_uint type 
      _in dm_ptr  points 
      _in dm_uint count 
  )

  CCI_DECL_METHOD_PP(Parameters,11, StoreBuffer , _in dm_uint index ) 
  CCI_DECL_METHOD_PP(Parameters,12, SelectBuffer, _in dm_uint index ) 

  CCI_DECL_METHOD_NP(Parameters,13, RoiToMask  )
  CCI_DECL_METHOD_NP(Parameters,14, InvertMask )

  CCI_DECL_METHOD_PP(Parameters,15, Fill       , _in dm_bool foreground )

  CCI_DECL_METHOD_PP(Parameters,16, StoreMask  , _in dm_uint index _in dm_int op )
  CCI_DECL_METHOD_PP(Parameters,17, CallMask   , _in dm_uint index _in dm_int op )
  CCI_DECL_METHOD_PP(Parameters,18, ClearMask  , _in dm_uint index )

  CCI_DECL_METHOD_NP(Parameters,19, OutlineRoi )

  CCI_DECL_METHOD_PP(Parameters,20, StoreSelection, _in dm_uint index )

  CCI_DECL_METHOD_NP(Parameters,21, PushMask   )
  CCI_DECL_METHOD_NP(Parameters,22, QueueMask  )
  CCI_DECL_METHOD_NP(Parameters,23, PopMask    )
  CCI_DECL_METHOD_PP(Parameters,24, SwapMask   , _in dm_uint index )
  CCI_DECL_METHOD_PP(Parameters,25, PopMaskOp  , _in dm_int op     )
  CCI_DECL_METHOD_NP(Parameters,26, RotateMask )

  CCI_DECL_METHOD_PP(Parameters,27, GetImageSize,
     _in  dm_int   format 
     _in  dm_int   width
     _in  dm_int   height
     _out dm_size  size
  )

  CCI_DECL_METHOD_PP(Parameters,28, CreateImageBuffer , 
     _in dm_uint padding
     _in dm_int  padding_mode 
  )

  CCI_DECL_METHOD_NP(Parameters,29, RestoreImageBuffer )
  CCI_DECL_METHOD_NP(Parameters,30, ClearImageBuffer   )

  CCI_DECL_METHOD_NP(Parameters,32, KillMask   )

  CCI_DECL_METHOD_PP(Parameters,33, FillChannel, 
    _in dm_real value
    _in dm_real alpha
  )

  CCI_DECL_METHOD_PP(Parameters,34, InvertPixels,
    _in dm_real minrange
    _in dm_real maxrange
  )

  CCI_DECL_METHOD_NP( Parameters, 35, InvertRoi )

  CCI_DECL_METHOD_PP( Parameters, 36, GetImageData      , 
      _out dm_int  format 
      _out dm_uint width  
      _out dm_uint height 
      _out dm_int  stride 
      _out dm_ptr  scan0
  )

  CCI_DECL_METHOD_PP(Parameters,37, CreateMask,
    _in dm_uint index 
    _in dm_uint predicat
    _in dm_real value
    _in dm_int  op
  )


  CCI_DECL_METHOD_PP(Parameters,38, GetRectSelection, 
      _out dm_int left 
      _out dm_int top 
      _out dm_int right
      _out dm_int bottom 
  )

  CCI_DECL_METHOD_PP(Parameters,39, FillRGB, 
    _in dm_uint16 red 
    _in dm_uint16 green 
    _in dm_uint16 blue 
    _in dm_real   alpha
  )

  CCI_DECL_METHOD_NP(Parameters,40, ReleaseImageBuffer )

  #ifdef DAIM_UTILITIES
  /* Use shared link for efficiency */
  CCI_DECL_METHOD_PP(Parameters,50,  GetImageA, 
     _in dm_param         me
     _in dmLink<dmImage>* link
  )
  #endif // DAIM_UTILITIES


  /* Attributs */
  CCI_DECL_ATTR(Parameters, 1,  Image       , dmImage*          ,[.SG.] )
  CCI_DECL_ATTR(Parameters, 2,  Mask        , dmRegion*         ,[.SG.] )
  CCI_DECL_ATTR(Parameters, 3,  Roi         , dmRegion*         ,[.SGN] )
  CCI_DECL_ATTR(Parameters, 4,  ImageList   , dm_cci(ImageList) ,[.SGN] )
  CCI_DECL_ATTR(Parameters, 5,  MaskBuffer  , dm_cci(MaskBuffer),[.SGN] )
  CCI_DECL_ATTR(Parameters, 6,  ImageBuffer , dmImageBuffer*    ,[..G.] )
  CCI_DECL_ATTR(Parameters, 7,  ColorSpace  , dm_cci(ColorSpace),[.SGN] )
  CCI_DECL_ATTR(Parameters, 8,  Channel     , dm_uint           ,[.SGN] )

  /* Special values */
  #define CCIV_Parameters_AllChannels TGZ_DEFAULT

CCI_END_MODULE_DECL( Parameters )

#endif // __doxygen
//----------------------------------------------------------------

#ifdef __cplusplus

#include "CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Handle parameters for processing operations.
 * 
 *  This interface expose methods for packing the parameters
 *  common to all processing operations. It can be used to set custom 
 *  mask and image parameters as well as retrieving informations in global namespace
 *  or from the current image info. If a parameters is not set and is required by 
 *  by a processing operation, it will be searched in the global namespace and from
 *  the current image info. 
 */
//------------------------------------------------------------------------------------
class dmIParameters : public dmINotify
{    
  public:
   dmDECLARE_GENERIC_IINTERFACE(Parameters)

   enum Constants {
     flAllChannels = CCIV_Parameters_AllChannels
   };

   //--------------------------------------------------------------
   /*! \brief Set the url used as namespace for retrieving objects. */
   //--------------------------------------------------------------
   bool UseNameSpace( CONST char_t* url )  { 
      CCI_RETURN_IMETHOD_PP_OK(Parameters,UseNameSpace, _in url ) 
   }

   //--------------------------------------------------------------
   /*! \brief Set \a image as the current Image parameter.
    *
    *  You must ensure that the \a image pointer reference a valid object
    *  while used by the parameters object.
    */
   //--------------------------------------------------------------
   bool SetImage( dmImage*  image )       { return CCI_ISET(Parameters,Image,image); }

   //--------------------------------------------------------------
   /*! \brief Set the mask parameter
    *
    *  If \a mask is NULL, then the mask parameter will be obtained
    *  from the default mask of the dmIMaskBuffer internal object.
    *  \sa SetMaskBuffer
    */
   //--------------------------------------------------------------
   bool SetMask ( const dmRegion* mask  ) { return CCI_ISET(Parameters,Mask, mask); }

   //--------------------------------------------------------------
   /*! \brief Set the ROI parameter. */
   //--------------------------------------------------------------
   bool SetRoi( const dmRegion* roi   ) { return CCI_ISET(Parameters,Roi, roi ); }
  
   //--------------------------------------------------------------
   /*! \brief Create a ROI
    *
    *  \param type   : the type of the ROI to be created
    *  \param points : the object representing the coordinates of the ROI   
    */
   //--------------------------------------------------------------
   bool CreateRoi( dm_uint type, dm_ptr points, dm_uint count = 1 ) {
      CCI_RETURN_IMETHOD_PP_OK(Parameters,CreateRoi, _in type _in points _in count )
   }

   //--------------------------------------------------------------
   /*! \brief Return the current image parameter. */
   //--------------------------------------------------------------
   dmImage*  GetImage() { return CCI_IGET(Parameters,Image,(dmImage* )NULL); }

   //--------------------------------------------------------------
   /*! \brief Return the current mask parameter. */
   //--------------------------------------------------------------
   dmRegion* GetMask () { return CCI_IGET(Parameters,Mask ,(dmRegion*)NULL); }

   //--------------------------------------------------------------
   /*! \brief Return the current ROI parameter. */
   //--------------------------------------------------------------
   dmRegion* GetRoi  () { return CCI_IGET(Parameters,Roi  ,(dmRegion*)NULL); }

   //--------------------------------------------------------------
   /*! \brief Set the imagelist used by this parameter object.
    *
    *  \param imagelist a valid interface to a dmIImageList object.
    */
   //--------------------------------------------------------------
   bool SetImageList ( dm_cci(ImageList) imagelist ) {
      return CCI_ISET(Parameters,ImageList ,imagelist); 
   }

   //--------------------------------------------------------------
   /*! \brief Set the mask buffers used by this parameter object.
    *
    *  \param maskbuffer a valid interface to a dmIMaskBuffer object.
    */
   //--------------------------------------------------------------
   bool SetMaskBuffer( dm_cci(MaskBufer) maskbuffer) { 
       return CCI_ISET(Parameters,MaskBuffer,maskbuffer); 
   }

   //--------------------------------------------------------------
   /*! \brief Return the imagelist object held by this dmIParameters object.
    *  \note As an exception to rules, the parameter object do not call AddRef() 
    *   before returning instance of the imagelist object.
    *
    *  \sa SetImageList()
    *  \sa dmIImageList()
    */
   //--------------------------------------------------------------   
   dm_cci(ImageList) GetImageList () { 
      return CCI_IGET(Parameters,ImageList ,(cci_Object*)NULL); 
   }

   //--------------------------------------------------------------
   /*! \brief Return the mask buffer interface used by this dmIParameters object.
    *
    *  \sa SetMaskBuffer()
    */
   //--------------------------------------------------------------   
   dm_cci(MaskBuffer) GetMaskBuffer() { 
     return CCI_IGET(Parameters,MaskBuffer,(cci_Object*)NULL); 
   }

   //--------------------------------------------------------------
   /*! \brief Transform the current mask into ROI                */
   //--------------------------------------------------------------
   bool MaskToRoi() { CCI_RETURN_IMETHOD_NP_OK(Parameters,MaskToRoi) }

   //--------------------------------------------------------------
   /*! \brief Transform the current ROI into mask                */
   //--------------------------------------------------------------
   bool RoiToMask() { CCI_RETURN_IMETHOD_NP_OK(Parameters,RoiToMask) }

   //--------------------------------------------------------------
   /*! \brief Kill/Remove the current ROI                        */
   //--------------------------------------------------------------
   bool KillRoi() { CCI_RETURN_IMETHOD_NP_OK(Parameters,KillRoi ) }     

   //--------------------------------------------------------------
   /*! \brief Check if there is a valid current mask             */
   //--------------------------------------------------------------
   bool HasMask() { CCI_RETURN_IMETHOD_NP_OK(Parameters,HasMask ) }

   //--------------------------------------------------------------
   /*! \brief Store the curent image parameter in buffer \a index */
   //--------------------------------------------------------------
   bool StoreBuffer( dm_uint index ) {
     CCI_RETURN_IMETHOD_PP_OK(Parameters,StoreBuffer,_in index) 
   } 

   //--------------------------------------------------------------
   /*! \brief Select the image in buffer \a index as the current image */
   //--------------------------------------------------------------
   bool SelectBuffer( dm_uint index ) {
     CCI_RETURN_IMETHOD_PP_OK(Parameters,SelectBuffer,_in index) 
   }

   dmImageBuffer* GetImageBuffer() { return CCI_IGET(Parameters,ImageBuffer,(dmImageBuffer*)NULL); }

   //--------------------------------------------------------------
   /*! \brief Return the dmIColorSpace object used by this dmIParameters object
    *
    *  If no colorspaces was assigned, the RGB colorspace will be returned.
    *  \sa dmIColorSpace
    */
   //--------------------------------------------------------------
   dm_cci(ColorSpace) GetColorSpace() { 
      return CCI_IGET(Parameters,ColorSpace,(cci_Object*)NULL); 
   }

   //--------------------------------------------------------------
   /*! \brief Set a dmIColorSpace object used to handle operations on RGB images
    *
    *  The ColorSpace object will be used to split image into channels 
    *  components for subsequent use in image processing operations.
    *  \sa dmIColorSpace
    *  \sa dmIParameters::SetChannel()
    */
   //--------------------------------------------------------------
   bool SetColorSpace( dm_cci(ColorSpace) colorspace ) {
     return CCI_ISET(Parameters,ColorSpace,colorspace);
   }

   //--------------------------------------------------------------
   /*! \brief Set the channel used to handle operations on RGB images
    *
    *  A channel number always start at 1
    *  \sa SetColorSpace()
    */
   //--------------------------------------------------------------
   bool SetChannel( dm_uint channel ) { return CCI_ISET(Parameters,Channel,channel); }

   //--------------------------------------------------------------
   /*! \brief Return the channel used to handle operations on RGB images */
   //--------------------------------------------------------------
   dm_uint GetChannel() { return CCI_IGET(Parameters,Channel,(dm_uint)0); }

   //--------------------------------------------------------------
   /*! \brief Fill or Clear the current image according to the ROI */
   //--------------------------------------------------------------
   bool Fill( dm_bool foreground ) {
     CCI_RETURN_IMETHOD_PP_OK(Parameters,Fill, _in foreground )
   }

   //----------------------------------------------------------------
   /*! \brief Store a \e copy of the current mask at buffer index \a index. */
   //----------------------------------------------------------------
   bool StoreMask( dm_uint index,  dm_int op = 0 ) {
     CCI_RETURN_IMETHOD_PP_OK(Parameters,StoreMask, _in index _in op )
   }
 
   //----------------------------------------------------------------
   /*! \brief Retrieve a copy of the current mask from buffer index \a index. */
   //----------------------------------------------------------------
   bool CallMask(dm_uint index, dm_int op = 0 ) {
     CCI_RETURN_IMETHOD_PP_OK(Parameters,CallMask, _in index _in op )
   }

   //----------------------------------------------------------------
   /*! \brief Clear the current mask at buffer index \a index. */
   //----------------------------------------------------------------
   bool ClearMask( dm_uint index ) {
     CCI_RETURN_IMETHOD_PP_OK(Parameters,ClearMask,_in index)
   }

   bool OutlineROI() { CCI_RETURN_IMETHOD_NP_OK(Parameters,OutlineRoi) }

   //----------------------------------------------------------------
   /*! \brief Store a part of the image according to the current roi
    *
    *  Store the rectangular part of the current image corresponding
    *  to the bounding box of the current roi
    */ 
   //----------------------------------------------------------------
   bool StoreSelection( dm_uint index ) {
     CCI_RETURN_IMETHOD_PP_OK(Parameters,StoreSelection, _in index )
   }

   //----------------------------------------------------------------
   /*! \brief Return an estimation of the image size in byte
    *
    *  If \a height and \a width are set to -1 the rectangular part 
    *  the bounding box of the current roi is used.
    *  If \a format is set to 0, the format of the current image is used
    * 
    *  Example :
    *  \code  
    *    GetImageSize(0,-1,-1);
    *  \endcode 
    *  Return the size of the current image cropped to the bounding box
    *  of the current roi
    */ 
   //----------------------------------------------------------------
   dm_size GetImageSize( dm_int format, dm_int width, dm_int height )
   {
     IF_CCI_IMETHOD_RP(Parameters,GetImageSize, _in format _in width _in height _out(size) 0 )
     THEN return CCI_RETVAL(size);
     ELSE return 0;
     ENDIF
   }

   //----------------------------------------------------------------
   /*! \brief Create a buffer image using the current roi
    *
    * This method create a buffer by copying the part
    * of the image corresponding to the current roi in 
    * the internal buffer.
    * 
    * \see ReleaseImageBuffer
    * \see RestoreImageBuffer
    */
   //----------------------------------------------------------------
   bool CreateImageBuffer ( dm_int padding  = 0, dm_int padding_mode = 0 ) { 
     CCI_RETURN_IMETHOD_PP_OK(Parameters,CreateImageBuffer, _in padding _in padding_mode ) 
   }

   //----------------------------------------------------------------
   /*! \brief Restore the image from the internal buffer
    *
    * This method restore the image from the internal buffer
    * created by CreateImageBuffer(). The image is restored 
    * at the exact location of the roi used to create the buffer.
    * 
    * Note that this can be used to copy a specified region of an image into
    * another :
    *  \code
    *     dmIInterface<dmIParameters> _Params;
    *     ...
    *     _Params.SelectBuffer(0);      // Select the image in buffer 0
    *     _Params.CreateImageBuffer();  // Create the image buffer
    *     _Params.SelectBuffer(1);      // Select the image in buffer 1
    *     _Params.RestoreImageBuffer(); // Copy the image part from internal buffer
    *  \endcode
    * 
    *  Note that this method creates a temporary buffer for copying the selected
    *  part of the image. 
    * 
    * \see ReleaseImageBuffer
    * \see CreateImageBuffer
    * 
    */
    //----------------------------------------------------------------
    bool RestoreImageBuffer() { CCI_RETURN_IMETHOD_NP_OK(Parameters,RestoreImageBuffer ) }

   //----------------------------------------------------------------
   /*! \brief Clear internal buffer
    * 
    * \see CreateImageBuffer
    * \see RestoreImageBuffer
    */
   //----------------------------------------------------------------
   bool ClearImageBuffer() { CCI_RETURN_IMETHOD_NP_OK(Parameters,ClearImageBuffer   ) }

   //----------------------------------------------------------------
   /*! \brief Release internal buffer
    * 
    * \see CreateImageBuffer
    * \see RestoreImageBuffer
    */
   //----------------------------------------------------------------
   bool ReleaseImageBuffer() { CCI_RETURN_IMETHOD_NP_OK(Parameters,ReleaseImageBuffer   ) }

   //-------------------------------------------------------
   /*! \brief Set the image pixels values to the value \a value.
    *
    *  For rgb image, the modified channel is set according to
    *  the current channel (see dmIParameters::SetChannel)
    */
   //-------------------------------------------------------
   bool FillChannel( dm_real  value, dm_real alpha = 1.0 ) { 
     CCI_RETURN_IMETHOD_PP_OK(Parameters,FillChannel, _in value _in alpha )
   }

   //-------------------------------------------------------
   /*! \brief InvertPixels Invert pixels values
    *
    *  For rgb image, the modified channel is set according to
    *  the current channel (see dmIParameters::SetChannel)
    */
   //-------------------------------------------------------
   bool InvertPixels( dm_real  minrange, dm_real maxrange ) { 
     CCI_RETURN_IMETHOD_PP_OK(Parameters,InvertPixels,
        _in minrange 
        _in maxrange
     )
   }

   //-------------------------------------------------------
   /*! \brief Invert the current roi.
    *
    *  Invert,i.e take the complement of the current roi
    *  according to the image rectangle. 
    */
   //-------------------------------------------------------
   bool InvertRoi() { CCI_RETURN_IMETHOD_NP_OK(Parameters,InvertRoi); }

   //--------------------------------------------------------------
   /*! \brief Get image pixels data
    *  \param data         A dmImageData object that will hold pixels buffer infos. 
    *                      
    *  \note Any transformation involving buffer reallocation will result
    *        in invalid data !
    */ 
   //--------------------------------------------------------------
   bool GetImageData( dmImageData& data ) {
     IF_CCI_IMETHOD_RP(Parameters,GetImageData,_out(format) 0 _out(width) 0 _out(height) 0 _out(stride) 0 _out(scan0) 0 )
     THEN
       data.Width       = CCI_RETVAL(width);
       data.Height      = CCI_RETVAL(height);
       data.Stride      = CCI_RETVAL(stride);
       data.Scan0       = CCI_RETVAL(scan0); 
       data.PixelFormat = static_cast<EPixelFormat>(CCI_RETVAL(format));
     ENDIF_RETURN_BOOL()
   }

   //--------------------------------------------------------------
   /*! \brief Create a mask using a predicat on pixels values  */ 
   //--------------------------------------------------------------
   bool CreateMask( dm_uint index, dm_uint predicat, dm_real value, dm_int op = 0  ) { 
      CCI_RETURN_IMETHOD_PP_OK(Parameters,CreateMask, 
                              _in index 
                              _in predicat 
                              _in value 
                              _in op); 
   }
  
   //--------------------------------------------------------------
   /*! \brief Return the bounding rectangle ot the current roi  */ 
   //--------------------------------------------------------------
   bool GetRectSelection( dm_rect& rect ) {
     IF_CCI_IMETHOD_RP(Parameters,GetRectSelection, _out(left) 0 _out(top) 0 )
     THEN
       rect.top_left.x     = CCI_RETVAL(left);
       rect.top_left.y     = CCI_RETVAL(top);
       rect.bottom_right.x = CCI_RETVAL(right);
       rect.bottom_right.y = CCI_RETVAL(bottom); 
     ENDIF_RETURN_BOOL()
   }
   
   //-------------------------------------------------------
   /*! \brief Set the image pixels values to the given rgb value. */
   //-------------------------------------------------------
   bool FillRGB( dm_uint16  red, dm_uint16 green, dm_uint16 blue, dm_real alpha = 1.0 ) { 
     CCI_RETURN_IMETHOD_PP_OK(Parameters,FillRGB, 
                              _in red
                              _in green 
                              _in blue 
                              _in alpha )
   }


   #ifdef DAIM_UTILITIES

   bool GetImageA( dmLink<dmImage>& link ) {
     CCI_RETURN_IMETHOD_PP_OK(Parameters,GetImageA, _in CCI_ME_GET  _in &link ) 
   }

   bool SetImageA( dmLink<dmImage>& link ) {
     CCI_RETURN_IMETHOD_PP_OK(Parameters,GetImageA, _in CCI_ME_SET  _in &link ) 
   }

   #endif // DAIM_UTILITIES

};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_Parameters.h */
