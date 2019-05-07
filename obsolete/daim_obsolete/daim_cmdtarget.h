#ifndef daim_cmdtarget_h
#define daim_cmdtarget_h

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


#include "daim/daim_base.h"

#ifndef NO_DAIM_USER_INTERFACE

class daim_CmdTarget;
//------------------------------
struct daim_CmdUI
{
  dm_uint nCode;
  bool    bEnabled;
  int     checked;

  void SetCheck(int );
  void Enable  (bool);

  bool DoUpdate( dm_uint nCode, daim_CmdTarget* );
  
  daim_CmdUI() {}

};
//------------------------------
typedef void (daim_CmdTarget::*_REG_CMD)();
typedef void (daim_CmdTarget::*_REG_UPDATE)(daim_CmdUI*);
//------------------------------
struct COMMAND_MAP_ENTRY
{
  dm_uint     nCode;
  _REG_CMD    pfn; 
  _REG_UPDATE pfnUpdate;
};
//------------------------------
struct COMMAND_MAP
{
  const COMMAND_MAP*       pBaseMap;
  const COMMAND_MAP_ENTRY* lpEntries;
};
//------------------------------

#define DECLARE_COMMAND2(theClass, id, memberfx,updatefx) \
	{ (dm_uint)id,(_REG_CMD)&theClass::memberfx,(_REG_UPDATE)&theClass::updatefx },

#define REG_COMMAND(theClass, memberfx,updatefx) DECLARE_COMMAND2(theClass, cmd##memberfx,On##memberfx,updatefx)
#define REG_COMMAND_UPDATE(theClass, memberfx)   DECLARE_COMMAND2(theClass, cmd##memberfx,On##memberfx,OnUpdate##memberfx)


#define DECLARE_COMMAND_MAP() \
private:                                              \
	static const COMMAND_MAP_ENTRY _messageEntries[]; \
protected:                                            \
	static  const COMMAND_MAP  messageMap;            \
	virtual const COMMAND_MAP* GetMessageMap() const;


#define BEGIN_COMMAND_MAP(theClass, baseClass)                \
	const COMMAND_MAP* theClass::GetMessageMap() const        \
		{ return &theClass::messageMap; }                     \
	const COMMAND_MAP theClass::messageMap =                  \
	{ &baseClass::messageMap, &theClass::_messageEntries[0] };\
	const COMMAND_MAP_ENTRY theClass::_messageEntries[] =     \
	{                                                         \

#define END_COMMAND_MAP() {0,NULL,NULL }  \
     };                                   \
//------------------------------
class daim_CmdTarget 
{
  public:
   virtual bool OnCommandUpdate ( daim_CmdUI*  );
   virtual bool OnCommand       ( dm_uint nCode );

   virtual ~daim_CmdTarget() {}

   DECLARE_COMMAND_MAP()
};
//------------------------------
#define __command
//------------------------------
#endif //NO_DAIM_USER_INTERFACE

#endif //daim_cmdtarget_h
