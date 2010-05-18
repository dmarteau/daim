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

/* $Id: grszcore.cpp 17 mai 2010 20:58:09 */


#include "grszcore.h"
#include "templates/processing/dmObjectDetection.h"
/////////////////////////////////////////////////////////////////////////////

#define TO_PROFILE_TYPE(t,x) static_cast<dm_real>(t::scalar_value(x))

namespace ns_GRSZ {

//#ifdef _DEBUG
//  std::vector<dmPoint> ProfileDump;
//#endif

/**
 * Define a validator that bind an image and implement the processor 
 * for 'boundary_search' algorithm.
 */
template<class T> 
class ValidateBoundsImpl
{
 public:
 
   typedef typename daim::image<T>::traits_type traits_type;
   
   const daim::image<T>& _img;
   dm_real               _fmax;
   
   int            ke;
   bool           bAccept;
   int            count; 
            
   const dmIntensityProfile& _Profil;
   GRSZIntercepts&           _Defs;

 public:
    ValidateBoundsImpl( const daim::image<T>& im, 
                        GRSZIntercepts& aDefs, 
                        dm_real _fm, int _ke, 
                        const dmIntensityProfile& aProfil )             
   :_img(im)
   ,_fmax(_fm)
   , ke(_ke)
   , bAccept(true)
   , count(0)
   ,_Profil(aProfil)
   ,_Defs(aDefs)
   {}
   
   /**
    * This ensure that the path stay were pixels are below/above
    * given threshold
    */
   bool check_bounds_impl( const int& x, const int& y ) {
      return _Defs.bInvert ?
        ( (TO_PROFILE_TYPE(traits_type,_img[y][x]) - _fmax) >= _Defs.fComputedThreshold) : 
        ( (_fmax - TO_PROFILE_TYPE(traits_type,_img[y][x])) >= _Defs.fComputedThreshold) ; 
   } 
   
   /**
    * Validate selected pixel: make sure that we don't have a path too long 
    * (which lead to success) or verify that we are not going back at our 
    * starting point (found a 'spot') not a real boundary
    */
   bool process_impl( const int& x, const int& y ) 
   {
      dmPoint p(x,y);
    
      //#ifdef _DEBUG
      //  ProfileDump.push_back(p);
      //#endif
   
      if(count > _Defs.nMaxBorderLength) {
        return false;
      } else if(_Profil.GetPoint(ke)==p ) { 
        bAccept = false;
        return false;
      } 
      ++count;
      return true;
   }
     
};
  
/**
 * The validator itself that define a functor that operator
 * onto an image and call the 'boundary search' algorithm.
 */
struct ValidateBounds
{
  public:
   dm_real fm;
   int     ke;
   bool    bResult;
   
   const dmIntensityProfile& _Profil;
   GRSZIntercepts&           _Defs;
  
   ValidateBounds( GRSZIntercepts& aDefs,dm_real _fm,  dm_int _ke, 
                   const dmIntensityProfile& aProfil )
  :  fm(_fm)
   , ke(_ke)
   ,_Profil(aProfil)
   ,_Defs(aDefs)
   {}          

   /**
    * The two functors used in the 'boundary_search' algorithm
    */
   
   template<class T>
   struct check_bounds 
   {
     ValidateBoundsImpl<T>& _validator;
     check_bounds( ValidateBoundsImpl<T>& op ) : _validator(op) {}
     
     bool operator()( const int& x, const int& y ) { 
       return _validator.check_bounds_impl(x,y); 
     }       
   };

   template<class T>
   struct process 
   {
     ValidateBoundsImpl<T>& _validator;
     process( ValidateBoundsImpl<T>& op ) : _validator(op) {}
     
     bool operator()( const int& x, const int& y ) { 
       return _validator.process_impl(x,y); 
     }        
   };
     
   /**
    * Process an image
    */
   template<EPixelFormat _PixelFormat>
   void operator()( const dmIImage<_PixelFormat>& aSrc ) 
   {
     typedef typename dmIImage<_PixelFormat>::pixel_type pixel_type;
    
     ValidateBoundsImpl<pixel_type> _Validate(aSrc.Gen(),_Defs,fm,ke,_Profil);
  
     const dmPoint& ps = _Profil.GetPoint(ke);
     const dmRect&  rc = aSrc.Rect();
  
     daim::boundary_search(ps,rc,
          process<pixel_type>(_Validate),
          check_bounds<pixel_type>(_Validate));
   
     bResult = _Validate.bAccept;
   }
};

// Helper function
static bool Validate( dmImage* _Src, dm_int ke, 
                      GRSZIntercepts& _Defs,dm_real fm, 
                      const dmIntensityProfile& _Profil ) 
{
  if(!_Src) 
    return true;
     
  ValidateBounds _Validate(_Defs,fm,ke,_Profil);
  dmImplementOperation(_Validate,*_Src);

  return _Validate.bResult;
}

/**
 * Filter profil values and compute real threshold
 * according we use a relative threshold or not.
 * 
 * Abs: Tr' := Tr
 * Rel: Tr' := Tr * (max-min) [min and max computed along the profil]
 */
static dm_real DoFiltering( const dmIntensityProfile& _Profil,
                            std::vector<dm_real>&     _Result, 
                            GRSZIntercepts&           _Defs ) 
{ 
  _Result.clear();
  int i,cnt = _Profil.Size();
  dm_real f,fmax,fmin;

  _Result.resize( cnt );

  // Get MinMax along the profil
  fmin = fmax = _Profil.GetValue(0);
  for(i=1;i<cnt;++i) {          
    f = _Profil.GetValue(i); 
    if(f>fmax) fmax = f; else
    if(f<fmin) fmin = f;
  } 

  dm_real _delta = fmax - fmin;
  _Defs.fComputedThreshold = _Defs.fProfilThreshold;

  if(_delta>0) 
  {
    if(_Defs.bInvert) 
    {
      for(i=0;i<cnt;++i) 
          _Result[i] = (_Profil.GetValue(i)-fmin);
      fmax = fmin;
    }
    else 
    {
      for(i=0;i<cnt;++i) 
          _Result[i] = (fmax-_Profil.GetValue(i)); 
    }
    
    if(_Defs.bRelThreshold) 
       _Defs.fComputedThreshold *= _delta;

  } else for(i=0;i<cnt;++i) 
     _Result[i] = 0;
   
  return fmax;
}

} // namespace ns_GRSZ
 
/**
 * Compute intercepted segments
 */
int GRSZComputeFromProfil(const dmIntensityProfile& _Profil, 
                          GRSZIntercepts&        _Defs,
                          std::vector<dm_real>&  _Result,
                          std::vector<dm_int>&   _Indices, 
                          dmImage* img )
{

//#ifdef _DEBUG
//  ns_GRSZ::ProfileDump.clear();
//#endif

  const dmSpatialUnits& _Units = _Profil.Units();

  dm_int  ret = 0,ks,ke,k,cnt = (_Profil.Size());
  dm_real gsz;

  if(cnt) 
  {
    dm_real fm = ns_GRSZ::DoFiltering( _Profil,_Result,_Defs);

    // Count transitions
    _Indices.clear();
    dm_real thr  = _Defs.fComputedThreshold;
    std::vector<dm_real>::iterator iS,iF = _Result.begin();
    std::vector<dm_real>::iterator iL    = _Result.end();
    
    iS = iF;
    
    bool cont = iL!=iF, in_grain = *iF < thr; // initial state
    
    ke = 0; ks = cnt;
    ret = JOINT_JOINT; // 1
    bool ke_valid = true;
    
    while(cont) 
    {
      while( (in_grain == *iF < thr) && cont ) {
         cont = ++iF!=iL;  // skip over same state
      }
    
      if(cont) {        
        in_grain = !in_grain;    // transition
        k = iF-iS;
        if(in_grain) { 
          if( ke_valid ) ks = k;   // joint -> grain
        } else {                   // grain -> joint
          ke_valid = ns_GRSZ::Validate(img,k,_Defs,fm,_Profil);
          if(ke_valid)
          {                  
            ke = k - 1;           
            if( ke > ks ) {
              gsz = _Units.GetLength(_Profil.GetPoint(ks),_Profil.GetPoint(ke));  
              if(gsz >=  _Defs.fMinGrainSize ) {
                _Defs.fSegments += 1.0;
                _Indices.push_back(ks);
                _Indices.push_back(ke);
              }    
            } else if( ks==cnt )  { // 1st transition => we begin in grain
              _Defs.fSegments += 0.5;
              _Indices.push_back(ke);
              ret = GRAIN_JOINT; // 3
            }
          } // ke_valid
        } // !in_grain
      } // if(cont)
    } // while(cont) 

    if(in_grain)  // We end on a grain 
    {
      if(_Defs.eProfilType==eGRSZCircular) 
      {    
        if(ret!=GRAIN_JOINT) 
           ke = cnt-1; 
        else 
           ke = *_Indices.begin(); // Cas o˘ on a manquÈ la 1ere transition
           
        gsz = _Units.GetLength(_Profil.GetPoint(ks),_Profil.GetPoint(ke));
        
        if(gsz >= _Defs.fMinGrainSize )
        { 
          _Indices.push_back(ks);
          if( ret!=GRAIN_JOINT ) { 
             _Indices.push_back(ke); 
             _Defs.fSegments += 1.0f; 
          } else { 
             _Defs.fSegments += 0.5;
             ret = GRAIN_GRAIN; 
          }
        } else { // Remove reference to first point
          if(ret==GRAIN_JOINT) {
             _Indices.erase(_Indices.begin()); 
             ret = JOINT_JOINT;
           } 
        }
      } else {  // linear case
        _Indices.push_back(ks); 
        _Defs.fSegments += 0.5;
        ++ret;
      }
    } 
  } 
  return ret;
}
///////////////////////////////////////////////////

int GRSZCompute::operator()( const dmIntensityProfile& _profil, dmImage* im )
{
  Clear();
  std::vector<dm_real> _temp;
  std::vector<int>&    _trans = m_SegPoints;

  int k, cnt = _profil.Size();
  int resvalue = GRSZComputeFromProfil(_profil,*this,_temp,_trans,im);

  std::vector<int>::iterator it   = _trans.begin();
  std::vector<int>::iterator last = _trans.end();
 
  if(resvalue && it!=last) 
  {
    if(eProfilType==eGRSZCircular && resvalue==JOINT_JOINT) 
    {
      k = ( (cnt + *it++  + *--last)/2 ) % cnt;
      m_Points.push_back(k ); 
    } 
    else 
    {
      switch(resvalue) 
      {
        case JOINT_JOINT:
        case JOINT_GRAIN:
          m_Points.push_back(*it++);        // we start in a joint
          if(resvalue!=JOINT_JOINT) break;
        case GRAIN_JOINT:                   // we end in a joint
          m_Points.push_back(*--last);
      }
    }
    // Compute midpoints
    for(;it<last;it += 2) {
      m_Points.push_back( (*it + *(it+1))/2 );
    };
  }
  return resvalue;
}
//------------------------------------------------------------------------------
void GRSZCompute::Clear() 
{
  m_Points.clear();
  m_SegPoints.clear();

  fSegments = 0;
}
//---------------------------------------------------------------


