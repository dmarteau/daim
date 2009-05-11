#ifndef CCI_NotifyUtils_h
#define CCI_NotifyUtils_h

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
// File         : CCI_NotifyUtils.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

//---------------------------------------------------------
// C++ Binders for CCI notifications
//---------------------------------------------------------
template<class Op>
struct CCIP_BindNotify : public CCIP_DECL_(Notify,CallBack)
{
  static CCI_METHOD callback(dm_param*,cci_Object*,dm_param*);
  
  Op  op;

  CCIP_BindNotify( const Op& _op ) : op(_op) { 
     Id     = CCIM_DECL_(Notify,CallBack);
     func   = &CCIP_BindNotify<Op>::callback;
     data   = NULL;
  }
};
//---------------------------------------------------------
// Static function used as Callback
//---------------------------------------------------------
template<class Op> 
CCI_METHOD CCIP_BindNotify<Op>::callback(dm_param* data,cci_Object* obj ,dm_param* param1)
{
  (*(reinterpret_cast<Op*>(param1+1)))(obj);
  return CCI_ERR_OK;
}
//---------------------------------------------------------
// Binder for generic functor/function operator
//---------------------------------------------------------
template<class U> 
inline CCI_METHOD CCI_BindNotify(cci_Object* obj, cci_Tag Attr, cci_Data Val, 
                                 dm_uint Opts, cci_Object* ObjDest, const U& op )
{
  CCIP_BindNotify<U> msg(op);
  CCI_DECL_MSG(_This_msg, Notify,NotifyMsg, _in Attr _in Val _in Opts _in ObjDest
		  _in sizeof(CCIP_BindNotify<U>) _in CCI_TRANSLATE_MSG(cci_Msg, &msg) );

  return CCI_DoMethod(obj,CCI_TRANSLATE_MSG(cci_Msg,&_This_msg));
}
//---------------------------------------------------------
// Various generic binders/wrappers operators
//---------------------------------------------------------
template<class F> 
struct CCIP_BindNotify0
{
  F  _f;
  CCIP_BindNotify0( F f ) : _f(f) {} 
  void operator()( cci_Object* )  { _f(); }
};
//---------------------------------------------------------
template<class X> 
struct CCIP_BindNotifyMember
{
  X* _x;
  void (X::*_m)(cci_Object*);
  CCIP_BindNotifyMember( X* x,void (X::*m)(cci_Object*) ) : _x(x),_m(m) {} 
  void operator()( cci_Object* o ) { (_x->*_m)(o); }
};
//---------------------------------------------------------
template<class X> 
struct CCIP_BindNotifyMember0
{
  X* _x;
  void (X::*_m)();
  CCIP_BindNotifyMember0( X* x, void (X::*m)() ) : _x(x),_m(m) {} 
  void operator()( cci_Object* ) { (_x->*_m)(); }
};
//---------------------------------------------------------
template<class Op,class T=dm_param> 
struct CCIP_BindNotifyAttr
{
  T      _dflt;
  tg_Tag _tag;
  Op     _op;
  CCIP_BindNotifyAttr( const Op& op, tg_Tag tag, T dlft = 0 ) : _op(op),_tag(tag),_dflt(dlft) {} 
  void operator()( cci_Object* obj ) { _op(_tag,CCI_Get(obj,_tag,_dflt)); }
};
//---------------------------------------------------------
template<class Op,class T>
inline CCI_METHOD CCI_BindNotifyAttr( cci_Object* obj, cci_Tag Attr, cci_Data Val, 
                                      dm_uint Opts, cci_Object* ObjDest, 
                                      const Op& op, T dflt )
{  
  return CCI_BindNotify(obj,Attr,Val,Opts,ObjDest,
                        CCIP_BindNotifyAttr<Op,T>(op,Attr,dflt));
}
//---------------------------------------------------------
// Bind a full array of TagItem
//---------------------------------------------------------
template<class Op,class T>
inline CCI_METHOD CCI_BindNotifyAttrs( cci_Object* obj, cci_Tag Attr, cci_Data Val, 
                                       dm_uint Opts, cci_Object* ObjDest, const Op& op, 
                                       tg_TagItem *AttrList )
{
   cci_TagItem *tag, *tags = AttrList;   
   while( (tag = TG_kNextItem(&tags)) ) {
     return CCI_BindNotify(obj,Attr,Val,Opts,ObjDest,
                CCIP_BindNotifyAttr<Op>(op,tag->ti_Tag,tag->ti_Data));
   }
}
//---------------------------------------------------------
// Binders
//---------------------------------------------------------
template<class F>
inline CCI_METHOD CCI_BindNotify0( cci_Object* obj, cci_Tag Attr, cci_Data Val, 
                                   dm_uint Opts, cci_Object* ObjDest, F f )
{
  return CCI_BindNotify(obj,Attr,Val,Opts,ObjDest,CCIP_BindNotify0<F>(f));
}
//---------------------------------------------------------
template<class X>
inline CCI_METHOD CCI_BindNotifyMember( cci_Object* obj, cci_Tag Attr, cci_Data Val, 
                                        dm_uint Opts, cci_Object* ObjDest, X* x, 
                                        void (X::*f)(cci_Object*) ) 
{
  return CCI_BindNotify(obj,Attr,Val,Opts,ObjDest,CCIP_BindNotifyMember<X>(x,f));
}
//---------------------------------------------------------
template<class X>
inline CCI_METHOD CCI_BindNotifyMember0( cci_Object* obj, cci_Tag Attr, cci_Data Val, 
                                         dm_uint Opts, cci_Object* ObjDest, X* x, 
                                         void (X::*f)() )
{
  return CCI_BindNotify(obj,Attr,Val,Opts,ObjDest,CCIP_BindNotifyMember0<X>(x,f));
}
//---------------------------------------------------------
#endif // CCI_NotifyUtils_h

