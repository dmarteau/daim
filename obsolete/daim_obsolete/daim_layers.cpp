
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

#define dmUseKernelImageTemplates
#define dmUseKernelBuffer
#define dmUseToolManager
#define dmUseToolPrefs
#define dmUseToolFactory

#include "daim_tools.h"
#include "daim_info.h"
#include "daim_manager.h"
#include "daim_layers.h"

#include "daim_kernel/images/dmNativeImage.h"
//---------------------------------------------------------------
void daim_layers::CreateLayer(const char_t* _Name, EColorType _Color, const dmRegion& _Region )
{
  daim_layer_rgn* _Layer = GetLayer(_Name);
  if(_Layer) {
     _Layer->Region = _Region;
     _Layer->Color  = _Color;
  } else {
     _Layers.Push_Back(daim_layer_rgn(_Name,_Color,_Region));
  }
}
//---------------------------------------------------------------
void daim_layers::DeleteLayer( const char_t* _Name )
{
  dmTList<daim_layer_rgn>::iterator it = Find(_Name);
  if(it != _Layers.End()) {
     _Layers.Remove(it);
  }
}
//---------------------------------------------------------------
void daim_layers::ClearAll()
{
  _Layers.Clear();
}
//---------------------------------------------------------------
void daim_layers::MoveLayerToBack  ( const char_t* _Name )
{
  dmTList<daim_layer_rgn>::iterator it = Find(_Name);
  if(it != _Layers.End() && it != _Layers.Begin()) {
     _Layers.Push_Front( *it );
     _Layers.Remove( it );
  }
}
//---------------------------------------------------------------
void daim_layers::MoveLayerToFront ( const char_t* _Name )
{
  dmTList<daim_layer_rgn>::iterator it = Find(_Name);
  if(it != _Layers.End() && it != _Layers.Last()) {
     _Layers.Push_Back( *it );
     _Layers.Remove( it );     
  }
}
//---------------------------------------------------------------
void daim_layers::MoveLayerBackward( const char_t* _Name )
{
  dmTList<daim_layer_rgn>::iterator it = Find(_Name);
  if(it != _Layers.End() && it != _Layers.Begin()) {
     _Layers.Insert( it.Prev() , *it );
     _Layers.Remove( it );     
  }
}
//---------------------------------------------------------------  
void daim_layers::MoveLayerForward( const char_t* _Name )
{
  dmTList<daim_layer_rgn>::iterator it = Find(_Name);
  if(it != _Layers.End() && it != _Layers.Last()) {
     _Layers.Insert( it.Next() , *it );
     _Layers.Remove( it );     
  }  
}
//---------------------------------------------------------------  
void daim_layers::SetLayerColor( const char_t* _Name, EColorType _Color )
{
  dmTList<daim_layer_rgn>::iterator it = Find(_Name); 
  if(it != _Layers.End()) {
    (*it).Color = _Color;
  }
}
//---------------------------------------------------------------   
dmTList<daim_layer_rgn>::iterator daim_layers::Find( const char_t* _Name )
{
  dmTList<daim_layer_rgn>::iterator it   = _Layers.Begin();  
  dmTList<daim_layer_rgn>::iterator last = _Layers.End();    
  for(;it!=last;++it) {
    if( (*it).Name == _Name )
        return it;
  }
  return _Layers.End(); 
}
//---------------------------------------------------------------
// Layer Manager
//---------------------------------------------------------------
class daim_layer_mngr : public daim_manager
{
 public:
  daim_layers* mLayers;

 protected: 
  void Initialize();

 public:
   daim_layer_mngr( daim_layers*, dmTool* _parent = NULL  );
   daim_layer_mngr( daim_layers*, daim_info* _info, dmTool* _parent );
   daim_layer_mngr( daim_layers*, daim_info* _info, const dmRect& _r, dmTool* _parent = NULL );
   virtual ~daim_layer_mngr();

   virtual bool InitialUpdate();

   virtual bool OnCommand ( dm_uint nCode );
   virtual bool OnCommandUpdate ( daim_CmdUI*  );

 protected:
   virtual bool ProcessMouseEvent( dmMouseEvent& );
   virtual bool ProcessKeyEvent  ( dmKeyEvent&   );

   virtual bool OnFocus( bool _focus );

 protected:
   DECLARE_COMMAND_MAP()
};
/////////////////////////////////////////////////////////////////
daim_layer_mngr::daim_layer_mngr( daim_layers* _Layers, dmTool* _parent )
:daim_manager(_parent)
,mLayers(_Layers)
{
  Initialize();
}
//-----------------------------------------------------
daim_layer_mngr::daim_layer_mngr( daim_layers* _Layers, daim_info* _info, dmTool* _parent )
:daim_manager(_info,_parent)
,mLayers(_Layers)
{
  Initialize();  
}
//-----------------------------------------------------
daim_layer_mngr::daim_layer_mngr( daim_layers* _Layers, daim_info* _info,const dmRect& _r, dmTool* _parent )
:daim_manager(_info,_r,_parent) 
,mLayers(_Layers)
{
  Initialize();
}
//-----------------------------------------------------
daim_layer_mngr::~daim_layer_mngr() 
{
}
//-----------------------------------------------------
void daim_layer_mngr::Initialize()
{
}
//-----------------------------------------------------
bool daim_layer_mngr::OnFocus( bool _focus )
{
  return daim_manager::OnFocus(_focus);
}
//-----------------------------------------------------
bool daim_layer_mngr::InitialUpdate()
{
  if(daim_manager::InitialUpdate()) {
    SetToolName(DM_LAYERS);
    return true;
  }
  return false;
}
//-----------------------------------------------------
bool daim_layer_mngr::ProcessMouseEvent( dmMouseEvent& evt )
{
  /*
  switch(evt.mEventType) 
  {
    case evMouseDown: break;
    default: break;
  }
  */
  return daim_manager::ProcessMouseEvent(evt);
}
//----------------------------------------------------------------------------
bool daim_layer_mngr::ProcessKeyEvent( dmKeyEvent& evt )
{
  switch(evt.mKeyCode) 
  {
    case DM_VK_RETURN : break;
    case DM_VK_DELETE : break;
  }
  return daim_manager::ProcessKeyEvent(evt);
}
//--------------------------------------------------------------------------------------
bool daim_layer_mngr::OnCommand ( dm_uint nCode )
{
  return daim_manager::OnCommand(nCode);
}
//--------------------------------------------------------------------------------------
bool daim_layer_mngr::OnCommandUpdate ( daim_CmdUI* pCmdUI )
{
  return daim_manager::OnCommandUpdate(pCmdUI);
}
//-------------------------------------------------------
BEGIN_COMMAND_MAP(daim_layer_mngr, daim_manager)

END_COMMAND_MAP()
//-----------------------------------------------------
// Create LAYER manager
//-----------------------------------------------------
daim_manager* DAIM_Create_LAYERS( daim_info* info, daim_layers* layers, 
                                  const dmRect& r, dmTool* parent )
{
  return new daim_layer_mngr(layers,info,r,parent);
}
//-----------------------------------------------------
