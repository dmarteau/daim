/* ::: BEGIN LICENSE BLOCK:::
 * Copyright (c) 2005 David Marteau
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


#include "daim_cci.h"
#include "daim_utilities.h"
#include "dmCCIClass.h"

#include <stdio.h>
#include <time.h>

#define NO_FLUSH ~0
#define CL_FLUSHABLE( _cl ) (_cl->cl_Idle != NO_FLUSH)
#define CL_PURGEABLE( _cl ) (CL_FLUSHABLE(_cl) && _cl->cl_Idle>0)

#define CCI_ENSURE_STATE()    (cci_pModuleState!=NULL)
#define CCI_ENSURE_CLASS(_cl) (_cl->cl_Status == 0)
//--------------------------------------------------------------
cci_ModuleState* cci_pModuleState = NULL;
//--------------------------------------------------------------
void CCI_Initialize()
{
  cci_pModuleState = new cci_ModuleState;
  cci_pModuleState->mClassCount = 0;
}
//--------------------------------------------------------------
long          __dm_GetCustomHook    ( const char_t*, void** hook );
void          __dm_FreeCustomHook   ( void* , const char_t* );
cci_Class*    __dm_MakeClassFromHook( void* );
//--------------------------------------------------------------

extern "C" {

//--------------------------------------------------------------
/// CCI
//--------------------------------------------------------------
__daim_export cci_Class* CCI_ResolveExternal( const char_t* name )
{
  // the class is not in memory, try to open it as external
  // This go into a recursive loop as to resolve all references
  // to the full class hierarchy

  void* CCI_Hook;
  long  rv = __dm_GetCustomHook(name,&CCI_Hook);

  if(CCI_SUCCEEDED(rv))
  {
    // New call to CCI_MakeClass(..)
    cci_ClassImpl* cl = (cci_ClassImpl*)__dm_MakeClassFromHook(CCI_Hook);
    if( cl ) {
      cl->cl_CustomHook = CCI_Hook;
      return (cci_Class*)cl;
    } else
      __dm_FreeCustomHook(CCI_Hook,name); 

  } else
    CCI_Message(rv,0,NULL,NULL,
       dmString::FormatString(dmString::npos,_TXT("Cannot load class module %s"), name ).Get());

  return NULL;
}
//--------------------------------------------------------------
#define _ENOVERSION_ ((dm_uint)TGZ_DEFAULT)
//--------------------------------------------------------------
__daim_export cci_Class* CCI_FindPubClass ( const char_t* idstr )
{
  cci_ClassImpl* cl = NULL;

  if(!CCI_ENSURE_STATE())
    return NULL;

  dmCRITICAL_SECTION(cci_pModuleState->mClassFactoryMutex);

  if(idstr) 
  {
     dmList::iterator it   = cci_pModuleState->mListOfClasses.Begin();
     dmList::iterator last = cci_pModuleState->mListOfClasses.End();
  
     cci_ClassImpl* _found;
     while(it!=last) 
     {
       _found = (cci_ClassImpl *)(*it); 
       if( !_found->cl_PublicID.Empty() ) 
       {
         if(_tcscasecmp(idstr,_found->cl_PublicID.Get())==0) { 
           cl = _found; 
           break;
         }
       }
       it++;
     }
     
     if(!cl) 
         cl = reinterpret_cast<cci_ClassImpl*>(CCI_ResolveExternal(idstr));
   }

   return (cci_Class*)cl;
}
//--------------------------------------------------------------
// MakeClass is usually called by the module itself
//--------------------------------------------------------------
__daim_export cci_Class* CCI_MakeClass( const char_t* pubClassID, 
                                        const char_t* pubSuperClassID, 
                                        cci_Class* privSuperClass,
		                                    dm_uint datasize , cci_Method_Entry *me)
{
  cci_ClassImpl *cl,*super;

  // Look for parent public class
  if( pubSuperClassID ) {
    if( (super = reinterpret_cast<cci_ClassImpl*>(CCI_FindPubClass( pubSuperClassID ))) == NULL)
      return NULL;
  } else 
    super = reinterpret_cast<cci_ClassImpl*>(privSuperClass);

  cl = new cci_ClassImpl(const_cast<char_t*>(pubClassID),super,datasize,me);

  if(super)
     super->Release();

  if(cl!=NULL) 
  {
    cci_Class* _Class = reinterpret_cast<cci_Class*>(cl);

    if(cl->cl_Methods.me_Initialize) 
    {
      if(CCI_FAILED(cl->cl_Methods.me_Initialize(_Class,cl->cl_Methods.me_UserData))) 
      {
        dmLOG(_TXT("CCI:ERROR: Failed to initialize class <%s>\n"),pubClassID);
        delete cl;
        _Class = NULL;
      }
    }

    return _Class;

  } else
    CCI_Message(CCI_ERR_MEMORY,0,0,0,_TXT(""));

  return NULL;
}
//--------------------------------------------------------------
__daim_export void  CCI_FreeClass( cci_Class* cl )
{
   cci_ClassImpl* _Class = reinterpret_cast<cci_ClassImpl*>(cl);
   
   // Preserve inheritence
   // Normally, classes should be destroyed in reverse order of
   // creation order.
   if(_Class->cl_numSubClasses==0)
   {   
     if(CCI_ENSURE_CLASS(_Class)) 
     {
        if(_Class->cl_Methods.me_Finalize)
           _Class->cl_Methods.me_Finalize(cl,_Class->cl_Methods.me_UserData);
       
        _Class->freeAllObjects();       // free all object instances
        _Class->cl_Status = 0xDEADBEEF; // invalidate class status
     }
     
     // Check if somebody hold a lock on this class 
     if(_Class->References() > 0)  
     {
       dmLOG(_TXT("CCI:ERROR: Cannot delete referenced class <%s>!!\n"),_Class->name());
       return;
     }

     void*  CCI_Hook = _Class->cl_CustomHook;

     dmString _PublicId = _Class->cl_PublicID;

     delete _Class;  // Truly delete class and all instances
    
     // Free hook if exists
	   if(CCI_Hook)
        __dm_FreeCustomHook(CCI_Hook,_PublicId.Get());
   }
}
//--------------------------------------------------------------
__daim_export cci_Object* CCI_MakeNewObject( cci_Class* cl, tg_TagItem* attrs )
{ 
   cci_ClassImpl *_cl = reinterpret_cast<cci_ClassImpl*>(cl);
   if(CL_FLUSHABLE(_cl)) _cl->cl_Idle = 0;
   return _cl->newObject(attrs); 
}
//--------------------------------------------------------------
__daim_export cci_Object* CCI_NewObject( const char_t* id, tg_TagItem* attrs)
{
  cci_ClassImpl *cl;
  if( (cl = reinterpret_cast<cci_ClassImpl*>(CCI_FindPubClass(id))) ) {
     return (cl->Release(),cl->newObject(attrs));
  } else return NULL;
}
//--------------------------------------------------------------
__daim_export dm_ptr CCI_InstData( cci_Class* cl , cci_Object* object) 
{ 
  //dmDEBUG_ASSERT( CCI_ENSURE_STATE() );
  //dmDEBUG_ASSERT( CCI_ENSURE_VALID(object) );
  //dmDEBUG_ASSERT( cl != NULL );
  return CCI_INST_DATA( reinterpret_cast<cci_ClassImpl*>(cl) ,object); 
}
//--------------------------------------------------------------
__daim_export dm_long_ptr CCI_ObjectType( const char_t* ClassID, cci_Object* object)
{
  if(CCI_ENSURE_STATE() && CCI_ENSURE_VALID(object)) 
  {
    cci_ClassImpl* cl = CCI_OCLASS(object);
    if(!ClassID) 
      return dm_ptr_to_long(cl->name());

    while(cl) {
      if(_tcscmp( ClassID, cl->name() )==0 ) 
        return dm_ptr_to_long(cl);
	  cl = cl->cl_Parent;
    }
  }
  return dm_ptr_to_long((cci_ClassImpl*)NULL);
}
//--------------------------------------------------------------
__daim_export cci_Class* CCI_ObjectClass( cci_Object* object )
{
  if(CCI_ENSURE_STATE() && CCI_ENSURE_VALID(object)) {
    cci_ClassImpl* cl = CCI_OCLASS(object);
    cl->Obtain();
    return reinterpret_cast<cci_Class*>(cl);
  }
  return NULL;
}
//--------------------------------------------------------------
__daim_export void CCI_FreeObject( cci_Object *object )
{
  //----------------------------------------------
  // Check CCI_Status before freeing object
  //----------------------------------------------
  if(CCI_ENSURE_STATE() && CCI_ENSURE_VALID(object)) 
  {
    cci_ClassImpl* cl = CCI_OCLASS(object);
    if(cl) {
      dmCRITICAL_SECTION( cl->cl_Lock )
      // May be should test if object is still valid at this point ? 
      cl->freeObject(object);
      // Start Idle time count
      if(CL_FLUSHABLE(cl) && cl->cl_numinsts<=0) cl->cl_Idle = clock(); 
    }
  }
}
//--------------------------------------------------------------
__daim_export dm_param CCI_SetAttrs( cci_Object *object, tg_TagItem* attrs)
{
  if(CCI_ENSURE_STATE() && CCI_ENSURE_VALID(object))
  {
    struct ccip_Set _opSet = { CCI_ME_SET, attrs }; // Envoie ME_SET

    // Check against lock

    cci_LockHandle *_Lock = CCI_OBJECT(object)->o_Lock;
    if(_Lock && (_Lock->o_Strategy & CCIV_LOCK_SET)) 
    {
      dmWRITE_LOCK(_Lock->o_Lock);    
      return DOMETHOD(CCI_OCLASS(object),object,CCI_TRANSLATE_MSG(cci_Msg,&_opSet));
    }
    else
      return DOMETHOD(CCI_OCLASS(object),object,CCI_TRANSLATE_MSG(cci_Msg,&_opSet));
  } 
  dmLOG(_TXT("CCI:ERRROR: ME_SET called on invalid object\n"));
  return CCI_ERR_INVALID_OBJECT;
}
//--------------------------------------------------------------
__daim_export dm_param CCI_GetAttrs( cci_Object *object, dm_param AttrID, dm_param *storage )
{
  if(CCI_ENSURE_STATE() && CCI_ENSURE_VALID(object)) 
  {
    struct ccip_Get _opGet = { CCI_ME_GET, AttrID, storage }; // Envoie ME_GET

    // Check against lock

    cci_LockHandle *_Lock = CCI_OBJECT(object)->o_Lock;
    if(_Lock && (_Lock->o_Strategy & CCIV_LOCK_SET)) 
    {
      dmREAD_LOCK(_Lock->o_Lock);
      return DOMETHOD(CCI_OCLASS(object),object,CCI_TRANSLATE_MSG(cci_Msg,&_opGet));
    } 
    else
      return DOMETHOD(CCI_OCLASS(object),object,CCI_TRANSLATE_MSG(cci_Msg,&_opGet));
  }
  dmLOG(_TXT("CCI:ERRROR: ME_GET called on invalid object"));
  return CCI_ERR_INVALID_OBJECT;
}
//--------------------------------------------------------------
// fait Executer par l'objet la methode specifiee par "MethodeID"
//--------------------------------------------------------------
__daim_export dm_param CCI_DoMethod(cci_Object *object, cci_Msg *msg)
{ 
  if(CCI_ENSURE_STATE() && CCI_ENSURE_VALID(object))
  { 
    // Check against lock

    cci_LockHandle *_Lock = CCI_OBJECT(object)->o_Lock;
    if(_Lock && (_Lock->o_Strategy & CCIV_LOCK_METHOD)) 
    {
      dmWRITE_LOCK(_Lock->o_Lock);
      return DOMETHOD(CCI_OCLASS(object),object,msg);
    }
    else
      return DOMETHOD(CCI_OCLASS(object),object,msg);
  }
  dmLOG(_TXT("CCI:ERROR: CCI_Method [0x%lx] called on invalid object\n"),msg->Id); 
  return CCI_ERR_INVALID_OBJECT; 
}
//--------------------------------------------------------------
__daim_export dm_param CCI_DoSuperMethod( cci_Class* cl, cci_Object* object , cci_Msg* msg)
{ 
  if(!CCI_ENSURE_STATE() || !CCI_ENSURE_VALID(object)) { 
    dmLOG(_TXT("CCI:ERROR: invalid Object in CCI_DoSuperMethod\n")); 
    return CCI_ERR_INVALID_OBJECT; 
  }
  if(!cl) { 
    dmLOG(_TXT("CCI:ERROR: Null Class  in CCI_DoSuperMethod\n")) ; 
    return CCI_ERR_INVALID_OBJECT; 
  }
  
  cci_ClassImpl *cci_cl = reinterpret_cast<cci_ClassImpl*>(cl);
  if(!CCI_ENSURE_CLASS(cci_cl)) {
     dmLOG(_TXT("CCI:ERROR: Invalid Class in CCI_DoSuperMethod\n"));
     return CCI_ERR_INVALID_OBJECT; 
  }
  
  return DOMETHOD( cci_cl->cl_Parent,object,msg ); 
}
//--------------------------------------------------------------
__daim_export dm_param CCI_CoerceMethod(  cci_Class* cl, cci_Object* object, cci_Msg* msg)
{ 
  // Allow null object
  if(!CCI_ENSURE_STATE()) 
     return CCI_ERR_INVALID_OBJECT;

  if(object && !CCI_ENSURE_VALID(object)) { 
     dmLOG("CCI;ERROR: invalid Object in CCI_CoerceMethod\n"); 
     return CCI_ERR_INVALID_OBJECT; 
  }
  if(!cl) { 
    dmLOG(_TXT("CCI:ERROR: Null Class  in CCI_CoerceMethod\n")); 
    return CCI_ERR_INVALID_OBJECT; 
  }

  cci_ClassImpl *cci_cl = reinterpret_cast<cci_ClassImpl*>(cl);

  if(!CCI_ENSURE_CLASS(cci_cl)) {
     dmLOG(_TXT("CCI:ERROR: Invalid Class in CCI_CoerceMethod\n"));
     return CCI_ERR_INVALID_OBJECT; 
  }

  return DOMETHOD( cci_cl, object, msg); 
}
//--------------------------------------------------------------
__daim_export void CCI_Exit()
{
  if(cci_pModuleState) 
  {
    // Desallocation Globale de toute les classes et Instances */
    cci_ClassImpl *cl;

    dmLOG(_TXT("CCI: Cleaning cci system...\n"));
    dmList& listOfClasses = cci_pModuleState->mListOfClasses;

    while( !listOfClasses.Empty() )
    {
      // recupere l'adresse sur la liste de classe
	    if( (cl = reinterpret_cast<cci_ClassImpl *>(listOfClasses.Pop_Back())) ) 
      {         
         // Unloading modules eplicitely may gives unexpected results 
         // because it is possible to have references to
         // objects or methods implemented in the module beeing
         // unloaded.

	       CCI_FreeClass(reinterpret_cast<cci_Class*>(cl));
	    }
    }

    // Free the garbage object list
    cci_ClassImpl::freeGarbageList();

    dmLOG(_TXT("CCI: Exiting\n"));

    delete cci_pModuleState;
    cci_pModuleState = NULL;
  }
}
//--------------------------------------------------------------
// Utility functions
//--------------------------------------------------------------
__daim_export void CCI_Message( dm_uint severity , dm_param me, cci_Class* cl, cci_Object* ob, const char_t* _msg )
{  
  if(cci_ClassImpl::mLogFun) (*cci_ClassImpl::mLogFun)(severity,me,cl,ob,_msg);
  else {
    dmStrStream msg_str;
    if(cl) {
      msg_str  << _TXT("CCI Message from  [");
      msg_str  << reinterpret_cast<cci_ClassImpl*>(cl)->name(); 
      msg_str  << _TXT("]\n");
      msg_str  << "severity=" << severity << "\n"; 
      msg_str  << "<<" << _msg << ">>\n"; 
    } else  {
      msg_str << _msg; 
    }

    dmLOG( msg_str << _TXT("\n"));
  }
}
//--------------------------------------------------------------
__daim_export _cci_logfun CCI_LogFunction( _cci_logfun _pLogFun )
{
  _cci_logfun _pOld  = cci_ClassImpl::mLogFun;
  cci_ClassImpl::mLogFun = _pLogFun;
  return _pOld;
}
//--------------------------------------------------------------
__daim_export dm_uint CCI_Flush( cci_Class* cl, long nIdle )
{
  dm_bool bRes = dm_false;

  if(!cci_pModuleState) return bRes;

  if(cl) 
  {
    cci_ClassImpl* _cl = reinterpret_cast<cci_ClassImpl*>(cl);

    dmCRITICAL_SECTION( _cl->cl_Lock )

    if(_cl->References()>0) 
       return dm_false;

    if(nIdle==NO_FLUSH) _cl->cl_Idle = nIdle; else
    if(nIdle==0) {
      if(_cl->cl_numinsts<=0) _cl->cl_Idle = clock();
      else                    _cl->cl_Idle = 0;
    } else if(CL_PURGEABLE(_cl) && _cl->cl_CustomHook) {
      nIdle *= CLOCKS_PER_SEC;
      if(static_cast<long>(clock())-_cl->cl_Idle > nIdle) {
        CCI_FreeClass(cl);
        bRes = dm_true;
      }
    }
  } 
  else if(!cci_pModuleState->mListOfClasses.Empty()) 
  {
    dmCRITICAL_SECTION(cci_pModuleState->mClassFactoryMutex);

    if(nIdle==NO_FLUSH) return dm_true;
    long t = clock();
    
    nIdle *= CLOCKS_PER_SEC;
 
    cci_ClassImpl* _cl;
    dmList::iterator _end  = cci_pModuleState->mListOfClasses.End();
    dmList::iterator _last = cci_pModuleState->mListOfClasses.Last();
    do {
      _cl   = (cci_ClassImpl*)(*_last--);            
      if(CL_PURGEABLE(_cl) && _cl->cl_CustomHook) {
        if(t - _cl->cl_Idle > nIdle) {
          CCI_FreeClass(reinterpret_cast<cci_Class*>(_cl));
          bRes = dm_true;
        }
      }
    } while( _last != _end );
  }    
  return bRes;
}
//--------------------------------------------------------------
__daim_export dm_uint CCI_UnLock( cci_Class* cl )
{
  if(CCI_ENSURE_STATE() && cl)
  {
    cci_ClassImpl* cci_cl = reinterpret_cast<cci_ClassImpl*>(cl);
    int refs = cci_cl->Release();
    
    // This class was marked as invalid
    // try to delete instance again
    if(refs == 0 && cci_cl->cl_Status == 0xDEADBEEF)
       CCI_FreeClass( cl );

    return refs;
  }
  
  return 0;
}
//--------------------------------------------------------------
__daim_export dm_uint CCI_LockObject( cci_Object* object, dm_uint _Strategy )
{
  if(CCI_ENSURE_STATE() && CCI_ENSURE_VALID(object))
  { 
    cci_ObjectImpl* _object = CCI_OBJECT(object);
    cci_LockHandle* _Lock   = _object->o_Lock;

    if(_Lock == NULL) 
    {
      _object->o_Lock   = _Lock = new cci_LockHandle;
      _Lock->o_Strategy = _Strategy;
      return CCIV_LOCK_NONE;
    }
    else
    {
      dm_uint _oldStrategy = _Lock->o_Strategy;

      dmWRITE_LOCK(_Lock->o_Lock)

      _Lock->o_Strategy = _Strategy;
      return _oldStrategy;      
    }
  } 
  else
    dmLOG(_TXT("CCI:ERROR: invalid Object in CCI_LockObject\n")); 

  return CCIV_LOCK_NONE;
}
//--------------------------------------------------------------
__daim_export dm_param CCI_Assert( cci_Class* cl, const char_t* clsid )
{ 
  if(!CCI_ENSURE_STATE()) 
     return CCI_ERR_INVALID_OBJECT;

  if(!cl) { 
    dmLOG(_TXT("CCI:ERROR: Null Class  in CCI_Assert\n")); 
    return CCI_ERR_INVALID_OBJECT; 
  }

  cci_ClassImpl *cci_cl = reinterpret_cast<cci_ClassImpl*>(cl);
  if(!CCI_ENSURE_CLASS(cci_cl)) {
     dmLOG(_TXT("CCI:ERROR: Invalid Class in CCI_Assert\n"));
     return CCI_ERR_INVALID_OBJECT; 
  }

  if(cci_cl->assertClass(clsid))
     return CCI_ERR_OK;

  return CCI_ERR_METHOD_FAILED;
}

//--------------------------------------------------------------

} // extern "C"
