
/* :::BEGIN LICENSE BLOCK:::
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
 *  :::END LICENSE BLOCK::: */

#define dmUseCCI
#define dmUseUtilitiesExtra
#include "daim_utilities.h"
#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_Loader.h"
#include "daim_modules/CCI_StringList.h"
#include "daim_modules/CCI_Properties.h"
#include "daim_modules/CCI_Interface.h"

#define CCIC_NAME  CCIC_Loader
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_Loader_Version
#define CCI_REVISION  0
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))
//-------------------------------------------------------------------
#define CCI_INTERNAL_MODULE

#include "daim_modules/CCI_Base.h"
//-----------------------------------------

CCI_DECL_DATA( Loader )
{
  dm_uint dummy;
};
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static inline CCI_CONSTRUCTOR( Loader )
{
  CCI_CONSTRUCT_SUPER()
  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static inline CCI_DESTRUCTOR( Loader )
{
  CCI_DESTROY_SUPER()
}
/*
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( Loader )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( Loader )
{
  CCI_RETURN_INVOKE_SUPER()
}
*/
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( Loader ) 
  CCI_ATTR_GET( Loader, HasStreamSupport, CCIA_RETVAL = dm_true )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( Loader )
  CCIA_DISCARD(Loader,HasStreamSupport)
CCI_END_SETTER()
//---------------------------------------------------------------------
// Method:  Initialize
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Loader, Initialize )
{
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method:  Infos2
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Loader, Infos2 )
{
  CCI_INSTANCE( Loader )
    
  dm_cci(Notify)       ioStream   = CCI_P(ioStream);
  dm_cci(PropertyList) properties = CCI_P(properties);
  dm_param             key        = CCI_P(key); 
       
  if(CCI_P(me) == CCI_ME_GET)
  {    
     CCIP_DECL_(Loader,Infos2) * _Msg =  _This_msg;

     IF_CCI_INVOKE_RP(_THIS_OBJECT,Loader,Infos,_in CCI_ME_GET _in ioStream ) 
     THEN
      _Msg->format = CCI_RETVAL(format);  /* pixel type format id        */
      _Msg->index  = CCI_RETVAL(index);   /* index of the current record */
      _Msg->count  = CCI_RETVAL(count);   /* number of records           */
      _Msg->width  = CCI_RETVAL(width);   /* width  of image             */
      _Msg->height = CCI_RETVAL(height);  /* height of image             */
      
      if(properties) {
         dmIPropertyList _IPropList;
         if(_IPropList.QueryInterface(properties)) {
            _IPropList.SetPropertyStr ( key,NULL,"units:Name"         ,CCI_RETVAL(units));
            _IPropList.SetPropertyReal( key,NULL,"units:AspectRatio"  ,CCI_RETVAL(ar)   );
            _IPropList.SetPropertyReal( key,NULL,"units:UnitsPerPixel",CCI_RETVAL(uppx) );         
         }
     }


     CCI_RETURN_OK()

     ENDIF     
  }
  
  if(CCI_P(me) == CCI_ME_SET)
  {
     const char_t*  units = "";
     dm_real  uppx  = 1.0, ar = 1.0;

     if(properties) {
       dmIPropertyList _IPropList;
       if(_IPropList.QueryInterface(properties)) {
          units = _IPropList.GetPropertyStr ( key,NULL,"units:Name"         ,units);
          uppx  = _IPropList.GetPropertyReal( key,NULL,"units:UnitsPerPixel",uppx );         
          ar    = _IPropList.GetPropertyReal( key,NULL,"units:AspectRatio"  ,ar   );
       }
     }

     dm_param result;
     CCI_IMETHOD_PP_(result,_THIS_OBJECT,Loader,Infos,_in CCI_ME_SET
        _in ioStream
        _in CCI_P(format)
        _in CCI_P(index)
        _in CCI_P(count)
        _in CCI_P(width)
        _in CCI_P(height)
        _in units
        _in ar
        _in uppx
        _in CCI_P(flags)
      );
      
      if(CCI_SUCCEEDED(result))
         CCI_RETURN_OK()
      
      CCI_RETURN_FAIL()
  }

  CCI_RETURN_INVALID_PARAM()

}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( Loader )
  CCI_REGISTER_METHOD_PP( Loader, Initialize )
  CCI_REGISTER_METHOD_PP( Loader, Infos2 )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( Loader )
{
  CCI_INVOKE_FACTORY(Loader);
}
//---------------------------------------------------------------------
