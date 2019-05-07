
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

#define dmUseCCI
#define dmUseKernelImageTemplates
#define dmUseKernelBuffer
#include "daim_kernel.h"
#include "tools/dmIntensityProfile.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_Parameters.h"
#include "daim_modules/CCI_StorageMap.h"

#include "daim_modules/processing/CCI_Profile.h"

//------------------------------------------------------------------------
#define CCIC_NAME  CCIC_Profile
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_Profile_Version
#define CCI_REVISION  1
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2005"))
//---------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"
#include "dmMatrixBase.h"
#include "dmMatrixSG.h"
#include "dmMatrixUtils.h"

#include "daim_modules/CCI_Interface.h"

typedef std::vector<dm_real>  RealArray;
typedef std::vector<dm_point> PointArray;

#define MODE_ABS 1

CCI_DECL_DATA( Profile ) 
{
  dmIntensityProfile mProfile;
  RealArray          mValues;
  PointArray         mPoints;

  dmRowVector mCoeffs;
  dmRowVector mData;
  dmRowVector mTemp;
  int         mDeriv;
  int         mOrder;
  int         mExtend;
  
  dmString    mMode;
};

typedef CCI_INSTANCE_PTR(Profile) _Instance_Ptr;

//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( Profile )
{
  CCI_GENERIC_CONSTRUCT(Profile)
  
  _This->mExtend = 0;
  
  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( Profile )
{
  CCI_GENERIC_DESTROY(Profile)
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( Profile ) // Use CCIA_RETVAL
  CCI_ATTR_GET(Profile, Count, CCIA_RETVAL = _This->mProfile.Count() )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( Profile ) // Use CCIA_DATA
  CCIA_DISCARD(Profile, Count )
CCI_END_SETTER()
/*
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( Profile )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( Profile )
{
  CCI_RETURN_INVOKE_SUPER()
}
*/
//---------------------------------------------------------------------
// Method     : SetPath
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Profile , SetPath )
{
  CCI_INSTANCE(Profile)
   
  dmIntensityProfile& _Profile = _This->mProfile;
 
  _Profile.Clear();  
    
  dm_ptr p = CCI_P(points);
  if(p!=NULL) 
  {    
    switch(CCI_P(type)) 
    {
      case dmRectangle_t: _Profile.SetCoordinates(dmRect   (*static_cast<dm_rect*>   (p))); break;
      case dmEllipse_t  : _Profile.SetCoordinates(dmEllipse(*static_cast<dm_ellipse*>(p))); break;
      case dmCircle_t   : _Profile.SetCoordinates(dmCircle (*static_cast<dm_circle*> (p))); break;
      case dmLine_t     : _Profile.SetCoordinates(dmLine   (*static_cast<dm_line*>   (p))); break;
      case dmPoly_t     : 
        if(CCI_P(count)>0) {
          dmPoly poly(static_cast<dm_point*>(p),CCI_P(count));
          _Profile.SetCoordinates(poly);
          break;
        } 
      default:
        CCI_RETURN_INVALID_PARAM()
    }
 
    if(!_Profile.Empty())
      CCI_RETURN_OK()    
  }
  
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method     : Compute
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Profile , Compute )
{
  CCI_INSTANCE(Profile)

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage* img = _IParams.GetImage();
    if(img) {
       _This->mProfile.ComputeProfile(*img,false);
       CCI_RETURN_OK()
    } else
      CCI_SET_ERROR_MSG(_TXT("No image !"));
  
  } else {
    CCI_SET_ERROR_MSG(_TXT("Invalid parameters !"));
    CCI_RETURN_INVALID_PARAM()
  } 

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method     : Clear
//---------------------------------------------------------------------
static CCI_IMPL_METHOD_NP( Profile , Clear )
{
  CCI_INSTANCE(Profile)
  _This->mProfile.Clear();
    
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method     : GetValues
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Profile , GetValues )
{
  CCI_INSTANCE(Profile)

  int _Count = _This->mProfile.Count();  

  if(_Count > 0) 
  {
    _This->mValues.resize(_Count);
    
    RealArray::iterator dst = _This->mValues.begin();    
    
    dmIntensityProfile::const_iterator it   = _This->mProfile.Begin();
    dmIntensityProfile::const_iterator last = _This->mProfile.End();
    
    // Apply filter
    // Reduce borders effects by imposing extra data to
    // be anti-symmetric (this will preserve derivative and mean value
    // at end points over a length equal to the half-size of the filter)
    
    if(_This->mOrder > 0 &&
       _Count > 2*_This->mExtend)
    {
       int i,k,extend = _This->mExtend;
       int m          = _Count + 2*extend;
       
       _This->mData.Resize(m);
       _This->mTemp.Resize(m);
       
       dmMatrixRow VR(&_This->mTemp);
       
       // Left border
       for(k=1,i=extend;i>=1;++k,--i)
           VR[i] = static_cast<dm_float>((*(it+k)).value);
       
       _Count += extend;

       // Right border
       for(i=1;i<=extend;++i)
           VR[i+_Count] = static_cast<dm_float>((*(last-1-i)).value);
  
       // Copy Remaining of data =
       for(i=extend+1;it!=last; ++i,++it)
           VR[i] = static_cast<dm_float>((*it).value);
           
       // Apply filter
       dmMatrixRow _Coeffs(&_This->mCoeffs);
       dmMatrixRow _Output(&_This->mData);
       dmMatrix_ApplyFilter(_Coeffs,VR,_Output);  
       
       // Copy back data, apply transformation (mode!)
       if(_This->mMode == "abs") {
         for(i=extend+1;i<=_Count; ++i,++dst)
            *dst = dm_numerical::abs( static_cast<dm_real>(_Output[i]) );
       } else {
         for(i=extend+1;i<=_Count; ++i,++dst)
            *dst = static_cast<dm_real>(_Output[i]);            
       }          
    } 
    else
    {  
      for(;it!=last;++it,++dst)
          *dst = (*it).value;
    }
    
    CCI_RETVAL_P(values) = &_This->mValues[0];
    CCI_RETVAL_P(size)   = _Count; 
    
    CCI_RETURN_OK()
  }
    
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method     : GetPath
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Profile , GetPath )
{
  CCI_INSTANCE(Profile)
    
  dm_uint _Count = _This->mProfile.Count();  

  if(_Count > 0) 
  {
    _This->mPoints.resize(_Count);
    
    PointArray::iterator dst = _This->mPoints.begin();
    
    dmIntensityProfile::const_iterator it   = _This->mProfile.Begin();
    dmIntensityProfile::const_iterator last = _This->mProfile.End();
    
    for(;it!=last;++it,++dst)
        *dst = (*it).p;
    
    CCI_RETVAL_P(points) = &_This->mPoints[0];
    CCI_RETVAL_P(size)   = _Count; 
    
    CCI_RETURN_OK()
  }
  
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method     : GetData
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Profile , GetData )
{
  CCI_INSTANCE(Profile)
  
  dmIStorageMap _Storage;
  if(_Storage.QueryInterface(CCI_P(storage)))
  {
    dm_uint _Count = _This->mProfile.Count();  
    if(_Count > 0) 
    {
      const char_t* profile1 = CCI_P(profile1);
      const char_t* profile2 = CCI_P(profile2);
      const char_t* xpath    = CCI_P(xpath);
      const char_t* ypath    = CCI_P(ypath);

      dmIntensityProfile::const_iterator first = _This->mProfile.Begin();
      dmIntensityProfile::const_iterator last  = _This->mProfile.End();

      // Compute filtered profile if needed
      if(!EMPTY_STRING(profile1))
      {
    	  CCI_INVOKE_PP_(_This_ob,Profile,GetValues, _in 0 _in 0);
          dm_real* store = _Storage.GetCol(profile1,_Count);
          if(store)
             std::copy(_This->mValues.begin(),
                       _This->mValues.end(),
                       store+1);
          else  
            CCI_RETURN_FAIL()
      }
      
      // Store raw profile
      if(!EMPTY_STRING(profile2)) 
      {
        dm_real* store = _Storage.GetCol(profile2,_Count);
        if(store) {
           dmIntensityProfile::const_iterator it = first;
           dm_real* dst = store+1;
           for(;it!=last;++it,++dst)
                *dst = (*it).value;
        } else  
          CCI_RETURN_FAIL()        
      }

      // Store x path coordinate profile
      if(!EMPTY_STRING(xpath)) 
      {
        dm_real* store = _Storage.GetCol(xpath,_Count);
        if(store) {
           dmIntensityProfile::const_iterator it = first;
           dm_real* dst = store+1;
           for(;it!=last;++it,++dst)
               *dst = (*it).p.x;
        } else  
          CCI_RETURN_FAIL()        
      } 
      
      // Store y path coordinate profile
      if(!EMPTY_STRING(ypath)) 
      {
        dm_real* store = _Storage.GetCol(ypath,_Count);
        if(store) {
           dmIntensityProfile::const_iterator it = first;
           dm_real* dst = store+1;
           for(;it!=last;++it,++dst)
               *dst = (*it).p.y;
        } else  
          CCI_RETURN_FAIL()        
      } 
       
      CCI_RETURN_OK()
    }
  }
  
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method     : GetRange
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Profile , GetRange )
{
  CCI_INSTANCE(Profile)
  
  dm_uint _Count = _This->mProfile.Count();  

  if(_Count > 0) 
  {
    daim::gap<dm_real> _Range = _This->mProfile.GetRange();
    
    CCI_RETVAL_P(upper) = _Range.max();
    CCI_RETVAL_P(lower) = _Range.min();
    
    CCI_RETURN_OK()
  }
  
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method     : SetFilterParameters
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( Profile , SetFilterParameters )
{
  CCI_INSTANCE(Profile)
  
  dmTRY

   _This->mOrder  = CCI_P(order);
  
   if(_This->mOrder > 0) 
   {
     dm_int nSize = 2*CCI_P(extend) + 1;
    
     _This->mExtend = CCI_P(extend);
     _This->mDeriv  = CCI_P(deriv);
     _This->mMode   = CCI_P(mode);
     
     _This->mCoeffs.Resize(nSize);
      
     dmMatrixRow _Row(&_This->mCoeffs);
     dmMatrix_SGCoeffs(_Row,nSize,CCI_P(order),CCI_P(deriv));
     
     CCI_RETURN_OK()
   }
  
  dmCATCH(_E)
    CCI_SET_ERROR_MSG(_E.Text().CStr());
  dmDONE
  
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( Profile )
  CCI_REGISTER_METHOD_PP(Profile,SetPath)
  CCI_REGISTER_METHOD_PP(Profile,Compute)
  CCI_REGISTER_METHOD_NP(Profile,Clear)
  CCI_REGISTER_METHOD_PP(Profile,GetValues)
  CCI_REGISTER_METHOD_PP(Profile,GetPath)
  CCI_REGISTER_METHOD_PP(Profile,GetData)
  CCI_REGISTER_METHOD_PP(Profile,GetRange)
  CCI_REGISTER_METHOD_PP(Profile,SetFilterParameters)
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( Profile )
{
  CCI_INVOKE_FACTORY(Profile);
}
//---------------------------------------------------------------------
