#ifndef CCI_Colorspace_h
#define CCI_Colorspace_h

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
// File         : CCI_Colorspace.h
// Date         : 12/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//--------------------------------------------------------

#define CCIC_ColorSpace _TXT("processing/colorspace.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( ColorSpace, ImageList )
  
  CCI_MODULE_UUID( ColorSpace, 62cee349-caa9-4b38-8ba6-570ec6a440e4)

  CCI_MODULE_VERSION( ColorSpace, 1 ) 

  /* Methods */
  CCI_DECL_METHOD_PP(ColorSpace, 1, Split, _in dm_cci(Parameters) params ) 
  CCI_DECL_METHOD_PP(ColorSpace, 2, Merge, _in dm_cci(Parameters) params )
  CCI_DECL_METHOD_PP(ColorSpace, 4, Extract, 
     _in dm_cci(Parameters) params
     _in dm_uint            channel
     _in dm_uint            index
  )

  CCI_DECL_METHOD_PP(ColorSpace,5, SetRect, _in CONST dm_rect* rect )

  CCI_DECL_METHOD_PP(ColorSpace,6, MergeColor, _out dm_real q1 _out dm_real q2 _out dm_real q3 _out dm_real q4 )
  CCI_DECL_METHOD_PP(ColorSpace,7, SplitColor, _out dm_real q1 _out dm_real q2 _out dm_real q3 _out dm_real q4 )

  CCI_DECL_METHOD_PP(ColorSpace,8, SplitImage, _in CONST dmImage* image ) 
  CCI_DECL_METHOD_PP(ColorSpace,9, Reserve   , _in dm_cci(Parameters) params ) 

  CCI_DECL_METHOD_PP(ColorSpace,10, ToColorSpace, 
    _in dm_cci(Parameters) params
    _in dm_cci(ColorMap)   colormap 
  )

  /* Attributs */
  CCI_DECL_ATTR(ColorSpace,1, ChannelFmt  , dm_uint,  [.SGN] )
  CCI_DECL_ATTR(ColorSpace,2, NumChannels , dm_uint,  [.SGN] )
  CCI_DECL_ATTR(ColorSpace,3, CanMerge    , dm_bool,  [..G.] )


CCI_END_MODULE_DECL( ColorSpace )

#endif //__doxygen

//----------------------------------------------------------------
#ifdef __cplusplus

#include "daim_modules/CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Merge and split RGB images into multiple channels images
 *
 * Components image are stored into this object in buffer 0..n-1, where n
 * is the number of components from the selected type of decomposition.
 *
 * The format of the channels are either 8 bits and floating point pixel format
 * ( see SetChannelFmt() for setting the format of the decomposition).
 * In floating point format the results are unormalized and the range of values
 * varies on the type of decomposition used.
 *
 */
//------------------------------------------------------------------------------------
class dmIColorSpace : public dmIImageList
{ 
  public:
   dmDECLARE_IINTERFACE(ColorSpace)

   bool Create( const char_t* clsid, dm_uint version = -1 ) {
       return CreateObject(clsid,version,NULL);
   }

   //-------------------------------------------------------------------
   /*! \brief Split image into components */
   //-------------------------------------------------------------------
   bool Split( dm_cci(Parameters) params ) { 
     CCI_RETURN_IMETHOD_PP_OK(ColorSpace,Split, _in params )
   }

   //-------------------------------------------------------
   /*! \brief Merge image from imagelist components. */
   //-------------------------------------------------------
   bool Merge( dm_cci(Parameters) params ) { 
     CCI_RETURN_IMETHOD_PP_OK(ColorSpace,Merge, _in params )
   }

   //-------------------------------------------------------------------
   /*! \brief Get the pixel format of the channels */
   //-------------------------------------------------------------------  
   dm_uint GetChannelFmt() { return CCI_IGET(ColorSpace,ChannelFmt, (dm_uint)0 );  }

   //-------------------------------------------------------------------
   /*! \brief Set the pixel format of the channels
    *
    *  Note that only 8 bits and floating point pixel images are 
    *  currently supported 
    */ 
   //-------------------------------------------------------------------  
   bool SetChannelFmt( dm_uint channelfmt ) {
     return CCI_ISET(ColorSpace,ChannelFmt, channelfmt ); 
   }

   //-------------------------------------------------------------------
   /*! \brief Return true if the merge operation can be performed in
    *         the current colorspace 
    */ 
   //-------------------------------------------------------------------  
   bool CanMerge() { return CCI_IGET(ColorSpace,CanMerge,dm_false)==dm_true; }

   //-------------------------------------------------------------------
   /*! \brief Return the number of channels used in the decomposition */ 
   //-------------------------------------------------------------------  
   dm_uint NumChannels() { return CCI_IGET(ColorSpace,NumChannels, (dm_uint)0 ); } 

   //-------------------------------------------------------------------
   /*! \brief Extract channel component from current parameter image
    *
    *  The extracted image will be stored at buffer \a index of the 
    *  parameter image list. If \a index is -1, the image will be stored 
    *  in the parameter imagebuffer object.
    */ 
   //------------------------------------------------------------------- 
   bool Extract(dm_cci(Parameters) params,dm_uint channel, dm_uint index) {
     CCI_RETURN_IMETHOD_PP_OK(ColorSpace,Extract,
        _in params
        _in channel
        _in index
     )
   }

   //-------------------------------------------------------
   /*! \brief Set the rectangular ROI used for merge operation */
   //-------------------------------------------------------
   bool SetRect( CONST dm_rect* rect ) { 
     CCI_RETURN_IMETHOD_PP_OK(ColorSpace,SetRect, _in rect )
   }

   //-------------------------------------------------------
   /*! \brief Split color specified by q1,q2,q2,q4
    *
    *  On input  q1, q2, and q3 are the RGB components
    *  On output q1, q2, and q3 are the transformed components
    */ 
   //-------------------------------------------------------
   bool SplitColor( dm_real& q1, dm_real& q2, dm_real& q3, dm_real& q4 ) { 
     IF_CCI_IMETHOD_RP(ColorSpace,SplitColor, _in q1 _in q2 _in q3 _in q4 )
     THEN 
       CCI_RETURN_P(q1)
       CCI_RETURN_P(q2)
       CCI_RETURN_P(q3)
       CCI_RETURN_P(q4)
     ENDIF_RETURN_BOOL()
   }

   //-------------------------------------------------------
   /*! \brief Merge color specified by q1,q2,q2,q4 
    *
    *  On input   q1, q2, and q3 are the transformed components
    *  On output  q1, q2, and q3 are the RGB components
    */ 
   //-------------------------------------------------------
   bool MergeColor( dm_real& q1, dm_real& q2, dm_real& q3, dm_real& q4 ) { 
     IF_CCI_IMETHOD_RP(ColorSpace,MergeColor, _in q1 _in q2 _in q3 _in q4 )
     THEN 
       CCI_RETURN_P(q1)
       CCI_RETURN_P(q2)
       CCI_RETURN_P(q3)
       CCI_RETURN_P(q4)
     ENDIF_RETURN_BOOL()
   }

   //-------------------------------------------------------------------
   /*! \brief Split image into components */
   //-------------------------------------------------------------------
   bool SplitImage( const dmImage* image ) { 
     CCI_RETURN_IMETHOD_PP_OK(ColorSpace,SplitImage, _in image )
   }

   bool ToColorSpace(  dm_cci(Parameters) params ,  dm_cci(ColorMap) colormap ) {
      CCI_RETURN_IMETHOD_PP_OK(ColorSpace, ToColorSpace, _in params _in colormap )
   }
    
};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_Colorspace_h */
