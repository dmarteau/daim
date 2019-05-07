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

extern cci_ModuleState* cci_pModuleState;

static tg_TagItem NULL_ATTRS = { TGZ_DONE, NULL };

static inline tg_TagItem* cci_CheckAttrs( tg_TagItem* attrs ) {
	return (attrs?attrs:&NULL_ATTRS);
}

//--------------------------------------------------------------
dm_uint     cci_ClassImpl::mMagicNumber = CCI_MAKE_ID('C','C','I','M');
_cci_logfun cci_ClassImpl::mLogFun      = NULL;
//--------------------------------------------------------------
#define ALLOC_OBJ( cl ) static_cast<cci_ObjectImpl*>( dmMemory::Malloc(CCI_SIZEOF_INSTANCE(cl)) )
#define FREE_OBJ( ob  ) dmMemory::Free(ob)

#define AddMethods( cl_me ,me ) { \
    cl_me.me_Dispatcher  = me->me_Dispatcher; \
    cl_me.me_UserData    = me->me_UserData;   \
    cl_me.me_Initialize  = me->me_Initialize; \
    cl_me.me_Finalize    = me->me_Finalize;   \
  }
//--------------------------------------------------------------
cci_ClassImpl::cci_ClassImpl(char_t *pubClassID, cci_ClassImpl* SuperClass, 
                             dm_uint datasize,cci_Method_Entry *me,dm_uint flags)
: cl_Instances()
{
    // Initialiser la nouvelle structure de Classe
    cl_numinsts      = 0;
    cl_numSubClasses = 0;
    cl_InternalID    = 0;
    cl_PublicID      = pubClassID;

    cl_InstSize = datasize;
    cl_Parent   = SuperClass;

    cl_CustomHook = NULL;
    cl_Idle       = 0;
    cl_InstOffset = 0;

    if(cl_Parent) {
       cl_InstOffset    = cl_Parent->cl_InstOffset + cl_Parent->cl_InstSize;
       cl_Parent->cl_numSubClasses++;
    }

    cci_pModuleState->mListOfClasses.Push_Back(this);
    cci_pModuleState->mClassCount++;

    cl_Status = 0;

    AddMethods( cl_Methods , me );
}
//--------------------------------------------------------------
// Desallocation de la classse et de toute les instances de
// Cette Classe : la classe n'est effectivement liberee que
// si elle ne possede plus de classe derivee
//--------------------------------------------------------------
void cci_ClassImpl::freeAllObjects()
{
  cci_ObjectHandle *oh;

  // Liberer la liste des objets
  while(!cl_Instances.Empty() ) {
    oh = static_cast<cci_ObjectHandle*>(cl_Instances.Pop_Back());
    freeObject( CCI_BASEOBJECT(oh->o_Object) );
  }   
}
//--------------------------------------------------------------
cci_ClassImpl::~cci_ClassImpl()
{
  dmDEBUG_ASSERT(cl_numSubClasses==0);

  freeAllObjects();

  if(cl_Parent) cl_Parent->cl_numSubClasses--;
  cci_pModuleState->mClassCount--;
}
//--------------------------------------------------------------
// Allocation d'un nouvel objet
//--------------------------------------------------------------
cci_Object* cci_ClassImpl::newObject(tg_TagItem* attrs)
{
  struct ccip_Set _opSet = { CCI_ME_NEW, cci_CheckAttrs(attrs) };

  cci_ObjectImpl*  _object = ALLOC_OBJ( this );

  if(_object) 
  {

    // Construct the handle
    dmConstruct(&_object->o_Handle);

    // Set it's own address so that we can get it back
    // through the object handle

    _object->o_Handle.o_Object  = _object;

    _object->o_Class   = this;
    _object->o_Magic   = mMagicNumber;
    _object->o_Lock    = NULL;

    if( DOMETHOD(this,CCI_BASEOBJECT(_object),CCI_TRANSLATE_MSG(cci_Msg,&_opSet)) ) 
    {
      dmCRITICAL_SECTION( cl_Lock )
      cl_Instances.Push_Back(&_object->o_Handle);
      cl_numinsts++;
      return CCI_BASEOBJECT(_object);
    } else { 
      dmDestroy(&_object->o_Handle);
      FREE_OBJ(_object); 
    }
  } else 
    CCI_Message(CCI_ERR_MEMORY,0,reinterpret_cast<cci_Class*>(this),0,_TXT(""));

   return NULL;
}
//--------------------------------------------------------------
void cci_ClassImpl::freeObject(cci_Object *object)
{ 
  cci_Msg _msg = { CCI_ME_DISPOSE };

  cci_ObjectImpl* _object = CCI_OBJECT(object);

  // delete the lock handle
  if(_object->o_Lock) {
    delete _object->o_Lock;
    _object->o_Lock = NULL; 
  }

  // Send CCIM_Dispose  to the object
  if(CCI_SUCCEEDED(DOMETHOD( this, object, &_msg))) {
     dmDestroy(&_object->o_Handle);
     FREE_OBJ(_object); // FreeMemory
  } else {
    // The object a returned NOT_SUCCESS, so keep the handle
    // in the garbage list, this can occur when a object is still
    // handled by someone

    cci_ObjectHandle* oh = &_object->o_Handle;
    cl_Instances.Remove(oh); // Remove from main class list
    
    _object->o_Magic = 0; // Set the magic 

    #ifdef _DEBUG
    dmLOG("CCI:WARNING: object from class %s moved to garbage list\n",
          (cl_PublicID.Empty()?"<unknown>":cl_PublicID.CStr()));
    #endif

    cci_pModuleState->mGarbageList.Push_Back(oh);
  }

   cl_numinsts--;
}
//--------------------------------------------------------------
void cci_ClassImpl::freeGarbageList()
{
  cci_ObjectHandle *oh;

  dmList& garbageList = cci_pModuleState->mGarbageList;

  while(!garbageList.Empty() ) {
    oh = static_cast<cci_ObjectHandle*>(garbageList.Pop_Back());
    cci_ObjectImpl* _object = oh->o_Object;

    dmDestroy(oh);
    FREE_OBJ(_object); // FreeMemory
  } 
}
//--------------------------------------------------------------
dm_bool cci_ClassImpl::assertClass( const char* _clsid )
{
  if(_clsid!=NULL) 
  { 
    cci_ClassImpl* cl = this;
    while(cl) 
    {
      if(_tcscmp( _clsid, cl->name() )==0 ) 
        return dm_true;
      cl = cl->cl_Parent;
    } 
  }
  return dm_false;
}
//--------------------------------------------------------------
