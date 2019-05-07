#ifndef CCI_Profile_h
#define CCI_Profile_h

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
// File         : CCI_Profile.h
// Date         : 11/2005
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//--------------------------------------------------------
#define CCIC_Profile _TXT("processing/profile.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( Profile, Notify )

  CCI_MODULE_UUID( Profile, 0124b87f-8722-4126-8f54-ceafaa477870)

  CCI_MODULE_VERSION( Profile, 1 ) 

  /* Methods  */

  CCI_DECL_METHOD_PP(Profile,1,  SetPath, 
      _in dm_uint type 
      _in dm_ptr  points 
      _in dm_uint count 
  )

  CCI_DECL_METHOD_PP(Profile,2, Compute, 
      _in dm_cci(Parameters) params
  )

  CCI_DECL_METHOD_NP(Profile,3, Clear )

  CCI_DECL_METHOD_PP(Profile,4, GetValues,
      _out dm_array(dm_real,size) values
      _out dm_uint                size
  )

  CCI_DECL_METHOD_PP(Profile,5, GetPath,
      _out dm_array(dm_point,size) points
      _out dm_uint                 size
  )

  
  CCI_DECL_METHOD_PP(Profile,6, GetRange,
      _out dm_real  upper
      _out dm_real  lower
  )  

  CCI_DECL_METHOD_PP(Profile,7, SetFilterParameters,
      _in dm_uint       order
      _in dm_uint       deriv
      _in dm_uint       extend
      _in CONST char_t* mode 
  )  
 
  CCI_DECL_METHOD_PP(Profile,8, GetData,
      _in dm_cci(StorageMap) storage
      _in CONST char_t*      profile1
      _in CONST char_t*      profile2
      _in CONST char_t*      xpath
      _in CONST char_t*      ypath
  )

  /* Attributs */

  CCI_DECL_ATTR( Profile,1, Count , dm_uint , [..G.] )

  /* Special values */


CCI_END_MODULE_DECL( Profile )

#endif // __doxygen
//----------------------------------------------------------------
#ifdef __cplusplus

#include "daim_modules/CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Compute intensity profile from given path
 */
//------------------------------------------------------------------------------------
class dmIProfile : public dmINotify
{ 
  public:
   dmDECLARE_GENERIC_IINTERFACE(Profile)
  
   //--------------------------------------------------------------
   /*! \brief Set the profile path  
    *
    *  \param type   : the type of path to be created
    *  \param points : the object holding the coordinates of the path  
    * 
    *  The path is computed using internal bresenham algorithms.  
    */
   //--------------------------------------------------------------
   bool SetPath( dm_uint type, dm_ptr points, dm_uint count = 1 ) {
      CCI_RETURN_IMETHOD_PP_OK(Profile,SetPath, _in type _in points _in count )
   }
  
   //--------------------------------------------------------------
   /*! \brief Compute the profil    
    *
    *  \param params  the parameters object handle ( see dmIParameters )      
    */
   //--------------------------------------------------------------
   bool Compute( dm_cci(Parameters) params ) {
      CCI_RETURN_IMETHOD_PP_OK(Profile,Compute,_in params)
   }
 
   //--------------------------------------------------------------
   /*! \brief Clear profile data */
   //--------------------------------------------------------------
   bool Clear() { CCI_RETURN_IMETHOD_NP_OK(Profile,Clear) }

   //--------------------------------------------------------------
   /*! \brief Return an arrays of the profile intensity values */
   //--------------------------------------------------------------
   bool GetValues( dm_real*& values, dm_uint& size ) {
     IF_CCI_IMETHOD_RP(Profile,GetValues, _out(values) 0 ) THEN 
       CCI_RETURN_P(values)
       CCI_RETURN_P(size)
     ENDIF_RETURN_BOOL()
   }

   //--------------------------------------------------------------
   /*! \brief Return an array of points corresponding to
    *   the computed profile */
   //--------------------------------------------------------------
   bool GetPath( dm_point*& points, dm_uint& size ) {
     IF_CCI_IMETHOD_RP(Profile,GetPath, _out(points) 0 ) THEN 
       CCI_RETURN_P(points)
       CCI_RETURN_P(size)
     ENDIF_RETURN_BOOL()
   }

   //--------------------------------------------------------------
   /*! \brief Store specified data in dmIStorageMap object
    *
    * \param storage An instance of a dmIStorageMap object
    * \param profile1 The label used to store the profile values, 
    * \param profile2 The label used to store the raw profiles values (not filtered)
    * \param xpath    The label used to store x coordinates of the points corresponding to
    *                 the computed profile
    * \param ypath    The label used to store y coordinates of the points corresponding to
    *                 the computed profile
    */
   //--------------------------------------------------------------  
   bool GetData( dm_cci(StorageMap) storage, 
                 const char_t*      profile1,
                 const char_t*      profile2,
                 const char_t*      xpath,
                 const char_t*      ypath) 
   {
     CCI_RETURN_IMETHOD_PP_OK(Profile,GetData,_in storage
                              _in profile1
                              _in profile2
                              _in xpath
                              _in ypath)
   }
   
   //-------------------------------------------------------
   //*! \brief Return the min and max values the computed profile */
   //-----------------------------------------------------------
   bool GetRange( dm_real& upper, dm_real lower ) {
     IF_CCI_IMETHOD_RP(Profile,GetRange, _out(upper) 0 _out(lower) 0 ) THEN
       CCI_RETURN_P(lower)
       CCI_RETURN_P(upper)
     ENDIF_RETURN_BOOL()
   }

   //-----------------------------------------------------------------------
   /*! \brief Set the Savitzky Golay filter parameters applied on profil 
    * 
    * By setting the parameters with \a order greater than 0, the values 
    * of the profil will be filtered by a 1D symetrical a Savitzky Golay 
    * smoothing filter of order \a order and half width \a extend.
    * 
    * The parameter \a deriv set the order of the dérivation applied to 
    * the signal 
    * 
    * Setting \a order to zero disable the filter
    */ 
   //-----------------------------------------------------------------------
   bool SetFilterParameters( dm_uint order, dm_uint deriv, dm_uint extend,
                             const char_t* mode )
   {
     CCI_RETURN_IMETHOD_PP_OK(Profile,SetFilterParameters,
       _in order
       _in deriv
       _in extend
       _in mode)
   }   

   dm_uint Count() { return CCI_IGET(Profile,Count,(dm_uint)0); }
      
};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_Profiles_h */
