#ifndef dmCCIClass_h
#define dmCCIClass_h

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
// File         : dmCCIClass.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

//--------------------------------------------------------
// Utility macros
//--------------------------------------------------------

#define CCI_OBJ( o )   reinterpret_cast<struct cci_ObjectImpl*>(o)

// Obtient un pointeur sur les donnees a partir de la structure 'cci_ObjectImpl'
#define CCI_BASEOBJECT( _obj )  (reinterpret_cast<cci_Object*>(CCI_OBJ(_obj) + 1))

// Obtient un pointeur sur 'cci_ObjectImpl' a partir de la structure de donnees
#define CCI_OBJECT( o )        (CCI_OBJ(o) - 1)

// Recupere le pointeur sur le handle de la 'classe'
#define CCI_OCLASS( o ) ( (CCI_OBJECT(o))->o_Class )

#define CCI_INST_DATA( cl , o) \
		reinterpret_cast<dm_ptr>( reinterpret_cast<dm_byte*>(o) + cl->cl_InstOffset )

#define CCI_SIZEOF_INSTANCE(cl) ( cl->cl_InstOffset + \
		  cl->cl_InstSize  + sizeof (struct cci_ObjectImpl ) )

#define DOMETHOD(cl,o,m) \
  cl->cl_Methods.me_Dispatcher(reinterpret_cast<cci_Class*>(cl), o, m)

#define CCI_ENSURE_MAGIC(o) (CCI_OBJECT(o)->o_Magic==cci_ClassImpl::mMagicNumber)
#define CCI_ENSURE_VALID(o) ((o)!=NULL && CCI_ENSURE_MAGIC(o))

//--------------------------------------------------------
//typedef CCI_METHOD (*cci_dispatcher)(void*, cci_Object*,cci_Msg*);

class  cci_ClassImpl;
struct cci_ObjectImpl;

struct cci_ObjectHandle : public dmNode {
  cci_ObjectImpl *o_Object;
};

struct cci_LockHandle {
  dm_uint  o_Strategy;
  dmRWLock o_Lock;
};

// Private handle on an Object
struct cci_ObjectImpl {
    dm_uint          o_Magic;
    cci_ObjectHandle o_Handle;
    cci_LockHandle*  o_Lock;
    cci_ClassImpl*   o_Class;
	//... instance data follows
};
//-------------------------------------------------------
// Global variables for the CCI system
//-------------------------------------------------------
struct cci_ModuleState 
{
  long        mClassCount;
  dmList      mListOfClasses;
  dmList      mGarbageList;
  dmMutex     mClassFactoryMutex;
};
//-------------------------------------------------------
class cci_ClassImpl : public dmNode
                    , public dmList
                    , public dmReferenced
{
  friend cci_Class*     CCI_ResolveExternal( CONST char_t* );
  friend cci_Class*     CCI_FindPubClass( CONST char_t*, dm_uint );
  friend cci_Class*     CCI_MakeClass( CONST char_t*, CONST char_t*, cci_Class* , dm_uint, cci_Method_Entry* );
  friend void           CCI_FreeClass( cci_Class* );
  friend cci_Object*    CCI_MakeNewObject( cci_Class*, tg_TagItem* );
  friend cci_Object*    CCI_NewObject(  CONST char_t* , dm_uint, tg_TagItem* );
  friend void           CCI_FreeObject( cci_Object* );
  friend dm_ptr         CCI_InstData( cci_Class* , cci_Object* );
  friend dm_bool        CCI_SetAttrs( cci_Object*, tg_TagItem* );
  friend dm_bool        CCI_GetAttrs( cci_Object*, dm_param, dm_param* );
  friend dm_param       CCI_DoMethod( cci_Object*, cci_Msg* );
  friend dm_param       CCI_DoSuperMethod( cci_Class*, cci_Object*, cci_Msg* );
  friend dm_param       CCI_CoerceMethod (  cci_Class*, cci_Object*, cci_Msg* );
  friend dm_long_ptr    CCI_ObjectType( CONST char_t*, cci_Object* );
  friend _cci_logfun    CCI_LogFunction( _cci_logfun );
  friend void           CCI_Message( dm_uint , dm_param, cci_Class*, cci_Object*, CONST char_t* );
  friend void           CCI_Exit();
  friend dm_uint        CCI_Flush(cci_Class* , long );
  friend cci_Class*     CCI_ObjectClass( cci_Object* );
  friend dm_uint        CCI_UnLock( cci_Class* );
  friend dm_uint        CCI_LockObject( cci_Object* , dm_uint );
  friend dm_param       CCI_Assert( cci_Class* , CONST char_t* );

private:
  static _cci_logfun mLogFun;
  static dm_uint     mMagicNumber;

  static void   freeGarbageList();

  cci_ClassImpl*   cl_Parent;
  cci_Method_Entry cl_Methods;

  dm_uint   cl_InstOffset;
  dm_uint   cl_InstSize;
  dm_uint   cl_numinsts;
  dm_uint   cl_numSubClasses;
  dm_param  cl_InternalID;
  long      cl_Idle;

  dmString  cl_PublicID;
  dmList    cl_Instances;
  dmMutex   cl_Lock;
  
  dm_uint   cl_Status;

  // hook to the information from external CCI Class
  void*     cl_CustomHook;

public:
  const char_t* name() { return cl_PublicID.Get(); }

  cci_Object* newObject ( tg_TagItem* );
  void        freeObject( cci_Object* );
  void        freeAllObjects();
  
  dm_bool     assertClass( const char* _clsid );

  cci_ClassImpl(char_t*, cci_ClassImpl*, dm_uint, cci_Method_Entry*, dm_uint flags=0 );
  virtual ~cci_ClassImpl();
};

//----------------------------------------------------
#endif // dmCCIClass_h
