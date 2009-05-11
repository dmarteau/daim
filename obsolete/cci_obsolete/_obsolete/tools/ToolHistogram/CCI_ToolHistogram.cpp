
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

#define dmUseToolManager
#include "daim_tools.h"

#include "daim/daim_base.h"
#include "daim/daim_interfaces.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_Document.h"
#include "daim_modules/CCI_Parameters.h"
#include "daim_modules/CCI_Manager.h"

#include "daim_modules/processing/CCI_Histograms.h"
#include "daim_modules/tools/CCI_ToolHistogram.h"

//------------------------------------------------------------------------
#define CCIC_NAME  CCIC_ToolHistogram
#define CCIC_SUPER CCIC_Manager
#define CCIC_SUPER_VERSION CCI_Manager_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_ToolHistogram_Version
#define CCI_REVISION  0
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))
//--------------------------------------------------------------------

#include "daim_modules/CCI_Base.h"
#include "daim_modules/CCI_Interface.h"


#include "dmToolHistogram.h"

#include <limits> // stl numeric_limits

//--------------------------------------------------------------------
CCI_DECL_DATA( ToolHistogram )
{
  dmIManager       Manager;
  dm_bool          LogScale;
  dm_bool          ReadOnly;
  dm_bool          Thresholding;

  dmToolHistogram* Tool;
  dmIHistograms    IHistogram;

  dm_real          MinRange;
  dm_real          MaxRange;

  dm_real          LowerThr;
  dm_real          UpperThr;
};

#define THIS_MANAGER _This->Manager

//--------------------------------------------------------------------
// ME_NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( ToolHistogram )
{
  CCI_CONSTRUCT_SUPER()

  CCI_INSTANCE(ToolHistogram)
  CCI_INIT_MEMBER(Manager);
  CCI_INIT_MEMBER(IHistogram);

  _This->Thresholding = dm_false;
  _This->ReadOnly     = dm_false;
  _This->LogScale     = dm_false;

  _This->MinRange = -1;
  _This->MaxRange = -1;

  // These settings ensure that these values are initialised
  _This->LowerThr = 1;
  _This->UpperThr = 0;

  if(THIS_MANAGER.QueryInterface(_THIS_OBJECT) && 
     _This->IHistogram.Create() ) 
  {
    CCI_RETURN_THIS()
  }
  else
   CCI_RETURN_FAILED_TO_CONSTRUCT()
}
//---------------------------------------------------------------------
// ME_DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( ToolHistogram )
{
  CCI_INSTANCE(ToolHistogram)

  _This->IHistogram.Release();

  CCI_DESTROY_MEMBER(Manager);
  CCI_DESTROY_MEMBER(IHistogram);
  CCI_DESTROY_SUPER()
}

//---------------------------------------------------------------------
// ME_GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( ToolHistogram )
  CCI_ATTR_GET( ToolHistogram,LogScale    , CCIA_RETVAL = _This->LogScale     )
  CCI_ATTR_GET( ToolHistogram,ReadOnly    , CCIA_RETVAL = _This->ReadOnly     )
  CCI_ATTR_GET( ToolHistogram,Thresholding, CCIA_RETVAL = _This->Thresholding )
  CCI_ATTR_GET( ToolHistogram,Channels  ,   CCIA_RETVAL = _This->IHistogram.GetChannels()  )
  CCI_ATTR_GET( ToolHistogram,Histograms,   
      CCIA_RETVAL = CCIA_TRANSLATE_VAL(dmIINTERFACE_PTR(_This->IHistogram)))
CCI_END_GETTER()
//---------------------------------------------------------------------
// ME_SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( ToolHistogram )
  CCI_ATTR_SET( ToolHistogram,LogScale , 
     _This->LogScale  = CCIA_DATA;
     if(_This->Tool) {
        _This->Tool->mLogScale = _This->LogScale;
        _This->Tool->Redraw();
     }
  )
  CCI_ATTR_SET( ToolHistogram,ReadOnly , 
     _This->ReadOnly = CCIA_DATA;
     if(_This->Tool) _This->Tool->mReadOnly = _This->ReadOnly;
  )

  CCI_ATTR_SET( ToolHistogram,Thresholding , 
     if(_This->Tool) {
        _This->Thresholding = _This->Tool->DoThresholding(CCIA_DATA);
     }
     CCIA_DATA = _This->Thresholding;
  )


  CCI_ATTR_SET( ToolHistogram,Channels , _This->IHistogram.SetChannels(CCIA_DATA) )
  CCIA_DISCARD( ToolHistogram,Histograms )
CCI_END_SETTER()
//---------------------------------------------------------------------
// Method: Scan
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ToolHistogram, Scan )
{
  CCI_INSTANCE(ToolHistogram)

  dm_param rv = CCI_Invoke(_This->IHistogram,Histograms,Scan,
                           _in CCI_P(params)
                           _in CCI_P(index)
                           _in CCI_P(flags));

  _This->IHistogram.GetRange(_This->MinRange,_This->MaxRange);

  if(CCI_SUCCEEDED(rv) && _This->Tool) 
  {
    O_INVALIDATE(_This->Tool)

    _This->Tool->SetRangeValues( 
        _This->LowerThr ,_This->UpperThr,
        _This->MinRange ,_This->MaxRange );

    _This->Tool->SetChannel(CCI_P(index));
  }

  return rv;
}
//---------------------------------------------------------------------
// Method: GetRange
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ToolHistogram, GetRange )
{
  CCI_INSTANCE(ToolHistogram)
  if(_This->IHistogram.GetRange(CCI_P(lower),CCI_P(upper)))
    CCI_RETURN_OK()

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: SetRange
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ToolHistogram, SetRange )
{
  CCI_INSTANCE(ToolHistogram)

  if(_This->IHistogram.SetRange(CCI_P(lower),CCI_P(upper))) 
  {
    _This->MinRange = CCI_P(lower);
    _This->MaxRange = CCI_P(upper);
    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: GetValues
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ToolHistogram, GetValues )
{
  CCI_INSTANCE(ToolHistogram)

  dmToolHistogram* _Tool = _This->Tool;

  if(_Tool && _Tool->IsValidData()) 
  {
    if(_Tool->mData) 
    {
      dm_int index  = _Tool->GetIndex(CCI_P(cursor));
      dm_real count = _Tool->mCount;
      if(index >=0) 
      {
        dm_real density = _This->MinRange + ((_This->MaxRange-_This->MinRange)/(count-1)) * index; 
        
 
        CCI_RETVAL_P(density) = density;
        CCI_RETVAL_P(value)   = _Tool->mData[index];
      }

     CCI_RETURN_OK();
    }
  }

  CCI_RETVAL_P(density) = 0;
  CCI_RETVAL_P(value)   = -1;

  CCI_RETURN_OK();
}
//---------------------------------------------------------------------
// Method: Index
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ToolHistogram, Index )
{
  CCI_INSTANCE(ToolHistogram)
  if(_This->Tool) {
    switch(CCI_P(me))
    {
      case CCI_ME_SET :
       if( _This->Tool->mChannel != CCI_P(index)) {
           _This->Tool->SetChannel(CCI_P(index));
       }
       CCI_RETURN_OK()

      case CCI_ME_GET : 
       CCI_RETVAL_P(index) = _This->Tool->mChannel; 
       CCI_RETURN_OK()
    }
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: Data
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ToolHistogram, Data )
{
  CCI_INSTANCE(ToolHistogram)

  dmToolHistogram* _Tool = _This->Tool;
  if(_Tool && _Tool->mData) {
     CCI_RETVAL_P(count) =  _Tool->mCount;
     CCI_RETVAL_P(data)  =  _Tool->mData;
     CCI_RETURN_OK();
  }

  CCI_RETURN_FAIL()
}

//---------------------------------------------------------------------
// Method: Create
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Manager, Create )
{
  CCI_INSTANCE(ToolHistogram)
  if(CCI_SUCCEEDED(CCI_INVOKE_SUPER())) 
  {
    dmToolManager* manager = THIS_MANAGER.GetManager();
    if(manager) 
    {  
      manager->SetDefaultCursor(eCursor_crosshair);

      _This->Tool = new dmToolHistogram(_This->IHistogram,manager);
      _This->Tool->Show(true);
      _This->Tool->Initialize();

      manager->Add(_This->Tool);

     _This->Tool->Select(true);
     _This->Tool->SetFocus(true,NULL);

      CCI_RETURN_OK()
    }
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: Threshold
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ToolHistogram, Threshold )
{
  CCI_INSTANCE(ToolHistogram)

  dmToolHistogram* _Tool = _This->Tool;

  switch(CCI_P(me))
  {
    case CCI_ME_SET : 
      _This->LowerThr =  CCI_P(lower);
      _This->UpperThr =  CCI_P(upper);

      if(_Tool) {     
         _Tool->SetRangeValues( 
           _This->LowerThr ,_This->UpperThr,
           _This->MinRange ,_This->MaxRange );
      }
 
      CCI_RETURN_OK()

    case CCI_ME_GET : 
      if(_Tool &&  _Tool->Thresholding()) {
        dm_real count = _Tool->mCount;
        CCI_RETVAL_P(lower) = _This->MinRange + ((_This->MaxRange-_This->MinRange)/(count-1)) * _Tool->GetMinIndex(); 
        CCI_RETVAL_P(upper) = _This->MinRange + ((_This->MaxRange-_This->MinRange)/(count-1)) * _Tool->GetMaxIndex(); 
      }

      CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: GetStats
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ToolHistogram, GetStats )
{
  CCI_INSTANCE(ToolHistogram)

  dmToolHistogram* _Tool = _This->Tool;

  typedef std::numeric_limits<dm_real>  limits_type;

  if(_Tool) 
  {
    dm_uint minIndex,maxIndex;

    if(_Tool->Thresholding()) {
      minIndex = static_cast<dm_uint>(_Tool->GetMinIndex());
      maxIndex = static_cast<dm_uint>(_Tool->GetMaxIndex());
    } else {
      minIndex = 0;
      maxIndex = _Tool->mCount  - 1;
    }  

    dm_real fminrange = _This->MinRange; 
    dm_real fDelta    = ((_This->MaxRange-_This->MinRange)/(_Tool->mCount-1));

    

    //CCI_RETVAL_P(min) = fminrange + fDelta * minIndex;
    //CCI_RETVAL_P(max) = fminrange + fDelta * maxIndex;

    CCI_RETVAL_P(min) = fminrange + fDelta * _Tool->MinDensityValue(minIndex,maxIndex);
    CCI_RETVAL_P(max) = fminrange + fDelta * _Tool->MaxDensityValue(minIndex,maxIndex);


 
    dm_real* Data = _Tool->mData;
    dm_real  Sum,Var,Ave,Rms;
    dm_real  s,x,p;

    Sum = Ave = 0; 

    dm_uint index;

    for(index=minIndex;index<=maxIndex;++index) {
      s    = Data[index];
      Sum += s;
      Ave += s * (fminrange + fDelta * index);
    }

    CCI_RETVAL_P(count) = Sum;

    if( Sum > 0 ) 
    {
      Ave /= Sum;
      Rms = Var = 0.0;

      for(index=minIndex;index<=maxIndex;++index)
      {
        s = Data[index];
        x = fminrange + fDelta * index; 
        p = (x - Ave);

        Rms  += s * x * x;
        Var  += s * p * p;
      }

      CCI_RETVAL_P(mean  ) = Ave;
      CCI_RETVAL_P(rms   ) = sqrt(Rms / Sum);
    
      if(Sum > 1) CCI_RETVAL_P(stddev) = sqrt(Var/(Sum - 1));
      else        CCI_RETVAL_P(stddev) = 0;
    }


    CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: Draw
//---------------------------------------------------------------------
/*
static inline CCI_IMPL_METHOD( Document, Draw )
{
  CCI_INSTANCE(ToolHistogram)

  if(CCI_P(flags)==CCIV_Document_DrawBckgnd) 
  {
  }

  return CCI_INVOKE_SUPER();
}
*/
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( ToolHistogram )
  CCI_REGISTER_METHOD_PP( ToolHistogram, Scan      )      
  CCI_REGISTER_METHOD_PP( ToolHistogram, GetRange  )
  CCI_REGISTER_METHOD_PP( ToolHistogram, SetRange  )
  CCI_REGISTER_METHOD_PP( ToolHistogram, GetValues )
  CCI_REGISTER_METHOD_PP( ToolHistogram, Index     )
  CCI_REGISTER_METHOD_PP( ToolHistogram, Data      )
  CCI_REGISTER_METHOD_PP( ToolHistogram, Threshold )
  CCI_REGISTER_METHOD_PP( ToolHistogram, GetStats  )
  CCI_REGISTER_METHOD_PP( Manager      , Create    )
//CCI_REGISTER_METHOD_PP( Document     , Draw      )
CCI_NEXT_DISPATCH_MAP()

//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( ToolHistogram )
{   
  CCI_INVOKE_FACTORY(ToolHistogram)
}
//---------------------------------------------------------------------
