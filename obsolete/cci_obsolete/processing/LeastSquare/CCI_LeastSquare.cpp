
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
#define dmUseUtilitiesExtra
#include "daim_utilities.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_StorageMap.h"

#include "daim_modules/processing/CCI_LeastSquare.h"


//------------------------------------------------------------------------
#define CCIC_NAME  CCIC_LeastSquare
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR "David Marteau"
#define CCI_VERSION   CCI_LeastSquare_Version
#define CCI_REVISION  0
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2005"))
//------------------------------------------------------------------------
#define SET_ERROR( ob,me,txt ) CCI_SetError(ECCI_ERROR,me,CCI_ObjectClass(ob),ob,txt)
//--------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"
#include "daim_modules/CCI_Interface.h"

#include "daim/daim_base.h"
#include "daim/daim_interfaces.h"

#include "dmMatrixBase.h"
#include "dmMatrixLSQ.h"
#include "dmMatrixFunc.h"
#include "dmMatrixUtils.h"


using namespace DP;

CCI_DECL_DATA( LeastSquare )
{
  dmIStorageMap mData;
  dmString      mXColumn;  
  dmString      mYColumn; 
  dmString      mBuffer;
  
  dmString      mFit;
  
  dmRectMatrix   U,V;
  dmColumnVector SV;
  dmColumnVector Sig;
  dmColumnVector P;
  dmRowVector    Y,X;

  dm_uint mStart;
  dm_uint mEnd;

} mData;
//--------------------------------------------------------------------
void LeastSquare_SetData( dmMatrixBase& _Matrix, dm_real *data )
{
   dm_matrix_t* it   = _Matrix.BlkStart();
   dm_matrix_t* last = _Matrix.BlkEnd();
     
   for(;it!=last;++it,++data)
        *it = static_cast<dm_matrix_t>(*data);
}
//--------------------------------------------------------------------
static bool LeastSquare_Update( CCI_INSTANCE_PTR(LeastSquare) _This )
{
  if(_This->mData.IsValid())
  {
    dm_real* xc = _This->mData.GetCol(_This->mXColumn.CStr(),0);
    dm_real* yc = _This->mData.GetCol(_This->mYColumn.CStr(),0);
      
    if(xc && yc)
    {
      dm_uint ndata = static_cast<dm_uint>(dm_numerical::min(xc[0],yc[0]));
      dm_uint start = 1;
      dm_uint end   = ndata;
      
      if(_This->mStart < _This->mEnd) {
         if(_This->mEnd <= ndata) {
            start =  _This->mStart;
            end   =  _This->mEnd;
            ndata =  end - start + 1;
         }
      }
       
      _This->Sig.Resize(ndata);
      _This->Sig.Set(1.0);
      
      _This->Y.Resize(ndata);
      _This->X.Resize(ndata);
      
      LeastSquare_SetData(_This->X,&xc[start]);
      LeastSquare_SetData(_This->Y,&yc[start]);
      
      return true;
    }
  }
  
  return false;
}
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( LeastSquare )
{
  CCI_GENERIC_CONSTRUCT(LeastSquare)

  _This->mStart = _This->mEnd = 0;

  CCI_RETURN_THIS()
}
//--------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( LeastSquare )
{
  CCI_INSTANCE(LeastSquare)

  if(_This->mData.IsValid()) {
     _This->mData.Release();
     _This->mData.Detach();
  }

  CCI_DESTROY_DATA()     
  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( LeastSquare )
  CCI_ATTR_GET(LeastSquare,XColumn , CCIA_RETVAL = CCIA_TRANSLATE_VAL(_This->mXColumn.CStr()) )
  CCI_ATTR_GET(LeastSquare,YColumn , CCIA_RETVAL = CCIA_TRANSLATE_VAL(_This->mYColumn.CStr()) )
  CCI_ATTR_GET(LeastSquare,Storage , CCIA_RETVAL = CCIA_TRANSLATE_VAL(dmIINTERFACE_PTR(_This->mData)))
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( LeastSquare )
  CCIA_DISCARD(LeastSquare,Storage)
  CCIA_DISCARD(LeastSquare,XColumn)
  CCIA_DISCARD(LeastSquare,YColumn)
CCI_END_SETTER()
//---------------------------------------------------------------------
// Method     : SetData
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( LeastSquare , SetData )
{  
  CCI_INSTANCE(LeastSquare)   
  
  if(_This->mData.IsValid()) {
     _This->mData.Release();
     _This->mData.Detach();
  }

  if(_This->mData.QueryInterface(CCI_P(storage)))
     _This->mData.AddRef();
        
  _This->mXColumn = CCI_P(xcolumn);
  _This->mYColumn = CCI_P(ycolumn);
  
  if(_This->mData.IsValid() && (_This->mXColumn.Empty() || _This->mYColumn.Empty()))
      CCI_RETURN_INVALID_PARAM()

  LeastSquare_Update(_This);

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method     : Fit
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( LeastSquare , Fit )
{ 
  CCI_INSTANCE(LeastSquare)
  
  dmTRY
   
   const char_t* type = CCI_P(type);
   if(!EMPTY_STRING(type))
   {
     dmMatrixRow XC(&_This->X);
     dmMatrixRow YC(&_This->Y);
     dmMatrixCol SC(&_This->Sig);
    
     //==============
     // Gaussian fit
     //==============
    
     // FIXME: Must use a non-linear fit
    
     if(EQUAL_STRING(type,"gauss"))
     {
       _This->P.Resize(3);
       _This->P.Set(1.0);
       
      //MatrixDump("X",_This->X);
      //MatrixDump("Y",_This->Y);
       
       dmMatrixCol PC(&_This->P);

       CCI_RETVAL_P(chi2) =  dmMatrixGaussFit(PC,YC,XC,SC,
                              _This->U,_This->V,_This->SV);
                              
       _This->mFit = type;      
       CCI_RETURN_OK()
     }

     //==============
     // Exponential fit
     //==============

     if(EQUAL_STRING(type,"exp"))
     {
       _This->P.Resize(2);
       _This->P.Set(1.0);

       dmMatrixCol PC(&_This->P);
          
       CCI_RETVAL_P(chi2) =  dmMatrixExpFit(PC,YC,XC,SC,
                              _This->U,_This->V,_This->SV);
       _This->mFit = type;      
       CCI_RETURN_OK()
     }
     
     //==============
     // Polynomial fit
     //==============

     if(EQUAL_STRING(type,"poly"))
     {
       if(CCI_P(iparam) < 1)
          CCI_RETURN_INVALID_PARAM()
      
       _This->P.Resize(CCI_P(iparam));
       _This->P.Set(1.0);

       dmMatrixCol PC(&_This->P);
            
       CCI_RETVAL_P(chi2) =  dmMatrixPolyFit(PC,YC,XC,SC,
                              _This->U,_This->V,_This->SV);
       _This->mFit = type;      
       CCI_RETURN_OK()
     }    
   }

  dmCATCH(_E)
    CCI_SET_ERROR_MSG(_E.Text().CStr());
    CCI_RETURN_FAIL()
  dmDONE
    
  CCI_RETURN_INVALID_PARAM()

}

//---------------------------------------------------------------------
// Method     : Update
//---------------------------------------------------------------------
static CCI_IMPL_METHOD_NP( LeastSquare , Update )
{
  CCI_INSTANCE(LeastSquare)
  LeastSquare_Update( _This );
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method     : GetParam
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( LeastSquare , GetParam )
{
  CCI_INSTANCE(LeastSquare)
  
  int index = CCI_P(index);
  
  if(index <= _This->P.NRows()) 
  {
    CCI_RETVAL_P(value) = _This->P(index);  
    CCI_RETURN_OK()
  }
  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method     : SetDataRange
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( LeastSquare , SetDataRange )
{
  CCI_INSTANCE(LeastSquare)
  
  _This->mStart = CCI_P(start);
  _This->mEnd   = CCI_P(end);
  
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method     : Predict
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( LeastSquare , Predict )
{
  CCI_INSTANCE(LeastSquare)
  
  dm_real* xc = _This->mData.GetCol(_This->mXColumn.CStr(),0);

  if(xc)
  {
    dm_uint ndata = static_cast<dm_uint>(xc[0]);
    dm_uint start = 1;
    dm_uint end   = ndata;
      
    if(_This->mStart < _This->mEnd) {
       if(_This->mEnd <= ndata) {
          start =  _This->mStart;
          end   =  _This->mEnd;
          ndata =  end - start + 1;
       }
    }
    
    dm_real* yc = _This->mData.GetCol(CCI_P(ycolumn),ndata);
    if(yc)
    {
      dmMatrixCol PC(&_This->P);
        
      if(_This->mFit == "poly") 
      {
        dm_uint j,k,n=PC.Length();
        dm_real y,x;
    
        for(k=start;k<=end;++k)
        {
          x = xc[k];
          y = PC[n];
          for(j=n;j>1;--j) {
             y = PC[j-1] + y * x;      
          }
          (*++yc) = y;
        }    
        CCI_RETURN_OK()
      }
      
      if(_This->mFit == "gauss") 
      {
        dm_real y;
    
        dm_real A = PC[1];
        dm_real k = PC[2];
        dm_real b = PC[3];
         
        for(dm_uint j=start;j<=end;++j)
        {
          y = xc[j] - b;
          (*++yc) = A * exp( - k * ( y * y ) );    
        }    
        CCI_RETURN_OK()
      }
    }
  }
    
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( LeastSquare )
  CCI_REGISTER_METHOD_PP(LeastSquare,SetData      )
  CCI_REGISTER_METHOD_PP(LeastSquare,Fit          )
  CCI_REGISTER_METHOD_NP(LeastSquare,Update       )
  CCI_REGISTER_METHOD_PP(LeastSquare,GetParam     )
  CCI_REGISTER_METHOD_PP(LeastSquare,SetDataRange )
  CCI_REGISTER_METHOD_PP(LeastSquare,Predict      )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( LeastSquare )
{
  //DM_INIT_API()
  CCI_INVOKE_FACTORY(LeastSquare)
}
//---------------------------------------------------------------------
