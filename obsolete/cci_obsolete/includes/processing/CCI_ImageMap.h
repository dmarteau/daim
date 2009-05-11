#ifndef CCI_ImageMap_h
#define CCI_ImageMap_h

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
// File         : CCI_ImageMap.h
// Date         : 01/2005
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//
//--------------------------------------------------------

#define CCIC_ImageMap _TXT("processing/imagemap.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( ImageMap, Notify )

  CCI_MODULE_UUID( ImageMap, a39dc034-5500-4b53-afff-83b15fc59dc9)

  CCI_MODULE_VERSION( ImageMap, 1 ) 
 
  /* Methods   */
  CCI_DECL_METHOD_PP(ImageMap, 1,  Build, 
      _in dm_cci(Parameters) params
      _in dm_uint            connect
      _in dm_int             flags
  )

  CCI_DECL_METHOD_PP(ImageMap, 2,  CreateRegion,
      _in dm_cci(Parameters) params
      _in dm_int             label
      _in dm_bool            include_holes
  )

  CCI_DECL_METHOD_PP(ImageMap, 3,  Clear    , _in dm_int label )
  CCI_DECL_METHOD_PP(ImageMap, 4,  GetLabels, 
      _in  CONST dmRegion*        rgn 
      _out dm_array(dm_int,count) labels 
      _out dm_uint                count 
  )

  CCI_DECL_METHOD_PP(ImageMap, 5,  GetLabelFromPoint, _in dm_int x _in dm_int y _out dm_int label )
  CCI_DECL_METHOD_PP(ImageMap, 6,  RemoveLabels     , _in dm_array(dm_int,count) labels _in dm_uint count )
  CCI_DECL_METHOD_PP(ImageMap, 7,  SelectLabels     , 
     _in dm_cci(Parameters)      params
     _in dm_array(dm_int,count)  labels 
     _in dm_uint                 count 
  )

  CCI_DECL_METHOD_PP(ImageMap, 8,  SetCalibration   , _in dm_real uppx _in dm_real ar )

  CCI_DECL_METHOD_PP(ImageMap, 9,  GetChildRegions  ,
     _in  dm_int                 label 
     _in  dm_int                 depth 
     _out dm_array(dm_int,count) labels
     _out dm_uint                count
  )

  CCI_DECL_METHOD_PP(ImageMap,10, GetIndexTable, _in dm_array(dm_int,count) table _in dm_uint count )

  CCI_DECL_METHOD_NP(ImageMap,11, FillHoles    )
  CCI_DECL_METHOD_NP(ImageMap,12, CleanBorders )

  CCI_DECL_METHOD_PP(ImageMap,13, GetProperties,
     _in  dm_int    label
     _out dm_uint   flags
     _out dm_uint   area
     _out dm_real   dir
     _out dm_real   l1
     _out dm_real   l2
     _out dm_int    cx
     _out dm_int    cy
     _out dm_point* points
     _out dm_uint   count
     _out dm_int    euler    
     _out dm_uint   depth    
     _out dm_uint   holes
     _out dm_int    parent    
  )

  CCI_DECL_METHOD_PP(ImageMap,14, Reconstruct, _in dm_cci(Parameters) params ) 

  CCI_DECL_METHOD_PP(ImageMap,16, RegionProps,
     _in dm_cci(PropertyList) props
     _in dm_param             key
     _in dm_cci(StorageMap)   store
     _in dm_cci(Parameters)   params
  )

  CCI_DECL_METHOD_PP(ImageMap,17,  RemoveLabelArray,  
     _in dm_cci(StorageMap) storage
     _in CONST char_t*      column  
  )
  
  CCI_DECL_METHOD_PP(ImageMap,18,  SelectLabelArray, 
     _in dm_cci(Parameters)  params
     _in dm_cci(StorageMap)  storage 
     _in CONST char_t*       column 
  )

  CCI_DECL_METHOD_PP(ImageMap,19,  CleanRegionBorders,
      _in dm_cci(Parameters) params
  )


  /* Attributs */
  CCI_DECL_ATTR(ImageMap, 1, Connectivity, dm_uint, [..G.] )
  CCI_DECL_ATTR(ImageMap, 2, Count       , dm_uint, [..G.] )
  CCI_DECL_ATTR(ImageMap, 3, Depth       , dm_uint, [..G.] )

  /* Flags for Build method */
  #define CCIV_ImageMap_NoClip    0x01L
  #define CCIV_ImageMap_OffsetRoi 0x02L

  /* Flags for GetProperties method */
  #define CCIV_ImageMap_Area   0x01L
  #define CCIV_ImageMap_Dir    0x02L
  #define CCIV_ImageMap_Rect   0x04L
  #define CCIV_ImageMap_Loc    0x08L
  #define CCIV_ImageMap_Shape  0x10L
  #define CCIV_ImageMap_Euler  0x20L


  
CCI_END_MODULE_DECL( ImageMap )

#endif // __doxygen
//----------------------------------------------------------------
#ifdef __cplusplus
#include "daim_modules/CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Compute various topological elements from a region description 
 *
 *  The ImageMap interface will compute topological attributs
 *  from a partition in connexe components of a given region. 
 *  The partition is organized so has to take advantage of the hierarchical structure
 *  of objects and holes in the computation of the objects properties.
 *  Each connexe components is designated by a label used as an object réference
 *  in ImageMap methods.  
 *
 *  Note that the label 0 always correspond to background.
 */
//------------------------------------------------------------------------------------
class dmIImageMap : public dmINotify
{ 
  public:
   dmDECLARE_GENERIC_IINTERFACE(ImageMap)

   //----------------------------------------------------------------
   /*! \brief Structure holding properties values \see GetProperties
    */             
   //----------------------------------------------------------------
   struct Properties
   {
     dm_int    label;
     dm_uint   flags;
     dm_uint   area;   /*!< Area covered by of the object, computed using \a CCIV_ImageMap_Area           */ 
     dm_real   dir;    /*!< Direction of the minimum bounding box, computed using \a CCIV_ImageMap_Dir    */ 
     dm_real   l1;     /*!< Minor axis of the minimum bounding box, computed using \a CCIV_ImageMap_Rect  */ 
     dm_real   l2;     /*!< Major axis of the minimum bounding box, computed using \a CCIV_ImageMap_Rect  */ 
     dm_int    cx;     /*!< X coordinate of the centroid, computed using \a CCIV_ImageMap_Loc             */ 
     dm_int    cy;     /*!< Y coordinate of the centroid, computed using \a CCIV_ImageMap_Loc             */ 
     dm_point* points; /*!< Enclosing polygon, computed using \a CCIV_ImageMap_Shape                      */ 
     dm_uint   count;  /*!< Number of points in the enclosing polygon                                     */ 
     dm_int    euler;  /*!< Euler number, computed using \a CCIV_ImageMap_Euler                           */ 
     dm_uint   holes;  /*!< Number of holes in the object                                                 */ 
     dm_uint   depth;
     dm_int    parent;
   };

   //----------------------------------------------------------------
   /*! \brief build a region partition map              
    *
    * Build a partition from the mask obtained from the \a params object 
    * 
    * \param params  parameter object ( see dmIParameters ) 
    * \param connect the connectivity ( 4 or 8 )
    * \param flags   flags cn be a combination of the following values:
    * - \c CCIV_ImageMap_NoClip   The region will not be clipped with 
    *      the input image rectangle (if any). Instead the bounding rectangle
    *      will be set to the bounding rectangle of the input region. 
    * - \c CCIV_ImageMap_OffsetRoi Used in conjunction with \c CCIV_ImageMap_NoClip,
    *      the input region will be offset so as to have the bounding rectangle top left
    *      corner at the origin (0,0). 
    *
    */
   //----------------------------------------------------------------
   bool Build( dm_cci(Parameters) params, dm_uint connect, dm_uint flags ) {
     CCI_RETURN_IMETHOD_PP_OK(ImageMap,Build, _in params _in connect _in flags )
   }

   //----------------------------------------------------------------
   /*! \brief Create the mask corresponding to the object designated by \a label
    *
    *  The mask will be stored in the \a params object (using dmIparameteres::SetMask).
    *
    *  - If \a label==-1 the mask will correspond to the background region (i.e
    *                    the region adjacent to borders).
    *  - If \a label== 0 the mask will correspond to the reunion of all objects.
    *  - If \a label== x the mask will correspond to object designated by \a label.
    */
   //----------------------------------------------------------------
   bool CreateRegion( dm_cci(Parameters) params, dm_int label, dm_bool include_holes ) {
     CCI_RETURN_IMETHOD_PP_OK(ImageMap,CreateRegion,_in params _in label 
                                                    _in include_holes )
   }

   //----------------------------------------------------------------
   /*! \brief Remove object corresponding to \a label
    *
    *  Warning that \a label is likely to be affected to the parent object
    */
   //----------------------------------------------------------------
   bool Clear( dm_int label ) { CCI_RETURN_IMETHOD_PP_OK(ImageMap,Clear,_in label) }

   //----------------------------------------------------------------
   /*! \brief Return a list of labels of objects that intersects the region \a rgn.
    *
    *  Warning that \a labels is a temporary object and should not be 
    *  stored for later use.
    */
   //----------------------------------------------------------------
   dm_uint GetLabels( CONST dmRegion* rgn, dm_int*& labels ) {
     IF_CCI_IMETHOD_RP(ImageMap,GetLabels, _in rgn _out(labels) NULL _out(count) 0)
       THEN labels = CCI_RETVAL(labels);
            return CCI_RETVAL(count);
       ELSE return 0;
     ENDIF
   }

   //----------------------------------------------------------------
   /*! \brief Return the label of the object located at (x,y)  */
   //----------------------------------------------------------------
   dm_int GetLabelFromPoint( dm_int x , dm_int y ) {
     IF_CCI_IMETHOD_RP(ImageMap,GetLabelFromPoint,_in x _in y _out(label) 0 )
       THEN return CCI_RETVAL(label);
       ELSE return 0;
     ENDIF
   }

   //----------------------------------------------------------------
   /*! \brief Clear all labels in \a labels  */
   //----------------------------------------------------------------
   bool RemoveLabels( dm_int* labels _in dm_uint count ) {
     CCI_RETURN_IMETHOD_PP_OK(ImageMap,RemoveLabels,_in labels _in count )
   }

   //----------------------------------------------------------------
   /*! \brief Create the mask corresponding to all the objects in \a labels  */
   //----------------------------------------------------------------
   bool SelectLabels( dm_cci(Parameters) params, dm_int* labels _in dm_uint count ) {
     CCI_RETURN_IMETHOD_PP_OK(ImageMap,SelectLabels,_in params _in labels _in count)
   }

   //----------------------------------------------------------------
   /*! \brief Set the spatial calibration */
   //----------------------------------------------------------------
   bool SetCalibration( dm_real uppx, dm_real ar ) {
     CCI_RETURN_IMETHOD_PP_OK(ImageMap,SetCalibration,_in uppx _in ar )
   }

   //----------------------------------------------------------------
   /*! \brief Return a list of labels correponding to connnexe objects
    *         included in the object designated by \a label. 
    *
    *  Warning that \a labels is a temporary object and should not be 
    *  stored for later use.
    */
   //----------------------------------------------------------------
   dm_uint GetChildRegions( dm_int label, dm_int*&  labels ) {
     IF_CCI_IMETHOD_RP(ImageMap,GetChildRegions, _in label _in 0 _out(labels) NULL _out(count) 0)
       THEN labels = CCI_RETVAL(labels);
            return CCI_RETVAL(count);
       ELSE return 0;
     ENDIF
   }

   //----------------------------------------------------------------
   /*! \brief Return a inversed index tables for all objects labels.
    *
    *  This table will enable to fetch values from labels in arrays 
    *  returned by the Compute() method. Given a label value, the array return
    *  the index for the value of a given property.
    *
    *  \return the number of elements in \a table
    *
    *  Warning that \a table is a temporary object and should not be 
    *  stored for later use.
    *
    */
   //----------------------------------------------------------------
   dm_uint GetIndexTable( dm_int*& table ) {
     IF_CCI_IMETHOD_RP(ImageMap,GetIndexTable, _out(table) NULL _out(count) 0)
       THEN table = CCI_RETVAL(table);
            return  CCI_RETVAL(count);
       ELSE return 0;
     ENDIF
   }

   //----------------------------------------------------------------
   /*! \brief Fill holes in objects */
   //----------------------------------------------------------------
   bool FillHoles() { CCI_RETURN_IMETHOD_NP_OK(ImageMap,FillHoles)    }

   //----------------------------------------------------------------
   /*! \brief Remove objects adjacent to borders  */
   //----------------------------------------------------------------
   bool CleanBorders() { CCI_RETURN_IMETHOD_NP_OK(ImageMap,CleanBorders) }

   //----------------------------------------------------------------
   /*! \brief Return properties for the object designated by \a label
    *
    *  \param label the object label
    *  \param flags A combinaison of CCIV_ImageMap_Values which deifine the property 
    *         returned by the GetProperties() method. 
    *         See the Properties structure for information on the returned
    *         properties and the associated flags.
    *  \param props A reference to a Properties structure.
    */
   //----------------------------------------------------------------
   bool GetProperties( dm_int label, dm_uint flags, Properties& props )
   {
     IF_CCI_IMETHOD_RP(ImageMap,GetProperties, _in label _in flags )
     THEN 
       props.label  = label;
       props.flags  = flags;
       props.area   = CCI_RETVAL(area  );
       props.dir    = CCI_RETVAL(dir   );
       props.l1     = CCI_RETVAL(l1    );
       props.l2     = CCI_RETVAL(l2    );
       props.cx     = CCI_RETVAL(cx    );
       props.cy     = CCI_RETVAL(cy    );
       props.points = CCI_RETVAL(points);
       props.count  = CCI_RETVAL(count );
       props.euler  = CCI_RETVAL(euler );
       props.depth  = CCI_RETVAL(depth );
       props.holes  = CCI_RETVAL(holes );
       props.parent = CCI_RETVAL(parent);
     ENDIF_RETURN_BOOL()
   }

   //----------------------------------------------------------------
   /*! \brief Perform a reconstruction using the mask given in \a params
    *
    *  The Reconstruction is done by constructing a new mask
    *  from objects that intersect the mask given in \a params.
    *
    *  This method is an equivalent of calling GetLabels() followed by the 
    *  SelectLabels() method.
    */
   //----------------------------------------------------------------
   bool Reconstruct( dm_cci(Parameters) params ) {
     CCI_RETURN_IMETHOD_PP_OK(ImageMap,Reconstruct,_in params )
   }

   //--------------------------------------------------------------------------------
   /*! \brief Remove all objects outside or adjacent to the border of the given roi */
   //--------------------------------------------------------------------------------
   bool CleanRegionBorders( dm_cci(Parameters) params ) {
     CCI_RETURN_IMETHOD_PP_OK(ImageMap,CleanRegionBorders,_in params)
   }

   //----------------------------------------------------------------
   /*! \brief Compute object properties
    *
    * FIXME !!!!
    * THIS IS AN INVALID DOCUMENTATION !!!!
    *  \param store  an instance of a dmIStorageMap object. Storage will
    *                be obtained with dmIStorageMap::GetCol(). 
    *  \param params array of tagitem structure defining the desired 
    *                properties. The \a tg_Tag member can contain following values:
    *    - CCIA_RgnDesc_Labels   return labels
    *    - CCIA_RgnDesc_Border   return full border length
    *    - CCIA_RgnDesc_Boundary return external border length
    *    - CCIA_RgnDesc_Area     return size
    *    - CCIA_RgnDesc_Dir      return direction
    *    - CCIA_RgnDesc_Euler    return euler number
    *    - CCIA_RgnDesc_CX       return X-Centroid
    *    - CCIA_RgnDesc_CY       return Y-Centroid
    *    - CCIA_RgnDesc_L1       return min axis length 
    *    - CCIA_RgnDesc_L2       return max axis length
    *    - CCIA_RgnDesc_Moment   return central moment of order (aguments: xn,yn)
    *  
    *  if \a params is supplied then the following properties can be computed:
    *    - CCIA_ImgDesc_TotalDensity     return total density value of order n  (arguments: n)
    *    - CCIA_ImgDesc_MeanDensity      return mean density value
    *    - CCIA_ImgDesc_MinDensity       return min luminance value
    *    - CCIA_ImgDesc_MaxDensity       return max luminance value
    *    - CCIA_ImgDesc_MeanDensity      return mean luminance value
    *
    *  The \a tg_Data member contains the string id  of the the stored data
    *  together with any parameters that could be passed to the routines for
    *  computing the given property. Parameters are given use POST syntax:
    *  \c 'name?arg1=value1&arg2=value2...'
    *
    *  The following example will ask to compute the 2 order moment in x and y :
    *  \code
    *    tg_TagItem _params = { 
    *        CCIA_RgnDesc_Moment,dm_ptr_to_long("m22?xn=2&yn=2"),
    *        TGZ_DONE 
    *     };    
    *  \endcode
    */
   //----------------------------------------------------------------
  bool RegionProps( dm_cci(PropertyList) props, dm_param key, dm_cci(StorageMap) store, dm_cci(Parameters) params ) {
     CCI_RETURN_IMETHOD_PP_OK(ImageMap, RegionProps, _in props _in key _in store _in params ) 
   }

};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_ImageMap_h */
