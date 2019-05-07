#ifndef CCI_Notify_h
#define CCI_Notify_h

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
// File         : CCI_Notify.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------
#define CCIC_Notify _TXT("notify.cci")
#include "daim_modules/CCI_Prolog.h"

struct CallBackHook {
	CCI_METHOD (*cb_HookEntry)(struct CallBackHook*, cci_Object* ,dm_param* );
	dm_param    *cb_Data;
};

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( Notify, CCI_ROOT )
  
  CCI_MODULE_UUID( Notify, da1dea8f-6613-414c-972c-b9163ea9da92)

  CCI_MODULE_VERSION( Notify, 2 ) 

  /* Methods */

  CCI_DECL_METHOD_PP( Notify, 1, Notify, 
       _in cci_Tag        Attr 
       _in cci_Data       Val 
       _in dm_uint        Opts 
       _in dm_cci(Notify) ObjDest 
       _in dm_uint        MsgLength 
       /* CCI Msg follow ... */ 
  ) 

  CCI_DECL_METHOD_PP( Notify, 2, NotifySet   , _in cci_TagItem tg /*...*/ )
  CCI_DECL_METHOD_PP( Notify, 3, KillNotify  , _in cci_Tag attr _in dm_cci(Notify) object )
  CCI_DECL_METHOD_PP( Notify, 7, NoNotifySet , _in cci_TagItem tg /*...*/ )
  CCI_DECL_METHOD_PP( Notify, 8, CallBackHook, _in struct CallBackHook* hook _in dm_param param1 /*...*/ )

  CCI_DECL_METHOD_PP( Notify,15, Map         , _in cci_Tag Attr )

  CCI_DECL_METHOD_PP( Notify,16, NotifyMsg, 
        _in cci_Tag        Attr
        _in cci_Data       Val 
        _in dm_uint        Opts 
        _in dm_cci(Notify) ObjDest 
        _in dm_uint        MsgLength
        _in cci_Msg*       Msg 
  )

  CCI_DECL_METHOD_PP( Notify,17, CallBack, 
       _in CCI_METHOD (*func)(dm_param*,cci_Object*,dm_param*) 
       _in dm_param* data 
       _in dm_param  param1
       /* More params...*/ 
  )

  CCI_DECL_METHOD_NP( Notify,18, AddRef  )
  CCI_DECL_METHOD_NP( Notify,19, Release )

  /* Attributs */

  CCI_DECL_ATTR( Notify,1, NoNotify, BOOL , [....] )
  CCI_DECL_ATTR( Notify,2, UserData, dm_param, [ISGN] )
  CCI_DECL_ATTR( Notify,3, ObjectID, dm_param, [ISGN] )
  CCI_DECL_ATTR( Notify,4, RefCount, dm_uint , [..G.] )

  /* Special Values */

  /* Options flags for Notify method */
  #define CCIV_EveryTime   0x01L /* Trig on every value */
  #define CCIV_DoNotCheck  0x02L /* Does not Check for previous value */
  #define CCIV_NotifySelf  0x10L
  #define CCIV_LockNotify  0x20L /* Lock notification in multi threaded environment */ 
  #define CCIV_TrigAlways (CCIV_EveryTime|CCIV_DoNotCheck)

CCI_END_MODULE_DECL( Notify )

/* Helpers macros */

#define CCI_AddRef( _TheObject )  CCI_Invoke_NP(_TheObject,Notify,AddRef)
#define CCI_Release( _TheObject ) CCI_Invoke_NP(_TheObject,Notify,Release)

#define CCI_NOTIFY_SINK( TheClass, _TheMethod, _TheParams ) \
  sizeof(CCIP_DECL_(TheClass,_TheMethod)),CCIM_DECL_(TheClass,_TheMethod) _TheParams

/* Use this to évaluate Msg size in CCIM_Notify */
#define CCI_ARGS( nArgs ) ((nArgs)*sizeof(dm_param))

#endif //__doxygen


#ifdef __cplusplus

#include "CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Base class for all daim interfaces
 *
 *  This is the base interface for all daim interfaces objects.
 *  It implements the notification mecanism based on attributs values.
 */
//------------------------------------------------------------------------------------
class dmINotify : public dmIUnknown
{
  public:
    dmDECLARE_IINTERFACE(Notify)

     dmINotify() {}
    ~dmINotify() {}

    //-------------------------------------------------------
    /*! Add reference count for the underlying object.
     *  Objects are always created with a reference count of 1
     */
    //-------------------------------------------------------
    dm_uint AddRef()  { CCI_RETURN_IMETHOD_NP(Notify,AddRef);  }

    //-------------------------------------------------------
    /*! Release reference count for the underlying object
     *  If the reference count goes to zero, the object is deleted. 
     */
    //-------------------------------------------------------
    dm_uint Release() {
      dm_uint ref = 0; 
      if(IsValid()) {
        CCI_IMETHOD_NP(ref,Notify,Release);
        if(ref==0) Detach(); // Object Is no more valid 
      }
      return ref;
    }

    bool     SetObjectId( dm_param _data ) { return CCI_ISET(Notify,ObjectID,_data);      }
    dm_param GetObjectId() const           { return CCI_IGET(Notify,ObjectID,(dm_param)0); } 
    

    //-------------------------------------------------------
    /*! Attach user data to the object
     *! This will trig a notification 
     */
    //-------------------------------------------------------
    bool SetUserData( dm_param _data ) { return CCI_ISET(Notify,UserData, _data);     }

    //-------------------------------------------------------
    /*! Retrieve user data previously set
     *  using SetUserData
     */
    //-------------------------------------------------------
    dm_param GetUserData() const  { return CCI_IGET(Notify,UserData,(dm_param)0); }

    //-------------------------------------------------------
    // NOTE: Use the following method with the syntax :
    // Notify(aAttr,aVal,aOpts,ObjDest,CCI_NOTIFY_SINK(TheClass,TheMethod, _in arg1 ...));
    //-------------------------------------------------------
      
    CCI_METHOD Notify(cci_Tag Attr, cci_Data Val, dm_uint Opts, dm_cci(Notify) ObjDest, 
                      dm_uint MsgLength, dm_param MethodID, ... ) {
    	CCI_RETURN_IMETHOD_PP(Notify,NotifyMsg, _in Attr _in Val _in Opts _in ObjDest
                           _in MsgLength _in CCI_TRANSLATE_MSG(cci_Msg,&MethodID));
    }
         
    void KillNotify(cci_Tag Attr, dm_cci(Notify) Object = NULL ) {
    	CCI_IINVOKE_PP(Notify,KillNotify, _in Attr _in Object);
    }

    
    CCI_METHOD Map(cci_Tag Attr, cci_Data Val, dm_uint Opts, dm_cci(Notify) ObjDest, 
    		      cci_Tag AttrDest ) {
        return Notify(Attr,Val,Opts,ObjDest,CCI_NOTIFY_SINK(Notify,Map,_in AttrDest));
    }

    CCI_METHOD NotifyMsg(cci_Tag Attr, cci_Data Val, dm_uint Opts, dm_cci(Notify) ObjDest, 
                         dm_uint MsgLength, cci_Msg* Msg ) {
    	CCI_RETURN_IMETHOD_PP(Notify,NotifyMsg, _in Attr _in Val _in Opts _in ObjDest
                              _in MsgLength _in Msg);
    }


    //-------------------------------------------------------
    // Notification function templates
    // The callback will be invoked by the Objdest parameter
    //-------------------------------------------------------

    template<class Op>
    CCI_METHOD BindNotify(cci_Tag Attr, cci_Data Val, Op op, dm_uint Opts=0, dm_cci(Notify) ObjDest=NULL) {
      return CCI_BindNotify(_IInstance,Attr,Val,Opts,ObjDest,op);
    }

    template<class Op>
    CCI_METHOD BindNotify0(cci_Tag Attr, cci_Data Val, Op op, dm_uint Opts=0, dm_cci(Notify) ObjDest=NULL) {
      return CCI_BindNotify0(_IInstance,Attr,Val,Opts,ObjDest,op);
    }

    template<class X>
    CCI_METHOD BindNotifyMember(cci_Tag Attr, cci_Data Val, X* x, void (X::*f)(cci_Object*), 
                                dm_uint Opts=0, dm_cci(Notify) ObjDest=NULL) {
      if(Opts & CCIV_NotifySelf) ObjDest = reinterpret_cast<cci_Object*>(x); 
      return CCI_BindNotifyMember(_IInstance,Attr,Val,Opts,ObjDest,x,f);
    }

    template<class X>
    CCI_METHOD BindNotifyMember0(cci_Tag Attr, cci_Data Val, X* x, void (X::*f)(), 
                                 dm_uint Opts=0,dm_cci(Notify) ObjDest=NULL ) {
      if(Opts & CCIV_NotifySelf) ObjDest = reinterpret_cast<cci_Object*>(x); 
      return CCI_BindNotifyMember0(_IInstance,Attr,Val,Opts,ObjDest,x,f);
    }
};

#include "daim_modules/CCI_NotifyUtils.h"

#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"


//------------------------------------------------------------------------------------
#endif /* CCI_Notify_h */
