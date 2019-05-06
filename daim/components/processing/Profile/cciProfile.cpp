/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code 3liz code.
 *
 * The Initial Developer of the Original Code is 3Liz SARL.
 * Portions created by the Initial Developer are Copyright (C) 2010
 * 3Liz SARL. All Rights Reserved.
 *
 * Contributor(s):
 *   David Marteau  (original author)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

/* $Id: cciProfile.cpp 14 mai 2010 21:57:54 */

#include "cciCOMPtr.h"
#include "cciComponentManagerUtils.h"
#include "cciIRegionContainer.h"
#include "cciIImageContainer.h"
#include "cciIImageList.h"
#include "cciIResultSet.h"
#include "cciIProfile.h"
#include "cciIIntercepts.h"

#define dmUseKernelImageTemplates
#define dmUseKernelBuffer
#include "daim_kernel.h"
#include "tools/dmIntensityProfile.h"

#include "dmMatrixBase.h"
#include "dmMatrixSG.h"
#include "dmMatrixUtils.h"

#include "grsz/grszcore.h"

typedef std::vector<dm_real>  RealArray;
typedef std::vector<dm_point> PointArray;

#define MODE_ABS 1

class cciProfile final : public cciIProfile,
                         public cciIIntercepts
{
public:
  CCI_DECL_ISUPPORTS
  CCI_DECL_IPROFILE
  CCI_DECL_IINTERCEPTS

  cciProfile();

private:
  ~cciProfile();

protected:
  dmIntensityProfile mProfile;
  RealArray          mValues;
  PointArray         mPoints;

  dmRowVector mCoeffs;
  dmRowVector mData;
  dmRowVector mTemp;
  int         mDeriv;
  int         mOrder;
  int         mExtend;
  
  bool     mbProfileComputed;
  
  dmCString   mMode;
  
  GRSZCompute mGRSZCompute;
  bool     mbGRSZComputed;
  int         mnGRSZReturn;
};

/* Implementation file */
CCI_IMPL_ISUPPORTS1(cciProfile, cciIProfile)

cciProfile::cciProfile()
: mDeriv(0)
, mOrder(2)
, mExtend(0)
{
  mbProfileComputed = false;
  mbGRSZComputed    = false;
  
  mGRSZCompute.bInvert           = false;
  mGRSZCompute.bRelThreshold     = true;
  mGRSZCompute.fMinGrainSize     = 0;
  mGRSZCompute.fProfilThreshold  = 0.5;
  mGRSZCompute.fSegments         = 0;
  mGRSZCompute.nMaxBorderLength  = 10;
  mGRSZCompute.fComputedThreshold = 0;
}

cciProfile::~cciProfile()
{
}

/* [noscript] void setPath (in unsigned long type, in voidPtr points, in unsigned long count); */
CCI_IMETHODIMP 
cciProfile::SetPath(dm_uint32 type, void *points, dm_uint32 count)
{
  CCI_ENSURE_ARG_POINTER(points);
 
  mProfile.Clear();   
  mGRSZCompute.Clear();
  mbGRSZComputed    = false;
  mbProfileComputed = false;

  switch(type)
  {
    case dmRectangle_t: 
      mProfile.SetCoordinates(dmRect(*static_cast<dm_rect*>(points))); 
      mGRSZCompute.eProfilType = eGRSZCircular;
      break;
    case dmEllipse_t: 
      mProfile.SetCoordinates(dmEllipse(*static_cast<dm_ellipse*>(points))); 
      mGRSZCompute.eProfilType = eGRSZCircular;
      break;
    case dmCircle_t: 
      mProfile.SetCoordinates(dmCircle(*static_cast<dm_circle*> (points))); 
      mGRSZCompute.eProfilType = eGRSZCircular;
      break;
    case dmLine_t: 
      mProfile.SetCoordinates(dmLine(*static_cast<dm_line*>(points))); 
      mGRSZCompute.eProfilType = eGRSZLinear;
      break;
    case dmPoly_t: 
      if(count>0) {
        dmPoly poly(static_cast<dm_point*>(points),count);
        mProfile.SetCoordinates(poly);
        if(poly.Closed())
           mGRSZCompute.eProfilType = eGRSZCircular;
        break;
      } 
    default:
      return CCI_ERROR_INVALID_ARG;
  }
 
  if(mProfile.Empty())
     return CCI_ERROR_FAILURE;
  
  return CCI_OK;
}

/* [noscript,notxpcom] unsigned long getPath (out pointArray points); */
CCI_IMETHODIMP_(dm_uint32) 
cciProfile::GetPath(dm_point **points CCI_OUTPARAM)
{
  CCI_ENSURE_ARG_POINTER(points);
  
  dm_uint _Count = mProfile.Count();  

  if(_Count > 0) 
  {
    mPoints.resize(_Count);
    
    PointArray::iterator dst = mPoints.begin();
    
    dmIntensityProfile::const_iterator it   = mProfile.Begin();
    dmIntensityProfile::const_iterator last = mProfile.End();
    
    for(;it!=last;++it,++dst)
        *dst = (*it).p;
    
    *points = &mPoints[0];
  }
  
  return _Count;
}

/* void compute (in cciImage image); */
CCI_IMETHODIMP 
cciProfile::Compute(cciImage _image)
{
  dmImage*  image = CCI_IF_NATIVE(_image);
  CCI_ENSURE_ARG_POINTER(image);

  mProfile.ComputeProfile(*image,false);
  mbProfileComputed = true;
  mbGRSZComputed    = false;
  
  return CCI_OK;
}

/* void clear (); */
CCI_IMETHODIMP 
cciProfile::Clear()
{
  mProfile.Clear();   
  mGRSZCompute.Clear();
  
  mbGRSZComputed    = false;
  mbProfileComputed = false;

  return CCI_OK;
}

/* [noscript,notxpcom] unsigned long getValues (out doubleArray data); */
CCI_IMETHODIMP_(dm_uint32) 
cciProfile::GetValues(dm_real **data CCI_OUTPARAM)
{
  if(mProfile.Empty())
     return 0;

  int _Count = mProfile.Count();  

  mValues.resize(_Count);
    
  RealArray::iterator dst = mValues.begin();    
    
  dmIntensityProfile::const_iterator it   = mProfile.Begin();
  dmIntensityProfile::const_iterator last = mProfile.End();
    
  // Apply filter
  // Reduce borders effects by imposing extra data to
  // be anti-symmetric (this will preserve derivative and mean value
  // at end points over a length equal to the half-size of the filter)
    
  if(mOrder > 0 && _Count > 2*mExtend)
  {
    int i,k,extend = mExtend;
    int m          = _Count + 2*extend;
       
    mData.Resize(m);
    mTemp.Resize(m);
       
    dmMatrixRow VR(&mTemp);
       
    // Left border
    for(k=1,i=extend;i>=1;++k,--i)
        VR[i] = static_cast<float>((*(it+k)).value);
       
    _Count += extend;

    // Right border
    for(i=1;i<=extend;++i)
        VR[i+_Count] = static_cast<float>((*(last-1-i)).value);
  
    // Copy Remaining of data =
    for(i=extend+1;it!=last; ++i,++it)
        VR[i] = static_cast<float>((*it).value);
           
    // Apply filter
    dmMatrixRow _Coeffs(&mCoeffs);
    dmMatrixRow _Output(&mData);
    dmMatrix_ApplyFilter(_Coeffs,VR,_Output);  
       
    // Copy back data, apply transformation (mode!)
    if(mMode == "abs") {
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
  
  if(data)
    *data = &mValues[0];

  return _Count; 
}

#ifndef CCI_EMPTY_STRING
#define CCI_EMPTY_STRING(lpsz) ((lpsz)==NULL || (lpsz)[0]=='\0')
#endif

/* void getData (in cciIResultSet results, in string label, in string rawLabel, in string xlabel, in string ylabel); */
CCI_IMETHODIMP 
cciProfile::GetData(cciIResultSet *results, const char * label, const char * rawLabel, const char * xlabel, const char * ylabel)
{
  CCI_ENSURE_ARG_POINTER(results);
  
  if(mProfile.Empty())
     return CCI_ERROR_NOT_AVAILABLE;
    
  dm_uint _Count = mProfile.Count();  
    
  dmIntensityProfile::const_iterator first = mProfile.Begin();
  dmIntensityProfile::const_iterator last  = mProfile.End();

  // Compute filtered profile if needed
  if(!CCI_EMPTY_STRING(label))
  {
    if(GetValues(NULL)<=0)
       return CCI_ERROR_FAILURE;
    
    dm_real* store = results->CreateCol(label,_Count);
    if(!store)
        return CCI_ERROR_OUT_OF_MEMORY;

    std::copy(mValues.begin(),mValues.end(),store);
  }
    
  // Store raw profile
  if(!CCI_EMPTY_STRING(rawLabel)) 
  {
    dm_real* store = results->CreateCol(rawLabel,_Count);
    if(!store)
       return CCI_ERROR_OUT_OF_MEMORY;

    dmIntensityProfile::const_iterator it = first;
    for(dm_real* dst=store;it!=last;++it,++dst)
        *dst = (*it).value;
  }

  // Store x path coordinate profile
  if(!CCI_EMPTY_STRING(xlabel)) 
  {
    dm_real* store = results->CreateCol(xlabel,_Count);
    if(!store)
       return CCI_ERROR_OUT_OF_MEMORY;

    dmIntensityProfile::const_iterator it = first;
    for(dm_real* dst=store;it!=last;++it,++dst)
        *dst = (*it).p.x;
  } 
 
  // Store x path coordinate profile
  if(!CCI_EMPTY_STRING(ylabel)) 
  {
    dm_real* store = results->CreateCol(ylabel,_Count);
    if(!store)
       return CCI_ERROR_OUT_OF_MEMORY;

    dmIntensityProfile::const_iterator it = first;
    for(dm_real* dst=store;it!=last;++it,++dst)
        *dst = (*it).p.y;
  } 
   
  return CCI_OK;
}

/* void getRange (out double upper, out double lower); */
CCI_IMETHODIMP 
cciProfile::GetRange(double *upper CCI_OUTPARAM, double *lower CCI_OUTPARAM)
{
  if(mProfile.Empty())
    return CCI_ERROR_NOT_AVAILABLE;

  daim::gap<dm_real> _Range = mProfile.GetRange();

  if(upper) *upper = _Range.max();
  if(lower) *lower = _Range.min();

  return CCI_OK;
}

/* void setFilterParameters (in unsigned long order, in unsigned long deriv, in unsigned long extend, in string mode); */
CCI_IMETHODIMP 
cciProfile::SetFilterParameters(dm_uint32 order, dm_uint32 deriv, dm_uint32 extend, const char * mode)
{
  dmTRY

   mOrder = order;
  
   if(mOrder > 0) 
   {
     dm_int nSize = 2*extend + 1;
    
     mExtend = extend;
     mDeriv  = deriv;
     mMode   = mode;
     
     mCoeffs.Resize(nSize);
      
     dmMatrixRow _Row(&mCoeffs);
     dmMatrix_SGCoeffs(_Row,nSize,order,deriv);
   }
   
   return CCI_OK;

  dmCATCH(_E)
    dmLOG_ERROR(_E.Text().get());
  dmDONE
  
  return CCI_ERROR_FAILURE;
}

/* readonly attribute unsigned long count; */
CCI_IMETHODIMP 
cciProfile::GetCount(dm_uint32 *aCount)
{
  *aCount = mProfile.Count();
  return CCI_OK;
}

//////////////////////////////////
// cciIIntercepts implementation
//////////////////////////////////

/* attribute float profilThreshold; */
CCI_IMETHODIMP 
cciProfile::GetProfilThreshold(float *aProfilThreshold)
{
  *aProfilThreshold = mGRSZCompute.fProfilThreshold;
  return CCI_OK;
}
CCI_IMETHODIMP 
cciProfile::SetProfilThreshold(float aProfilThreshold)
{
  mGRSZCompute.fProfilThreshold = aProfilThreshold;
  mbGRSZComputed = false;
  
  return CCI_OK;
}

/* attribute float minSegmentSize; */
CCI_IMETHODIMP 
cciProfile::GetMinSegmentSize(float *aMinSegmentSize)
{
  *aMinSegmentSize = mGRSZCompute.fMinGrainSize;
  return CCI_OK;
}
CCI_IMETHODIMP 
cciProfile::SetMinSegmentSize(float aMinSegmentSize)
{
  mGRSZCompute.fMinGrainSize = aMinSegmentSize;
  mbGRSZComputed = false;
  
  return CCI_OK;
}

/* attribute long maxTestBorderLength; */
CCI_IMETHODIMP 
cciProfile::GetMaxTestBorderLength(dm_int32 *aMaxTestBorderLength)
{
  *aMaxTestBorderLength = mGRSZCompute.nMaxBorderLength;
  return CCI_OK;
}
CCI_IMETHODIMP 
cciProfile::SetMaxTestBorderLength(dm_int32 aMaxTestBorderLength)
{
  mGRSZCompute.nMaxBorderLength = aMaxTestBorderLength;
  mbGRSZComputed = false;
  
  return CCI_OK;
}

/* attribute boolean relThreshold; */
CCI_IMETHODIMP 
cciProfile::GetRelThreshold(bool *aRelThreshold)
{
  *aRelThreshold = mGRSZCompute.bRelThreshold;
  return CCI_OK;
}
CCI_IMETHODIMP 
cciProfile::SetRelThreshold(bool aRelThreshold)
{
  mGRSZCompute.bRelThreshold = aRelThreshold;
  mbGRSZComputed = false;
  
  return CCI_OK;
}

/* attribute boolean invert; */
CCI_IMETHODIMP 
cciProfile::GetInvert(bool *aInvert)
{
  *aInvert = mGRSZCompute.bInvert;
  return CCI_OK;
}
CCI_IMETHODIMP 
cciProfile::SetInvert(bool aInvert)
{
  mGRSZCompute.bInvert = aInvert;
  mbGRSZComputed       = false;
  
  return CCI_OK;
}

/* long computeSegments (in cciImage image); */
CCI_IMETHODIMP 
cciProfile::ComputeSegments(cciImage _image, dm_int32 *_retval CCI_OUTPARAM)
{
  dmImage*  image = CCI_IF_NATIVE(_image);
  CCI_ENSURE_ARG_POINTER(image);
  
  if(!mbProfileComputed) {
     cci_result rv = Compute(_image);
     CCI_ENSURE_SUCCESS(rv,rv);
  }

  dmTRY
    mnGRSZReturn   = mGRSZCompute(mProfile,image);
    mbGRSZComputed = true;
    
    *_retval = mnGRSZReturn;
    
    return CCI_OK;
  dmCATCH(_E)
   dmLOG_ERROR(_E.Text().get());
  dmDONE
 
  return CCI_ERROR_FAILURE;
}

/* readonly attribute float fSegments; */
CCI_IMETHODIMP 
cciProfile::GetFSegments(float *aFSegments)
{
  CCI_ENSURE_TRUE(mbGRSZComputed,CCI_ERROR_NOT_AVAILABLE);
  
  *aFSegments = mGRSZCompute.fSegments;
  return CCI_OK;
}

/* readonly attribute long startPointStatus; */
CCI_IMETHODIMP 
cciProfile::GetStartPointStatus(dm_int32 *aStartPointStatus)
{
  CCI_ENSURE_TRUE(mbGRSZComputed,CCI_ERROR_NOT_AVAILABLE);
  
  *aStartPointStatus = mnGRSZReturn;
  return CCI_OK;
}

/* readonly attribute float fComputedThreshold; */
CCI_IMETHODIMP 
cciProfile::GetFComputedThreshold(float *aFComputedThreshold)
{
  CCI_ENSURE_TRUE(mbGRSZComputed,CCI_ERROR_NOT_AVAILABLE);
  
  *aFComputedThreshold = mGRSZCompute.fComputedThreshold;
  return CCI_OK;
}

/* void getSegmentData (in cciIResultSet results, in string name, in boolean midpoints); */
/* unsigned long getSegmentData (in cciIResultSet results, in string name, in boolean midpoints); */
CCI_IMETHODIMP 
cciProfile::GetSegmentData(cciIResultSet *results, const char * label, bool midpoints, 
                           dm_uint32 *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_TRUE(mbGRSZComputed,CCI_ERROR_NOT_AVAILABLE);
  
  if(CCI_EMPTY_STRING(label))
     return CCI_ERROR_INVALID_ARG;

  dm_uint32 length;
  
  std::vector<int>::const_iterator it,last,first; 
  if(midpoints) {
    first  = mGRSZCompute.m_Points.begin();
    last   = mGRSZCompute.m_Points.end();     
    length = mGRSZCompute.m_Points.size();
  } else {
    first  = mGRSZCompute.m_SegPoints.begin();
    last   = mGRSZCompute.m_SegPoints.end();
    length = mGRSZCompute.m_SegPoints.size();
  }  
  
  dm_real* store = results->CreateCol(label,length);
  if(!store)
     return CCI_ERROR_OUT_OF_MEMORY;
  
  std::copy(mValues.begin(),mValues.end(),store);

  *_retval = length;
  
  return CCI_OK;
}

//=====================================
// Module
//=====================================

#include "cciIGenericFactory.h"

CCI_GENERIC_FACTORY_CONSTRUCTOR(cciProfile)

static const cciModuleComponentInfo components[] = {
    {  CCI_PROFILE_CLASSNAME,
       CCI_PROFILE_CID,
       CCI_PROFILE_CONTRACTID,
       cciProfileConstructor
    },
};

CCI_IMPL_GETMODULE(ProfileModule, components)


