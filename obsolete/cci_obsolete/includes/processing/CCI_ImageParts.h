#ifndef CCI_ImageParts_h
#define CCI_ImageParts_h

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
// File         : CCI_ImageParts.h
// Date         : 5/2005
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//
//--------------------------------------------------------
#define CCIC_ImageParts _TXT("processing/imageparts.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( ImageParts, Notify )

  CCI_MODULE_UUID( ImageParts, 9ee1409c-fad5-4e28-9a4b-65d8a68c436c)

  CCI_MODULE_VERSION( ImageParts, 1 ) 
 
  /* Methods   */
  CCI_DECL_METHOD_PP(ImageParts, 1,  BuildPartition, 
      _in dm_cci(Parameters) params
      _in dm_real            distance
      _in dm_uint            connectivity
      _in dm_param           modes
  )

  CCI_DECL_METHOD_PP(ImageParts, 2,  CompactPartition, _out dm_int count )
  CCI_DECL_METHOD_NP(ImageParts, 3,  RemoveBorderRgns )

  CCI_DECL_METHOD_PP(ImageParts, 4,  ResolveOverlappingRgn,
      _in  dm_cci(Parameters) params
      _out dm_int             label
  )
 
  CCI_DECL_METHOD_PP(ImageParts, 5,  MergeRegions,
      _in  dm_int firstlabel
      _in  dm_int secondlabel
      _out dm_int resultlabel
  )

  CCI_DECL_METHOD_PP(ImageParts, 6,  GetLabelFromPoint, 
      _in  dm_int x
      _in  dm_int y
      _out dm_int label
  )

  CCI_DECL_METHOD_PP(ImageParts, 7,  GetMaxRegion , _out dm_int label )
  CCI_DECL_METHOD_PP(ImageParts, 8,  GetRegionSize,
     _in  dm_int  label 
     _out dm_uint size
  )

  CCI_DECL_METHOD_PP(ImageParts, 9,  GetRegionIndex,
     _in  dm_int  label 
     _out dm_int  index
  )

  CCI_DECL_METHOD_PP(ImageParts,10,  GetRegionLabels, 
     _in  CONST dmRegion*       region
     _out dm_array(dm_int,size) labels
     _out dm_uint               size
  )

  CCI_DECL_METHOD_PP(ImageParts,11,  GetRegionMask, 
      _in dm_cci(Parameters) params
      _in dm_int             label
  )
 
  CCI_DECL_METHOD_PP(ImageParts,12,  MergeLabels, 
     _in dm_int                label
     _in dm_array(dm_int,size) labels
     _in dm_uint               size
  )

  CCI_DECL_METHOD_PP(ImageParts,13,  SelectRegionsBySize, 
     _in  dm_uint                minsize
     _in  dm_uint                maxsize
     _in  dm_bool                exclude
     _out dm_array(dm_int,count) labels
     _out dm_uint                count
  )

  CCI_DECL_METHOD_PP(ImageParts,14,  StoreRegion,
      _in dm_int             label
      _in dm_cci(MaskBuffer) maskbuffer
      _in dm_uint            index 
  )

  /* Not implemented : interface description only */

  CCI_DECL_METHOD_PP(ImageParts,21,  GetMaskLabels, 
     _in  dm_cci(Parameters)    params
     _out dm_array(dm_int,size) labels
     _out dm_uint               size
  )


  /* Attributs */
  CCI_DECL_ATTR(ImageParts,1,  Count  , dm_uint, [..G.] )

  #define CCIV_ImageParts_Average 0x01L

CCI_END_MODULE_DECL( ImageParts )

#endif // __doxygen
//----------------------------------------------------------------
#ifdef __cplusplus

#include "daim_modules/CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Compute image partition and handle operations on it.
 *
 *  The partition is computed by agregation of pixels.
 *  Once computed, the \a ImageParts object will store a partition table
 *  and a image map of the same dimensions as the input image.
 */
//------------------------------------------------------------------------------------
class dmIImageParts : public dmINotify
{ 
  public:
   dmDECLARE_GENERIC_IINTERFACE(ImageParts)

   enum Constants {
     flAverage = CCIV_ImageParts_Average
   };

  //-------------------------------------------------------
  /*! \brief Compute partition of the given input image
   *
   *  Compute a partition of the image stored in \a param by
   *  agregation of pixels whose value is less or equal than \a distance.
   *  Onc computed, the \a ImageParts object will store a partition table
   *  and a image map of the same dimensions as the input image.
   */
  //-------------------------------------------------------
  bool BuildPartition( dm_cci(Parameters) params, 
                       dm_real            distance, 
                       dm_uint            connectivity,
                       dm_param           modes ) 
  {
     CCI_RETURN_IMETHOD_PP_OK(ImageParts,BuildPartition, 
                               _in params 
                               _in distance
                               _in connectivity
                               _in modes )
  }

  //-------------------------------------------------------
  /*! \brief Compact the partition
   *
   *  Compact the partition : i.e make the partition table the same
   *  as the index table and perform a relabelling of the internal
   *  stored map. Return the number of partitions.
   */
  //-------------------------------------------------------
  dm_int CompactPartition() {
    IF_CCI_IMETHOD_RP(ImageParts,CompactPartition, _out(count) 0 )
    THEN return CCI_RETVAL(count); ENDIF
    return 0;
  }

  //-------------------------------------------------------
  /*! \brief Remove region touching the border of the map */
  //-------------------------------------------------------
  bool RemoveBorderRgns() { CCI_RETURN_IMETHOD_NP_OK(ImageParts,RemoveBorderRgns) }

  //-------------------------------------------------------
  /*! \brief Merge regions overlapping the input region mask */
  //-------------------------------------------------------
  dm_int ResolveOverlappingRgn( dm_cci(Parameters) params ) {
     IF_CCI_IMETHOD_RP(ImageParts,ResolveOverlappingRgn, _in params )
     THEN return CCI_RETVAL(label); ENDIF
     return -1;
  }
 
  //-------------------------------------------------------
  /*! \brief Merge regions  */
  //-------------------------------------------------------
  dm_int MergeRegions( dm_int firstlabel,dm_int secondlabel ) {
     IF_CCI_IMETHOD_RP(ImageParts,MergeRegions,
       _in  firstlabel
       _in  secondlabel
     ) THEN return CCI_RETVAL(resultlabel); ENDIF
     return -1;
  }

  //-------------------------------------------------------
  /*! \brief Return the label at the given location        */
  //-------------------------------------------------------
  dm_int GetLabelFromPoint( const dm_point& point ) {
     IF_CCI_IMETHOD_RP(ImageParts,GetLabelFromPoint, _in  point.x _in point.y ) 
     THEN return CCI_RETVAL(label); ENDIF
     return -1;
  }

  //-------------------------------------------------------
  /*! \brief Return the label of the largest region       */
  //-------------------------------------------------------
  dm_int GetMaxRegion() {
     IF_CCI_IMETHOD_RP(ImageParts,GetMaxRegion, _out(label) -1 ) 
     THEN return CCI_RETVAL(label); ENDIF
     return -1;
  }

  //-------------------------------------------------------
  /*! \brief Return the size of the given region          */
  //-------------------------------------------------------
  dm_uint GetRegionSize( dm_int label ) {
     IF_CCI_IMETHOD_RP(ImageParts,GetRegionSize, _in label _out(size) 0 ) 
     THEN return CCI_RETVAL(size); ENDIF
     return 0;
  }

  //-------------------------------------------------------
  /*! \brief Return the index of the given region */
  //-------------------------------------------------------
  dm_int GetRegionIndex( dm_int label ) {
     IF_CCI_IMETHOD_RP(ImageParts,GetRegionIndex, _in label _out(index) -1 ) 
     THEN return CCI_RETVAL(index); ENDIF
     return -1;
  }

  //-------------------------------------------------------
  /*! \brief Return an array of labels of the regions overlapping
   *         the input region.
   */
  //-------------------------------------------------------
  bool GetRegionLabels( CONST dmRegion* region, dm_int*& labels, dm_uint& size ) {
     IF_CCI_IMETHOD_RP(ImageParts,GetRegionLabels, _in region _in labels _in size ) 
     THEN
       CCI_RETURN_P(labels)
       CCI_RETURN_P(size)
     ENDIF_RETURN_BOOL()
  }

  //-------------------------------------------------------
  /*! \brief Compute the mask of the region corresponding to the given label
   *
   *  \param params dmIParameters object where the mask will be stored
   *  \param label  the label for the region partition.
   *
   * \note if \a label is set to -1 and the input mask is not empty, then 
   *       the method will compute the region corresponding to the union
   *       of all overlapping regions from the partition.
   */
  //-------------------------------------------------------
  bool GetRegionMask( dm_cci(Parameters) params, dm_int label, dm_uint index ) {
    CCI_RETURN_IMETHOD_PP_OK(ImageParts,GetRegionMask, 
      _in params
      _in label
    )
  }
 
  //-------------------------------------------------------
  /*! \brief Merge regions specified by the array \a labels */
  //-------------------------------------------------------
  bool MergeLabels( dm_int label, dm_int* labels, dm_uint size ) {
    CCI_RETURN_IMETHOD_PP_OK(ImageParts,MergeLabels, 
      _in label
      _in labels
      _in size
    )
  }


  //-------------------------------------------------------
  /*! \brief Store the region specified the label \a label 
   *               at buffer index 
   */
  //-------------------------------------------------------
  bool StoreRegion( dm_int label, dm_cci(maskbuffer) maskbuffer,  dm_uint index ) {
    CCI_RETURN_IMETHOD_PP_OK(ImageParts,StoreRegion, 
      _in label
      _in maskbuffer
      _in index 
    )
  }

  bool  SelectRegionsBySize( dm_uint minsize, dm_uint maxsize, dm_bool exclude, 
        dm_int*& labels, dm_uint& count ) {
     IF_CCI_IMETHOD_RP(ImageParts,SelectRegionsBySize, _in minsize _in maxsize ) 
     THEN
       CCI_RETURN_P(labels)
       CCI_RETURN_P(count)
     ENDIF_RETURN_BOOL()
  }


  dm_uint Count() { return CCI_IGET(ImageParts,Count,0UL); }

};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_ImageParts_h */
